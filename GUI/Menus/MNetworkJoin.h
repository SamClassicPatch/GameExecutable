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

#ifndef SE_INCL_GAME_MENU_NETWORKJOIN_H
#define SE_INCL_GAME_MENU_NETWORKJOIN_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGTitle.h"

class CNetworkJoinMenu : public CGameMenu {
  public:
    CMGTitle gm_mgTitle;
    CMGButton gm_mgLAN;
    CMGButton gm_mgNET;
    CMGButton gm_mgOpen;

    void Initialize_t(void);

    // [Cecil] Change to the menu
    static void ChangeTo(void);
};

#endif /* include-once check. */