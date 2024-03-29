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

#ifndef SE_INCL_MENU_GADGET_BUTTON_H
#define SE_INCL_MENU_GADGET_BUTTON_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "MenuGadget.h"

class CMGButton : public CMenuGadget {
  // [Cecil] Private strings
  private:
    CTString mg_strLabel; // for those that have labels separately from main text
    CTString mg_strText;

  public:
    INDEX mg_iCenterI, mg_iCenterJ; // [Cecil] Vertical centering
    FLOAT mg_fTextScale; // [Cecil] Text scaling
    enum ButtonFontSize mg_bfsFontSize;
    BOOL mg_bEditing;
    BOOL mg_bHighlighted;
    BOOL mg_bRectangle;
    BOOL mg_bMental;
    BOOL mg_bHiddenText; // [Cecil] Hide value when not editing
    INDEX mg_iTextMode;
    INDEX mg_iCursorPos;
    INDEX mg_iIndex;

    void (*mg_pActivatedFunction)(void);

    CMGButton(void);
    void OnActivate(void);
    void Render(CDrawPort *pdp);
    PIX GetCharOffset(CDrawPort *pdp, INDEX iCharNo);

    // [Cecil] Get node name
    virtual const CTString &GetName(void) const {
      return mg_strLabel;
    };
    
    // [Cecil] Set node name
    virtual void SetName(const CTString &strNew) {
      mg_strLabel = strNew;
    };

    // [Cecil] Get text
    virtual const CTString &GetText(void) const {
      return mg_strText;
    };
    
    // [Cecil] Set text
    virtual void SetText(const CTString &strNew) {
      mg_strText = strNew;
    };
};

#endif /* include-once check. */