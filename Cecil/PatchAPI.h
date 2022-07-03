/* Copyright (c) 2022 Dreamy Cecil
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

#ifndef CECIL_INCL_API_H
#define CECIL_INCL_API_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

// Declare patch class
class CPatch;

// Pointer to a function patch under a hashed name
struct SFuncPatch {
  CTString strName; // Patch name
  ULONG ulHash; // Name hash

  CPatch *pPatch; // Pointer to the patch
  
  // Default constructor
  SFuncPatch() : strName(""), ulHash(0), pPatch(NULL)
  {
  };

  // Constructor from name and patch
  SFuncPatch(const CTString &strSetName, CPatch *pSetPatch) :
    strName(strSetName), pPatch(pSetPatch)
  {
    // Calculate name hash
    ulHash = strName.GetHash();
  };
};

// Patch API class
class CPatchAPI {
  public:
    // Network provider type for CGame
    enum ENetworkProvider {
      NP_LOCAL,
      NP_SERVER,
      NP_CLIENT,
    };

  public:
    CTString strVersion; // Patch version
    CStaticStackArray<SFuncPatch> aPatches; // Function patch storage

  // Declared but undefined methods cannot be used outside the EXE patch project
  public:
    // Constructor
    CPatchAPI();

  // CGame field wrappers
  public:
    // Get console state
    INDEX GetConState(void) {
      return _pGame->gm_csConsoleState;
    };

    // Set console state
    void SetConState(INDEX iState) {
      (INDEX &)_pGame->gm_csConsoleState = iState;
    };

    // Get computer state
    INDEX GetCompState(void) {
      return _pGame->gm_csComputerState;
    };

    // Set computer state
    void SetCompState(INDEX iState) {
      (INDEX &)_pGame->gm_csComputerState = iState;
    };

    // Set network provider
    void SetNetworkProvider(ENetworkProvider eProvider) {
      static const char *astrProviders[3] = {
        "Local", "TCP/IP Server", "TCP/IP Client",
      };

      _pGame->gm_strNetworkProvider = astrProviders[eProvider];
    };

    // Get first loading state
    BOOL GetFirstLoading(void) {
      return _pGame->gm_bFirstLoading;
    };

    // Set if loading for the first time
    void SetFirstLoading(BOOL bState) {
      _pGame->gm_bFirstLoading = bState;
    };

    // Get menu state
    BOOL GetMenuState(void) {
      return _pGame->gm_bMenuOn;
    };

    // Set menu state
    void SetMenuState(BOOL bState) {
      _pGame->gm_bMenuOn = bState;
    };

    // Get game state
    BOOL GetGameState(void) {
      return _pGame->gm_bGameOn;
    };

    // Set game state
    void SetGameState(BOOL bState) {
      _pGame->gm_bGameOn = bState;
    };

  // CGame session property wrappers
  public:
    // Get custom level filename
    CTString &GetCustomLevel(void) {
      return _pGame->gam_strCustomLevel;
    };
    
    // Get session name
    CTString &GetSessionName(void) {
      return _pGame->gam_strSessionName;
    };
    
    // Get address for joining
    CTString &GetJoinAddress(void) {
      return _pGame->gam_strJoinAddress;
    };
};

// Don't use this variable outside the EXE patch project. Visit for more info:
// https://github.com/SamClassicPatch/GameExecutable/wiki/Mod-support#api-utilization
extern "C" __declspec(dllexport) CPatchAPI *_pPatchAPI;

#endif
