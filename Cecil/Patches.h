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

#include "Cecil/Patcher/patcher.h"

// Force instruction rewrite
void Patch_ForceRewrite(const int iLength);

// Patch storage
extern CDynamicContainer<CPatch> _cPatches;

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
  CPatch *pPatch = new CPatch(funcOld, funcNew, true, true);

  // Add to the patch registry
  if (pPatch->ok()) {
    _cPatches.Add(pPatch);

  // Couldn't patch
  } else {
    PATCH_ERROR_OUTPUT("Cannot set function patch for %s!\nAddress: 0x%p", strName, funcOld);
  }

  return pPatch;
};
