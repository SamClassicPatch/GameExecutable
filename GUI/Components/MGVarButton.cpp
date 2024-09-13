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
#include "VarList.h"
#include "MGVarButton.h"

extern PIX _pixCursorPosI;
extern PIX _pixCursorPosJ;

BOOL CMGVarButton::IsSeparator(void) {
  if (mg_pvsVar == NULL) {
    return FALSE;
  }

  // [Cecil] Check the separator type
  return (mg_pvsVar->vs_eType == CVarSetting::E_SEPARATOR);
}

BOOL CMGVarButton::IsEnabled(void) {
  return (_gmRunningGameMode == GM_NONE || mg_pvsVar == NULL || mg_pvsVar->vs_bCanChangeInGame);
}

// return slider position on scren
PIXaabbox2D CMGVarButton::GetSliderBox(INDEX iSliderType) {
  extern CDrawPort *pdp;
  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIR = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioR;
  PIX pixJ = box.Min()(2);

  PIX pixISize, pixJSize;

  // [Cecil] Big fill slider
  if (iSliderType == CVarSetting::SLD_BIGFILL) {
    pixJSize = box.Size()(2) * 0.95f;
    pixISize = box.Size()(1) * _fGadgetSideRatioL;

  } else {
    pixISize = box.Size()(1) * 0.13f;
    pixJSize = box.Size()(2);
  }

  return PIXaabbox2D(PIX2D(pixIR, pixJ + 1), PIX2D(pixIR + pixISize - 4, pixJ + pixJSize - 6));
}

extern BOOL _bVarChanged;
BOOL CMGVarButton::OnKeyDown(int iVKey)
{
  if (mg_pvsVar == NULL || mg_pvsVar->vs_eType == CVarSetting::E_SEPARATOR || !mg_pvsVar->Validate() || !mg_bEnabled) {
    // [Cecil] CMenuGadget::OnKeyDown() would call CMGEdit::OnActivate(), which shouldn't happen
    return (iVKey == VK_RETURN || iVKey == VK_LBUTTON);
  }

  // [Cecil] Editing the textbox
  if (mg_bEditing) {
    return CMGEdit::OnKeyDown(iVKey);
  }

  // [Cecil] Toggleable setting
  if (mg_pvsVar->vs_eType == CVarSetting::E_TOGGLE)
  {
    // handle slider
    if (mg_pvsVar->vs_eSlider != CVarSetting::SLD_NOSLIDER && !mg_pvsVar->vs_bCustom) {
      // ignore RMB
      if (iVKey == VK_RBUTTON) {
        return TRUE;
      }

      // handle LMB
      if (iVKey == VK_LBUTTON) {
        // get position of slider box on screen
        PIXaabbox2D boxSlider = GetSliderBox(mg_pvsVar->vs_eSlider);
        // if mouse is within
        if (boxSlider >= PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
          // set new position exactly where mouse pointer is
          mg_pvsVar->vs_iValue = (FLOAT)(_pixCursorPosI - boxSlider.Min()(1)) / boxSlider.Size()(1) * (mg_pvsVar->vs_ctValues);
          _bVarChanged = TRUE;
        }

        // handled
        return TRUE;
      }
    }

  // [Cecil] Button setting
  } else if (mg_pvsVar->vs_eType == CVarSetting::E_BUTTON) {
    // Enter another option config on click
    if (iVKey == VK_RETURN || iVKey == VK_LBUTTON) {
      // Copy the string from the setting
      const CTString strConfig = mg_pvsVar->vs_strSchedule;

      CVarMenu &gmCurrent = _pGUIM->gmVarMenu;
      FlushVarSettings(FALSE);
      gmCurrent.EndMenu();

      gmCurrent.gm_fnmMenuCFG = strConfig;
      gmCurrent.StartMenu();

      extern CSoundData *_psdPress;
      PlayMenuSound(_psdPress);
      return TRUE;
    }
  }

  if (iVKey == VK_RETURN) {
    FlushVarSettings(TRUE);
    MenuGoToParent();
    return TRUE;
  }

  // [Cecil] Different types
  switch (mg_pvsVar->vs_eType) {
    // Toggle values
    case CVarSetting::E_TOGGLE: {
      // Select next value
      if (iVKey == VK_LBUTTON || iVKey == VK_RIGHT) {
        if (mg_pvsVar != NULL) {
          INDEX iOldValue = mg_pvsVar->vs_iValue;
          mg_pvsVar->vs_iValue++;

          if (mg_pvsVar->vs_iValue >= mg_pvsVar->vs_ctValues) {
            // wrap non-sliders, clamp sliders
            if (mg_pvsVar->vs_eSlider != CVarSetting::SLD_NOSLIDER) {
              mg_pvsVar->vs_iValue = mg_pvsVar->vs_ctValues - 1L;
            } else {
              mg_pvsVar->vs_iValue = 0;
            }
          }

          if (iOldValue != mg_pvsVar->vs_iValue) {
            _bVarChanged = TRUE;
            mg_pvsVar->vs_bCustom = FALSE;
            mg_pvsVar->Validate();
          }
        }
        return TRUE;

      // Select previous value
      } else if (iVKey == VK_LEFT || iVKey == VK_RBUTTON) {
        if (mg_pvsVar != NULL) {
          INDEX iOldValue = mg_pvsVar->vs_iValue;
          mg_pvsVar->vs_iValue--;

          if (mg_pvsVar->vs_iValue < 0) {
            // wrap non-sliders, clamp sliders
            if (mg_pvsVar->vs_eSlider != CVarSetting::SLD_NOSLIDER) {
              mg_pvsVar->vs_iValue = 0;
            } else {
              mg_pvsVar->vs_iValue = mg_pvsVar->vs_ctValues - 1L;
            }
          }

          if (iOldValue != mg_pvsVar->vs_iValue) {
            _bVarChanged = TRUE;
            mg_pvsVar->vs_bCustom = FALSE;
            mg_pvsVar->Validate();
          }
        }
        return TRUE;
      }
    } break;

    // Reset editing value
    case CVarSetting::E_TEXTBOX: {
      OnStringCanceled();
    } break;
  }

  // not handled
  return CMenuGadget::OnKeyDown(iVKey);
}

