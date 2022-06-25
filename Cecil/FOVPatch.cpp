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
static void P_RenderView(CWorld &woWorld, CEntity &enViewer, CAnyProjection3D &prProjection, CDrawPort &dp)
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
      AdjustVFOV(FLOAT2D(dp.GetWidth(), dp.GetHeight()), fVFOV);

      CPrintF("View VFOV: %.2f\n", fVFOV);
    }

    // Set custom FOV if not zooming in
    if (sam_fCustomFOV > 0.0f && fNewFOV > 80.0f) {
      fNewFOV = Clamp(sam_fCustomFOV, 60.0f, 110.0f);
    }

    // Adjust FOV for wider resolutions (preserve vertical FOV instead of horizontal)
    if (sam_bUseVerticalFOV) {
      AdjustHFOV(FLOAT2D(dp.GetWidth(), dp.GetHeight()), fNewFOV);

      // Don't let FOV be invalid
      fNewFOV = Clamp(fNewFOV, 1.0f, 170.0f);
    }

    // Display proper FOVs
    if (sam_bCheckFOV) {
      // Horizontal FOV
      CPrintF("New HFOV:  %.2f\n", fNewFOV);

      // Vertical FOV
      FLOAT fVFOV = fNewFOV;
      AdjustVFOV(FLOAT2D(dp.GetWidth(), dp.GetHeight()), fVFOV);

      CPrintF("New VFOV:  %.2f\n", fVFOV);
    }
    
    sam_bCheckFOV = FALSE;
  }

  // Proceed to the original function
  (*pRenderView)(woWorld, enViewer, prProjection, dp);
};

class CProjectionPatch : public CPerspectiveProjection3D {
  public:
    FLOAT P_MipFactorDist(FLOAT fDistance) const {
      ASSERT(pr_Prepared);
      
      // Get inverted aspect ratio of the current resolution
      extern CDrawPort *pdp;
      FLOAT fInverseAspectRatio = (FLOAT)pdp->GetHeight() / (FLOAT)pdp->GetWidth();

      // Make engine think that the objects are closer for wider resolutions
      // Also decrease distance for wider FOV (i.e. rely on VFOV instead of HFOV)
      fDistance *= fInverseAspectRatio * (4.0f / 3.0f);

      return Log2((FLOAT)Abs(1024.0f * fDistance * ppr_fMipRatio));
    };

    FLOAT P_MipFactor(void) const {
      ASSERT(pr_Prepared);

      FLOAT fFOV = ppr_FOVWidth;

      // Cancel 4:3 aspect ratio from the FOV
      extern CDrawPort *pdp;
      AdjustHFOV(FLOAT2D(3, 4), fFOV);

      return -pr_TranslationVector(3) * TanFast(fFOV * 0.5f);
    };
};

extern void CECIL_ApplyFOVPatch(void) {
  pRenderView = &RenderView;
  NEW_PATCH(pRenderView, &P_RenderView, "::RenderView(...)");

  // Workaround for casting raw addresses into function pointers
  union {
    ULONG ulAddress;
    FLOAT (CPerspectiveProjection3D::*pFunc)(void) const;
  } factor;

  factor.ulAddress = 0x60100500; // Beginning of CPerspectiveProjection3D::MipFactor()
  NEW_PATCH(factor.pFunc, &CProjectionPatch::P_MipFactor, "CPerspectiveProjection3D::MipFactor()");
  
  // Workaround for casting raw addresses into function pointers
  union {
    ULONG ulAddress;
    FLOAT (CPerspectiveProjection3D::*pFunc)(FLOAT) const;
  } dist;

  dist.ulAddress = 0x601004D0; // Beginning of CPerspectiveProjection3D::MipFactor(FLOAT)
  NEW_PATCH(dist.pFunc, &CProjectionPatch::P_MipFactorDist, "CPerspectiveProjection3D::MipFactor(FLOAT)");
};
