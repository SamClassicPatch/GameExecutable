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
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <process.h>

#include "resource.h"
#include "SplashScreen.h"
#include "MainWindow.h"
#include "GlSettings.h"
#include "LevelInfo.h"
#include "CmdLine.h"
#include "Credits.h"

// [Cecil] Classics patch
#include <CoreLib/Query/QueryManager.h>
#include <CoreLib/Networking/Modules/VotingSystem.h>
#include <Engine/Sound/SoundData.h>

#include "Cecil/ScreenResolutions.h"
#include "Cecil/UpdateCheck.h"
#include "Cecil/WindowModes.h"

#include <locale.h>

// Application state variables
extern BOOL _bRunning = TRUE;
extern BOOL _bQuitScreen = TRUE;
extern BOOL bMenuActive = FALSE;
extern BOOL bMenuRendering = FALSE;

extern BOOL _bDefiningKey;
static BOOL _bReconsiderInput = FALSE;

// [Cecil] Computer screen resolution
extern PIX2D _vpixScreenRes = PIX2D(0, 0);

static INDEX sam_iMaxFPSActive = 500;
static INDEX sam_iMaxFPSInactive = 10;
static INDEX sam_bPauseOnMinimize = TRUE; // auto-pause when window has been minimized
static INDEX sam_bWideScreen = FALSE; // [Cecil] Dummy; replaced with 'sam_bAdjustForAspectRatio'
extern FLOAT sam_fPlayerOffset = 0.0f;

// Display mode settings
extern INDEX sam_iWindowMode = 0; // [Cecil] Different window modes
extern INDEX sam_iScreenSizeI = 1024; // current size of the window
extern INDEX sam_iScreenSizeJ = 768;  // current size of the window
extern INDEX sam_iDisplayDepth = 0; // 0 = default, 1 = 16bit, 2 = 32bit
extern INDEX sam_iDisplayAdapter = 0;
extern INDEX sam_iGfxAPI = 0; // 0 = OpenGL
extern INDEX sam_bFirstStarted = FALSE;
extern FLOAT sam_tmDisplayModeReport = 5.0f;
extern INDEX sam_bShowAllLevels = FALSE;
static INDEX sam_bMentalActivated = FALSE;

// Network settings
extern CTString sam_strNetworkSettings = "";

// Command line
extern CTString sam_strCommandLine = "";

// 0...app started for the first time
// 1...all ok
// 2...automatic fallback
static INDEX _iDisplayModeChangeFlag = 0;
static TIME _tmDisplayModeChanged = 100.0f; // when display mode was last changed

// Rendering preferences for automatic settings
extern INDEX sam_iVideoSetup = 1;

// Automatic adjustment of audio quality
extern BOOL sam_bAutoAdjustAudio = TRUE;

extern INDEX sam_bAutoPlayDemos = TRUE;
static INDEX _bInAutoPlayLoop = TRUE;

// Menu calling
extern INDEX sam_bMenuSave = FALSE;
extern INDEX sam_bMenuLoad = FALSE;
extern INDEX sam_bMenuControls = FALSE;
extern INDEX sam_bMenuHiScore = FALSE;
extern INDEX sam_bToggleConsole = FALSE;
extern INDEX sam_iStartCredits = FALSE;

// For mod re-loading
extern CTFileName _fnmModToLoad = CTString("");
extern CTString _strModServerJoin = CTString("");
extern CTString _strURLToVisit = CTString("");

// State variables fo addon execution
// 0 - nothing
// 1 - start (invoke console)
// 2 - console invoked, waiting for one redraw
extern INDEX _iAddonExecState = 0;
extern CTFileName _fnmAddonToExec = CTString("");

// Logo textures
static CTextureObject _toLogoCT;
static CTextureObject _toLogoODI;
static CTextureObject _toLogoEAX;
extern CTextureObject *_ptoLogoCT = NULL;
extern CTextureObject *_ptoLogoODI = NULL;
extern CTextureObject *_ptoLogoEAX = NULL;

// [Cecil] The First Encounter
#if SE1_GAME == SS_TFE
  CTString sam_strModName = "-   T H E   F I R S T   E N C O U N T E R   -";
  CTString sam_strTechTestLevel = "Levels\\TechTest.wld";
  CTString sam_strTrainingLevel = "Levels\\KarnakDemo.wld";
#else
  CTString sam_strModName = "-   T H E   S E C O N D   E N C O U N T E R   -";
  CTString sam_strTechTestLevel = "Levels\\LevelsMP\\TechTest.wld";
  CTString sam_strTrainingLevel = "Levels\\KarnakDemo.wld";
#endif

ENGINE_API extern INDEX snd_iFormat;

// [Cecil] Set new LCD drawport for Game
void SetDrawportForGame(CDrawPort *pdpSet) {
  _pGame->LCDSetDrawport(pdpSet);

  // Adjust aspect ratio for the menu
  if (IConfig::mod[k_EModDataProps_ProperTextScaling]) {
    pdpSet->dp_fWideAdjustment = ((FLOAT)pdpSet->GetHeight() / (FLOAT)pdpSet->GetWidth()) * (4.0f / 3.0f);
  }
};

// Main window canvas
CDrawPort *pdp;
CDrawPort *pdpNormal;
CViewPort *pvpViewPort;
HINSTANCE _hInstance;

static void PlayDemo(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  const CTString &strDemoFilename = *NEXT_ARG(CTString *);

  _gmMenuGameMode = GM_DEMO;

  CTFileName fnDemo = "demos\\" + strDemoFilename + ".dem";

  extern BOOL LSLoadDemo(const CTFileName &fnm);
  LSLoadDemo(fnDemo);
}

static void ApplyRenderingPreferences(void) {
  ApplyGLSettings(TRUE);
}

