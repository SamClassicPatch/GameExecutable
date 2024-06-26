/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"

#include "MenuManager.h"
#include "MenuStarters.h"
#include "MenuStuff.h"
#include "GUI/Components/MenuGadget.h"
#include "LevelInfo.h"
#include "VarList.h"

// [Cecil] Classics patch
#include <CoreLib/Query/QueryManager.h>

#include "Cecil/ScreenResolutions.h"
#include "Cecil/WindowModes.h"

ENGINE_API extern INDEX snd_iFormat;
extern BOOL _bMouseUsedLast;

extern CMenuGadget *_pmgLastActivatedGadget;
extern CMenuGadget *_pmgUnderCursor;

static INDEX _ctAdapters = 0;
static CTString *_astrAdapterTexts = NULL;
static INDEX _ctResolutions = 0;
static CTString *_astrResolutionTexts = NULL;
static CDisplayMode *_admResolutionModes = NULL;

#define VOLUME_STEPS 100

// make description for a given resolution
static CTString GetResolutionDescription(CDisplayMode &dm) {
  CTString str;

  // if dual head
  if (dm.IsDualHead()) {
    str.PrintF(LOCALIZE("%dx%d double"), dm.dm_pixSizeI / 2, dm.dm_pixSizeJ);

  // if widescreen
  } else if (dm.IsWideScreen()) {
    str.PrintF(LOCALIZE("%dx%d wide"), dm.dm_pixSizeI, dm.dm_pixSizeJ);

  // otherwise it is normal
  } else {
    str.PrintF("%dx%d", dm.dm_pixSizeI, dm.dm_pixSizeJ);
  }

  // [Cecil] Matching the screen
  if (PIX2D(dm.dm_pixSizeI, dm.dm_pixSizeJ) == _vpixScreenRes) {
    str += TRANS(" (Native)");
  }

  return str;
}

// make description for a given resolution
static void SetResolutionInList(INDEX iRes, PIX2D vpixSize) {
  ASSERT(iRes >= 0 && iRes < _ctResolutions);

  CTString &str = _astrResolutionTexts[iRes];
  CDisplayMode &dm = _admResolutionModes[iRes];
  dm.dm_pixSizeI = vpixSize(1);
  dm.dm_pixSizeJ = vpixSize(2);
  str = GetResolutionDescription(dm);
}

static void ResolutionToSize(INDEX iRes, PIX2D &vpixSize) {
  ASSERT(iRes >= 0 && iRes < _ctResolutions);

  CDisplayMode &dm = _admResolutionModes[iRes];
  vpixSize = PIX2D(dm.dm_pixSizeI, dm.dm_pixSizeJ);
}

static void SizeToResolution(PIX2D vpixSize, INDEX &iRes) {
  for (iRes = 0; iRes < _ctResolutions; iRes++) {
    CDisplayMode &dm = _admResolutionModes[iRes];
    if (dm.dm_pixSizeI == vpixSize(1) && dm.dm_pixSizeJ == vpixSize(2)) {
      return;
    }
  }
  // if none was found, search for default
  for (iRes = 0; iRes < _ctResolutions; iRes++) {
    CDisplayMode &dm = _admResolutionModes[iRes];
    if (dm.dm_pixSizeI == 640 && dm.dm_pixSizeJ == 480) {
      return;
    }
  }
  // if still none found
  ASSERT(FALSE); // this should never happen
  // return first one
  iRes = 0;
}

// [Cecil] Set all resolutions of some aspect ration in list
static void SetAspectRatioResolutions(const CAspectRatio &arAspectRatio, INDEX &ctResCounter) {
  const INDEX ctResolutions = arAspectRatio.Count();

  for (INDEX iRes = 0; iRes < ctResolutions; iRes++) {
    const PIX2D &vpix = arAspectRatio[iRes];

    if (vpix(1) > _vpixScreenRes(1) || vpix(2) > _vpixScreenRes(2)) {
      continue; // [Cecil] Skip resolutions bigger than the screen
    }

    SetResolutionInList(ctResCounter++, vpix);
  }
};

// ------------------------ CConfirmMenu implementation
extern CTFileName _fnmModToLoad;
extern CTString _strModServerJoin;

CTFileName _fnmModSelected;
CTString _strModURLSelected;
CTString _strModServerSelected;

static void ExitGame(void) {
  _bRunning = FALSE;
  _bQuitScreen = TRUE;
}

