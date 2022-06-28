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

// Create a new function patch
#define NEW_PATCH(OldFunc, NewFunc, FuncName) { \
  CPutString("  " FuncName "\n"); \
  CPatch *pPatch = new CPatch(OldFunc, NewFunc, true, true); \
  /* Add to the patch registry if patched */ \
  if (pPatch->ok()) _cPatches.Add(pPatch); \
  else FatalError("Cannot set function patch for " FuncName "!\nAddress: 0x%p", OldFunc); \
}