extern void ApplyVideoMode(void) {
  StartNewMode((GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
               (enum DisplayDepth)sam_iDisplayDepth, sam_iWindowMode);
}

static void BenchMark(void) {
  _pGfx->Benchmark(pvpViewPort, pdp);
}

static void QuitGame(void) {
  _bRunning = FALSE;
  _bQuitScreen = FALSE;
}

// Check if another app is already running
static HANDLE _hLock = NULL;
static CTFileName _fnmLock;
static void DirectoryLockOn(void) {
  // create lock filename
  _fnmLock = IDir::AppPath() + "SeriousSam.loc";

  // try to open lock file
  _hLock = CreateFileA(
    _fnmLock,
    GENERIC_WRITE,
    0, // no sharing
    NULL, // pointer to security attributes
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, // file attributes
    NULL);

  // if failed
  if (_hLock == NULL || GetLastError() != 0) {
    // report warning
    CPutString(LOCALIZE("WARNING: SeriousSam didn't shut down properly last time!\n"));
  }
}

static void DirectoryLockOff(void) {
  // if lock is open
  if (_hLock != NULL) {
    // close it
    CloseHandle(_hLock);
  }
}

void End(void);

// Automaticaly manage input enable/disable toggling
static BOOL _bInputEnabled = FALSE;
void UpdateInputEnabledState(void) {
  // do nothing if window is invalid
  if (_hwndMain == NULL) {
    return;
  }

  // input should be enabled if application is active
  // and no menu is active and no console is active
  BOOL bShouldBeEnabled = (!IsIconic(_hwndMain) && !bMenuActive && GetGameAPI()->GetConState() == CS_OFF
                       && (GetGameAPI()->GetCompState() == CS_OFF || GetGameAPI()->GetCompState() == CS_ONINBACKGROUND))
                       || _bDefiningKey;

  // if should be turned off
  if ((!bShouldBeEnabled && _bInputEnabled) || _bReconsiderInput) {
    // disable it and remember new state
    _pInput->DisableInput();
    _bInputEnabled = FALSE;
  }

  // if should be turned on
  if (bShouldBeEnabled && !_bInputEnabled) {
    // enable it and remember new state
    _pInput->EnableInput(_hwndMain);
    _bInputEnabled = TRUE;
  }

  _bReconsiderInput = FALSE;
}

// Automaticaly manage pause toggling
void UpdatePauseState(void) {
  // [Cecil] Get states
  const INDEX iConState = GetGameAPI()->GetConState();
  const INDEX iCompState = GetGameAPI()->GetCompState();

  // [Cecil] Check Steam overlay
  BOOL bShouldPause = (bMenuActive || GetSteamAPI()->IsOverlayOn() ||
                       iConState  == CS_ON || iConState  == CS_TURNINGON || iConState  == CS_TURNINGOFF ||
                       iCompState == CS_ON || iCompState == CS_TURNINGON || iCompState == CS_TURNINGOFF);

  _pNetwork->SetLocalPause(_gmRunningGameMode == GM_SINGLE_PLAYER && bShouldPause);
}

// Limit current frame rate if neeeded
void LimitFrameRate(void) {
  // measure passed time for each loop
  static CTimerValue tvLast(-1.0f);
  CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();
  TIME tmCurrentDelta = (tvNow - tvLast).GetSeconds();

  // limit maximum frame rate
  sam_iMaxFPSActive = ClampDn((INDEX)sam_iMaxFPSActive, 1L);
  sam_iMaxFPSInactive = ClampDn((INDEX)sam_iMaxFPSInactive, 1L);
  INDEX iMaxFPS = sam_iMaxFPSActive;

  if (IsIconic(_hwndMain)) {
    iMaxFPS = sam_iMaxFPSInactive;
  }

  if (GetGameAPI()->GetCurrentSplitCfg() == CGame::SSC_DEDICATED) {
    iMaxFPS = ClampDn(iMaxFPS, 60L); // never go very slow if dedicated server
  }

  TIME tmWantedDelta = 1.0f / iMaxFPS;
  if (tmCurrentDelta < tmWantedDelta) {
    Sleep((tmWantedDelta - tmCurrentDelta) * 1000.0f);
  }

  // remember new time
  tvLast = _pTimer->GetHighPrecisionTimer();
}

// Load first demo
void StartNextDemo(void) {
  if (!sam_bAutoPlayDemos || !_bInAutoPlayLoop) {
    _bInAutoPlayLoop = FALSE;
    return;
  }

  // skip if no demos
  if (_lhAutoDemos.IsEmpty()) {
    _bInAutoPlayLoop = FALSE;
    return;
  }

  // get first demo level and cycle the list
  CLevelInfo *pli = LIST_HEAD(_lhAutoDemos, CLevelInfo, li_lnNode);
  pli->li_lnNode.Remove();
  _lhAutoDemos.AddTail(pli->li_lnNode);

  // if intro
  if (pli->li_fnLevel == sam_strIntroLevel) {
    // start intro
    _gmRunningGameMode = GM_NONE;

    // [Cecil] Reset start player indices
    GetGameAPI()->ResetStartProfiles();
    GetGameAPI()->SetProfileForStart(0, 0);

    GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);
    GetGameAPI()->SetStartSplitCfg(CGame::SSC_PLAY1);

    // [Cecil] Use difficulties and game modes from the API
    _pShell->SetINDEX("gam_iStartDifficulty", ClassicsModData_GetDiff(2)->m_iLevel); // Normal
    _pShell->SetINDEX("gam_iStartMode", GetGameAPI()->GetGameMode(0)); // Flyover

    // [Cecil] Pass byte container
    CSesPropsContainer sp;
    _pGame->SetSinglePlayerSession((CSessionProperties &)sp);

    GetGameAPI()->SetFirstLoading(TRUE);

    // [Cecil] Start game through the API
    if (GetGameAPI()->NewGame(sam_strIntroLevel, sam_strIntroLevel, (CSessionProperties &)sp)) {
      _gmRunningGameMode = GM_INTRO;
    }

  // if not intro
  } else {
    // start the demo
    GetGameAPI()->SetStartSplitCfg(CGame::SSC_OBSERVER);

    // [Cecil] Reset start player indices
    GetGameAPI()->ResetStartProfiles();

    // play the demo
    GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);
    _gmRunningGameMode = GM_NONE;

    if (_pGame->StartDemoPlay(pli->li_fnLevel)) {
      _gmRunningGameMode = GM_DEMO;
      CON_DiscardLastLineTimes();
    }
  }

  if (_gmRunningGameMode == GM_NONE) {
    _bInAutoPlayLoop = FALSE;
  }
}

// Run web browser and view an url
void RunBrowser(const char *strUrl, BOOL bUseSteamOverlay) {
  // [Cecil] Try web browser in Steam overlay
  if (bUseSteamOverlay) {
    if (GetSteamAPI()->OpenWebPage(strUrl)) return;
  }

  int iResult = (int)ShellExecuteA(_hwndMain, "OPEN", strUrl, NULL, NULL, SW_SHOWMAXIMIZED);

  if (iResult < 32) {
    // should report error?
    NOTHING;
  }
}

void LoadAndForceTexture(CTextureObject &to, CTextureObject *&pto, const CTFileName &fnm) {
  try {
    to.SetData_t(fnm);

    CTextureData *ptd = (CTextureData *)to.GetData();
    ptd->Force(TEX_CONSTANT);
    ptd = ptd->td_ptdBaseTexture;

    if (ptd != NULL) {
      ptd->Force(TEX_CONSTANT);
    }

    pto = &to;

  } catch (char *pchrError) {
    (void *)pchrError;
    pto = NULL;
  }
}

