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
#include "MCustomizeAxis.h"

// [Cecil] Select new axis action
static void ChangeAxis(void) {
  // Apply current axis
  _pGUIM->gmCustomizeAxisMenu.ApplyActionSettings();

  // Select new axis
  const INDEX iAxis = ((CMGButton *)_pmgLastActivatedGadget)->mg_iIndex;
  _pGUIM->gmCustomizeAxisMenu.gm_iAxisAction = iAxis;

  _pGUIM->gmCustomizeAxisMenu.ObtainActionSettings();
};

// [Cecil] How much to shift axis action tabs by
#define AXIS_ACTION_TAB_SHIFT 3

static FLOATaabbox2D BoxAxisAction(INDEX i) {
  static const FLOAT fSizeW = 0.15f;
  static const FLOAT fSizeH = 0.03f;
  static const FLOAT fFirstShift = 0.5f - fSizeW * 2;

  const FLOAT fRowShift = fSizeW * 2 * (i % 3);
  const FLOAT fColShift = fSizeH * 2 * (i / 3);

  const FLOAT fX = fFirstShift + fRowShift;
  const FLOAT fY = 0.23f + fColShift;

  return FLOATaabbox2D(
    FLOAT2D(fX - fSizeW + 0.005f, fY - fSizeH + 0.005f),
    FLOAT2D(fX + fSizeW - 0.010f, fY + fSizeH - 0.010f));
};

