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

// General
INDEX sam_bBackgroundGameRender = TRUE;
INDEX sam_bAdjustForAspectRatio = TRUE;
INDEX sam_bOptionTabs = TRUE;

// FOV patch
INDEX sam_bUseVerticalFOV = TRUE;
FLOAT sam_fCustomFOV = -1.0f;
FLOAT sam_fThirdPersonFOV = -1.0f;
INDEX sam_bFixMipDistance = TRUE;
INDEX sam_bFixViewmodelFOV = TRUE;
INDEX sam_bCheckFOV = FALSE;

// Red screen on damage
INDEX sam_bRedScreenOnDamage = TRUE;

// Calculate horizontal FOV according to the aspect ratio
void AdjustHFOV(const FLOAT2D &vScreen, FLOAT &fHFOV) {
  // Get aspect ratio of the current resolution
  FLOAT fAspectRatio = vScreen(1) / vScreen(2);

  // 4:3 resolution = 1.0 ratio; 16:9 = 1.333 etc.
  FLOAT fSquareRatio = fAspectRatio * (3.0f / 4.0f);

  // Take current FOV angle and apply square ratio to it
  FLOAT fVerticalAngle = Tan(fHFOV * 0.5f) * fSquareRatio;

  // 90 FOV on 16:9 resolution will become 106.26...
  fHFOV = 2.0f * ATan(fVerticalAngle);
};

// Calculate vertical FOV from horizontal FOV according to the aspect ratio
void AdjustVFOV(const FLOAT2D &vScreen, FLOAT &fHFOV) {
  // Get inverted aspect ratio of the current resolution
  FLOAT fInverseAspectRatio = vScreen(2) / vScreen(1);

  // Take current FOV angle and apply aspect ratio to it
  FLOAT fVerticalAngle = Tan(fHFOV * 0.5f) * fInverseAspectRatio;

  // 90 FOV on 4:3 or 106.26 FOV on 16:9 will become 73.74...
  fHFOV = 2.0f * ATan(fVerticalAngle);
};

// Custom initialization
void CECIL_Init(void) {
  // Initialize the core
  CECIL_InitCore();

  {
    CPrintF("^c00ffffSam:\nIntercepting Engine functions:\n");

    extern void CECIL_ApplyFOVPatch(void);
    extern void CECIL_ApplyScreenBlendPatch(void);
    extern void CECIL_ApplyUndecoratedPatch(void);
    extern void CECIL_ApplySoundListenPatch(void);
    CECIL_ApplyFOVPatch();
    CECIL_ApplyScreenBlendPatch();
    CECIL_ApplyUndecoratedPatch();
    CECIL_ApplySoundListenPatch();

    CPrintF("^c00ffffDone!\n");
  }

  // Custom symbols
  _pShell->DeclareSymbol("persistent user INDEX sam_bBackgroundGameRender;", &sam_bBackgroundGameRender);
  _pShell->DeclareSymbol("persistent user INDEX sam_bAdjustForAspectRatio;", &sam_bAdjustForAspectRatio);
  _pShell->DeclareSymbol("persistent user INDEX sam_bOptionTabs;",           &sam_bOptionTabs);

  // Initalize other modules
  extern void CECIL_InitLocalCheats(void);
  CECIL_InitLocalCheats();
};