BOOL Init(HINSTANCE hInstance, int nCmdShow, CTString strCmdLine) {
  _hInstance = hInstance;

  // [Cecil] Mark as a game
  ClassicsPatch_Setup(k_EClassicsPatchAppType_Game);

  // [Cecil] Set DPI awareness
  SetDPIAwareness();

  ShowSplashScreen(hInstance);

#if _PATCHCONFIG_ENGINEPATCHES
  // [Cecil] Function patches
  _EnginePatches.FileSystem();
#endif

  // [Cecil] Get screen resolution
  _vpixScreenRes = PIX2D(::GetSystemMetrics(SM_CXSCREEN),
                         ::GetSystemMetrics(SM_CYSCREEN));

  // prepare main window
  MainWindow_Init();
  OpenMainWindowInvisible();

  // [Cecil] Remember command line arguments
  _strRestartCommandLine = strCmdLine;

  // parse command line before initializing engine
  ParseCommandLine(strCmdLine, TRUE);

  #if SE1_GAME == SS_REV
    // [Cecil] Rev: Use Steam the same way as in Revolution's EXE
    if (strCmdLine.FindSubstr("-nosteam") == -1) {
      if (_pSteam->IsSteamRunning()) {
        _pSteam->Initialize(FALSE, 227780, "Serious Sam: Revolution");

      } else {
        // [Cecil] NOTE: Restarts the client but doesn't initialize?
        _pSteam->RestartSteamIfNecessary();
      }
    }
  #endif

  // initialize engine
  SE_InitEngine(sam_strGameName);

  // [Cecil] Custom initialization
  ClassicsPatch_InitExt();

  SE_LoadDefaultFonts();

  // now print the output of command line parsing
  CPrintF("%s", cmd_strOutput);

  // lock the directory
  DirectoryLockOn();

  // load all translation tables
  InitTranslation();

  try {
    AddTranslationTablesDir_t(CTString("Data\\Translations\\"), CTString("*.txt"));
    FinishTranslationTable();

  } catch (char *strError) {
    FatalError("%s", strError);
  }

  // [Cecil] Translate the mod name
  sam_strModName = TRANSV(sam_strModName);

  // always disable all warnings when in serious sam
  _pShell->Execute("con_bNoWarnings=1;");

  // declare shell symbols
  _pShell->DeclareSymbol("user void PlayDemo(CTString);", &PlayDemo);
  _pShell->DeclareSymbol("persistent INDEX sam_iWindowMode;",   &sam_iWindowMode); // [Cecil] Window modes
  _pShell->DeclareSymbol("persistent INDEX sam_iScreenSizeI;",  &sam_iScreenSizeI);
  _pShell->DeclareSymbol("persistent INDEX sam_iScreenSizeJ;",  &sam_iScreenSizeJ);
  _pShell->DeclareSymbol("persistent INDEX sam_iDisplayDepth;", &sam_iDisplayDepth);
  _pShell->DeclareSymbol("persistent INDEX sam_iDisplayAdapter;", &sam_iDisplayAdapter);
  _pShell->DeclareSymbol("persistent INDEX sam_iGfxAPI;",         &sam_iGfxAPI);
  _pShell->DeclareSymbol("persistent INDEX sam_bFirstStarted;", &sam_bFirstStarted);
  _pShell->DeclareSymbol("persistent INDEX sam_bAutoAdjustAudio;", &sam_bAutoAdjustAudio);
  _pShell->DeclareSymbol("persistent user INDEX sam_bWideScreen;", &sam_bWideScreen);
  _pShell->DeclareSymbol("persistent user FLOAT sam_fPlayerOffset;",  &sam_fPlayerOffset);
  _pShell->DeclareSymbol("persistent user INDEX sam_bAutoPlayDemos;", &sam_bAutoPlayDemos);
  _pShell->DeclareSymbol("persistent user INDEX sam_iMaxFPSActive;",    &sam_iMaxFPSActive);
  _pShell->DeclareSymbol("persistent user INDEX sam_iMaxFPSInactive;",  &sam_iMaxFPSInactive);
  _pShell->DeclareSymbol("persistent user INDEX sam_bPauseOnMinimize;", &sam_bPauseOnMinimize);
  _pShell->DeclareSymbol("persistent user FLOAT sam_tmDisplayModeReport;",   &sam_tmDisplayModeReport);
  _pShell->DeclareSymbol("persistent user CTString sam_strNetworkSettings;", &sam_strNetworkSettings);
  _pShell->DeclareSymbol("user CTString sam_strModName;", &sam_strModName);
  _pShell->DeclareSymbol("persistent INDEX sam_bShowAllLevels;", &sam_bShowAllLevels);
  _pShell->DeclareSymbol("persistent INDEX sam_bMentalActivated;", &sam_bMentalActivated);

  _pShell->DeclareSymbol("user CTString sam_strTechTestLevel;", &sam_strTechTestLevel);
  _pShell->DeclareSymbol("user CTString sam_strTrainingLevel;", &sam_strTrainingLevel);
  
  _pShell->DeclareSymbol("user void Quit(void);", &QuitGame);

  _pShell->DeclareSymbol("persistent user INDEX sam_iVideoSetup;",     &sam_iVideoSetup);
  _pShell->DeclareSymbol("user void ApplyRenderingPreferences(void);", &ApplyRenderingPreferences);
  _pShell->DeclareSymbol("user void ApplyVideoMode(void);",            &ApplyVideoMode);
  _pShell->DeclareSymbol("user void Benchmark(void);", &BenchMark);

  _pShell->DeclareSymbol("user INDEX sam_bMenuSave;",     &sam_bMenuSave);
  _pShell->DeclareSymbol("user INDEX sam_bMenuLoad;",     &sam_bMenuLoad);
  _pShell->DeclareSymbol("user INDEX sam_bMenuControls;", &sam_bMenuControls);
  _pShell->DeclareSymbol("user INDEX sam_bMenuHiScore;",  &sam_bMenuHiScore);
  _pShell->DeclareSymbol("user INDEX sam_bToggleConsole;",&sam_bToggleConsole);
  _pShell->DeclareSymbol("INDEX sam_iStartCredits;", &sam_iStartCredits);

  // [Cecil] Load Game library as a module
  GetPluginAPI()->LoadGameLib("Data\\SeriousSam.gms");

  _pNetwork->md_strGameID = sam_strGameName;

  _pGame->LCDInit();

  if (sam_bFirstStarted) {
    InfoMessage(LOCALIZE(
      "SeriousSam is starting for the first time.\n"
      "If you experience any problems, please consult\n"
      "ReadMe file for troubleshooting information."));
  }

  // initialize sound library
  snd_iFormat = Clamp(snd_iFormat, (INDEX)CSoundLibrary::SF_NONE, (INDEX)CSoundLibrary::SF_44100_16);
  _pSound->SetFormat((enum CSoundLibrary::SoundFormat)snd_iFormat);

  if (sam_bAutoAdjustAudio) {
    _pShell->Execute("include \"" SCRIPTS_ADDONS_DIR "SFX-AutoAdjust.ini\"");
  }

  // execute script given on command line
  if (cmd_strScript != "") {
    CPrintF("Command line script: '%s'\n", cmd_strScript);

    CTString strCmd;
    strCmd.PrintF("include \"%s\"", cmd_strScript);

    _pShell->Execute(strCmd);
  }

  // load logo textures
  LoadAndForceTexture(_toLogoCT,  _ptoLogoCT,  CTFILENAME("Textures\\Logo\\LogoCT.tex"));
  LoadAndForceTexture(_toLogoODI, _ptoLogoODI, CTFILENAME("Textures\\Logo\\GodGamesLogo.tex"));
  LoadAndForceTexture(_toLogoEAX, _ptoLogoEAX, CTFILENAME("Textures\\Logo\\LogoEAX.tex"));

  // !! NOTE !! Re-enable these to allow mod support.
  LoadStringVar(CTString("Data\\Var\\Sam_Version.var"), sam_strVersion);
  LoadStringVar(CTString("Data\\Var\\ModName.var"), sam_strModName);
  CPrintF(LOCALIZE("Serious Sam version: %s\n"), sam_strVersion);
  CPrintF(LOCALIZE("Active mod: %s\n"), sam_strModName);
  InitializeMenus();

  // if there is a mod
  if (_fnmMod != "") {
    // execute the mod startup script
    _pShell->Execute(CTString("include \"Scripts\\Mod_startup.ini\";"));
  }

  // init gl settings module
  InitGLSettings();

  // init level-info subsystem
  LoadLevelsList();
  LoadDemosList();

  // [Cecil] Load in-game plugins
  GetPluginAPI()->LoadPlugins(k_EPluginFlagGame);

  // apply application mode
  StartNewMode((GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
               (enum DisplayDepth)sam_iDisplayDepth, sam_iWindowMode);

  // set default mode reporting
  if (sam_bFirstStarted) {
    _iDisplayModeChangeFlag = 0;
    sam_bFirstStarted = FALSE;
  }

  HideSplashScreen();

  // [Cecil] If nothing has been started by the command line
  if (!ExecuteCommandLine()) {
    // Start playing demos
    StartNextDemo();
  }

  return TRUE;
}

void End(void) {
  _pGame->DisableLoadingHook();

  // cleanup level-info subsystem
  ClearLevelsList();
  ClearDemosList();

  // destroy the main window and its canvas
  if (pvpViewPort != NULL) {
    _pGfx->DestroyWindowCanvas(pvpViewPort);
    pvpViewPort = NULL;
    pdpNormal = NULL;
  }

  CloseMainWindow();
  MainWindow_End();
  DestroyMenus();
  _pGame->End();
  _pGame->LCDEnd();

  // [Cecil] Clean up the core
  ClassicsPatch_Shutdown();

  // unlock the directory
  DirectoryLockOff();
  SE_EndEngine();
}

// Print display mode info if needed
void PrintDisplayModeInfo(void) {
  // skip if timed out
  if (_pTimer->GetRealTimeTick() > (_tmDisplayModeChanged + sam_tmDisplayModeReport)) {
    return;
  }

  // cache some general vars
  SLONG slDPWidth = pdp->GetWidth();
  SLONG slDPHeight = pdp->GetHeight();

  if (pdp->IsDualHead()) {
    slDPWidth /= 2;
  }

  CDisplayMode dm;
  dm.dm_pixSizeI = slDPWidth;
  dm.dm_pixSizeJ = slDPHeight;

  // determine proper text scale for statistics display
  FLOAT fTextScale = (FLOAT)slDPHeight / 480.0f; // [Cecil] Use height instead of width

  // get resolution
  CTString strRes;
  extern CTString _strPreferencesDescription;
  strRes.PrintF("%dx%dx%s", slDPWidth, slDPHeight, _pGfx->gl_dmCurrentDisplayMode.DepthString());

  if (dm.IsDualHead()) {
    strRes += LOCALIZE(" DualMonitor");
  }

  if (dm.IsWideScreen()) {
    strRes += LOCALIZE(" WideScreen");
  }

  if (_pGfx->gl_eCurrentAPI == GAT_OGL) {
    strRes += " (OpenGL)";
  }

#ifdef SE1_D3D
  else if (_pGfx->gl_eCurrentAPI == GAT_D3D) {
    strRes += " (Direct3D)";
  }
#endif // SE1_D3D

  CTString strDescr;
  strDescr.PrintF("\n%s (%s)\n", _strPreferencesDescription, RenderingPreferencesDescription(sam_iVideoSetup));
  strRes += strDescr;

  // tell if application is started for the first time, or failed to set mode
  if (_iDisplayModeChangeFlag == 0) {
    strRes += LOCALIZE("Display mode set by default!");
  } else if (_iDisplayModeChangeFlag == 2) {
    strRes += LOCALIZE("Last mode set failed!");
  }

  // print it all
  pdp->SetFont(_pfdDisplayFont);
  pdp->SetTextScaling(fTextScale);
  pdp->SetTextAspect(1.0f);
  pdp->PutText(strRes, slDPWidth * 0.05f, slDPHeight * 0.85f, _pGame->LCDGetColor(C_GREEN | 255, "display mode"));
}

// Do the main game loop and render screen
void DoGame(void) {
  // set flag if not in game
  if (!GetGameAPI()->IsGameOn()) {
    _gmRunningGameMode = GM_NONE;
  }

  if ((_gmRunningGameMode == GM_DEMO && _pNetwork->IsDemoPlayFinished())
   || (_gmRunningGameMode == GM_INTRO && _pNetwork->IsGameFinished())) {
    _pGame->StopGame();
    _gmRunningGameMode = GM_NONE;

    // load next demo
    StartNextDemo();

    if (!_bInAutoPlayLoop) {
      // start menu
      StartMenus();
    }
  }

  // do the main game loop
  if (_gmRunningGameMode != GM_NONE) {
    _pGame->GameMainLoop();

  // if game is not started
  } else {
    // [Cecil] Update master server
    IMasterServer::EnumUpdate();

    // just handle broadcast messages
    _pNetwork->GameInactive();
  }

  // [Cecil] Update current vote
  IVotingSystem::UpdateVote();

  if (sam_iStartCredits > 0) {
    Credits_On(sam_iStartCredits);
    sam_iStartCredits = 0;
  }

  if (sam_iStartCredits < 0) {
    Credits_Off();
    sam_iStartCredits = 0;
  }

  if (_gmRunningGameMode == GM_NONE) {
    Credits_Off();
    sam_iStartCredits = 0;
  }

  // redraw the view
  if (!IsIconic(_hwndMain) && pdp != NULL && pdp->Lock()) {
    // [Cecil] Set drawport that will be used for custom Steam screenshots
    GetSteamAPI()->SetScreenshotHook(pdp);

    // [Cecil] Keep rendering the game in the background while in menu
    BOOL bRenderGame = (!bMenuActive || sam_bBackgroundGameRender);

    if (_gmRunningGameMode != GM_NONE && bRenderGame) {
      // [Cecil] Call API before redrawing the game
      IHooks::OnPreDraw(pdp);

      // [Cecil] Don't wait for server while playing demos (removes "Waiting for server to continue" message)
      if (_pNetwork->IsPlayingDemo()) {
        _pNetwork->ga_sesSessionState.ses_bWaitingForServer = FALSE;
      }

    #if _PATCHCONFIG_ENGINEPATCHES
      // [Cecil] Don't listen to in-game sounds if rendering the game in the menu
      _EnginePatches._bNoListening = bMenuActive;
    #endif

      // handle pretouching of textures and shadowmaps
      pdp->Unlock();
      _pGame->GameRedrawView(pdp, (GetGameAPI()->GetConState() != CS_OFF) ? 0 : GRV_SHOWEXTRAS);

      pdp->Lock();
      _pGame->ComputerRender(pdp);

      // [Cecil] Call API after redrawing the game
      IHooks::OnPostDraw(pdp);

      pdp->Unlock();

      CDrawPort dpScroller(pdp, TRUE);
      dpScroller.Lock();

      if (Credits_Render(&dpScroller) == 0) {
        Credits_Off();
      }

      dpScroller.Unlock();
      pdp->Lock();

    } else {
      pdp->Fill(_pGame->LCDGetColor(C_dGREEN | CT_OPAQUE, "bcg fill"));
    }

    // do menu
    if (bMenuRendering) {
      // clear z-buffer
      pdp->FillZBuffer(ZBUF_BACK);

      // remember if we should render menus next tick
      bMenuRendering = DoMenu(pdp);
    }

    // print display mode info if needed
    PrintDisplayModeInfo();

    #if SE1_GAME == SS_REV
      // [Cecil] Rev: Render Steam overlay
      _pSteam->RenderCustomOverlay(pdp);
    #endif

    // render console
    _pGame->ConsoleRender(pdp);

    // [Cecil] Call API every render frame
    IHooks::OnFrame(pdp);

    // done with all
    pdp->Unlock();

    // show
    pvpViewPort->SwapBuffers();

  // [Cecil] Reset drawport for screenshots
  } else {
    GetSteamAPI()->SetScreenshotHook(NULL);
  }
}

void TeleportPlayer(int iPosition) {
  CTString strCommand;
  strCommand.PrintF("cht_iGoToMarker = %d;", iPosition);

  _pShell->Execute(strCommand);
}

CTextureObject _toStarField;
static FLOAT _fLastVolume = 1.0f;

void RenderStarfield(CDrawPort *pdp, FLOAT fStrength) {
  CTextureData *ptd = (CTextureData *)_toStarField.GetData();

  // skip if no texture
  if (ptd == NULL) {
    return;
  }

  PIX pixSizeI = pdp->GetWidth();
  PIX pixSizeJ = pdp->GetHeight();

  // [Cecil] Use height instead of width
  FLOAT fStretch = HEIGHT_SCALING(pdp);
  fStretch *= FLOAT(ptd->GetPixWidth()) / ptd->GetWidth();

  PIXaabbox2D boxScreen(PIX2D(0, 0), PIX2D(pixSizeI, pixSizeJ));
  MEXaabbox2D boxTexture(MEX2D(0, 0), MEX2D(pixSizeI / fStretch, pixSizeJ / fStretch));

  pdp->PutTexture(&_toStarField, boxScreen, boxTexture, LerpColor(C_BLACK, C_WHITE, fStrength) | CT_OPAQUE);
}

FLOAT RenderQuitScreen(CDrawPort *pdp, CViewPort *pvp) {
  CDrawPort dpQuit(pdp, TRUE);

  // redraw the view
  if (!dpQuit.Lock()) {
    return 0;
  }

  dpQuit.Fill(C_BLACK | CT_OPAQUE);
  RenderStarfield(&dpQuit, _fLastVolume);

  FLOAT fVolume = Credits_Render(&dpQuit);
  _fLastVolume = fVolume;

  dpQuit.Unlock();
  pvp->SwapBuffers();

  return fVolume;
}

void QuitScreenLoop(void) {
  Credits_On(3);
  CSoundObject soMusic;

  try {
    _toStarField.SetData_t(CTFILENAME("Textures\\Background\\Night01\\Stars01.tex"));
    soMusic.Play_t(CTFILENAME("Music\\Credits.mp3"), SOF_NONGAME | SOF_MUSIC | SOF_LOOP);

  } catch (char *strError) {
    CPrintF("%s\n", strError);
  }

  // while it is still running
  FOREVER {
    FLOAT fVolume = RenderQuitScreen(pdp, pvpViewPort);

    if (fVolume <= 0) {
      return;
    }

    // assure we can listen to non-3d sounds
    soMusic.SetVolume(fVolume, fVolume);
    _pSound->UpdateSounds();

    // while there are any messages in the message queue
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      // [Cecil] Change credits scroll speed
      if (msg.message == WM_MOUSEWHEEL) {
        SWORD swDir = SWORD(UWORD(HIWORD(msg.wParam)));

        if (swDir > 0) {
          Credits_Speed(2, -1);
        } else if (swDir < 0) {
          Credits_Speed(2, +1);
        }

      // if it is not a keyboard or mouse message
      } else if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN || msg.message == WM_KEYDOWN) {
        return;
      }
    }

    //Sleep(5);
  }
}

