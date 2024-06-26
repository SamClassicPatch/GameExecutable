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

// This file contains starter fuctions for all menus.

#include "StdH.h"

#include "MenuManager.h"
#include "MenuStarters.h"
#include "MenuStartersAF.h"
#include "MenuStuff.h"
#include "LevelInfo.h"

extern void (*_pAfterLevelChosen)(void);
extern BOOL _bPlayerMenuFromSinglePlayer;

extern CTString _strLastPlayerAppearance;
extern CTString sam_strNetworkSettings;

extern CTFileName _fnmModSelected;
extern CTString _strModURLSelected;
extern CTString _strModServerSelected;

void StartVideoOptionsMenu(void) {
  ChangeToMenu(&_pGUIM->gmVideoOptionsMenu);
}

void StartAudioOptionsMenu(void) {
  ChangeToMenu(&_pGUIM->gmAudioOptionsMenu);
}

void StartSinglePlayerMenu(void) {
  ChangeToMenu(&_pGUIM->gmSinglePlayerMenu);
}

void StartNetworkMenu(void) {
  ChangeToMenu(&_pGUIM->gmNetworkMenu);
}

void StartNetworkJoinMenu(void) {
  ChangeToMenu(&_pGUIM->gmNetworkJoinMenu);
}

void StartNetworkStartMenu(void) {
  ChangeToMenu(&_pGUIM->gmNetworkStartMenu);
}

void StartNetworkOpenMenu(void) {
  ChangeToMenu(&_pGUIM->gmNetworkOpenMenu);
}

void StartSplitScreenMenu(void) {
  ChangeToMenu(&_pGUIM->gmSplitScreenMenu);
}

void StartSplitStartMenu(void) {
  ChangeToMenu(&_pGUIM->gmSplitStartMenu);
}

void StartSinglePlayerNewMenuCustom(void) {
  _pGUIM->gmSinglePlayerNewMenu.SetParentMenu(&_pGUIM->gmLevelsMenu);
  ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

static void SetQuickLoadNotes(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_pShell->GetINDEX("gam_iQuickSaveSlots") <= 8) {
    gmCurrent.gm_mgNotes.SetText(LOCALIZE(
      "In-game QuickSave shortcuts:\n"
      "F6 - save a new QuickSave\n"
      "F9 - load the last QuickSave\n"));
  } else {
    gmCurrent.gm_mgNotes.SetText("");
  }
}

void StartSinglePlayerNewMenu(void) {
  CSinglePlayerNewMenu &gmCurrent = _pGUIM->gmSinglePlayerNewMenu;

  GetGameAPI()->SetCustomLevel(sam_strFirstLevel);

  gmCurrent.SetParentMenu(&_pGUIM->gmSinglePlayerMenu);
  ChangeToMenu(&gmCurrent);
}

// [Cecil] Extra arguments for customization
void StartVarGameOptions(const CTString &strTitle, const CTFileName &fnmConfig, CGameMenu *pgmParentMenu) {
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;
  gmCurrent.SetParentMenu(pgmParentMenu); // [Cecil]

  gmCurrent.gm_mgTitle.SetName(strTitle);
  gmCurrent.gm_fnmMenuCFG = fnmConfig;
  ChangeToMenu(&gmCurrent);
};

void StartSinglePlayerGameOptions(void) {
  // [Cecil] More customization
  DECLARE_CTFILENAME(fnmConfig, "Scripts\\Menu\\SPOptions.cfg");
  StartVarGameOptions(LOCALIZE("GAME OPTIONS"), fnmConfig, &_pGUIM->gmSinglePlayerMenu);
};

void StartGameOptionsFromNetwork(void) {
  // [Cecil] More customization
  DECLARE_CTFILENAME(fnmConfig, "Scripts\\Menu\\GameOptions.cfg");
  StartVarGameOptions(LOCALIZE("GAME OPTIONS"), fnmConfig, &_pGUIM->gmNetworkStartMenu);
};