static void ExitConfirm(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &ExitGame;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.SetText(LOCALIZE("ARE YOU SERIOUS?"));
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void StopCurrentGame(void) {
  _pGame->StopGame();
  _gmRunningGameMode = GM_NONE;

  StopMenus(TRUE);
  StartMenus("");
}

static void StopConfirm(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &StopCurrentGame;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.SetText(LOCALIZE("ARE YOU SERIOUS?"));
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void ModLoadYes(void) {
  _fnmModToLoad = _fnmModSelected;
}

static void ModConnect(void) {
  _fnmModToLoad = _fnmModSelected;
  _strModServerJoin = _strModServerSelected;
}

extern void ModConnectConfirm(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  if (_fnmModSelected == " ") {
    _fnmModSelected = CTString("SeriousSam");
  }

  CTFileName fnmModPath = "Mods\\" + _fnmModSelected + "\\";
  if (!FileExists(fnmModPath + "BaseWriteInclude.lst")
   && !FileExists(fnmModPath + "BaseWriteExclude.lst")
   && !FileExists(fnmModPath + "BaseBrowseInclude.lst")
   && !FileExists(fnmModPath + "BaseBrowseExclude.lst")) {
    extern void ModNotInstalled(void);
    ModNotInstalled();
    return;
  }

  CPrintF(LOCALIZE("Server is running a different MOD (%s).\nYou need to reload to connect.\n"), _fnmModSelected);
  gmCurrent._pConfimedYes = &ModConnect;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.SetText(LOCALIZE("CHANGE THE MOD?"));
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

void SaveConfirm(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  extern void OnFileSaveOK(void);
  gmCurrent._pConfimedYes = &OnFileSaveOK;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.SetText(LOCALIZE("OVERWRITE?"));
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

void ExitAndSpawnExplorer(void) {
  _bRunning = FALSE;
  _bQuitScreen = FALSE;
  extern CTString _strURLToVisit;
  _strURLToVisit = _strModURLSelected;
}

void ModNotInstalled(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &ExitAndSpawnExplorer;
  gmCurrent._pConfimedNo = NULL;

  CTString strNoMod;
  strNoMod.PrintF(LOCALIZE("You don't have MOD '%s' installed.\nDo you want to visit its web site?"), (const char *)_fnmModSelected);

  gmCurrent.SetText(strNoMod);
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.BeSmall();
  ChangeToMenu(&gmCurrent);
}

extern void ModConfirm(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &ModLoadYes;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.SetText(LOCALIZE("LOAD THIS MOD?"));
  gmCurrent.SetParentMenu(&_pGUIM->gmLoadSaveMenu);
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void RevertVideoSettings(void);

void VideoConfirm(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  // FIXUP: keyboard focus lost when going from full screen to window mode
  // due to WM_MOUSEMOVE being sent
  _bMouseUsedLast = FALSE;
  _pmgUnderCursor = gmCurrent.gm_pmgSelectedByDefault;

  gmCurrent._pConfimedYes = NULL;
  gmCurrent._pConfimedNo = RevertVideoSettings;

  gmCurrent.SetText(LOCALIZE("KEEP THIS SETTING?"));
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void ConfirmYes(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  if (gmCurrent._pConfimedYes != NULL) {
    gmCurrent._pConfimedYes();
  }
  MenuGoToParent();
}

static void ConfirmNo(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  if (gmCurrent._pConfimedNo != NULL) {
    gmCurrent._pConfimedNo();
  }
  MenuGoToParent();
}

void InitActionsForConfirmMenu() {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent.gm_mgConfirmYes.mg_pActivatedFunction = &ConfirmYes;
  gmCurrent.gm_mgConfirmNo.mg_pActivatedFunction = &ConfirmNo;
}

// ------------------------ CMainMenu implementation
void InitActionsForMainMenu() {
  CMainMenu &gmCurrent = _pGUIM->gmMainMenu;

  gmCurrent.gm_mgSingle.mg_pActivatedFunction = &StartSinglePlayerMenu;
  gmCurrent.gm_mgNetwork.mg_pActivatedFunction = &StartNetworkMenu;
  gmCurrent.gm_mgSplitScreen.mg_pActivatedFunction = &StartSplitScreenMenu;
  gmCurrent.gm_mgDemo.mg_pActivatedFunction = &StartDemoLoadMenu;
  gmCurrent.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
  gmCurrent.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

// ------------------------ CInGameMenu implementation
// start load/save menus depending on type of game running
static void QuickSaveFromMenu() {
  _pShell->SetINDEX("gam_bQuickSave", 2); // force save with reporting
  StopMenus(TRUE);
}

static void StopRecordingDemo(void) {
  _pNetwork->StopDemoRec();
  void SetDemoStartStopRecText(void);
  SetDemoStartStopRecText();
}

void InitActionsForInGameMenu() {
  CInGameMenu &gmCurrent = _pGUIM->gmInGameMenu;

  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartCurrentQuickLoadMenu;
  gmCurrent.gm_mgQuickSave.mg_pActivatedFunction = &QuickSaveFromMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartCurrentLoadMenu;
  gmCurrent.gm_mgSave.mg_pActivatedFunction = &StartCurrentSaveMenu;
  gmCurrent.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
  gmCurrent.gm_mgStop.mg_pActivatedFunction = &StopConfirm;
  gmCurrent.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

extern void SetDemoStartStopRecText(void) {
  CInGameMenu &gmCurrent = _pGUIM->gmInGameMenu;

  if (_pNetwork->IsRecordingDemo()) {
    gmCurrent.gm_mgDemoRec.SetText(LOCALIZE("STOP RECORDING"));
    gmCurrent.gm_mgDemoRec.mg_strTip = LOCALIZE("stop current recording");
    gmCurrent.gm_mgDemoRec.mg_pActivatedFunction = &StopRecordingDemo;
  } else {
    gmCurrent.gm_mgDemoRec.SetText(LOCALIZE("RECORD DEMO"));
    gmCurrent.gm_mgDemoRec.mg_strTip = LOCALIZE("start recording current game");
    gmCurrent.gm_mgDemoRec.mg_pActivatedFunction = &StartDemoSaveMenu;
  }
}

// ------------------------ CSinglePlayerMenu implementation
extern CTString sam_strTechTestLevel;
extern CTString sam_strTrainingLevel;

extern void StartSinglePlayerGame(void);
static void StartTechTest(void) {
  _pGUIM->gmSinglePlayerNewMenu.SetParentMenu(&_pGUIM->gmSinglePlayerMenu);
  GetGameAPI()->SetCustomLevel(sam_strTechTestLevel);

  // [Cecil] Use difficulties and game modes from the API
  _pShell->SetINDEX("gam_iStartDifficulty", ClassicsModData_GetDiff(2)->m_iLevel);
  _pShell->SetINDEX("gam_iStartMode", GetGameAPI()->GetGameMode(1));
  StartSinglePlayerGame();
}

static void StartTraining(void) {
  _pGUIM->gmSinglePlayerNewMenu.SetParentMenu(&_pGUIM->gmSinglePlayerMenu);
  GetGameAPI()->SetCustomLevel(sam_strTrainingLevel);
  ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

void InitActionsForSinglePlayerMenu() {
  CSinglePlayerMenu &gmCurrent = _pGUIM->gmSinglePlayerMenu;

  gmCurrent.gm_mgNewGame.mg_pActivatedFunction = &StartSinglePlayerNewMenu;
  gmCurrent.gm_mgCustom.mg_pActivatedFunction = &StartSelectLevelFromSingle;
  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartSinglePlayerQuickLoadMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartSinglePlayerLoadMenu;
  gmCurrent.gm_mgTraining.mg_pActivatedFunction = &StartTraining;
  gmCurrent.gm_mgTechTest.mg_pActivatedFunction = &StartTechTest;
  gmCurrent.gm_mgPlayersAndControls.mg_pActivatedFunction = &StartChangePlayerMenuFromSinglePlayer;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartSinglePlayerGameOptions;
}

// ------------------------ CSinglePlayerNewMenu implementation
void StartSinglePlayerGame(void) {
  GetGameAPI()->SetStartSplitCfg(CGame::SSC_PLAY1);

  // [Cecil] Reset start player indices
  GetGameAPI()->ResetStartProfiles();
  GetGameAPI()->SetProfileForStart(0, GetGameAPI()->GetProfileForSP());

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);

  // [Cecil] Pass byte container
  CSesPropsContainer sp;
  _pGame->SetSinglePlayerSession((CSessionProperties &)sp);

  // [Cecil] Start game through the API
  if (GetGameAPI()->NewGame(GetGameAPI()->GetCustomLevel(), GetGameAPI()->GetCustomLevel(), (CSessionProperties &)sp)) {
    StopMenus();
    _gmRunningGameMode = GM_SINGLE_PLAYER;

  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

// [Cecil] Start new game based on the selected difficulty button
static void StartSinglePlayerGameFromDifficulty(void) {
  CMGButton &mgDiff = (CMGButton &)*_pmgLastActivatedGadget;

  // Use difficulties and game modes from the API
  _pShell->SetINDEX("gam_iStartDifficulty", ClassicsModData_GetDiff(mgDiff.mg_iIndex)->m_iLevel);
  _pShell->SetINDEX("gam_iStartMode", GetGameAPI()->GetGameMode(1));

  StartSinglePlayerGame();
};

void InitActionsForSinglePlayerNewMenu() {
  CSinglePlayerNewMenu &gmCurrent = _pGUIM->gmSinglePlayerNewMenu;

  // [Cecil] Set selection function to all difficulties
  const INDEX ct = gmCurrent.gm_amgDifficulties.Count();

  for (INDEX i = 0; i < ct; i++) {
    gmCurrent.gm_amgDifficulties[i].mg_pActivatedFunction = &StartSinglePlayerGameFromDifficulty;
  }
}

// ------------------------ CPlayerProfileMenu implementation
static void ChangeCrosshair(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  pps->ps_iCrossHairType = iNew - 1;
}

static void ChangeWeaponSelect(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  pps->ps_iWeaponAutoSelect = iNew;
}

static void ChangeWeaponHide(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_HIDEWEAPON;
  } else {
    pps->ps_ulFlags &= ~PSF_HIDEWEAPON;
  }
}

static void Change3rdPerson(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_PREFER3RDPERSON;
  } else {
    pps->ps_ulFlags &= ~PSF_PREFER3RDPERSON;
  }
}

static void ChangeQuotes(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_NOQUOTES;
  } else {
    pps->ps_ulFlags |= PSF_NOQUOTES;
  }
}

static void ChangeAutoSave(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_AUTOSAVE;
  } else {
    pps->ps_ulFlags &= ~PSF_AUTOSAVE;
  }
}

static void ChangeCompDoubleClick(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_COMPSINGLECLICK;
  } else {
    pps->ps_ulFlags |= PSF_COMPSINGLECLICK;
  }
}

static void ChangeViewBobbing(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_NOBOBBING;
  } else {
    pps->ps_ulFlags |= PSF_NOBOBBING;
  }
}

static void ChangeSharpTurning(INDEX iNew) {
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)GetGameAPI()->GetPlayerCharacter(iPlayer)->pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_SHARPTURNING;
  } else {
    pps->ps_ulFlags &= ~PSF_SHARPTURNING;
  }
}

extern void PPOnPlayerSelect(void) {
  ASSERT(_pmgLastActivatedGadget != NULL);
  if (_pmgLastActivatedGadget->mg_bEnabled) {
    _pGUIM->gmPlayerProfile.SelectPlayer(((CMGButton *)_pmgLastActivatedGadget)->mg_iIndex);
  }
}

void InitActionsForPlayerProfileMenu() {
  CPlayerProfileMenu &gmCurrent = _pGUIM->gmPlayerProfile;

  gmCurrent.gm_mgCrosshair.mg_pOnTriggerChange = ChangeCrosshair;
  gmCurrent.gm_mgWeaponSelect.mg_pOnTriggerChange = ChangeWeaponSelect;
  gmCurrent.gm_mgWeaponHide.mg_pOnTriggerChange = ChangeWeaponHide;
  gmCurrent.gm_mg3rdPerson.mg_pOnTriggerChange = Change3rdPerson;
  gmCurrent.gm_mgQuotes.mg_pOnTriggerChange = ChangeQuotes;
  gmCurrent.gm_mgAutoSave.mg_pOnTriggerChange = ChangeAutoSave;
  gmCurrent.gm_mgCompDoubleClick.mg_pOnTriggerChange = ChangeCompDoubleClick;
  gmCurrent.gm_mgSharpTurning.mg_pOnTriggerChange = ChangeSharpTurning;
  gmCurrent.gm_mgViewBobbing.mg_pOnTriggerChange = ChangeViewBobbing;
  gmCurrent.gm_mgCustomizeControls.mg_pActivatedFunction = &StartControlsMenuFromPlayer;
  gmCurrent.gm_mgModel.mg_pActivatedFunction = &StartPlayerModelLoadMenu;
}

// ------------------------ CControlsMenu implementation
void InitActionsForControlsMenu() {
  CControlsMenu &gmCurrent = _pGUIM->gmControls;

  gmCurrent.gm_mgButtons.mg_pActivatedFunction = &StartCustomizeKeyboardMenu;
  gmCurrent.gm_mgAdvanced.mg_pActivatedFunction = &StartCustomizeAxisMenu;
  gmCurrent.gm_mgPredefined.mg_pActivatedFunction = &StartControlsLoadMenu;
  gmCurrent.gm_mgCommon.mg_pActivatedFunction = &StartCustomizeCommonControlsMenu; // [Cecil]
}

// ------------------------ CCustomizeAxisMenu implementation
void PreChangeAxis(INDEX iDummy) {
  _pGUIM->gmCustomizeAxisMenu.ApplyActionSettings();
}

void PostChangeAxis(INDEX iDummy) {
  _pGUIM->gmCustomizeAxisMenu.ObtainActionSettings();
}

void InitActionsForCustomizeAxisMenu() {
  CCustomizeAxisMenu &gmCurrent = _pGUIM->gmCustomizeAxisMenu;

  gmCurrent.gm_mgActionTrigger.mg_pPreTriggerChange = PreChangeAxis;
  gmCurrent.gm_mgActionTrigger.mg_pOnTriggerChange = PostChangeAxis;
}

// ------------------------ COptionsMenu implementation
void InitActionsForOptionsMenu() {
  COptionsMenu &gmCurrent = _pGUIM->gmOptionsMenu;

  gmCurrent.gm_mgVideoOptions.mg_pActivatedFunction = &StartVideoOptionsMenu;
  gmCurrent.gm_mgAudioOptions.mg_pActivatedFunction = &StartAudioOptionsMenu;
  gmCurrent.gm_mgPlayerProfileOptions.mg_pActivatedFunction = &StartChangePlayerMenuFromOptions;
  gmCurrent.gm_mgNetworkOptions.mg_pActivatedFunction = &StartNetworkSettingsMenu;
  gmCurrent.gm_mgPatchOptions.mg_pActivatedFunction = &StartPatchOptionsMenu; // [Cecil]
  gmCurrent.gm_mgCustomOptions.mg_pActivatedFunction = &StartCustomLoadMenu;
  gmCurrent.gm_mgAddonOptions.mg_pActivatedFunction = &StartAddonsLoadMenu;
}

// ------------------------ CVideoOptionsMenu implementation
static INDEX sam_old_iWindowMode; // [Cecil] Different window modes
static INDEX sam_old_iScreenSizeI;
static INDEX sam_old_iScreenSizeJ;
static INDEX sam_old_iDisplayDepth;
static INDEX sam_old_iDisplayAdapter;
static INDEX sam_old_iGfxAPI;
static INDEX sam_old_iVideoSetup; // 0 = speed, 1 = normal, 2 = quality, 3 = custom

static void FillResolutionsList(void) {
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  // free resolutions
  if (_astrResolutionTexts != NULL) {
    delete[] _astrResolutionTexts;
  }
  if (_admResolutionModes != NULL) {
    delete[] _admResolutionModes;
  }
  _ctResolutions = 0;

  // [Cecil] Select current aspect ratio
  const INDEX iAspectRatio = gmCurrent.gm_mgAspectRatiosTrigger.mg_iSelected;
  const CAspectRatio &ar = *_aAspectRatios[iAspectRatio];

  // [Cecil] If 4:3 in borderless or fullscreen
  if (iAspectRatio == 0 && gmCurrent.gm_mgWindowModeTrigger.mg_iSelected != E_WM_WINDOWED) {
    // Get resolutions from the engine
    INDEX ctEngineRes = 0;

    CDisplayMode *pdm = _pGfx->EnumDisplayModes(ctEngineRes,
      SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected), gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected);

    _astrResolutionTexts = new CTString[ctEngineRes];
    _admResolutionModes = new CDisplayMode[ctEngineRes];

    // Add all engine resolutions to the list
    for (INDEX iRes = 0; iRes < ctEngineRes; iRes++) {
      SetResolutionInList(iRes, PIX2D(pdm[iRes].dm_pixSizeI, pdm[iRes].dm_pixSizeJ));
    }

    // Remember current amount
    _ctResolutions = ctEngineRes;
    
  // [Cecil] If any other aspect ratio or windowed mode
  } else {
    // Amount of resolutions under this aspect ratio
    _ctResolutions = ar.Count();

    _astrResolutionTexts = new CTString[_ctResolutions];
    _admResolutionModes = new CDisplayMode[_ctResolutions];

    // Add all resolutions from the selected aspect ratio
    INDEX ctRes = 0;
    SetAspectRatioResolutions(ar, ctRes);

    _ctResolutions = ctRes;
  }

  gmCurrent.gm_mgResolutionsTrigger.mg_astrTexts = _astrResolutionTexts;
  gmCurrent.gm_mgResolutionsTrigger.mg_ctTexts = _ctResolutions;
}

static void FillAdaptersList(void) {
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  if (_astrAdapterTexts != NULL) {
    delete[] _astrAdapterTexts;
  }

  _ctAdapters = 0;

  INDEX iApi = SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected);
  _ctAdapters = _pGfx->gl_gaAPI[iApi].ga_ctAdapters;
  _astrAdapterTexts = new CTString[_ctAdapters];
  for (INDEX iAdapter = 0; iAdapter < _ctAdapters; iAdapter++) {
    _astrAdapterTexts[iAdapter] = _pGfx->gl_gaAPI[iApi].ga_adaAdapter[iAdapter].da_strRenderer;
  }

  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_astrTexts = _astrAdapterTexts;
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_ctTexts = _ctAdapters;
}

