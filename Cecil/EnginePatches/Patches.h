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

#ifndef CECIL_INCL_PATCHES_H
#define CECIL_INCL_PATCHES_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "../Patcher/patcher.h"

// Patcher debug output
bool &Patch_DebugOutput(void);

// Force instruction rewrite
void Patch_ForceRewrite(const int iLength);

// Caster from raw addresses to function pointers
template<class FuncType>
struct FuncPtr {
  union {
    ULONG ulAddress; // Raw address of the function
    FuncType pFunction; // Pointer to the function
  };

  // Constructor from raw address
  FuncPtr(ULONG ulSetAddress = NULL) : ulAddress(ulSetAddress)
  {
  };
};

// Don't terminate the game in debug
#ifndef NDEBUG
  #define PATCH_ERROR_OUTPUT InfoMessage
#else
  #define PATCH_ERROR_OUTPUT FatalError
#endif

// Create a new function patch
template<class FuncType1, class FuncType2> inline
CPatch *NewPatch(FuncType1 &funcOld, FuncType2 funcNew, const char *strName) {
  CPrintF("  %s\n", strName);

  if (Patch_DebugOutput()) {
    InfoMessage(strName);
  }

  CPatch *pPatch = new CPatch(funcOld, funcNew, true, false);

  // Add to the patch registry
  if (pPatch->ok()) {
    _pPatchAPI->aPatches.Push() = SFuncPatch(strName, pPatch);

  // Couldn't patch
  } else {
    PATCH_ERROR_OUTPUT("Cannot set function patch for %s!\nAddress: 0x%p", strName, funcOld);
  }

  return pPatch;
};

// Check if the current function is being called from a specific address
__forceinline BOOL CallingFrom(const ULONG ulFrom, const INDEX ctDepth) {
  // Create thread context
  CONTEXT context;
  ZeroMemory(&context, sizeof(CONTEXT));
  context.ContextFlags = CONTEXT_CONTROL;

  // Retrieve call stack
  __asm {
  Label:
    mov [context.Ebp], ebp
    mov [context.Esp], esp
    mov eax, [Label]
    mov [context.Eip], eax
  }

  DWORD ulCallAddress = context.Eip;

  PDWORD pFrame = (PDWORD)context.Ebp;
  PDWORD pPrevFrame = NULL;

  // Iterate through the last N calls from here
  for (INDEX iDepth = 0; iDepth < ctDepth; iDepth++)
  {
    // Calling from the right address
    if (ulCallAddress == ulFrom) {
      return TRUE;
    }

    // Get next call address
    ulCallAddress = pFrame[1];

    // Advance the frame
    pPrevFrame = pFrame;
    pFrame = (PDWORD)pFrame[0];

    if ((DWORD)pFrame & 3) break;
    if (pFrame <= pPrevFrame) break;

    if (IsBadWritePtr(pFrame, sizeof(PVOID) * 2)) break;
  }

  return FALSE;
};

#endif