// [Cecil] Open server settings from the patch
void StartPatchServerOptionsFromNetwork(void) {
  DECLARE_CTFILENAME(fnmConfig, "Scripts\\ClassicsPatch\\02_ServerSettings.cfg");
  StartVarGameOptions(TRANS("SERVER OPTIONS"), fnmConfig, &_pGUIM->gmNetworkStartMenu);
};

void StartGameOptionsFromSplitScreen(void) {
  // [Cecil] More customization
  DECLARE_CTFILENAME(fnmConfig, "Scripts\\Menu\\GameOptions.cfg");
  StartVarGameOptions(LOCALIZE("GAME OPTIONS"), fnmConfig, &_pGUIM->gmSplitStartMenu);
};

// rendering options var settings
void StartRenderingOptionsMenu(void) {
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("RENDERING OPTIONS"));
  gmCurrent.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\RenderingOptions.cfg");
  gmCurrent.SetParentMenu(&_pGUIM->gmVideoOptionsMenu);
  ChangeToMenu(&gmCurrent);
}

void StartCustomizeKeyboardMenu(void) {
  // [Cecil] Set extra controls for editing player controls
  _pGUIM->gmCustomizeKeyboardMenu.SetControls(GetGameAPI()->GetControls());
  ChangeToMenu(&_pGUIM->gmCustomizeKeyboardMenu);
}

// [Cecil] Start customization of common controls
void StartCustomizeCommonControlsMenu(void) {
  _pGUIM->gmCustomizeKeyboardMenu.SetControls(GetGameAPI()->pctrlCommon);
  ChangeToMenu(&_pGUIM->gmCustomizeKeyboardMenu);
};

void StartCustomizeAxisMenu(void) {
  ChangeToMenu(&_pGUIM->gmCustomizeAxisMenu);
}

void StartOptionsMenu(void) {
  _pGUIM->gmOptionsMenu.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&_pGUIM->gmOptionsMenu);
}

void StartCurrentLoadMenu() {
  if (_gmRunningGameMode == GM_NETWORK) {
    void StartNetworkLoadMenu(void);
    StartNetworkLoadMenu();
  } else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
    void StartSplitScreenLoadMenu(void);
    StartSplitScreenLoadMenu();
  } else {
    void StartSinglePlayerLoadMenu(void);
    StartSinglePlayerLoadMenu();
  }
}

void StartCurrentSaveMenu() {
  if (_gmRunningGameMode == GM_NETWORK) {
    void StartNetworkSaveMenu(void);
    StartNetworkSaveMenu();
  } else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
    void StartSplitScreenSaveMenu(void);
    StartSplitScreenSaveMenu();
  } else {
    void StartSinglePlayerSaveMenu(void);
    StartSinglePlayerSaveMenu();
  }
}

void StartCurrentQuickLoadMenu() {
  if (_gmRunningGameMode == GM_NETWORK) {
    void StartNetworkQuickLoadMenu(void);
    StartNetworkQuickLoadMenu();
  } else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
    void StartSplitScreenQuickLoadMenu(void);
    StartSplitScreenQuickLoadMenu();
  } else {
    void StartSinglePlayerQuickLoadMenu(void);
    StartSinglePlayerQuickLoadMenu();
  }
}

void StartChangePlayerMenuFromOptions(void) {
  _bPlayerMenuFromSinglePlayer = FALSE;
  _pGUIM->gmPlayerProfile.gm_piCurrentPlayer = GetGameAPI()->piSinglePlayer;
  _pGUIM->gmPlayerProfile.SetParentMenu(&_pGUIM->gmOptionsMenu);
  ChangeToMenu(&_pGUIM->gmPlayerProfile);
}