extern void UpdateVideoOptionsButtons(INDEX iSelected) {
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  const BOOL _bVideoOptionsChanged = (iSelected != -1);

  const BOOL bOGLEnabled = _pGfx->HasAPI(GAT_OGL);
#ifdef SE1_D3D
  const BOOL bD3DEnabled = _pGfx->HasAPI(GAT_D3D);
  ASSERT(bOGLEnabled || bD3DEnabled);
#else
  const BOOL bD3DEnabled = FALSE; // [Cecil] No D3D
  ASSERT(bOGLEnabled);
#endif // SE1_D3D

  CDisplayAdapter &da = _pGfx->gl_gaAPI[SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected)]
    .ga_adaAdapter[gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected];

  // number of available preferences is higher if video setup is custom
  gmCurrent.gm_mgDisplayPrefsTrigger.mg_ctTexts = _iDisplayPrefsLastOpt; // [Cecil] Excludes "Custom"

  // Include "Custom" if it's selected
  if (sam_iVideoSetup == _iDisplayPrefsLastOpt) {
    gmCurrent.gm_mgDisplayPrefsTrigger.mg_ctTexts++;
  }

  // enumerate adapters
  FillAdaptersList();

  // show or hide buttons
  gmCurrent.gm_mgDisplayAPITrigger.mg_bEnabled = bOGLEnabled && bD3DEnabled; // [Cecil] Check for D3D
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_bEnabled = _ctAdapters > 1;
  gmCurrent.gm_mgApply.mg_bEnabled = _bVideoOptionsChanged;

  // determine which should be visible
  gmCurrent.gm_mgWindowModeTrigger.mg_bEnabled = TRUE;

  if (da.da_ulFlags & DAF_FULLSCREENONLY) {
    gmCurrent.gm_mgWindowModeTrigger.mg_bEnabled = FALSE;
    gmCurrent.gm_mgWindowModeTrigger.mg_iSelected = E_WM_FULLSCREEN;
    gmCurrent.gm_mgWindowModeTrigger.ApplyCurrentSelection();
  }

  gmCurrent.gm_mgBitsPerPixelTrigger.mg_bEnabled = TRUE;

  // [Cecil] If not fullscreen
  if (gmCurrent.gm_mgWindowModeTrigger.mg_iSelected != E_WM_FULLSCREEN) {
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_DEFAULT);
    gmCurrent.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();

  } else if (da.da_ulFlags & DAF_16BITONLY) {
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_16BIT);
    gmCurrent.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
  }

  // remember current selected resolution
  PIX2D vpixSize;
  ResolutionToSize(gmCurrent.gm_mgResolutionsTrigger.mg_iSelected, vpixSize);

  // select same resolution again if possible
  FillResolutionsList();
  SizeToResolution(vpixSize, gmCurrent.gm_mgResolutionsTrigger.mg_iSelected);

  // apply adapter and resolutions
  gmCurrent.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgResolutionsTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgAspectRatiosTrigger.ApplyCurrentSelection(); // [Cecil]
}