// [Cecil] Keep track of held mouse buttons
static BOOL _bLHeld = FALSE;
static BOOL _bRHeld = FALSE;
static BOOL _bMHeld = FALSE;

// [Cecil] Release all buttons
void ReleaseHeldMouseButtons(void) {
  _bLHeld = FALSE;
  _bRHeld = FALSE;
  _bMHeld = FALSE;
};

int SubMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  (void)hPrevInstance;

  if (!Init(hInstance, nCmdShow, lpCmdLine)) {
    return FALSE;
  }

  // initialy, application is running and active, console and menu are off
  _bRunning = TRUE;
  _bQuitScreen = TRUE;
  GetGameAPI()->SetConState(CS_OFF);
  GetGameAPI()->SetCompState(CS_OFF);

  //bMenuActive    = FALSE;
  //bMenuRendering = FALSE;

  // while it is still running
  while (_bRunning && _fnmModToLoad == "") {
    // [Cecil] Release all keys if the window loses focus
    if (GetActiveWindow() != _hwndMain) {
      ReleaseHeldMouseButtons();
    }

    // while there are any messages in the message queue
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      // if it is not a mouse message
      if (!(msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)) {
        // if not system key messages
        if (!(msg.message == WM_KEYDOWN && msg.wParam == VK_F10 || msg.message == WM_SYSKEYDOWN)) {
          // dispatch it
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }

      // system commands (also send by the application itself)
      if (msg.message == WM_SYSCOMMAND) {
        switch (msg.wParam & ~0x0F) {
          // if should minimize
          case SC_MINIMIZE:
            if (_bWindowChanging) {
              break;
            }

            _bWindowChanging = TRUE;
            _bReconsiderInput = TRUE;

            // if allowed, not already paused and only in single player game mode
            if (sam_bPauseOnMinimize && !_pNetwork->IsPaused() && _gmRunningGameMode == GM_SINGLE_PLAYER) {
              // pause game
              _pNetwork->TogglePause();
            }

            // [Cecil] If fullscreen
            if (sam_iWindowMode == E_WM_FULLSCREEN) {
              // reset display mode and minimize window
              _pGfx->ResetDisplayMode();
              ShowWindow(_hwndMain, SW_MINIMIZE);

            // if not in full screen
            } else {
              // just minimize the window
              ShowWindow(_hwndMain, SW_MINIMIZE);
            }
            break;

          // if should restore
          case SC_RESTORE:
            if (_bWindowChanging) {
              break;
            }

            _bWindowChanging = TRUE;
            _bReconsiderInput = TRUE;

            // [Cecil] If fullscreen
            if (sam_iWindowMode == E_WM_FULLSCREEN) {
              ShowWindow(_hwndMain, SW_SHOWNORMAL);
              // set the display mode once again
              StartNewMode((GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
                           (enum DisplayDepth)sam_iDisplayDepth, sam_iWindowMode);
            // if not in full screen
            } else {
              // restore window
              ShowWindow(_hwndMain, SW_SHOWNORMAL);
            }
            break;

          // if should maximize
          case SC_MAXIMIZE:
            if (_bWindowChanging) {
              break;
            }

            _bWindowChanging = TRUE;
            _bReconsiderInput = TRUE;

            // go to full screen
            StartNewMode((GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
                         (enum DisplayDepth)sam_iDisplayDepth, TRUE);
            ShowWindow(_hwndMain, SW_SHOWNORMAL);
            break;
        }
      }

      // toggle full-screen on alt-enter
      if (msg.message == WM_SYSKEYDOWN && msg.wParam == VK_RETURN && !IsIconic(_hwndMain)) {
        // [Cecil] Switch between windowed and fullscreen
        StartNewMode((GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
                     (enum DisplayDepth)sam_iDisplayDepth, (sam_iWindowMode != E_WM_FULLSCREEN ? E_WM_FULLSCREEN : E_WM_WINDOWED));
      }

      // if application should stop
      if (msg.message == WM_QUIT || msg.message == WM_CLOSE) {
        // stop running
        _bRunning = FALSE;
        _bQuitScreen = FALSE;
      }

      // if application is deactivated or minimized
      if ((msg.message == WM_ACTIVATE && (LOWORD(msg.wParam) == WA_INACTIVE || HIWORD(msg.wParam)))
       ||  msg.message == WM_CANCELMODE
       ||  msg.message == WM_KILLFOCUS
       || (msg.message == WM_ACTIVATEAPP && !msg.wParam)) {
        // if application is running and in full screen mode
        if (!_bWindowChanging && _bRunning) {
          // [Cecil] Minimize if in fullscreen
          if (sam_iWindowMode == E_WM_FULLSCREEN) {
            PostMessage(NULL, WM_SYSCOMMAND, SC_MINIMIZE, 0);

          // just disable input if not in full screen
          } else {
            _pInput->DisableInput();
          }
        }

      // if application is activated or minimized
      } else if ((msg.message == WM_ACTIVATE && (LOWORD(msg.wParam) == WA_ACTIVE || LOWORD(msg.wParam) == WA_CLICKACTIVE))
              ||  msg.message == WM_SETFOCUS
              || (msg.message == WM_ACTIVATEAPP && msg.wParam)) {
        // enable input back again if needed
        _bReconsiderInput = TRUE;
      }

      if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE &&
        (_gmRunningGameMode == GM_DEMO || _gmRunningGameMode == GM_INTRO)) {
        _pGame->StopGame();
        _gmRunningGameMode = GM_NONE;
      }

      if (GetGameAPI()->GetConState() == CS_TALK && msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
        GetGameAPI()->SetConState(CS_OFF);
        msg.message = WM_NULL;
      }

      BOOL bMenuForced = (_gmRunningGameMode == GM_NONE &&
        (GetGameAPI()->GetConState() == CS_OFF || GetGameAPI()->GetConState() == CS_TURNINGOFF));
      BOOL bMenuToggle = (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE
        && (GetGameAPI()->GetCompState() == CS_OFF || GetGameAPI()->GetCompState() == CS_ONINBACKGROUND));

      if (!bMenuActive) {
        if (bMenuForced || bMenuToggle) {
          // if console is active
          if (GetGameAPI()->GetConState() == CS_ON || GetGameAPI()->GetConState() == CS_TURNINGON) {
            // deactivate it
            GetGameAPI()->SetConState(CS_TURNINGOFF);
            _iAddonExecState = 0;
          }

          // delete key down message so menu would not exit because of it
          msg.message = WM_NULL;

          // start menu
          StartMenus();
        }

      } else {
        if (bMenuForced && bMenuToggle && pgmCurrentMenu->GetParentMenu() == NULL) {
          // delete key down message so menu would not exit because of it
          msg.message = WM_NULL;
        }
      }

      // if neither menu nor console is running
      if (!bMenuActive && (GetGameAPI()->GetConState() == CS_OFF || GetGameAPI()->GetConState() == CS_TURNINGOFF)) {
        // if current menu is not root
        if (!IsMenusInRoot()) {
          // start current menu
          StartMenus();
        }
      }

      if (sam_bMenuSave) {
        sam_bMenuSave = FALSE;
        StartMenus("save");
      }

      if (sam_bMenuLoad) {
        sam_bMenuLoad = FALSE;
        StartMenus("load");
      }

      if (sam_bMenuControls) {
        sam_bMenuControls = FALSE;
        StartMenus("controls");
      }

      if (sam_bMenuHiScore) {
        sam_bMenuHiScore = FALSE;
        StartMenus("hiscore");
      }

      // interpret console key presses
      if (_iAddonExecState == 0) {
        if (msg.message == WM_KEYDOWN) {
          _pGame->ConsoleKeyDown(msg);

          if (GetGameAPI()->GetConState() != CS_ON) {
            _pGame->ComputerKeyDown(msg);
          }

        } else if (msg.message == WM_KEYUP) {
          // special handler for talk (not to invoke return key bind)
          if (msg.wParam == VK_RETURN && GetGameAPI()->GetConState() == CS_TALK) {
            GetGameAPI()->SetConState(CS_OFF);
          }

        } else if (msg.message == WM_CHAR) {
          // [Cecil] Set computer locale for validating non-latin characters (handle Tab as intended)
          if (msg.wParam != VK_TAB) {
            setlocale(LC_ALL, "");
          }

          // [Cecil] TEMP: Ignore non-printable 'yo' cyrillic character in Windows-1251
          if (GetACP() != 1251 || (msg.wParam != 0xA8 && msg.wParam != 0xB8)) {
            _pGame->ConsoleChar(msg);
          }

          // [Cecil] Restore locale
          setlocale(LC_ALL, "C");
        }

        if (msg.message == WM_LBUTTONDOWN
         || msg.message == WM_RBUTTONDOWN
         || msg.message == WM_LBUTTONDBLCLK
         || msg.message == WM_RBUTTONDBLCLK
         || msg.message == WM_LBUTTONUP
         || msg.message == WM_RBUTTONUP) {
          if (GetGameAPI()->GetConState() != CS_ON) {
            _pGame->ComputerKeyDown(msg);
          }
        }
      }

      // if menu is active and no input on
      if (bMenuActive && !_pInput->IsInputEnabled()) {
        // [Cecil] Execute action on mouse button hold
        if (_bLHeld) {
          MenuOnMouseHold(VK_LBUTTON);
        } else if (_bRHeld) {
          MenuOnMouseHold(VK_RBUTTON);
        } else if (_bMHeld) {
          MenuOnMouseHold(VK_MBUTTON);
        }

        // pass keyboard/mouse messages to menu
        if (msg.message == WM_KEYDOWN) {
          MenuOnKeyDown(msg.wParam);

        } else if (msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONDBLCLK) {
          // [Cecil] Hold LMB
          _bLHeld = TRUE;
          SetCapture(_hwndMain);

          MenuOnKeyDown(VK_LBUTTON);

        } else if (msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONDBLCLK) {
          // [Cecil] Hold RMB
          _bRHeld = TRUE;
          SetCapture(_hwndMain);

          MenuOnKeyDown(VK_RBUTTON);

        // [Cecil] Press MMB
        } else if (msg.message == WM_MBUTTONDOWN || msg.message == WM_MBUTTONDBLCLK) {
          // Hold MMB
          _bMHeld = TRUE;
          SetCapture(_hwndMain);

          MenuOnKeyDown(VK_MBUTTON);

        // [Cecil] Release held LMB
        } else if (msg.message == WM_LBUTTONUP) {
          _bLHeld = FALSE;
          ReleaseCapture();

        // [Cecil] Release held RMB
        } else if (msg.message == WM_RBUTTONUP) {
          _bRHeld = FALSE;
          ReleaseCapture();

        // [Cecil] Release held MMB
        } else if (msg.message == WM_MBUTTONUP) {
          _bMHeld = FALSE;
          ReleaseCapture();

        } else if (msg.message == WM_MOUSEMOVE) {
          MenuOnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));

        } else if (msg.message == WM_MOUSEWHEEL) {
          SWORD swDir = SWORD(UWORD(HIWORD(msg.wParam)));

          if (swDir > 0) {
            MenuOnKeyDown(11);
          } else if (swDir < 0) {
            MenuOnKeyDown(10);
          }

        } else if (msg.message == WM_CHAR) {
          // [Cecil] Set computer locale for validating non-latin characters (handle Tab as intended)
          if (msg.wParam != VK_TAB) {
            setlocale(LC_ALL, "");
          }

          // [Cecil] TEMP: Ignore non-printable 'yo' cyrillic character in Windows-1251
          if (GetACP() != 1251 || (msg.wParam != 0xA8 && msg.wParam != 0xB8)) {
            MenuOnChar(msg);
          }

          // [Cecil] Restore locale
          setlocale(LC_ALL, "C");
        }
      }

      // if toggling console
      BOOL bConsoleKey = sam_bToggleConsole || msg.message == WM_KEYDOWN &&
        (MapVirtualKey(msg.wParam, 0) == 41 // scan code for '~'
        || msg.wParam == VK_F1 || (msg.wParam == VK_ESCAPE && _iAddonExecState == 3));

      if (bConsoleKey && !_bDefiningKey) {
        sam_bToggleConsole = FALSE;

        if (_iAddonExecState == 3) {
          _iAddonExecState = 0;
        }

        // if it is up, or pulling up
        if (GetGameAPI()->GetConState() == CS_OFF || GetGameAPI()->GetConState() == CS_TURNINGOFF) {
          // start it moving down and disable menu
          GetGameAPI()->SetConState(CS_TURNINGON);

          // stop all IFeel effects
          IFeel_StopEffect(NULL);

          if (bMenuActive) {
            StopMenus(FALSE);
          }

        // if it is down, or dropping down
        } else if (GetGameAPI()->GetConState() == CS_ON || GetGameAPI()->GetConState() == CS_TURNINGON) {
          // start it moving up
          GetGameAPI()->SetConState(CS_TURNINGOFF);
        }
      }

      if (_pShell->GetINDEX("con_bTalk") && GetGameAPI()->GetConState() == CS_OFF) {
        _pShell->SetINDEX("con_bTalk", FALSE);
        GetGameAPI()->SetConState(CS_TALK);
      }

      // if pause pressed
      if (msg.message == WM_KEYDOWN && msg.wParam == VK_PAUSE) {
        // toggle pause
        _pNetwork->TogglePause();
      }

      // if command sent from external application
      if (msg.message == WM_COMMAND) {
        // if teleport player
        if (msg.wParam == 1001) {
          // teleport player
          TeleportPlayer(msg.lParam);

          // restore
          PostMessage(NULL, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
      }

      // if demo is playing
      if (_gmRunningGameMode == GM_DEMO || _gmRunningGameMode == GM_INTRO) {
        // check if escape is pressed
        BOOL bEscape = (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE);

        // check if console-invoke key is pressed
        BOOL bTilde = (msg.message == WM_KEYDOWN &&
          (msg.wParam == VK_F1 || MapVirtualKey(msg.wParam, 0) == 41)); // scan code for '~'

        // check if any key is pressed
        BOOL bAnyKey = ((msg.message == WM_KEYDOWN && (msg.wParam == VK_SPACE || msg.wParam == VK_RETURN))
                      || msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN);

        // if escape is pressed
        if (bEscape) {
          _pGame->StopGame();

          // stop demo
          _bInAutoPlayLoop = FALSE;
          _gmRunningGameMode = GM_NONE;

        // if any other key is pressed except console invoking
        } else if (bAnyKey && !bTilde) {
          // if not in menu or in console
          if (!bMenuActive && !bMenuRendering && GetGameAPI()->GetConState() == CS_OFF) {
            _pGame->StopGame();

            // skip to next demo
            _gmRunningGameMode = GM_NONE;
            StartNextDemo();
          }
        }
      }
    } // loop while there are messages

    // when all messages are removed, window has surely changed
    _bWindowChanging = FALSE;

    // [Cecil] Join game via a friend
    CTString &strSteamJoinCommand = GetSteamAPI()->strJoinCommandMidGame;

    if (strSteamJoinCommand != "") {
      CPrintF(TRANS("Joining a game via Steam: %s\n"), strSteamJoinCommand);
      ParseCommandLine(strSteamJoinCommand, FALSE);
      ExecuteCommandLine();

      strSteamJoinCommand = "";
    }

    // [Cecil] Display notification about a new release in the menu
    if (bMenuActive && !_pInput->IsInputEnabled()) {
      QueryPatchUpdates();
      NotifyAboutNewVersion();
    }

    // get real cursor position
    if (GetGameAPI()->GetCompState() != CS_OFF && GetGameAPI()->GetCompState() != CS_ONINBACKGROUND) {
      POINT pt;
      ::GetCursorPos(&pt);
      ::ScreenToClient(_hwndMain, &pt);

      _pGame->ComputerMouseMove(pt.x, pt.y);
    }

    // if addon is to be executed
    if (_iAddonExecState == 1) {
      // print header and start console
      CPrintF(LOCALIZE("---- Executing addon: '%s'\n"), (const char *)_fnmAddonToExec);

      sam_bToggleConsole = TRUE;
      _iAddonExecState = 2;

    // if addon is ready for execution
    } else if (_iAddonExecState == 2 && GetGameAPI()->GetConState() == CS_ON) {
      // execute it
      CTString strCmd;
      strCmd.PrintF("include \"%s\"", (const char *)_fnmAddonToExec);
      _pShell->Execute(strCmd);

      CPutString(LOCALIZE("Addon done, press Escape to close console\n"));
      _iAddonExecState = 3;
    }

    // automaticaly manage input enable/disable toggling
    UpdateInputEnabledState();

    // automaticaly manage pause toggling
    UpdatePauseState();

    // notify game whether menu is active
    GetGameAPI()->SetMenuState(!!bMenuActive);

    // do the main game loop and render screen
    DoGame();

    // limit current frame rate if neeeded
    LimitFrameRate();
  } // end of main application loop

  _pInput->DisableInput();

  _pGame->StopGame();

  // [Cecil] Restart current game
  if (_bRestartGameClient) {
    STARTUPINFOA cif;
    ZeroMemory(&cif, sizeof(STARTUPINFOA));
    PROCESS_INFORMATION pi;

    // Use executable filename
    CTString strCmd = IDir::AppPath() + IDir::AppExe();
    CTString strParam = " " + _strRestartCommandLine;

    if (!CreateProcessA(strCmd.str_String, strParam.str_String, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &cif, &pi)) {
      // Report error
      CTString strError;
      strError.PrintF("Cannot restart the game:\n%s\n", GetWindowsError(GetLastError()));

      MessageBoxA(0, strError, "Serious Sam", MB_OK | MB_ICONERROR);
    }

    // Disable quit screen
    _bQuitScreen = FALSE;

  // Load some mod
  } else if (_fnmModToLoad != "") {
    STARTUPINFOA cif;
    ZeroMemory(&cif, sizeof(STARTUPINFOA));
    PROCESS_INFORMATION pi;

    const CTString strMod = _fnmModToLoad.FileName();

    // [Cecil] Use executable filename
    CTString strCmd = IDir::AppPath() + IDir::AppExe();
    CTString strParam = " +game " + strMod;

    if (_strModServerJoin != "") {
      strParam += " +connect " + _strModServerJoin + " +quickjoin";
    }

    if (!CreateProcessA(strCmd.str_String, strParam.str_String, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &cif, &pi)) {
      // [Cecil] Proper error message
      CTString strError;
      strError.PrintF("Cannot start '%s' mod:\n%s\n", strMod, GetWindowsError(GetLastError()));

      MessageBoxA(0, strError, "Serious Sam", MB_OK | MB_ICONERROR);
    }
  }

  // invoke quit screen if needed
  if (_bQuitScreen && _fnmModToLoad == "") {
    QuitScreenLoop();
  }

  End();
  return TRUE;
}

/*static void CheckModReload(void) {
  if (_fnmModToLoad != "") {
    // [Cecil] Use executable filename
    CTString strCommand = IDir::AppPath() + IDir::AppExe();
    //+mod " + _fnmModToLoad.FileName() + "\"";

    CTString strMod = _fnmModToLoad.FileName();
    const char *argv[7];

    argv[0] = strCommand;
    argv[1] = "+game";
    argv[2] = strMod;
    argv[3] = NULL;

    if (_strModServerJoin != "") {
      argv[3] = "+connect";
      argv[4] = _strModServerJoin;
      argv[5] = "+quickjoin";
      argv[6] = NULL;
    }

    _execv(strCommand, argv);
  }
}*/

static void CheckTeaser(void) {
  CTFileName fnmTeaser = IDir::AppBin() + "AfterSam.exe";

  if (fopen(fnmTeaser, "r") != NULL) {
    Sleep(500);
    _execl(fnmTeaser, "\"" + fnmTeaser + "\"", NULL);
  }
}

static void CheckBrowser(void) {
  if (_strURLToVisit != "") {
    RunBrowser(_strURLToVisit);
  }
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  int iResult;

  CTSTREAM_BEGIN {
    iResult = SubMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
  } CTSTREAM_END;

  //CheckModReload();
  //CheckTeaser();

  CheckBrowser();

  return iResult;
}

// [Cecil] Replaced 'bFullScreenMode' with 'eWindowMode'
// Try to start a new display mode
BOOL TryToSetDisplayMode(enum GfxAPIType eGfxAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                         enum DisplayDepth eColorDepth, EWindowModes eWindowMode) {
  CDisplayMode dmTmp;
  dmTmp.dm_ddDepth = eColorDepth;

  // [Cecil] Window mode name
  CTString strWindowMode = _astrWindowModes[eWindowMode];

  CPrintF(LOCALIZE("  Starting display mode: %dx%dx%s (%s)\n"), pixSizeI, pixSizeJ, dmTmp.DepthString(), strWindowMode);

  // mark to start ignoring window size/position messages until settled down
  _bWindowChanging = TRUE;

  // destroy canvas if existing
  _pGame->DisableLoadingHook();

  if (pvpViewPort != NULL) {
    _pGfx->DestroyWindowCanvas(pvpViewPort);
    pvpViewPort = NULL;
    pdpNormal = NULL;
  }

  // close the application window
  CloseMainWindow();

  // try to set new display mode
  BOOL bSuccess;

  // [Cecil] Fullscreen mode
  const BOOL bFullscreen = (eWindowMode == E_WM_FULLSCREEN);

  // [Cecil] Main window opening methods per type
  static void (*apWindowMethods[3])(PIX, PIX) = {
    &OpenMainWindowNormal,     // Windowed/normal
    &OpenMainWindowBorderless, // Borderless
    &OpenMainWindowFullScreen, // Fullscreen
  };

#ifdef SE1_D3D
  if (eGfxAPI == GAT_D3D) {
    (apWindowMethods[eWindowMode])(pixSizeI, pixSizeJ);
  }
#endif

  if (bFullscreen) {
    bSuccess = _pGfx->SetDisplayMode(eGfxAPI, iAdapter, pixSizeI, pixSizeJ, eColorDepth);
  } else {
    bSuccess = _pGfx->ResetDisplayMode(eGfxAPI);
  }

  if (bSuccess && eGfxAPI == GAT_OGL) {
    (apWindowMethods[eWindowMode])(pixSizeI, pixSizeJ);
  }
  
#ifdef SE1_D3D
  // [Cecil] Reset for non-fullscreen modes
  if (bSuccess && !bFullscreen && eGfxAPI == GAT_D3D) {
    ResetMainWindowNormal();
  }
#endif

  // if new mode was set
  if (bSuccess) {
    // create canvas
    ASSERT(pvpViewPort == NULL);
    ASSERT(pdpNormal == NULL);

    _pGfx->CreateWindowCanvas(_hwndMain, &pvpViewPort, &pdpNormal);

    // erase context of both buffers (for the sake of wide-screen)
    pdp = pdpNormal;

    if (pdp != NULL && pdp->Lock()) {
      pdp->Fill(C_BLACK | CT_OPAQUE);

      pdp->Unlock();
      pvpViewPort->SwapBuffers();
      pdp->Lock();

      pdp->Fill(C_BLACK | CT_OPAQUE);

      pdp->Unlock();
      pvpViewPort->SwapBuffers();
    }

    // initial screen fill and swap, just to get context running
    BOOL bSuccess = FALSE;

    if (pdp != NULL && pdp->Lock()) {
      pdp->Fill(_pGame->LCDGetColor(C_dGREEN | CT_OPAQUE, "bcg fill"));
      pdp->Unlock();
      pvpViewPort->SwapBuffers();
      bSuccess = TRUE;
    }

    _pGame->EnableLoadingHook(pdp);

    // if the mode is not working, or is not accelerated
    if (!bSuccess || !_pGfx->IsCurrentModeAccelerated()) {
      // report error
      CPutString(LOCALIZE("This mode does not support hardware acceleration.\n"));

      // destroy canvas if existing
      if (pvpViewPort != NULL) {
        _pGame->DisableLoadingHook();
        _pGfx->DestroyWindowCanvas(pvpViewPort);
        pvpViewPort = NULL;
        pdpNormal = NULL;
      }

      // close the application window
      CloseMainWindow();

      // report failure
      return FALSE;
    }

    // remember new settings
    sam_iWindowMode = eWindowMode; // [Cecil]
    sam_iScreenSizeI = pixSizeI;
    sam_iScreenSizeJ = pixSizeJ;
    sam_iDisplayDepth = eColorDepth;
    sam_iDisplayAdapter = iAdapter;
    sam_iGfxAPI = eGfxAPI;

    // report success
    return TRUE;

  // if couldn't set new mode
  } else {
    // close the application window
    CloseMainWindow();

    // report failure
    return FALSE;
  }
}

// List of possible display modes for recovery
const INDEX aDefaultModes[][3] = {
  // color, API, adapter
  { DD_DEFAULT, GAT_OGL, 0},
  { DD_16BIT,   GAT_OGL, 0},
  { DD_16BIT,   GAT_OGL, 1}, // 3dfx Voodoo2
#ifdef SE1_D3D
  { DD_DEFAULT, GAT_D3D, 0},
  { DD_16BIT,   GAT_D3D, 0},
  { DD_16BIT,   GAT_D3D, 1},
#endif // SE1_D3D
};
const INDEX ctDefaultModes = ARRAYCOUNT(aDefaultModes);

// [Cecil] Different window modes
void StartNewMode(enum GfxAPIType eGfxAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                  enum DisplayDepth eColorDepth, INDEX iWindowMode) {
  CPutString(LOCALIZE("\n* START NEW DISPLAY MODE ...\n"));

  // try to set the mode
  BOOL bSuccess = TryToSetDisplayMode(eGfxAPI, iAdapter, pixSizeI, pixSizeJ, eColorDepth, (EWindowModes)iWindowMode);

  // if failed
  if (!bSuccess) {
    // report failure and reset to default resolution
    _iDisplayModeChangeFlag = 2; // failure
    CPutString(LOCALIZE("Requested display mode could not be set!\n"));

    pixSizeI = 640;
    pixSizeJ = 480;
    iWindowMode = E_WM_WINDOWED; // [Cecil] Changed from fullscreen to windowed

    // try to revert to one of recovery modes
    for (INDEX iMode = 0; iMode < ctDefaultModes; iMode++) {
      eColorDepth = (DisplayDepth)aDefaultModes[iMode][0];
      eGfxAPI = (GfxAPIType)aDefaultModes[iMode][1];
      iAdapter = aDefaultModes[iMode][2];

      CPrintF(LOCALIZE("\nTrying recovery mode %d...\n"), iMode);
      bSuccess = TryToSetDisplayMode(eGfxAPI, iAdapter, pixSizeI, pixSizeJ, eColorDepth, (EWindowModes)iWindowMode);

      if (bSuccess) {
        break;
      }
    }

    // if all failed
    if (!bSuccess) {
      FatalError(LOCALIZE(
        "Cannot set display mode!\n"
        "Serious Sam was unable to find display mode with hardware acceleration.\n"
        "Make sure you install proper drivers for your video card as recommended\n"
        "in documentation and set your desktop to 16 bit (65536 colors).\n"
        "Please see ReadMe file for troubleshooting information.\n"));
    }

  // if succeeded
  } else {
    _iDisplayModeChangeFlag = 1; // all ok

    // [Cecil] Resize console horizontally to fit as many characters as possible
    const PIX pixConsoleChar = (_pfdConsoleFont->GetWidth() + _pfdConsoleFont->GetCharSpacing());
    const INDEX ctConsoleChars = pixSizeI / pixConsoleChar;
    ICore::ReinitConsole(ctConsoleChars - 15, 512);
  }

  // [Cecil] Calculate aspect ratio (half the width for dualhead resolutions)
  if (pdp->IsDualHead()) {
    _fAspectRatio = FLOAT(pixSizeI * 0.5f) / FLOAT(pixSizeJ);
  } else {
    _fAspectRatio = FLOAT(pixSizeI) / FLOAT(pixSizeJ);
  }

  // apply 3D-acc settings
  ApplyGLSettings(FALSE);

  // remember time of mode setting
  _tmDisplayModeChanged = _pTimer->GetRealTimeTick();
}
