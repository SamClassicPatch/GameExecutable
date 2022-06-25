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

#include "Cecil/CecilExtensions.h"
#include "Cecil/Patches.h"
#include "Cecil/WorldEntities.h"

// Original function pointer
static void (CDrawPort::*pBlendScreen)(void) = NULL;

// Patched function
class CDrawPortPatch : public CDrawPort {
  public:
    void P_BlendScreen(void)
    {
      if (!sam_bRedScreenOnDamage) {
        BOOL bReset = FALSE;

        // Reset blending altogether if no world glaring available
        if (GetWSC() == NULL) {
          bReset = TRUE;

        // Reset red screen blending
        } else if (dp_ulBlendingRA >  0 && dp_ulBlendingA  >  0
                && dp_ulBlendingGA == 0 && dp_ulBlendingBA == 0) {
          bReset = TRUE;
        }

        if (bReset) {
          dp_ulBlendingRA = 0;
          dp_ulBlendingGA = 0;
          dp_ulBlendingBA = 0;
          dp_ulBlendingA  = 0;
        }
      }

      // Proceed to the original function
      (this->*pBlendScreen)();
    };
};

extern void CECIL_ApplyScreenBlendPatch(void) {
  pBlendScreen = &CDrawPort::BlendScreen;
  NEW_PATCH(pBlendScreen, &CDrawPortPatch::P_BlendScreen, "CDrawPort::BlendScreen()");
};