extern void InitVideoOptionsButtons(void) {
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  // [Cecil] Limit to existing window modes
  INDEX iWindowMode = Clamp(sam_iWindowMode, (INDEX)E_WM_WINDOWED, (INDEX)E_WM_FULLSCREEN);
  gmCurrent.gm_mgWindowModeTrigger.mg_iSelected = iWindowMode;

  gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected = APIToSwitch((GfxAPIType)(INDEX)sam_iGfxAPI);
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected = sam_iDisplayAdapter;
  gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch((enum DisplayDepth)(INDEX)sam_iDisplayDepth);

  // [Cecil] Find aspect ratio and the resolution within it
  PIX2D vScreen(sam_iScreenSizeI, sam_iScreenSizeJ);
  SizeToAspectRatio(vScreen, gmCurrent.gm_mgAspectRatiosTrigger.mg_iSelected);

  FillResolutionsList();
  SizeToResolution(vScreen, gmCurrent.gm_mgResolutionsTrigger.mg_iSelected);

  gmCurrent.gm_mgDisplayPrefsTrigger.mg_iSelected = Clamp(sam_iVideoSetup, 0L, _iDisplayPrefsLastOpt);

  gmCurrent.gm_mgWindowModeTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgDisplayPrefsTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgDisplayAPITrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgResolutionsTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgAspectRatiosTrigger.ApplyCurrentSelection(); // [Cecil]
  gmCurrent.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
}