void CCustomizeAxisMenu::Initialize_t(void) {
  gm_strName = "CustomizeAxis";
  gm_pmgSelectedByDefault = &gm_amgAxes[0];
  gm_iAxisAction = AXIS_ACTION_TAB_SHIFT; // [Cecil]

  // intialize axis menu
  gm_mgTitle.SetName(LOCALIZE("CUSTOMIZE AXIS"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  // [Cecil] Add axis action tabs
  for (INDEX iAxisTab = 0; iAxisTab < AXIS_ACTIONS_CT; iAxisTab++) {
    CMGButton &mgTab = gm_amgAxes[iAxisTab];
    const INDEX iShiftTab = (iAxisTab + AXIS_ACTION_TAB_SHIFT) % AXIS_ACTIONS_CT;

    // Tab identity
    mgTab.mg_iIndex = iShiftTab;
    mgTab.SetText(TRANSV(GetGameAPI()->GetAxisName(iShiftTab)));
    mgTab.mg_strTip = LOCALIZE("choose action to customize");

    // Tab appearance
    mgTab.mg_bfsFontSize = BFS_MEDIUM;
    mgTab.mg_iCenterI = mgTab.mg_iCenterJ = 0;
    mgTab.mg_boxOnScreen = BoxAxisAction(iAxisTab);
    mgTab.mg_bRectangle = TRUE;

    mgTab.mg_pmgLeft  = &gm_amgAxes[(iAxisTab + AXIS_ACTIONS_CT - 1) % AXIS_ACTIONS_CT];
    mgTab.mg_pmgRight = &gm_amgAxes[(iAxisTab                   + 1) % AXIS_ACTIONS_CT];
    mgTab.mg_pmgUp    = &gm_amgAxes[(iAxisTab + AXIS_ACTIONS_CT - 3) % AXIS_ACTIONS_CT];
    mgTab.mg_pmgDown  = &gm_amgAxes[(iAxisTab                   + 3) % AXIS_ACTIONS_CT];

    // Top tabs wrap to the bottom button
    if (iAxisTab < 3) {
      mgTab.mg_pmgUp = &gm_mgSmoothTrigger;

    // Bottom tabs go further down
    } else if (iAxisTab >= AXIS_ACTIONS_CT - 3) {
      mgTab.mg_pmgDown = &gm_mgMountedTrigger;
    }

    mgTab.mg_pActivatedFunction = &ChangeAxis;
    AddChild(&mgTab);
  }

  TRIGGER_MG(gm_mgMountedTrigger, 5, gm_amgAxes[7], gm_mgSensitivity, LOCALIZE("MOUNTED TO"), astrNoYes);
  gm_mgMountedTrigger.mg_strTip = LOCALIZE("choose controller axis that will perform the action");

  INDEX ctAxis = _pInput->GetAvailableAxisCount();
  gm_mgMountedTrigger.mg_astrTexts = new CTString[ctAxis];
  gm_mgMountedTrigger.mg_ctTexts = ctAxis;

  // for all axis actions that can be mounted
  for (INDEX iAxis = 0; iAxis < ctAxis; iAxis++) {
    gm_mgMountedTrigger.mg_astrTexts[iAxis] = _pInput->GetAxisTransName(iAxis);
  }

  gm_mgSensitivity.mg_boxOnScreen = BoxMediumRow(7);
  gm_mgSensitivity.SetText(LOCALIZE("SENSITIVITY"));
  gm_mgSensitivity.mg_pmgUp = &gm_mgMountedTrigger;
  gm_mgSensitivity.mg_pmgDown = &gm_mgDeadzone;
  AddChild(&gm_mgSensitivity);
  gm_mgSensitivity.mg_strTip = LOCALIZE("set sensitivity for this axis");

  gm_mgDeadzone.mg_boxOnScreen = BoxMediumRow(8);
  gm_mgDeadzone.SetText(LOCALIZE("DEAD ZONE"));
  gm_mgDeadzone.mg_pmgUp = &gm_mgSensitivity;
  gm_mgDeadzone.mg_pmgDown = &gm_mgInvertTrigger;
  AddChild(&gm_mgDeadzone);
  gm_mgDeadzone.mg_strTip = LOCALIZE("set dead zone for this axis");

  TRIGGER_MG(gm_mgInvertTrigger, 9, gm_mgDeadzone, gm_mgRelativeTrigger, LOCALIZE("INVERTED"), astrNoYes);
  gm_mgInvertTrigger.mg_strTip = LOCALIZE("choose whether to invert this axis or not");

  TRIGGER_MG(gm_mgRelativeTrigger, 10, gm_mgInvertTrigger, gm_mgSmoothTrigger, LOCALIZE("RELATIVE"), astrNoYes);
  gm_mgRelativeTrigger.mg_strTip = LOCALIZE("select relative or absolute axis reading");

  TRIGGER_MG(gm_mgSmoothTrigger, 11, gm_mgRelativeTrigger, gm_amgAxes[1], LOCALIZE("SMOOTH"), astrNoYes);
  gm_mgSmoothTrigger.mg_strTip = LOCALIZE("turn this on to filter readings on this axis");
}

CCustomizeAxisMenu::~CCustomizeAxisMenu(void) {
  delete[] gm_mgMountedTrigger.mg_astrTexts;
}

void CCustomizeAxisMenu::ObtainActionSettings(void) {
  ControlsMenuOn();
  CControls &ctrls = *GetGameAPI()->GetControls();
  INDEX iSelectedAction = gm_iAxisAction;
  INDEX iMountedAxis = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_iAxisAction;

  // [Cecil] Select current tab
  for (INDEX iAxisTab = 0; iAxisTab < AXIS_ACTIONS_CT; iAxisTab++) {
    CMGButton &mgTab = gm_amgAxes[iAxisTab];
    mgTab.mg_bEnabled = (mgTab.mg_iIndex != iSelectedAction);
  }

  gm_mgMountedTrigger.mg_iSelected = iMountedAxis;

  gm_mgSensitivity.mg_iMinPos = 0;
  gm_mgSensitivity.mg_iMaxPos = 50;
  gm_mgSensitivity.mg_iCurPos = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity / 2;
  gm_mgSensitivity.ApplyCurrentPosition();

  gm_mgDeadzone.mg_iMinPos = 0;
  gm_mgDeadzone.mg_iMaxPos = 50;
  gm_mgDeadzone.mg_iCurPos = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone / 2;
  gm_mgDeadzone.ApplyCurrentPosition();

  gm_mgInvertTrigger.mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bInvert ? 1 : 0;
  gm_mgRelativeTrigger.mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bRelativeControler ? 1 : 0;
  gm_mgSmoothTrigger.mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bSmooth ? 1 : 0;

  gm_mgMountedTrigger.ApplyCurrentSelection();
  gm_mgInvertTrigger.ApplyCurrentSelection();
  gm_mgRelativeTrigger.ApplyCurrentSelection();
  gm_mgSmoothTrigger.ApplyCurrentSelection();
}

void CCustomizeAxisMenu::ApplyActionSettings(void) {
  CControls &ctrls = *GetGameAPI()->GetControls();
  INDEX iSelectedAction = gm_iAxisAction;
  INDEX iMountedAxis = gm_mgMountedTrigger.mg_iSelected;
  FLOAT fSensitivity =
    FLOAT(gm_mgSensitivity.mg_iCurPos - gm_mgSensitivity.mg_iMinPos) /
    FLOAT(gm_mgSensitivity.mg_iMaxPos - gm_mgSensitivity.mg_iMinPos) * 100.0f;
  FLOAT fDeadZone =
    FLOAT(gm_mgDeadzone.mg_iCurPos - gm_mgDeadzone.mg_iMinPos) /
    FLOAT(gm_mgDeadzone.mg_iMaxPos - gm_mgDeadzone.mg_iMinPos) * 100.0f;

  BOOL bInvert = gm_mgInvertTrigger.mg_iSelected != 0;
  BOOL bRelative = gm_mgRelativeTrigger.mg_iSelected != 0;
  BOOL bSmooth = gm_mgSmoothTrigger.mg_iSelected != 0;

  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_iAxisAction = iMountedAxis;
  if (INDEX(ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity) != INDEX(fSensitivity)) {
    ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity = fSensitivity;
  }
  if (INDEX(ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone) != INDEX(fDeadZone)) {
    ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone = fDeadZone;
  }
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bInvert = bInvert;
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bRelativeControler = bRelative;
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bSmooth = bSmooth;
  ctrls.CalculateInfluencesForAllAxis();

  ControlsMenuOff();
}

void CCustomizeAxisMenu::StartMenu(void) {
  ObtainActionSettings();
  CGameMenu::StartMenu();
}

void CCustomizeAxisMenu::EndMenu(void) {
  ApplyActionSettings();
  CGameMenu::EndMenu();
}

// [Cecil] Change to the menu
void CCustomizeAxisMenu::ChangeTo(void) {
  ChangeToMenu(&_pGUIM->gmCustomizeAxisMenu);
};