void StartChangePlayerMenuFromSinglePlayer(void) {
  _iLocalPlayer = -1;
  _bPlayerMenuFromSinglePlayer = TRUE;
  _pGUIM->gmPlayerProfile.gm_piCurrentPlayer = GetGameAPI()->piSinglePlayer;
  _pGUIM->gmPlayerProfile.SetParentMenu(&_pGUIM->gmSinglePlayerMenu);
  ChangeToMenu(&_pGUIM->gmPlayerProfile);
}

void StartControlsMenuFromPlayer(void) {
  _pGUIM->gmControls.SetParentMenu(&_pGUIM->gmPlayerProfile);
  ChangeToMenu(&_pGUIM->gmControls);
}

void StartControlsMenuFromOptions(void) {
  _pGUIM->gmControls.SetParentMenu(&_pGUIM->gmOptionsMenu);
  ChangeToMenu(&_pGUIM->gmControls);
}

void StartHighScoreMenu(void) {
  _pGUIM->gmHighScoreMenu.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&_pGUIM->gmHighScoreMenu);
}

void StartSplitScreenGame(void) {
  GetGameAPI()->SetStartSplitCfg(GetGameAPI()->GetMenuSplitCfg());

  // [Cecil] Set start players from menu players
  GetGameAPI()->SetStartProfilesFromMenuProfiles();

  CTFileName fnWorld = GetGameAPI()->GetCustomLevel();

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);

  // [Cecil] Pass byte container
  CSesPropsContainer sp;
  _pGame->SetMultiPlayerSession((CSessionProperties &)sp);

  // [Cecil] Start game through the API
  if (GetGameAPI()->NewGame(fnWorld.FileName(), fnWorld, (CSessionProperties &)sp)) {
    StopMenus();
    _gmRunningGameMode = GM_SPLIT_SCREEN;

  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void StartNetworkGame(void) {
  GetGameAPI()->SetStartSplitCfg(GetGameAPI()->GetMenuSplitCfg());

  // [Cecil] Set start players from menu players
  GetGameAPI()->SetStartProfilesFromMenuProfiles();

  CTFileName fnWorld = GetGameAPI()->GetCustomLevel();

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_SERVER);
  
  // [Cecil] Pass byte container
  CSesPropsContainer sp;
  _pGame->SetMultiPlayerSession((CSessionProperties &)sp);

  // [Cecil] Start game through the API
  if (GetGameAPI()->NewGame(GetGameAPI()->SessionName(), fnWorld, (CSessionProperties &)sp)) {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;

    // if starting a dedicated server
    if (GetGameAPI()->GetMenuSplitCfg() == CGame::SSC_DEDICATED) {
      // pull down the console
      extern INDEX sam_bToggleConsole;
      sam_bToggleConsole = TRUE;
    }
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void JoinNetworkGame(void) {
  GetGameAPI()->SetStartSplitCfg(GetGameAPI()->GetMenuSplitCfg());

  // [Cecil] Set start players from menu players
  GetGameAPI()->SetStartProfilesFromMenuProfiles();

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_CLIENT);
  if (_pGame->JoinGame(CNetworkSession(GetGameAPI()->JoinAddress()))) {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;
  } else {
    if (_pNetwork->ga_strRequiredMod != "") {
      extern CTString _strModServerJoin;
      char strModName[256] = {0};
      char strModURL[256] = {0};
      _pNetwork->ga_strRequiredMod.ScanF("%250[^\\]\\%s", &strModName, &strModURL);
      _fnmModSelected = CTString(strModName);
      _strModURLSelected = strModURL;
      if (_strModURLSelected == "") {
        _strModURLSelected = "http://www.croteam.com/mods/Old";
      }
      _strModServerSelected.PrintF("%s:%s", GetGameAPI()->JoinAddress(), _pShell->GetValue("net_iPort"));
      extern void ModConnectConfirm(void);
      ModConnectConfirm();
    }
    _gmRunningGameMode = GM_NONE;
  }
}

// -------- Servers Menu Functions
void StartSelectServerLAN(void) {
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  gmCurrent.m_bInternet = FALSE;
  ChangeToMenu(&gmCurrent);
  gmCurrent.SetParentMenu(&_pGUIM->gmNetworkJoinMenu);
}

void StartSelectServerNET(void) {
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  gmCurrent.m_bInternet = TRUE;
  ChangeToMenu(&gmCurrent);
  gmCurrent.SetParentMenu(&_pGUIM->gmNetworkJoinMenu);
}

// -------- Levels Menu Functions

// [Cecil] Open level or category selection screen
static void StartSelectLevel(ULONG ulFlags, void (*pAfterChosen)(void), CGameMenu *pgmParent) {
  // Set levels with appropriate gamemode flags
  _pGUIM->gmLevelsMenu.gm_ulSpawnFlags = ulFlags;

  // Go to level categories, if there are any
  CGameMenu *pgmCurrent = &_pGUIM->gmLevelsMenu;

  if (sam_bLevelCategories && _aLevelCategories.Count() != 0) {
    pgmCurrent = &_pGUIM->gmLevelCategories;
  }

  _pAfterLevelChosen = pAfterChosen;

  pgmCurrent->SetParentMenu(pgmParent);
  ChangeToMenu(pgmCurrent);
};

void StartSelectLevelFromSingle(void) {
  // [Cecil] Select singleplayer levels
  StartSelectLevel(SPF_SINGLEPLAYER, &StartSinglePlayerNewMenuCustom, &_pGUIM->gmSinglePlayerMenu);
};

void StartSelectLevelFromSplit(void) {
  const INDEX iGameType = _pGUIM->gmSplitStartMenu.gm_mgGameType.mg_iSelected;
  const ULONG ulFlags = GetGameAPI()->GetSpawnFlagsForGameTypeSS(iGameType);

  // [Cecil] Select multiplayer levels
  extern void StartSplitStartMenu(void);
  StartSelectLevel(ulFlags, &StartSplitStartMenu, &_pGUIM->gmSplitStartMenu);
};

void StartSelectLevelFromNetwork(void) {
  const INDEX iGameType = _pGUIM->gmNetworkStartMenu.gm_mgGameType.mg_iSelected;
  const ULONG ulFlags = GetGameAPI()->GetSpawnFlagsForGameTypeSS(iGameType);

  // [Cecil] Select multiplayer levels
  extern void StartNetworkStartMenu(void);
  StartSelectLevel(ulFlags, &StartNetworkStartMenu, &_pGUIM->gmNetworkStartMenu);
};

// [Cecil] After choosing a level category
void StartSelectLevelFromCategory(INDEX iCategory) {
  CLevelsMenu &gmCurrent = _pGUIM->gmLevelsMenu;
  gmCurrent.gm_iCategory = iCategory;

  gmCurrent.SetParentMenu(&_pGUIM->gmLevelCategories);
  ChangeToMenu(&gmCurrent);
};

// -------- Players Selection Menu Functions
void StartSelectPlayersMenuFromSplit(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_ulConfigFlags = 0;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
  gmCurrent.SetParentMenu(&_pGUIM->gmSplitStartMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromNetwork(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_ulConfigFlags = PLCF_DEDICATED | PLCF_OBSERVING;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartNetworkGame;
  gmCurrent.SetParentMenu(&_pGUIM->gmNetworkStartMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromNetworkLoad(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_ulConfigFlags = PLCF_OBSERVING;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartNetworkLoadGame;
  gmCurrent.SetParentMenu(&_pGUIM->gmLoadSaveMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromSplitScreenLoad(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_ulConfigFlags = 0;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGameLoad;
  gmCurrent.SetParentMenu(&_pGUIM->gmLoadSaveMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromOpen(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_ulConfigFlags = PLCF_OBSERVING | PLCF_PASSWORD;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
  gmCurrent.SetParentMenu(&_pGUIM->gmNetworkOpenMenu);
  ChangeToMenu(&gmCurrent);

  /*if (sam_strNetworkSettings == "")*/ {
    void StartNetworkSettingsMenu(void);
    StartNetworkSettingsMenu();
    _pGUIM->gmLoadSaveMenu.gm_bNoEscape = TRUE;
    _pGUIM->gmLoadSaveMenu.SetParentMenu(&_pGUIM->gmNetworkOpenMenu);
    _pGUIM->gmLoadSaveMenu.gm_pgmNextMenu = &gmCurrent;
  }
}

void StartSelectPlayersMenuFromServers(void) {
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_ulConfigFlags = PLCF_OBSERVING | PLCF_PASSWORD;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
  gmCurrent.SetParentMenu(&_pGUIM->gmServersMenu);
  ChangeToMenu(&gmCurrent);

  /*if (sam_strNetworkSettings == "")*/ {
    void StartNetworkSettingsMenu(void);
    StartNetworkSettingsMenu();
    _pGUIM->gmLoadSaveMenu.gm_bNoEscape = TRUE;
    _pGUIM->gmLoadSaveMenu.SetParentMenu(&_pGUIM->gmServersMenu);
    _pGUIM->gmLoadSaveMenu.gm_pgmNextMenu = &gmCurrent;
  }
}

// -------- Save/Load Menu Calling Functions
void StartPlayerModelLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("CHOOSE MODEL"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Models\\Player\\");
  gmCurrent.gm_strSelected = _strLastPlayerAppearance;
  gmCurrent.gm_fnmExt = CTString(".amc");
  gmCurrent.gm_pAfterFileChosen = &LSLoadPlayerModel;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(&_pGUIM->gmPlayerProfile);
  ChangeToMenu(&gmCurrent);
}

void StartControlsLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("LOAD CONTROLS"));
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Controls\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".ctl");
  gmCurrent.gm_pAfterFileChosen = &LSLoadControls;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(&_pGUIM->gmControls);
  ChangeToMenu(&gmCurrent);
}

void StartCustomLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("ADVANCED OPTIONS"));
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_NAMEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Scripts\\CustomOptions\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".cfg");
  gmCurrent.gm_pAfterFileChosen = &LSLoadCustom;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(&_pGUIM->gmOptionsMenu);
  ChangeToMenu(&gmCurrent);
}

void StartAddonsLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("EXECUTE ADDON"));
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_NAMEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString(SCRIPTS_ADDONS_DIR);
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".ini");
  gmCurrent.gm_pAfterFileChosen = &LSLoadAddon;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(&_pGUIM->gmOptionsMenu);
  ChangeToMenu(&gmCurrent);
}

void StartModsLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("CHOOSE MOD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_NAMEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Mods\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".des");
  gmCurrent.gm_pAfterFileChosen = &LSLoadMod;

  // [Cecil] Set current menu as the parent menu
  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

// [Cecil] Open extras menu
void StartExtrasMenu(void) {
  ChangeToMenu(&_pGUIM->gmExtras);
};

// [Cecil] Open credits menu
void StartPatchCreditsMenu(void) {
  ChangeToMenu(&_pGUIM->gmPatchCredits);
};

void StartNetworkSettingsMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("CONNECTION SETTINGS"));
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Scripts\\NetSettings\\");
  gmCurrent.gm_strSelected = sam_strNetworkSettings;
  gmCurrent.gm_fnmExt = CTString(".ini");
  gmCurrent.gm_pAfterFileChosen = &LSLoadNetSettings;

  if (sam_strNetworkSettings == "") {
    gmCurrent.gm_mgNotes.SetText(LOCALIZE(
      "Before joining a network game,\n"
      "you have to adjust your connection parameters.\n"
      "Choose one option from the list.\n"
      "If you have problems with connection, you can adjust\n"
      "these parameters again from the Options menu.\n"));
  } else {
    gmCurrent.gm_mgNotes.SetText("");
  }

  gmCurrent.SetParentMenu(&_pGUIM->gmOptionsMenu);
  ChangeToMenu(&gmCurrent);
}