static void ApplyVideoOptions(void) {
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  // Remember old video settings
  sam_old_iWindowMode = sam_iWindowMode;
  sam_old_iScreenSizeI = sam_iScreenSizeI;
  sam_old_iScreenSizeJ = sam_iScreenSizeJ;
  sam_old_iDisplayDepth = sam_iDisplayDepth;
  sam_old_iDisplayAdapter = sam_iDisplayAdapter;
  sam_old_iGfxAPI = sam_iGfxAPI;
  sam_old_iVideoSetup = sam_iVideoSetup;

  // [Cecil] Different window modes
  INDEX iWindowMode = gmCurrent.gm_mgWindowModeTrigger.mg_iSelected;
  PIX2D vpixWindowSize;
  ResolutionToSize(gmCurrent.gm_mgResolutionsTrigger.mg_iSelected, vpixWindowSize);
  enum GfxAPIType gat = SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected);
  enum DisplayDepth dd = SwitchToDepth(gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected);
  const INDEX iAdapter = gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected;

  // setup preferences
  extern INDEX _iLastPreferences;
  if (sam_iVideoSetup == _iDisplayPrefsLastOpt) {
    _iLastPreferences = _iDisplayPrefsLastOpt;
  }
  sam_iVideoSetup = gmCurrent.gm_mgDisplayPrefsTrigger.mg_iSelected;

  // force fullscreen mode if needed
  CDisplayAdapter &da = _pGfx->gl_gaAPI[gat].ga_adaAdapter[iAdapter];

  if (da.da_ulFlags & DAF_FULLSCREENONLY) {
    iWindowMode = E_WM_FULLSCREEN;
  }
  if (da.da_ulFlags & DAF_16BITONLY) {
    dd = DD_16BIT;
  }

  // force window to always be in default colors
  if (iWindowMode != E_WM_FULLSCREEN) {
    dd = DD_DEFAULT;
  }

  // (try to) set mode
  StartNewMode(gat, iAdapter, vpixWindowSize(1), vpixWindowSize(2), dd, iWindowMode);

  // refresh buttons
  InitVideoOptionsButtons();
  UpdateVideoOptionsButtons(-1);

  // ask user to keep or restore
  if (iWindowMode == E_WM_FULLSCREEN) {
    VideoConfirm();
  }
}

static void RevertVideoSettings(void) {
  // restore previous variables
  sam_iWindowMode = sam_old_iWindowMode;
  sam_iScreenSizeI = sam_old_iScreenSizeI;
  sam_iScreenSizeJ = sam_old_iScreenSizeJ;
  sam_iDisplayDepth = sam_old_iDisplayDepth;
  sam_iDisplayAdapter = sam_old_iDisplayAdapter;
  sam_iGfxAPI = sam_old_iGfxAPI;
  sam_iVideoSetup = sam_old_iVideoSetup;

  // update the video mode
  extern void ApplyVideoMode(void);
  ApplyVideoMode();

  // refresh buttons
  InitVideoOptionsButtons();
  UpdateVideoOptionsButtons(-1);
}

void InitActionsForVideoOptionsMenu() {
  // [Cecil] Prepare arrays with window resolutions
  PrepareVideoResolutions();

  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  gmCurrent.gm_mgDisplayPrefsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgDisplayAPITrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgWindowModeTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgResolutionsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgAspectRatiosTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons; // [Cecil]
  gmCurrent.gm_mgBitsPerPixelTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgVideoRendering.mg_pActivatedFunction = &StartRenderingOptionsMenu;
  gmCurrent.gm_mgApply.mg_pActivatedFunction = &ApplyVideoOptions;
}

