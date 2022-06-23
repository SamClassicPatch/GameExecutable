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

    // Set custom FOV
    if (sam_fCustomFOV > 0.0f) {
      fNewFOV = sam_fCustomFOV;
    }

    // Don't let FOV be invalid
    fNewFOV = Clamp(fNewFOV, 60.0f, 110.0f);

    // Wider FOV for wider resolutions (preserve vertical FOV instead of horizontal)
    if (sam_bUseVerticalFOV) {
      // Get aspect ratio of the current resolution
      extern CDrawPort *pdp;
      FLOAT fAspectRatio = (FLOAT)pdp->GetWidth() / (FLOAT)pdp->GetHeight();

      // 4:3 resolution = 1.0 ratio; 16:9 = 1.333 etc.
      FLOAT fSquareRatio = fAspectRatio / (4.0f / 3.0f);

      // Take current FOV angle and apply square ratio to it
      FLOAT fVerticalAngle = Tan(fNewFOV / 2.0f) * fSquareRatio;

      // 90 FOV on 16:9 resolution will become 106.26...
      fNewFOV = 2.0f * ATan(fVerticalAngle);

      // Don't let FOV be invalid
      fNewFOV = Clamp(fNewFOV, 1.0f, 170.0f);
    }
  }

  // Proceed to the original function
  (*pRenderView)(woWorld, enViewer, prProjection, dpDrawport);
};

extern void CECIL_ApplyFOVPatch(void) {
  CPrintF("  ::RenderView\n");
  pRenderView = &RenderView;

  NEW_PATCH(pPatchSet, pRenderView, &P_RenderView);

  if (!pPatchSet->ok()) {
    FatalError("Cannot set function patch for RenderView()!");
  }
};
