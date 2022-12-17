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
#include "LevelInfo.h"
#include "VarList.h"
#include "MGButton.h"

extern CSoundData *_psdPress;

extern CMenuGadget *_pmgLastActivatedGadget;

CMGButton::CMGButton(void) {
  mg_pActivatedFunction = NULL;
  mg_iIndex = 0;
  mg_iCenterI = 0;
  mg_iTextMode = 1;
  mg_bfsFontSize = BFS_MEDIUM;
  mg_iCursorPos = -1;
  mg_bRectangle = FALSE;
  mg_bMental = FALSE;
  mg_bEditing = FALSE;
  mg_bHighlighted = FALSE;
}

void CMGButton::OnActivate(void) {
  if (mg_pActivatedFunction != NULL && mg_bEnabled) {
    PlayMenuSound(_psdPress);
    IFeel_PlayEffect("Menu_press");
    _pmgLastActivatedGadget = this;
    (*mg_pActivatedFunction)();
  }
}

void CMGButton::Render(CDrawPort *pdp) {
  // [Cecil] Replaced if-else blocks with switch-case
  switch (mg_bfsFontSize)
  {
    default:
    case BFS_SMALL: {
      ASSERT(mg_bfsFontSize == BFS_SMALL);
      SetFontSmall(pdp);
    } break;

    case BFS_MEDIUM: {
      SetFontMedium(pdp);
    } break;

    case BFS_LARGE: {
      SetFontBig(pdp);
    } break;
  }

  pdp->SetTextMode(mg_iTextMode);

  // Convert to pixel box
  const PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);

  // [Cecil] Check if there's a button label
  const BOOL bLabel = (GetName() != "");

  // Get current colors
  COLOR col = GetCurrentColor();

  if (mg_bEditing) {
    col = LCDGetColor(C_GREEN | 0xFF, "editing");
  }

  COLOR colRectangle = col;

  // Get highlighted colors
  if (mg_bHighlighted) {
    col = LCDGetColor(C_WHITE | 0xFF, "hilited");

    if (!mg_bFocused) {
      colRectangle = LCDGetColor(C_WHITE | 0xFF, "hilited rectangle");
    }
  }

  // Fade in and out
  if (mg_bMental) {
    FLOAT tmIn = 0.2f;
    FLOAT tmOut = 1.0f;
    FLOAT tmFade = 0.1f;
    FLOAT tmExist = tmFade + tmIn + tmFade;
    FLOAT tmTotal = tmFade + tmIn + tmFade + tmOut;

    FLOAT tmTime = _pTimer->GetHighPrecisionTimer().GetSeconds();
    FLOAT fFactor = 1;

    if (tmTime > 0.1f) {
      tmTime = fmod(tmTime, tmTotal);
      fFactor = CalculateRatio(tmTime, 0, tmExist, tmFade / tmExist, tmFade / tmExist);
    }

    col = (col & ~0xFF) | INDEX(0xFF * fFactor);
  }

  // Draw rectangular border
  if (mg_bRectangle) {
    const PIX pixLeft = box.Min()(1);
    const PIX pixUp = box.Min()(2) - 3;
    const PIX pixWidth = box.Size()(1) + 1;
    const PIX pixHeight = box.Size()(2);

    pdp->DrawBorder(pixLeft, pixUp, pixWidth, pixHeight, colRectangle);
  }

  // Draw border for textbox editing
  if (mg_bEditing) {
    PIX pixLeft = box.Min()(1);
    PIX pixUp = box.Min()(2) - 3;
    PIX pixWidth = box.Size()(1) + 1;
    PIX pixHeight = box.Size()(2);

    // Shift to the right side if there's a button label
    if (bLabel) {
      pixLeft = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioR;
      pixWidth = box.Size()(1) * _fGadgetSideRatioL + 1;
    }

    pdp->Fill(pixLeft, pixUp, pixWidth, pixHeight, LCDGetColor(C_dGREEN | 0x40, "edit fill"));
  }

  INDEX iCursor = mg_iCursorPos;

  // If there's a button label
  if (bLabel) {
    PIX pixIL = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioL;
    PIX pixIR = box.Max()(1) - box.Size()(1) * _fGadgetSideRatioL;
    PIX pixJ = box.Min()(2);

    // Put label on the left and text on the right
    pdp->PutTextR(GetName(), pixIL, pixJ, col);
    pdp->PutText(GetText(), pixIR, pixJ, col);

  // If no button label
  } else {
    CTString str = GetText();

    // If using monospace font
    if (pdp->dp_FontData->fd_bFixedWidth) {
      // Undecorate the string
      str = str.Undecorated();

      const INDEX iMaxLen = ClampDn(box.Size()(1) / (pdp->dp_pixTextCharSpacing + pdp->dp_FontData->fd_pixCharWidth), 1L);

      // Trim the string if the cursor is past the maximum length
      if (iCursor >= iMaxLen) {
        str.TrimRight(iCursor);
        str.TrimLeft(iMaxLen);
        iCursor = iMaxLen;

      // Trim the string to the maximum length
      } else {
        str.TrimRight(iMaxLen);
      }
    }

    // Align text according to the centering
    if (mg_iCenterI == -1) {
      pdp->PutText(str, box.Min()(1), box.Min()(2), col);

    } else if (mg_iCenterI == +1) {
      pdp->PutTextR(str, box.Max()(1), box.Min()(2), col);

    } else {
      pdp->PutTextC(str, box.Center()(1), box.Min()(2), col);
    }
  }

  // Add blinking cursor to the string if editing
  if (mg_bEditing && ULONG(_pTimer->GetRealTimeTick() * 2.0f) & 1) {
    PIX pixX = box.Min()(1) + GetCharOffset(pdp, iCursor);
    PIX pixY = box.Min()(2);

    // Shift to the right side if there's a button label
    if (bLabel) {
      pixX += box.Size()(1) * _fGadgetSideRatioR;
    }

    if (!pdp->dp_FontData->fd_bFixedWidth) {
      pixY -= pdp->dp_fTextScaling * 2;
    }

    pdp->PutText("|", pixX, pixY, LCDGetColor(C_WHITE | 0xFF, "editing cursor"));
  }
}

PIX CMGButton::GetCharOffset(CDrawPort *pdp, INDEX iCharNo) {
  if (pdp->dp_FontData->fd_bFixedWidth) {
    return (pdp->dp_FontData->fd_pixCharWidth + pdp->dp_pixTextCharSpacing) * (iCharNo - 0.5f);
  }
  CTString strCut(GetText());
  strCut.TrimLeft(GetText().Length() - iCharNo);
  PIX pixFullWidth = pdp->GetTextWidth(GetText());
  PIX pixCutWidth = pdp->GetTextWidth(strCut);
  // !!!! not implemented for different centering
  return pixFullWidth - pixCutWidth;
}