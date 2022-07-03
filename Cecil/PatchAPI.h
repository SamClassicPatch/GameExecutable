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
};

// Don't use this variable outside the EXE patch project. Visit for more info:
// https://github.com/DreamyCecil/SamExePatch/wiki/Mod-support#api-utilization
extern "C" __declspec(dllexport) CPatchAPI *_pPatchAPI;

#endif
