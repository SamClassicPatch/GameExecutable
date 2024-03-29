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
#include "Credits.h"

static CStringStack _astrCredits;
static BOOL _bCreditsOn = FALSE;

static PIX pixW = 0;
static PIX pixH = 0;
static PIX pixJ = 0;
static FLOAT fResolutionScaling;
static PIX pixLineHeight;

static CTString strEmpty;
static FLOAT _fSpeed = 2.0f;

// [Cecil] Current credits offset
static FLOAT _fOffset = 0.0f;

static BOOL _bUseRealTime = FALSE;
static CTimerValue _tvStart;
static FLOAT _tmStart;

// [Cecil] When the last update occurred
static FLOAT _tmLast = 0.0f;

FLOAT GetTime(void) {
  if (!_bUseRealTime) {
    return _pTimer->GetLerpedCurrentTick() - _tmStart;
  }

  return (_pTimer->GetHighPrecisionTimer() - _tvStart).GetSeconds();
}

void PrintOneLine(CDrawPort *pdp, const CTString &strText) {
  pdp->SetTextScaling(fResolutionScaling);
  pdp->SetTextAspect(1.0f);
  pdp->PutTextC(strText, pixW / 2, pixJ, C_WHITE | 255);

  pixJ += pixLineHeight;
}

static void LoadOneFile(const CTFileName &fnm) {
  try {
    // open the file
    CTFileStream strm;
    strm.Open_t(fnm);

    // count number of lines
    INDEX ctLines = 0;

    while (!strm.AtEOF()) {
      CTString strLine;
      strm.GetLine_t(strLine);

      ctLines++;
    }

    strm.SetPos_t(0);

    // allocate that much
    CTString *astr = _astrCredits.Push(ctLines);

    // load all lines
    for (INDEX iLine = 0; iLine < ctLines && !strm.AtEOF(); iLine++) {
      strm.GetLine_t(astr[iLine]);
    }

    strm.Close();

    _bCreditsOn = TRUE;

  } catch (char *strError) {
    CPrintF("%s\n", strError);
  }
}

// Turn credits on
void Credits_On(INDEX iType) {
  if (_bCreditsOn) {
    Credits_Off();
  }

  _astrCredits.PopAll();

  if (iType == 1) {
    _fSpeed = 1.0f;
    LoadOneFile(CTFILENAME("Data\\Intro.txt"));

  } else if (iType == 2) {
    _fSpeed = 2.0f;
    LoadOneFile(CTFILENAME("Data\\Credits.txt"));
    LoadOneFile(CTFILENAME("Data\\Credits_End.txt"));

  } else {
    _fSpeed = 2.0f;
    LoadOneFile(CTFILENAME("Data\\Credits.txt"));
  }

  // if some file was loaded
  if (_bCreditsOn) {
    // remember start time
    if (iType == 1 || iType == 2) {
      _bUseRealTime = FALSE;
      _tmStart = _pTimer->GetLerpedCurrentTick();

    } else {
      _bUseRealTime = TRUE;
      _tvStart = _pTimer->GetHighPrecisionTimer();
    }

    // [Cecil] Reset
    _tmLast = 0.0f;
    _fOffset = 0.0f;
  }
}

// Turn credits off
void Credits_Off(void) {
  if (!_bCreditsOn) {
    return;
  }
  _bCreditsOn = FALSE;
  _astrCredits.Clear();
}

// Render credits to given drawport
FLOAT Credits_Render(CDrawPort *pdp) {
  if (!_bCreditsOn) {
    return 0;
  }

  CDrawPort dpWide(pdp, TRUE);

  pdp->Unlock();
  dpWide.Lock();

  FLOAT fTime = GetTime();

  // [Cecil] Difference since last update
  const FLOAT fTimeDiff = fTime - _tmLast;
  _tmLast = fTime;

  pixW = dpWide.GetWidth();
  pixH = dpWide.GetHeight();

  // [Cecil] Use height instead of width
  fResolutionScaling = (FLOAT)pixH / 480.0f;

  dpWide.SetFont(_pfdDisplayFont);
  pixLineHeight = floor(20 * fResolutionScaling);

  // [Cecil] Add offset each frame
  _fOffset += fTimeDiff * _fSpeed;

  INDEX ctLinesOnScreen = pixH / pixLineHeight;
  INDEX iLine1 = _fOffset;

  pixJ = iLine1 * pixLineHeight - _fOffset * pixLineHeight;
  iLine1 -= ctLinesOnScreen;

  INDEX ctLines = _astrCredits.Count();
  BOOL bOver = TRUE;

  for (INDEX i = iLine1; i < iLine1 + ctLinesOnScreen + 1; i++) {
    CTString *pstr = &strEmpty;
    INDEX iLine = i;

    if (iLine >= 0 && iLine < ctLines) {
      pstr = &_astrCredits[iLine];
      bOver = FALSE;
    }

    PrintOneLine(&dpWide, *pstr);
  }

  dpWide.Unlock();
  pdp->Lock();

  if (bOver) {
    return 0;

  } else if (ctLines - iLine1 < ctLinesOnScreen) {
    return FLOAT(ctLines - iLine1) / ctLinesOnScreen;

  } else {
    return 1;
  }
}

// [Cecil] Change scroll speed
void Credits_Speed(FLOAT fSetSpeed, INDEX iDir) {
  if (iDir == 0) {
    _fSpeed = fSetSpeed;

  } else if (iDir < 0) {
    _fSpeed -= fSetSpeed;

  } else if (iDir > 0) {
    _fSpeed += fSetSpeed;
  }

  _fSpeed = Clamp(_fSpeed, 0.0f, 20.0f);
};
