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

class CPatchAPI {
  public:
    CTString strVersion; // Patch version
    CDynamicContainer<UBYTE> cPatches; // Function patch storage

  public:
    // Constructor
    CPatchAPI();

    // Add new function patch
    void AddFuncPatch(class CPatch *pPatch) {
      cPatches.Add((UBYTE *)pPatch);
    };

    // Get function patch
    class CPatch *GetFuncPatch(INDEX iPatch) {
      return (CPatch *)cPatches.Pointer(iPatch);
    };
};

// Don't use this variable outside the EXE patch project. Visit for more info:
// https://github.com/DreamyCecil/SamExePatch/wiki/Mod-support#api-utilization
extern "C" __declspec(dllexport) CPatchAPI *_pPatchAPI;

#endif