// ------------------------ CAudioOptionsMenu implementation
extern void RefreshSoundFormat(void) {
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  switch (_pSound->GetFormat()) {
    case CSoundLibrary::SF_NONE: {
      gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 0;
      break;
    }

    case CSoundLibrary::SF_11025_16: {
      gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 1;
      break;
    }

    case CSoundLibrary::SF_22050_16: {
      gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 2;
      break;
    }

    case CSoundLibrary::SF_44100_16: {
      gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 3;
      break;
    }

    default: gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 0;
  }

  gmCurrent.gm_mgAudioAutoTrigger.mg_iSelected = Clamp(sam_bAutoAdjustAudio, 0, 1);
  gmCurrent.gm_mgAudioAPITrigger.mg_iSelected = Clamp(_pShell->GetINDEX("snd_iInterface"), 0L, 2L);

  gmCurrent.gm_mgWaveVolume.mg_iMinPos = 0;
  gmCurrent.gm_mgWaveVolume.mg_iMaxPos = VOLUME_STEPS;
  gmCurrent.gm_mgWaveVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fSoundVolume") * VOLUME_STEPS + 0.5f);
  gmCurrent.gm_mgWaveVolume.ApplyCurrentPosition();

  gmCurrent.gm_mgMPEGVolume.mg_iMinPos = 0;
  gmCurrent.gm_mgMPEGVolume.mg_iMaxPos = VOLUME_STEPS;
  gmCurrent.gm_mgMPEGVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fMusicVolume") * VOLUME_STEPS + 0.5f);
  gmCurrent.gm_mgMPEGVolume.ApplyCurrentPosition();

  gmCurrent.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgAudioAPITrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgFrequencyTrigger.ApplyCurrentSelection();
}

static void ApplyAudioOptions(void) {
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  sam_bAutoAdjustAudio = gmCurrent.gm_mgAudioAutoTrigger.mg_iSelected;
  if (sam_bAutoAdjustAudio) {
    _pShell->Execute("include \"" SCRIPTS_ADDONS_DIR "SFX-AutoAdjust.ini\"");
  } else {
    _pShell->SetINDEX("snd_iInterface", gmCurrent.gm_mgAudioAPITrigger.mg_iSelected);

    switch (gmCurrent.gm_mgFrequencyTrigger.mg_iSelected) {
      case 0: {
        _pSound->SetFormat(CSoundLibrary::SF_NONE);
        break;
      }

      case 1: {
        _pSound->SetFormat(CSoundLibrary::SF_11025_16);
        break;
      }

      case 2: {
        _pSound->SetFormat(CSoundLibrary::SF_22050_16);
        break;
      }

      case 3: {
        _pSound->SetFormat(CSoundLibrary::SF_44100_16);
        break;
      }

      default: _pSound->SetFormat(CSoundLibrary::SF_NONE);
    }
  }

  RefreshSoundFormat();
  snd_iFormat = _pSound->GetFormat();
}

static void OnWaveVolumeChange(INDEX iCurPos) {
  _pShell->SetFLOAT("snd_fSoundVolume", iCurPos / FLOAT(VOLUME_STEPS));
}

static void WaveSliderChange(void) {
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  gmCurrent.gm_mgWaveVolume.mg_iCurPos -= 5;
  gmCurrent.gm_mgWaveVolume.ApplyCurrentPosition();
}

static void FrequencyTriggerChange(INDEX iDummy) {
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  sam_bAutoAdjustAudio = 0;
  gmCurrent.gm_mgAudioAutoTrigger.mg_iSelected = 0;
  gmCurrent.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
}

static void MPEGSliderChange(void) {
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  gmCurrent.gm_mgMPEGVolume.mg_iCurPos -= 5;
  gmCurrent.gm_mgMPEGVolume.ApplyCurrentPosition();
}

static void OnMPEGVolumeChange(INDEX iCurPos) {
  _pShell->SetFLOAT("snd_fMusicVolume", iCurPos / FLOAT(VOLUME_STEPS));
}

void InitActionsForAudioOptionsMenu() {
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  gmCurrent.gm_mgFrequencyTrigger.mg_pOnTriggerChange = FrequencyTriggerChange;
  gmCurrent.gm_mgWaveVolume.mg_pOnSliderChange = &OnWaveVolumeChange;
  gmCurrent.gm_mgWaveVolume.mg_pActivatedFunction = WaveSliderChange;
  gmCurrent.gm_mgMPEGVolume.mg_pOnSliderChange = &OnMPEGVolumeChange;
  gmCurrent.gm_mgMPEGVolume.mg_pActivatedFunction = MPEGSliderChange;
  gmCurrent.gm_mgApply.mg_pActivatedFunction = &ApplyAudioOptions;
}

// ------------------------ CVarMenu implementation
static void VarApply(void) {
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;
  gmCurrent.gm_bApplying = TRUE; // [Cecil] Applying changes

  FlushVarSettings(TRUE);
  gmCurrent.EndMenu();
  gmCurrent.StartMenu();
}

void InitActionsForVarMenu() {
  _pGUIM->gmVarMenu.gm_mgApply.mg_pActivatedFunction = &VarApply;
}

// ------------------------ CServersMenu implementation

extern CMGButton mgServerColumn[7];
extern CMGEdit mgServerFilter[7];

static void SortByColumn(int i) {
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  if (gmCurrent.gm_mgList.mg_iSort == i) {
    gmCurrent.gm_mgList.mg_bSortDown = !gmCurrent.gm_mgList.mg_bSortDown;
  } else {
    gmCurrent.gm_mgList.mg_bSortDown = FALSE;
  }
  gmCurrent.gm_mgList.mg_iSort = i;
}

static void SortByServer(void)  { SortByColumn(0); }
static void SortByMap(void)     { SortByColumn(1); }
static void SortByPing(void)    { SortByColumn(2); }
static void SortByPlayers(void) { SortByColumn(3); }
static void SortByGame(void)    { SortByColumn(4); }
static void SortByMod(void)     { SortByColumn(5); }
static void SortByVer(void)     { SortByColumn(6); }

extern void RefreshServerList(void) {
  // [Cecil] Own method
  IMasterServer::EnumSessions(_pGUIM->gmServersMenu.m_bInternet);
}

