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
#include "MenuStuff.h"
#include "MAudioOptions.h"

extern void RefreshSoundFormat(void);

void CAudioOptionsMenu::Initialize_t(void) {
  // intialize Audio options menu
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(LOCALIZE("AUDIO"));
  AddChild(&gm_mgTitle);

  TRIGGER_MG(gm_mgAudioAutoTrigger, 0, gm_mgApply, gm_mgFrequencyTrigger, LOCALIZE("AUTO-ADJUST"), astrNoYes);
  gm_mgAudioAutoTrigger.mg_strTip = LOCALIZE("adjust quality to fit your system");

  TRIGGER_MG(gm_mgFrequencyTrigger, 1, gm_mgAudioAutoTrigger, gm_mgAudioAPITrigger, LOCALIZE("FREQUENCY"), astrFrequencyRadioTexts);
  gm_mgFrequencyTrigger.mg_strTip = LOCALIZE("select sound quality or turn sound off");
  gm_mgFrequencyTrigger.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgAudioAPITrigger, 2, gm_mgFrequencyTrigger, gm_mgWaveVolume, LOCALIZE("SOUND SYSTEM"), astrSoundAPIRadioTexts);
  gm_mgAudioAPITrigger.mg_strTip = LOCALIZE("choose sound system (API) to use");
  gm_mgAudioAPITrigger.mg_pOnTriggerChange = NULL;

  gm_mgWaveVolume.mg_boxOnScreen = BoxMediumRow(3);
  gm_mgWaveVolume.SetText(LOCALIZE("SOUND EFFECTS VOLUME"));
  gm_mgWaveVolume.mg_strTip = LOCALIZE("adjust volume of in-game sound effects");
  gm_mgWaveVolume.mg_pmgUp = &gm_mgAudioAPITrigger;
  gm_mgWaveVolume.mg_pmgDown = &gm_mgMPEGVolume;
  gm_mgWaveVolume.mg_pOnSliderChange = NULL;
  gm_mgWaveVolume.mg_pActivatedFunction = NULL;
  AddChild(&gm_mgWaveVolume);

  gm_mgMPEGVolume.mg_boxOnScreen = BoxMediumRow(4);
  gm_mgMPEGVolume.SetText(LOCALIZE("MUSIC VOLUME"));
  gm_mgMPEGVolume.mg_strTip = LOCALIZE("adjust volume of in-game music");
  gm_mgMPEGVolume.mg_pmgUp = &gm_mgWaveVolume;
  gm_mgMPEGVolume.mg_pmgDown = &gm_mgApply;
  gm_mgMPEGVolume.mg_pOnSliderChange = NULL;
  gm_mgMPEGVolume.mg_pActivatedFunction = NULL;
  AddChild(&gm_mgMPEGVolume);

  gm_mgApply.mg_bfsFontSize = BFS_LARGE;
  gm_mgApply.mg_boxOnScreen = BoxBigRow(4);
  gm_mgApply.SetText(LOCALIZE("APPLY"));
  gm_mgApply.mg_strTip = LOCALIZE("activate selected options");
  AddChild(&gm_mgApply);
  gm_mgApply.mg_pmgUp = &gm_mgMPEGVolume;
  gm_mgApply.mg_pmgDown = &gm_mgAudioAutoTrigger;
  gm_mgApply.mg_pActivatedFunction = NULL;
}

void CAudioOptionsMenu::StartMenu(void) {
  RefreshSoundFormat();
  CGameMenu::StartMenu();
}