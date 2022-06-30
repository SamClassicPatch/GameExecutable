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
static void P_RenderView(CWorld &woWorld, CEntity &enViewer, CAnyProjection3D &apr, CDrawPort &dp)
{
  // Not a perspective projection
  if (!apr.IsPerspective()) {
    // Proceed to the original function
    (*pRenderView)(woWorld, enViewer, apr, dp);
    return;
  }

  BOOL bPlayer = IsDerivedFromClass(&enViewer, "PlayerEntity");
  BOOL bView = IsOfClass(&enViewer, "Player View");

  // Change FOV for the player view
  if (bPlayer || bView) {
    CPerspectiveProjection3D &ppr = *((CPerspectiveProjection3D *)(CProjection3D *)apr);

    FLOAT2D vScreen(dp.GetWidth(), dp.GetHeight());

    // Adjust clip distance according to the aspect ratio
    ppr.FrontClipDistanceL() *= (vScreen(2) / vScreen(1)) * (4.0f / 3.0f);

    FLOAT &fNewFOV = ppr.ppr_FOVWidth;

    // Display view FOVs
    if (sam_bCheckFOV) {
      // Horizontal FOV
      CPrintF("View HFOV: %.2f\n", fNewFOV);

      // Vertical FOV
      FLOAT fVFOV = fNewFOV;
      AdjustVFOV(vScreen, fVFOV);

      CPrintF("View VFOV: %.2f\n", fVFOV);
    }

    // Set custom FOV if not zooming in
    if (fNewFOV > 80.0f) {
      // Set different FOV for the third person view
      if (bView && sam_fThirdPersonFOV > 0.0f) {
        fNewFOV = Clamp(sam_fThirdPersonFOV, 60.0f, 110.0f);

      // Set first person view FOV
      } else if (sam_fCustomFOV > 0.0f) {
        fNewFOV = Clamp(sam_fCustomFOV, 60.0f, 110.0f);
      }
    }

    // Adjust FOV for wider resolutions (preserve vertical FOV instead of horizontal)
    if (sam_bUseVerticalFOV) {
      AdjustHFOV(vScreen, fNewFOV);

      // Don't let FOV be invalid
      fNewFOV = Clamp(fNewFOV, 1.0f, 170.0f);
    }

    // Display proper FOVs
    if (sam_bCheckFOV) {
      // Horizontal FOV
      CPrintF("New HFOV:  %.2f\n", fNewFOV);

      // Vertical FOV
      FLOAT fVFOV = fNewFOV;
      AdjustVFOV(vScreen, fVFOV);

      CPrintF("New VFOV:  %.2f\n", fVFOV);
    }
    
    sam_bCheckFOV = FALSE;
  }

  // Proceed to the original function
  (*pRenderView)(woWorld, enViewer, apr, dp);
};

// Original function pointer
static void (*pModelRender)(CAnyProjection3D &, CDrawPort *) = NULL;

// Patched function
static void P_BeginModelRenderingView(CAnyProjection3D &apr, CDrawPort *pdp) {
  // Only fix for perspective projections
  if (sam_bFixViewmodelFOV && apr.IsPerspective()) {
    BOOL bFixFOV = TRUE;

    // Don't fix FOV for computer models
    if (_pGame->gm_csComputerState != CS_OFF && _pGame->gm_csComputerState != CS_ONINBACKGROUND) {
      bFixFOV = FALSE;

    } else {
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

      // Iterate through the last 10 calls from here
      for (INDEX iDepth = 0; iDepth < 10; iDepth++)
      {
        #ifdef SE1_TFE
          const ULONG ulInRenderer = 0x601A462D; // TFE 1.05 address
        #elif SE1_VER == 105
          const ULONG ulInRenderer = 0x6017470D; // TSE 1.05 address
        #else
          const ULONG ulInRenderer = 0x601AF17E; // TSE 1.07 address
        #endif

        // Calling from CRenderer::RenderModels()
        if (ulCallAddress == ulInRenderer) {
          bFixFOV = FALSE;
          break;
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
    }

    // Need to fix the FOV
    if (bFixFOV) {
      CPerspectiveProjection3D &ppr = *((CPerspectiveProjection3D *)(CProjection3D *)apr);

      // Adjust projection FOV according to the aspect ratio
      extern CDrawPort *pdp;
      AdjustHFOV(FLOAT2D(pdp->GetWidth(), pdp->GetHeight()), ppr.FOVL());
    }
  }

  // Proceed to the original function
  (*pModelRender)(apr, pdp);
};

class CProjectionPatch : public CPerspectiveProjection3D {
  public:
    FLOAT P_MipFactorDist(FLOAT fDistance) const {
      ASSERT(pr_Prepared);
      
      if (sam_bFixMipDistance) {
        // Get inverted aspect ratio of the current resolution
        extern CDrawPort *pdp;
        FLOAT fInverseAspectRatio = (FLOAT)pdp->GetHeight() / (FLOAT)pdp->GetWidth();

        // Make engine think that the objects are closer for wider resolutions
        // Also decrease distance for wider FOV (i.e. rely on VFOV instead of HFOV)
        fDistance *= fInverseAspectRatio * (4.0f / 3.0f);
      }

      return Log2((FLOAT)Abs(1024.0f * fDistance * ppr_fMipRatio));
    };

    FLOAT P_MipFactor(void) const {
      ASSERT(pr_Prepared);

      FLOAT fFOV = ppr_FOVWidth;

      // Cancel 4:3 aspect ratio from the FOV
      if (sam_bFixMipDistance) {
        extern CDrawPort *pdp;
        AdjustHFOV(FLOAT2D(3, 4), fFOV);
      }

      return -pr_TranslationVector(3) * TanFast(fFOV * 0.5f);
    };
};

extern void CECIL_ApplyFOVPatch(void) {
  pRenderView = &RenderView;
  NEW_PATCH(pRenderView, &P_RenderView, "::RenderView(...)");

  pModelRender = &BeginModelRenderingView;
  NEW_PATCH(pModelRender, &P_BeginModelRenderingView, "::BeginModelRenderingView(...)");
  
  // Workaround for casting raw addresses into function pointers
  union {
    ULONG ulAddress;
    FLOAT (CPerspectiveProjection3D::*pFunc)(FLOAT) const;
  } dist;

#ifdef SE1_TFE
  dist.ulAddress = 0x600F70D0; // Beginning of CPerspectiveProjection3D::MipFactor(FLOAT)
#elif SE1_VER == 105
  dist.ulAddress = 0x600C7160; // Beginning of CPerspectiveProjection3D::MipFactor(FLOAT)
#else
  dist.ulAddress = 0x601004D0; // Beginning of CPerspectiveProjection3D::MipFactor(FLOAT)
#endif
  NEW_PATCH(dist.pFunc, &CProjectionPatch::P_MipFactorDist, "CPerspectiveProjection3D::MipFactor(FLOAT)");

  // Workaround for casting raw addresses into function pointers
  union {
    ULONG ulAddress;
    FLOAT (CPerspectiveProjection3D::*pFunc)(void) const;
  } factor;
  
#ifdef SE1_TFE
  factor.ulAddress = 0x600F7100; // Beginning of CPerspectiveProjection3D::MipFactor()
#elif SE1_VER == 105
  factor.ulAddress = 0x600C7190; // Beginning of CPerspectiveProjection3D::MipFactor()
#else
  factor.ulAddress = 0x60100500; // Beginning of CPerspectiveProjection3D::MipFactor()
#endif
  NEW_PATCH(factor.pFunc, &CProjectionPatch::P_MipFactor, "CPerspectiveProjection3D::MipFactor()");
};