void RefreshServerListManually(void) {
  ChangeToMenu(&_pGUIM->gmServersMenu); // this refreshes the list and sets focuses
}

void InitActionsForServersMenu() {
  _pGUIM->gmServersMenu.gm_mgRefresh.mg_pActivatedFunction = &RefreshServerList;

  mgServerColumn[0].mg_pActivatedFunction = SortByServer;
  mgServerColumn[1].mg_pActivatedFunction = SortByMap;
  mgServerColumn[2].mg_pActivatedFunction = SortByPing;
  mgServerColumn[3].mg_pActivatedFunction = SortByPlayers;
  mgServerColumn[4].mg_pActivatedFunction = SortByGame;
  mgServerColumn[5].mg_pActivatedFunction = SortByMod;
  mgServerColumn[6].mg_pActivatedFunction = SortByVer;
}

// ------------------------ CNetworkMenu implementation
void InitActionsForNetworkMenu() {
  CNetworkMenu &gmCurrent = _pGUIM->gmNetworkMenu;

  gmCurrent.gm_mgJoin.mg_pActivatedFunction = &StartNetworkJoinMenu;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartNetworkStartMenu;
  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartNetworkQuickLoadMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartNetworkLoadMenu;
}

// ------------------------ CNetworkJoinMenu implementation
void InitActionsForNetworkJoinMenu() {
  CNetworkJoinMenu &gmCurrent = _pGUIM->gmNetworkJoinMenu;

  gmCurrent.gm_mgLAN.mg_pActivatedFunction = &StartSelectServerLAN;
  gmCurrent.gm_mgNET.mg_pActivatedFunction = &StartSelectServerNET;
  gmCurrent.gm_mgOpen.mg_pActivatedFunction = &StartNetworkOpenMenu;
}

// ------------------------ CNetworkStartMenu implementation
extern void UpdateNetworkLevel(INDEX iDummy) {
  const INDEX iGameType = _pGUIM->gmNetworkStartMenu.gm_mgGameType.mg_iSelected;
  const ULONG ulFlags = GetGameAPI()->GetSpawnFlagsForGameTypeSS(iGameType);
  ValidateLevelForFlags(ulFlags);

  _pGUIM->gmNetworkStartMenu.gm_mgLevel.SetText(FindLevelByFileName(GetGameAPI()->GetCustomLevel()).li_strName);
}

void InitActionsForNetworkStartMenu() {
  CNetworkStartMenu &gmCurrent = _pGUIM->gmNetworkStartMenu;

  gmCurrent.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromNetwork;
  gmCurrent.gm_mgGameOptions.mg_pActivatedFunction = &StartGameOptionsFromNetwork;
  gmCurrent.gm_mgPatchOptions.mg_pActivatedFunction = &StartPatchServerOptionsFromNetwork; // [Cecil]
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromNetwork;
}

// ------------------------ CSelectPlayersMenu implementation
static INDEX FindUnusedPlayer(void) {
  INDEX *ai = GetGameAPI()->aiMenuLocalPlayers;
  INDEX iPlayer = 0;
  for (; iPlayer < GetGameAPI()->GetProfileCount(); iPlayer++) {
    BOOL bUsed = FALSE;
    for (INDEX iLocal = 0; iLocal < MAX_GAME_LOCAL_PLAYERS; iLocal++) {
      if (ai[iLocal] == iPlayer) {
        bUsed = TRUE;
        break;
      }
    }
    if (!bUsed) {
      return iPlayer;
    }
  }
  ASSERT(FALSE);
  return iPlayer;
}

