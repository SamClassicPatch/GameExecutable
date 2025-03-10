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

#ifndef SE_INCL_MENUPRINTING_H
#define SE_INCL_MENUPRINTING_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

FLOATaabbox2D BoxTitle(void);
FLOATaabbox2D BoxVersion(void);
FLOATaabbox2D BoxBigRow(FLOAT fRow);
FLOATaabbox2D BoxBigLeft(FLOAT fRow);
FLOATaabbox2D BoxBigRight(FLOAT fRow);
FLOATaabbox2D BoxSaveLoad(FLOAT fRow);
FLOATaabbox2D BoxMediumRow(FLOAT fRow);
FLOATaabbox2D BoxMediumLeft(FLOAT fRow);
FLOATaabbox2D BoxPlayerSwitch(FLOAT fRow);
FLOATaabbox2D BoxPlayerEdit(FLOAT fRow);
FLOATaabbox2D BoxMediumMiddle(FLOAT fRow);
FLOATaabbox2D BoxMediumRight(FLOAT fRow);

// [Cecil] Height adjustment for popup boxes
FLOATaabbox2D BoxPopup(FLOAT fHeight);
FLOATaabbox2D BoxPopupLabel(FLOAT fHeight);
FLOATaabbox2D BoxPopupYesLarge(FLOAT fHeight);
FLOATaabbox2D BoxPopupNoLarge(FLOAT fHeight);
FLOATaabbox2D BoxPopupYesSmall(FLOAT fHeight);
FLOATaabbox2D BoxPopupNoSmall(FLOAT fHeight);

FLOATaabbox2D BoxInfoTable(INDEX iTable);
FLOATaabbox2D BoxChangePlayer(INDEX iTable, INDEX iButton);
FLOATaabbox2D BoxKeyRow(FLOAT fRow);
FLOATaabbox2D BoxArrow(enum ArrowDir ad);
FLOATaabbox2D BoxScrollbarArrow(ArrowDir ad); // [Cecil]
FLOATaabbox2D BoxScrollbar(void); // [Cecil]
FLOATaabbox2D BoxBack(void);
FLOATaabbox2D BoxNext(void);
FLOATaabbox2D BoxLeftColumn(FLOAT fRow);
FLOATaabbox2D BoxRightColumn(FLOAT fRow); // [Cecil]
FLOATaabbox2D BoxNoUp(FLOAT fRow);
FLOATaabbox2D BoxNoDown(FLOAT fRow);
FLOATaabbox2D BoxPlayerModel(void);
FLOATaabbox2D BoxPlayerModelName(void);
PIXaabbox2D FloatBoxToPixBox(CDrawPort *pdp, const FLOATaabbox2D &boxF);
FLOATaabbox2D PixBoxToFloatBox(CDrawPort *pdp, const PIXaabbox2D &boxP);
void SetFontTitle(CDrawPort *pdp);

// [Cecil] Added text scale
void SetFontBig(CDrawPort *pdp, FLOAT fScale);
void SetFontMedium(CDrawPort *pdp, FLOAT fScale);
void SetFontSmall(CDrawPort *pdp, FLOAT fScale);

#endif /* include-once check. */
