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

#include "StdH.h"

#include "Cecil/Patches.h"
#include <Engine/Base/ParsingSymbols.h>

// Patch storage
CDynamicContainer<CPatch> _cPatches;

extern void CECIL_InitPatches(void) {
  // Add patch registry into symbols
  CShellSymbol &ssNew = *_pShell->sh_assSymbols.New(1);

  ssNew.ss_strName = "PatchRegistry";
  ssNew.ss_istType = 0; // Should be '_shell_istUndeclared'
  ssNew.ss_pvValue = &_cPatches;
  ssNew.ss_ulFlags = 0;
  ssNew.ss_pPreFunc = NULL;
  ssNew.ss_pPostFunc = NULL;
};