// [Cecil] Open list of option configs from the patch
void StartPatchOptionsMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.SetName(TRANS("CLASSICS PATCH"));
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Scripts\\ClassicsPatch\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".cfg");
  gmCurrent.gm_pAfterFileChosen = &LSLoadPatchConfig;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(&_pGUIM->gmOptionsMenu);
  ChangeToMenu(&gmCurrent);
};

void StartSinglePlayerQuickLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SINGLE_PLAYER;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("QUICK LOAD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\Quick\\", GetGameAPI()->GetProfileForSP());
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSinglePlayer;
  SetQuickLoadNotes();

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSinglePlayerLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SINGLE_PLAYER;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("LOAD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", GetGameAPI()->GetProfileForSP());
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSinglePlayer;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSinglePlayerSaveMenu(void) {
  if (_gmRunningGameMode != GM_SINGLE_PLAYER) {
    return;
  }

  // if no live players
  if (_pGame->GetPlayersCount() > 0 && _pGame->GetLivePlayersCount() <= 0) {
    // do nothing
    return;
  }

  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SINGLE_PLAYER;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("SAVE"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", GetGameAPI()->GetProfileForSP());
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("SaveGame");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSSaveAnyGame;
  gmCurrent.gm_mgNotes.SetText("");
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartDemoLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_DEMO;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("PLAY DEMO"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("Demos\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".dem");
  gmCurrent.gm_pAfterFileChosen = &LSLoadDemo;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartDemoSaveMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_gmRunningGameMode == GM_NONE) {
    return;
  }

  _gmMenuGameMode = GM_DEMO;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("RECORD DEMO"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("Demos\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("Demo");
  gmCurrent.gm_fnmExt = CTString(".dem");
  gmCurrent.gm_pAfterFileChosen = &LSSaveDemo;
  gmCurrent.gm_mgNotes.SetText("");
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(FALSE);

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartNetworkQuickLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_NETWORK;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("QUICK LOAD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\Network\\Quick\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadNetwork;
  SetQuickLoadNotes();

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartNetworkLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_NETWORK;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("LOAD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\Network\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadNetwork;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartNetworkSaveMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_gmRunningGameMode != GM_NETWORK) {
    return;
  }

  _gmMenuGameMode = GM_NETWORK;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("SAVE"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\Network\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("SaveGame");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSSaveAnyGame;
  gmCurrent.gm_mgNotes.SetText("");
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSplitScreenQuickLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SPLIT_SCREEN;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("QUICK LOAD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\Quick\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSplitScreen;
  SetQuickLoadNotes();

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSplitScreenLoadMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SPLIT_SCREEN;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("LOAD"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSplitScreen;
  gmCurrent.gm_mgNotes.SetText("");

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

void StartSplitScreenSaveMenu(void) {
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_gmRunningGameMode != GM_SPLIT_SCREEN) {
    return;
  }

  _gmMenuGameMode = GM_SPLIT_SCREEN;

  gmCurrent.gm_mgTitle.SetName(LOCALIZE("SAVE"));
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
  gmCurrent.gm_strSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("SaveGame");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSSaveAnyGame;
  gmCurrent.gm_mgNotes.SetText("");
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  ChangeToMenu(&gmCurrent);
}

// -------- Disabled Menu Calling Function
void DisabledFunction(void) {
  CDisabledMenu &gmCurrent = _pGUIM->gmDisabledFunction;

  gmCurrent.SetParentMenu(pgmCurrentMenu);
  gmCurrent.gm_mgButton.SetText(LOCALIZE("The feature is not available in this version!"));
  gmCurrent.gm_mgTitle.SetName(LOCALIZE("DISABLED"));
  ChangeToMenu(&gmCurrent);
}