/* Copyright (c) 2002-2012 Croteam Ltd.
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
#include "MenuPrinting.h"
#include "MOptions.h"

void COptionsMenu::Initialize_t(void) {
  // intialize options menu
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(LOCALIZE("OPTIONS"));
  AddChild(&gm_mgTitle);

  gm_mgVideoOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgVideoOptions.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgVideoOptions.mg_pmgUp = &gm_mgAddonOptions;
  gm_mgVideoOptions.mg_pmgDown = &gm_mgAudioOptions;
  gm_mgVideoOptions.SetText(LOCALIZE("VIDEO OPTIONS"));
  gm_mgVideoOptions.mg_strTip = LOCALIZE("set video mode and driver");
  AddChild(&gm_mgVideoOptions);
  gm_mgVideoOptions.mg_pActivatedFunction = NULL;

  gm_mgAudioOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgAudioOptions.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgAudioOptions.mg_pmgUp = &gm_mgVideoOptions;
  gm_mgAudioOptions.mg_pmgDown = &gm_mgPlayerProfileOptions;
  gm_mgAudioOptions.SetText(LOCALIZE("AUDIO OPTIONS"));
  gm_mgAudioOptions.mg_strTip = LOCALIZE("set audio quality and volume");
  AddChild(&gm_mgAudioOptions);
  gm_mgAudioOptions.mg_pActivatedFunction = NULL;

  gm_mgPlayerProfileOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgPlayerProfileOptions.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgPlayerProfileOptions.mg_pmgUp = &gm_mgAudioOptions;
  gm_mgPlayerProfileOptions.mg_pmgDown = &gm_mgNetworkOptions;
  gm_mgPlayerProfileOptions.SetText(LOCALIZE("PLAYERS AND CONTROLS"));
  gm_mgPlayerProfileOptions.mg_strTip = LOCALIZE("change currently active player or adjust controls");
  AddChild(&gm_mgPlayerProfileOptions);
  gm_mgPlayerProfileOptions.mg_pActivatedFunction = NULL;

  gm_mgNetworkOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgNetworkOptions.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgNetworkOptions.mg_pmgUp = &gm_mgPlayerProfileOptions;
  gm_mgNetworkOptions.mg_pmgDown = &gm_mgPatchOptions;
  gm_mgNetworkOptions.SetText(LOCALIZE("NETWORK CONNECTION"));
  gm_mgNetworkOptions.mg_strTip = LOCALIZE("choose your connection parameters");
  AddChild(&gm_mgNetworkOptions);
  gm_mgNetworkOptions.mg_pActivatedFunction = NULL;

  // [Cecil] Patch settings
  gm_mgPatchOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgPatchOptions.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgPatchOptions.mg_pmgUp = &gm_mgNetworkOptions;
  gm_mgPatchOptions.mg_pmgDown = &gm_mgCustomOptions;
  gm_mgPatchOptions.SetText(TRANS("CLASSICS PATCH"));
  gm_mgPatchOptions.mg_strTip = TRANS("settings for the classics patch");
  AddChild(&gm_mgPatchOptions);
  gm_mgPatchOptions.mg_pActivatedFunction = NULL;

  gm_mgCustomOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgCustomOptions.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgCustomOptions.mg_pmgUp = &gm_mgPatchOptions;
  gm_mgCustomOptions.mg_pmgDown = &gm_mgAddonOptions;
  gm_mgCustomOptions.SetText(LOCALIZE("ADVANCED OPTIONS"));
  gm_mgCustomOptions.mg_strTip = LOCALIZE("for advanced users only");
  AddChild(&gm_mgCustomOptions);
  gm_mgCustomOptions.mg_pActivatedFunction = NULL;

  gm_mgAddonOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgAddonOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgAddonOptions.mg_pmgUp = &gm_mgCustomOptions;
  gm_mgAddonOptions.mg_pmgDown = &gm_mgVideoOptions;
  gm_mgAddonOptions.SetText(LOCALIZE("EXECUTE ADDON"));
  gm_mgAddonOptions.mg_strTip = LOCALIZE("choose from list of addons to execute");
  AddChild(&gm_mgAddonOptions);
  gm_mgAddonOptions.mg_pActivatedFunction = NULL;
}