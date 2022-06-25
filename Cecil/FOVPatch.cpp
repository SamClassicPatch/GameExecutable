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

// Original function pointer
static void (*pRenderView)(CWorld &, CEntity &, CAnyProjection3D &, CDrawPort &) = NULL;

// Patched function
static void P_RenderView(CWorld &woWorld, CEntity &enViewer, CAnyProjection3D &prProjection, CDrawPort &dpDrawport)
{
  // Change FOV for the player view
  if (prProjection.IsPerspective() && (IsDerivedFromClass(&enViewer, "PlayerEntity") || IsOfClass(&enViewer, "Player View"))) {
    CPerspectiveProjection3D &ppr = *((CPerspectiveProjection3D *)(CProjection3D *)prProjection);

    FLOAT &fNewFOV = ppr.ppr_FOVWidth;

    // Display view FOVs
    if (sam_bCheckFOV) {
      // Horizontal FOV
      CPrintF("View HFOV: %.2f\n", fNewFOV);

      // Vertical FOV
      FLOAT fVFOV = fNewFOV;
      AdjustVFOV(dpDrawport, fVFOV);

      CPrintF("View VFOV: %.2f\n", fVFOV);
    }

    // Set custom FOV if not zooming in
    if (sam_fCustomFOV > 0.0f && fNewFOV > 80.0f) {
      fNewFOV = Clamp(sam_fCustomFOV, 60.0f, 110.0f);
    }

    // Adjust FOV for wider resolutions (preserve vertical FOV instead of horizontal)
    if (sam_bUseVerticalFOV) {
      AdjustHFOV(dpDrawport, fNewFOV);

      // Don't let FOV be invalid
      fNewFOV = Clamp(fNewFOV, 1.0f, 170.0f);
    }

    // Display proper FOVs
    if (sam_bCheckFOV) {
      // Horizontal FOV
      CPrintF("New HFOV:  %.2f\n", fNewFOV);

      // Vertical FOV
      FLOAT fVFOV = fNewFOV;
      AdjustVFOV(dpDrawport, fVFOV);

      CPrintF("New VFOV:  %.2f\n", fVFOV);
    }
    
    sam_bCheckFOV = FALSE;
  }

  // Proceed to the original function
  (*pRenderView)(woWorld, enViewer, prProjection, dpDrawport);
};

extern void CECIL_ApplyFOVPatch(void) {
  pRenderView = &RenderView;
  NEW_PATCH(pRenderView, &P_RenderView, "::RenderView(...)");
};
