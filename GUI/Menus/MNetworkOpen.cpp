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
#include "MNetworkOpen.h"

void CNetworkOpenMenu::Initialize_t(void) {
  // intialize network join menu
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(LOCALIZE("JOIN"));
  AddChild(&gm_mgTitle);

  gm_mgAddressLabel.SetText(LOCALIZE("Address:"));
  gm_mgAddressLabel.mg_boxOnScreen = BoxMediumLeft(1);
  gm_mgAddressLabel.mg_iCenterI = -1;
  AddChild(&gm_mgAddressLabel);

  gm_mgAddress.SetText(GetGameAPI()->JoinAddress());
  gm_mgAddress.mg_ctMaxStringLen = 20;
  gm_mgAddress.mg_pstrToChange = &GetGameAPI()->JoinAddress();
  gm_mgAddress.mg_boxOnScreen = BoxMediumMiddle(1);
  gm_mgAddress.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgAddress.mg_iCenterI = -1;
  gm_mgAddress.mg_pmgUp = &gm_mgJoin;
  gm_mgAddress.mg_pmgDown = &gm_mgPort;
  gm_mgAddress.mg_strTip = LOCALIZE("specify server address");
  AddChild(&gm_mgAddress);

  gm_mgPortLabel.SetText(LOCALIZE("Port:"));
  gm_mgPortLabel.mg_boxOnScreen = BoxMediumLeft(2);
  gm_mgPortLabel.mg_iCenterI = -1;
  AddChild(&gm_mgPortLabel);

  gm_mgPort.SetText("");
  gm_mgPort.mg_ctMaxStringLen = 10;
  gm_mgPort.mg_pstrToChange = &gm_strPort;
  gm_mgPort.mg_boxOnScreen = BoxMediumMiddle(2);
  gm_mgPort.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgPort.mg_iCenterI = -1;
  gm_mgPort.mg_pmgUp = &gm_mgAddress;
  gm_mgPort.mg_pmgDown = &gm_mgJoin;
  gm_mgPort.mg_strTip = LOCALIZE("specify server address");
  AddChild(&gm_mgPort);

  gm_mgJoin.mg_boxOnScreen = BoxMediumMiddle(3);
  gm_mgJoin.mg_pmgUp = &gm_mgPort;
  gm_mgJoin.mg_pmgDown = &gm_mgAddress;
  gm_mgJoin.SetText(LOCALIZE("Join"));
  AddChild(&gm_mgJoin);
  gm_mgJoin.mg_pActivatedFunction = NULL;
}

void CNetworkOpenMenu::StartMenu(void) {
  gm_strPort = _pShell->GetValue("net_iPort");
  gm_mgPort.SetText(gm_strPort);
}

void CNetworkOpenMenu::EndMenu(void) {
  _pShell->SetValue("net_iPort", gm_strPort);
}