extern void SelectPlayersFillMenu(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  INDEX *ai = GetGameAPI()->aiMenuLocalPlayers;

  gmCurrent.gm_mgPlayer0Change.mg_iLocalPlayer = 0;
  gmCurrent.gm_mgPlayer1Change.mg_iLocalPlayer = 1;
  gmCurrent.gm_mgPlayer2Change.mg_iLocalPlayer = 2;
  gmCurrent.gm_mgPlayer3Change.mg_iLocalPlayer = 3;

  // [Cecil] Determine configuration via flags
  BOOL bDedicated = (gmCurrent.gm_ulConfigFlags & PLCF_DEDICATED);
  BOOL bObserving = (gmCurrent.gm_ulConfigFlags & PLCF_OBSERVING);

  if (bDedicated && GetGameAPI()->GetMenuSplitCfg() == CGame::SSC_DEDICATED) {
    gmCurrent.gm_mgDedicated.mg_iSelected = 1;
  } else {
    gmCurrent.gm_mgDedicated.mg_iSelected = 0;
  }

  gmCurrent.gm_mgDedicated.ApplyCurrentSelection();

  if (bObserving && GetGameAPI()->GetMenuSplitCfg() == CGame::SSC_OBSERVER) {
    gmCurrent.gm_mgObserver.mg_iSelected = 1;
  } else {
    gmCurrent.gm_mgObserver.mg_iSelected = 0;
  }

  gmCurrent.gm_mgObserver.ApplyCurrentSelection();

  if (GetGameAPI()->GetMenuSplitCfg() >= CGame::SSC_PLAY1) {
    gmCurrent.gm_mgSplitScreenCfg.mg_iSelected = GetGameAPI()->GetMenuSplitCfg();
    gmCurrent.gm_mgSplitScreenCfg.ApplyCurrentSelection();
  }

  BOOL bHasPlayers = TRUE;

  if (bDedicated && gmCurrent.gm_mgDedicated.mg_iSelected) {
    bObserving = FALSE;
    bHasPlayers = FALSE;
  }

  if (bObserving && gmCurrent.gm_mgObserver.mg_iSelected) {
    bHasPlayers = FALSE;
  }

  CMenuGadget *apmg[9];
  memset(apmg, 0, sizeof(apmg));
  INDEX i = 0;

  // [Cecil] Hide password field by default
  gmCurrent.gm_mgPassword.Disappear();

  if (bDedicated) {
    gmCurrent.gm_mgDedicated.Appear();
    apmg[i++] = &gmCurrent.gm_mgDedicated;
  } else {
    gmCurrent.gm_mgDedicated.Disappear();

    // [Cecil] Replace dedicated switch with a password field
    if (gmCurrent.gm_ulConfigFlags & PLCF_PASSWORD) {
      gmCurrent.gm_mgPassword.Appear();
      apmg[i++] = &gmCurrent.gm_mgPassword;
    }
  }

  if (bObserving) {
    gmCurrent.gm_mgObserver.Appear();
    apmg[i++] = &gmCurrent.gm_mgObserver;
  } else {
    gmCurrent.gm_mgObserver.Disappear();
  }

  for (INDEX iLocal = 0; iLocal < MAX_GAME_LOCAL_PLAYERS; iLocal++) {
    if (ai[iLocal] < 0 || ai[iLocal] >= GetGameAPI()->GetProfileCount()) {
      ai[iLocal] = 0;
    }
    for (INDEX iCopy = 0; iCopy < iLocal; iCopy++) {
      if (ai[iCopy] == ai[iLocal]) {
        ai[iLocal] = FindUnusedPlayer();
      }
    }
  }

  gmCurrent.gm_mgPlayer0Change.Disappear();
  gmCurrent.gm_mgPlayer1Change.Disappear();
  gmCurrent.gm_mgPlayer2Change.Disappear();
  gmCurrent.gm_mgPlayer3Change.Disappear();

  if (bHasPlayers) {
    gmCurrent.gm_mgSplitScreenCfg.Appear();
    apmg[i++] = &gmCurrent.gm_mgSplitScreenCfg;
    gmCurrent.gm_mgPlayer0Change.Appear();
    apmg[i++] = &gmCurrent.gm_mgPlayer0Change;
    if (gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
      gmCurrent.gm_mgPlayer1Change.Appear();
      apmg[i++] = &gmCurrent.gm_mgPlayer1Change;
    }
    if (gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 2) {
      gmCurrent.gm_mgPlayer2Change.Appear();
      apmg[i++] = &gmCurrent.gm_mgPlayer2Change;
    }
    if (gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 3) {
      gmCurrent.gm_mgPlayer3Change.Appear();
      apmg[i++] = &gmCurrent.gm_mgPlayer3Change;
    }
  } else {
    gmCurrent.gm_mgSplitScreenCfg.Disappear();
  }
  apmg[i++] = &gmCurrent.gm_mgStart;

  // relink
  for (INDEX img = 0; img < GetGameAPI()->GetProfileCount(); img++) {
    if (apmg[img] == NULL) {
      continue;
    }
    INDEX imgPred = (img + 8 - 1) % 8;
    for (; imgPred != img; imgPred = (imgPred + 8 - 1) % 8) {
      if (apmg[imgPred] != NULL) {
        break;
      }
    }
    INDEX imgSucc = (img + 1) % 8;
    for (; imgSucc != img; imgSucc = (imgSucc + 1) % 8) {
      if (apmg[imgSucc] != NULL) {
        break;
      }
    }
    apmg[img]->mg_pmgUp = apmg[imgPred];
    apmg[img]->mg_pmgDown = apmg[imgSucc];
  }

  gmCurrent.gm_mgPlayer0Change.SetPlayerText();
  gmCurrent.gm_mgPlayer1Change.SetPlayerText();
  gmCurrent.gm_mgPlayer2Change.SetPlayerText();
  gmCurrent.gm_mgPlayer3Change.SetPlayerText();

  if (bHasPlayers && gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
    gmCurrent.gm_mgNotes.SetText(LOCALIZE("Make sure you set different controls for each player!"));
  } else {
    gmCurrent.gm_mgNotes.SetText("");
  }
}

extern void SelectPlayersApplyMenu(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  // [Cecil] Determine configuration via flags
  const BOOL bDedicated = (gmCurrent.gm_ulConfigFlags & PLCF_DEDICATED);
  const BOOL bObserving = (gmCurrent.gm_ulConfigFlags & PLCF_OBSERVING);

  if (bDedicated && gmCurrent.gm_mgDedicated.mg_iSelected) {
    GetGameAPI()->SetMenuSplitCfg(CGame::SSC_DEDICATED);
    return;
  }

  if (bObserving && gmCurrent.gm_mgObserver.mg_iSelected) {
    GetGameAPI()->SetMenuSplitCfg(CGame::SSC_OBSERVER);
    return;
  }

  GetGameAPI()->SetMenuSplitCfg(gmCurrent.gm_mgSplitScreenCfg.mg_iSelected);
}

static void UpdateSelectPlayers(INDEX i) {
  SelectPlayersApplyMenu();
  SelectPlayersFillMenu();
}

void InitActionsForSelectPlayersMenu() {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_mgDedicated.mg_pOnTriggerChange = UpdateSelectPlayers;
  gmCurrent.gm_mgObserver.mg_pOnTriggerChange = UpdateSelectPlayers;
  gmCurrent.gm_mgSplitScreenCfg.mg_pOnTriggerChange = UpdateSelectPlayers;
}

// ------------------------ CNetworkOpenMenu implementation
void InitActionsForNetworkOpenMenu() {
  _pGUIM->gmNetworkOpenMenu.gm_mgJoin.mg_pActivatedFunction = &StartSelectPlayersMenuFromOpen;
}

// ------------------------ CSplitScreenMenu implementation
void InitActionsForSplitScreenMenu() {
  CSplitScreenMenu &gmCurrent = _pGUIM->gmSplitScreenMenu;

  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSplitStartMenu;
  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartSplitScreenQuickLoadMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartSplitScreenLoadMenu;
}

// ------------------------ CSplitStartMenu implementation
void InitActionsForSplitStartMenu() {
  CSplitStartMenu &gmCurrent = _pGUIM->gmSplitStartMenu;

  gmCurrent.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromSplit;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartGameOptionsFromSplitScreen;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromSplit;
}

extern void UpdateSplitLevel(INDEX iDummy) {
  CSplitStartMenu &gmCurrent = _pGUIM->gmSplitStartMenu;

  const INDEX iGameType = gmCurrent.gm_mgGameType.mg_iSelected;
  const ULONG ulFlags = GetGameAPI()->GetSpawnFlagsForGameTypeSS(iGameType);
  ValidateLevelForFlags(ulFlags);

  gmCurrent.gm_mgLevel.SetText(FindLevelByFileName(GetGameAPI()->GetCustomLevel()).li_strName);
}