// [Cecil] Adjust the slider by holding a button
BOOL CMGVarButton::OnMouseHeld(int iVKey)
{
  if (iVKey != VK_LBUTTON) return FALSE;

  // Forward the key if it's a toggleable slider without a custom value
  if (mg_pvsVar != NULL && mg_pvsVar->vs_eType == CVarSetting::E_TOGGLE
   && mg_pvsVar->vs_eSlider != CVarSetting::SLD_NOSLIDER && !mg_pvsVar->vs_bCustom)
  {
    OnKeyDown(iVKey);
  }

  return FALSE;
};

void CMGVarButton::Render(CDrawPort *pdp) {
  if (mg_pvsVar == NULL) {
    return;
  }

  SetFontMedium(pdp, mg_fTextScale);

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioL;
  PIX pixIR = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioR;
  PIX pixIC = box.Center()(1);
  PIX pixJ = box.Min()(2);

  // [Cecil] Different types
  switch (mg_pvsVar->vs_eType) {
    // Separator
    case CVarSetting::E_SEPARATOR: {
      mg_bEnabled = FALSE;
      COLOR col = _pGame->LCDGetColor(C_WHITE | 255, "separator");

      CTString strText = mg_pvsVar->vs_strName;
      pdp->PutTextC(strText, pixIC, pixJ, col);
    } break;

    // Toggleable
    case CVarSetting::E_TOGGLE: {
      if (mg_pvsVar->Validate()) {
        // check whether the variable is disabled
        if (mg_pvsVar->vs_strFilter != "") {
          mg_bEnabled = _pShell->GetINDEX(mg_pvsVar->vs_strFilter);
        }

        COLOR col = GetCurrentColor();
        pdp->PutTextR(mg_pvsVar->vs_strName, pixIL, pixJ, col);

        // custom is by default
        CTString strText = LOCALIZE("Custom");
        BOOL bCenteredText = FALSE;

        // Not a custom value
        if (!mg_pvsVar->vs_bCustom) {
          strText = mg_pvsVar->vs_astrTexts[mg_pvsVar->vs_iValue];

          const FLOAT fFactor = FLOAT(mg_pvsVar->vs_iValue + 1) / (FLOAT)mg_pvsVar->vs_ctValues;

          PIX pixISize = box.Size()(1) * 0.13f;
          PIX pixJSize = box.Size()(2);

          // need slider?
          if (mg_pvsVar->vs_eSlider == CVarSetting::SLD_FILL) {
            // draw box around slider
            _pGame->LCDDrawBox(0, -1, PIXaabbox2D(PIX2D(pixIR, pixJ + 1), PIX2D(pixIR + pixISize - 4, pixJ + pixJSize - 6)),
                               _pGame->LCDGetColor(C_GREEN | 255, "slider box"));

            // fill slider
            pdp->Fill(pixIR + 1, pixJ + 2, (pixISize - 6) * fFactor, pixJSize - 9, col);

            // move text printout to the right of slider
            pixIR += box.Size()(1) * 0.15f;

          // [Cecil] Big fill slider
          } else if (mg_pvsVar->vs_eSlider == CVarSetting::SLD_BIGFILL) {
            // get geometry
            pixJSize = box.Size()(2) * 0.95f;
            pixISize = box.Size()(1) * _fGadgetSideRatioL;

            // Draw box around the slider
            _pGame->LCDDrawBox(0, -1, PIXaabbox2D(PIX2D(pixIR + 1, pixJ), PIX2D(pixIR + pixISize - 2, pixJ + pixJSize - 2)),
                                _pGame->LCDGetColor(C_GREEN | 255, "slider box"));

            // Fiil slider
            pdp->Fill(pixIR + 2, pixJ + 1, (pixISize - 5) * fFactor, (pixJSize - 4), col);

            // Move text in the middle of the box
            pixIR += pixISize / 2;
            pixJ += 2;
            bCenteredText = TRUE;

          // ratio slider
          } else if (mg_pvsVar->vs_eSlider == CVarSetting::SLD_RATIO) {
            // draw box around slider
            _pGame->LCDDrawBox(0, -1, PIXaabbox2D(PIX2D(pixIR, pixJ + 1), PIX2D(pixIR + pixISize - 4, pixJ + pixJSize - 6)),
                                _pGame->LCDGetColor(C_GREEN | 255, "slider box"));

            FLOAT fUnitWidth = (FLOAT)(pixISize - 5) / mg_pvsVar->vs_ctValues;
            pdp->Fill(pixIR + 1 + (mg_pvsVar->vs_iValue * fUnitWidth), pixJ + 2, fUnitWidth, pixJSize - 9, col);

            // move text printout to the right of slider
            pixIR += box.Size()(1) * 0.15f;
          }
        }

        // Write value text
        if (bCenteredText) {
          pdp->PutTextC(strText, pixIR, pixJ, col);
        } else {
          pdp->PutText(strText, pixIR, pixJ, col);
        }
      }
    } break;

    // Textbox
    case CVarSetting::E_TEXTBOX: {
      CMGEdit::Render(pdp);
    } break;

    // Button
    case CVarSetting::E_BUTTON: {
      pdp->PutTextC(mg_pvsVar->vs_strName, pixIC, pixJ, GetCurrentColor());
    } break;
  }
}

// [Cecil] Change strings
void CMGVarButton::OnStringChanged(void) {
  // No textbox attached
  if (mg_pvsVar == NULL || mg_pvsVar->vs_eType != CVarSetting::E_TEXTBOX) {
    return;
  }

  // If new hash differs from the old one, mark as changed
  ULONG ulOldHash = static_cast<ULONG>(mg_pvsVar->vs_iOrgValue);

  if (mg_pvsVar->vs_strValue.GetHash() != ulOldHash) {
    _bVarChanged = TRUE;
  }
};

void CMGVarButton::OnStringCanceled(void) {
  // Restore string from the setting
  SetText(mg_pvsVar->vs_strValue);
};
