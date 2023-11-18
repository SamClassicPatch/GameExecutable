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

// [Cecil] For listing files
#include <CoreLib/Interfaces/FileFunctions.h>

// [Cecil] Tabs of options
CStaticStackArray<CVarTab> _aTabs;

// [Cecil] List of opened configs to prevent recursion
static CDynamicContainer<CTFileStream> _cOpenedConfigs;

CTString _strFile;
INDEX _ctLines;

CTString GetNonEmptyLine_t(CTStream &strm) {
  FOREVER {
    if (strm.AtEOF()) {
      ThrowF_t(LOCALIZE("Unexpected end of file"));
    }

    CTString str;
    _ctLines++;

    strm.GetLine_t(str);
    str.TrimSpacesLeft();

    if (str.RemovePrefix("//")) { // skip comments
      continue;
    }

    if (str != "") {
      str.TrimSpacesRight();
      return str;
    }
  }
}

void TranslateLine(CTString &str) {
  str.TrimSpacesLeft();

  // [Cecil] "TTRS" in the EXE as is gets picked up by the Depend utility
  if (str.RemovePrefix(CTString("TT") + "RS")) {
    str.TrimSpacesLeft();
    str = TRANSV(str);
  }

  str.TrimSpacesLeft();
}

void FixupFileName_t(CTString &strFnm) {
  strFnm.TrimSpacesLeft();
  strFnm.TrimSpacesRight();

  // [Cecil] "TFNM" in the EXE as is gets picked up by the Depend utility
  if (!strFnm.RemovePrefix(CTString("TF") + "NM ")) {
    ThrowF_t(LOCALIZE("Expected %s%s before filename"), "TF", "NM");
  }
}

void CheckPVS_t(CVarSetting *pvs) {
  if (pvs == NULL) {
    ThrowF_t("Gadget expected");
  }
}

// [Cecil] All settings list argument
static void ParseCFG_t(CTStream &strm, CListHead &lhAll) {
  CVarSetting *pvs = NULL;

  // [Cecil] Skip everything until the next gadget
  BOOL bSkipUntilNext = FALSE;

  // repeat
  FOREVER {
    // read one line
    CTString strLine = GetNonEmptyLine_t(strm);

    // Stop parsing
    if (strLine.RemovePrefix("MenuEnd")) break;

    // [Cecil] Keep skipping until a new gadget
    if (bSkipUntilNext && !strLine.HasPrefix("Gadget:")) {
      continue;
    }

    if (strLine.RemovePrefix("Gadget:")) {
      pvs = new CVarSetting;
      lhAll.AddTail(pvs->vs_lnNode);

      TranslateLine(strLine);
      strLine.TrimSpacesLeft();
      pvs->vs_strName = strLine;

      // [Cecil] Parse this gadget
      bSkipUntilNext = FALSE;

    } else if (strLine.RemovePrefix("Type:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      // [Cecil] Match type name with a type number
      if (strLine == "Toggle") {
        pvs->vs_eType = CVarSetting::E_TOGGLE;

      } else if (strLine == "Separator") {
        pvs->vs_eType = CVarSetting::E_SEPARATOR;

      } else if (strLine == "Textbox") {
        pvs->vs_eType = CVarSetting::E_TEXTBOX;
      }

    } else if (strLine.RemovePrefix("Schedule:")) {
      CheckPVS_t(pvs);
      FixupFileName_t(strLine);

      // [Cecil] This variable is now used as a shell command, so schedule inclusion of a script
      pvs->vs_strSchedule.PrintF("include \"%s\";", strLine);

    // [Cecil] Schedule an inline command instead of a script
    } else if (strLine.RemovePrefix("Command:")) {
      CheckPVS_t(pvs);
      pvs->vs_strSchedule = strLine;

    } else if (strLine.RemovePrefix("Tip:")) {
      CheckPVS_t(pvs);
      TranslateLine(strLine);

      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_strTip = strLine;

    } else if (strLine.RemovePrefix("Var:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      // [Cecil] Replace old wide screen command
      if (strLine == "sam_bWideScreen") {
        pvs->vs_strVar = "sam_bAdjustForAspectRatio";
      } else {
        pvs->vs_strVar = strLine;
      }

    } else if (strLine.RemovePrefix("Filter:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_strFilter = strLine;

    } else if (strLine.RemovePrefix("Slider:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      if (strLine == "Fill") {
        pvs->vs_iSlider = 1;
      } else if (strLine == "Ratio") {
        pvs->vs_iSlider = 2;
      } else {
        pvs->vs_iSlider = 0;
      }

    } else if (strLine.RemovePrefix("InGame:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      if (strLine == "No") {
        pvs->vs_bCanChangeInGame = FALSE;
      } else {
        ASSERT(strLine == "Yes");
        pvs->vs_bCanChangeInGame = TRUE;
      }

    // [Cecil] Hide value string
    } else if (strLine.RemovePrefix("Hidden:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      if (strLine == "No") {
        pvs->vs_bHidden = FALSE;
      } else {
        ASSERT(strLine == "Yes");
        pvs->vs_bHidden = TRUE;
      }

    // [Cecil] Skip adding this option if it doesn't belong to the current game
    } else if (strLine.RemovePrefix("Games:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      // Discard the setting if the string doesn't contain the current game anywhere
      if (strLine.FindSubstr(CHOOSE_FOR_GAME("TFE105", "TSE105", "TSE107")) == -1) {
        bSkipUntilNext = TRUE;
        pvs->vs_lnNode.Remove();

        delete pvs;
        pvs = NULL;
      }

    // [Cecil] List files under a specific directory as value options
    } else if (strLine.RemovePrefix("List:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      // Get directory and the filename pattern from the path
      CTFileName fnmListPath = strLine;
      CTString strPattern = fnmListPath.FileName() + fnmListPath.FileExt();

      CFileList afnmDir;
      IFiles::ListGameFiles(afnmDir, fnmListPath.FileDir(), strPattern, IFiles::FLF_SEARCHMOD);

      // Go through listed files
      const INDEX ct = afnmDir.Count();

      for (INDEX i = 0; i < ct; i++) {
        const CTFileName &fnm = afnmDir[i];

        // Set text to the filename by default
        CTString &strText = pvs->vs_astrTexts.Push();

        try {
          // Try loading text from the description file nearby
          strText.Load_t(fnm.NoExt() + ".des");

        } catch (char *strError) {
          // Just set text to the filename
          (void)strError;
          strText = fnm.FileName();
        }

        // Set value to the filename
        pvs->vs_astrValues.Push() = fnm;
      }

    } else if (strLine.RemovePrefix("String:")) {
      CheckPVS_t(pvs);
      TranslateLine(strLine);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_astrTexts.Push() = strLine;

    } else if (strLine.RemovePrefix("Value:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_astrValues.Push() = strLine;

    // [Cecil] Include another config in the middle of this one
    } else if (strLine.RemovePrefix("Include:")) {
      FixupFileName_t(strLine);

      CTFileStream strmInclude;
      strmInclude.Open_t(strLine);

      FOREACHINDYNAMICCONTAINER(_cOpenedConfigs, CTFileStream, itstrm) {
        // This config is already opened
        if (itstrm->GetDescription() == strmInclude.GetDescription()) {
          ThrowF_t(TRANS("Config '%s' is included recursively"), strLine.str_String);
        }
      }

      // Start new lines and file
      const INDEX ctRestore = _ctLines;
      const CTString strRestore = _strFile;
      _ctLines = 0;
      _strFile = strLine;

      // Parse included config
      ParseCFG_t(strmInclude, lhAll);

      // Restore lines and file
      _ctLines = ctRestore;
      _strFile = strRestore;

    } else {
      ThrowF_t(LOCALIZE("unknown keyword"));
    }
  }
}

// [Cecil] Start parsing the option config
static void StartConfigParsing(CTStream &strm) {
  // Create tab with all options
  CVarTab &tabAll = _aTabs.Push();
  tabAll.strName = TRANS("All options");

  CListHead &lhAll = tabAll.lhVars;

  // Parse the config
  ParseCFG_t(strm, lhAll);

  // Current tab
  CVarTab *pCur = NULL;
  BOOL bOnlySeparators = FALSE;

  // Go through each setting
  FOREACHINLIST(CVarSetting, vs_lnNode, lhAll, itvs) {
    CVarSetting &vs = *itvs;

    // It's a separator
    if (vs.vs_eType == CVarSetting::E_SEPARATOR)
    {
      // It has a name and there are different settings under the current tab
      if (!bOnlySeparators && vs.vs_strName != "") {
        // Start a new tab
        pCur = &_aTabs.Push();
        pCur->strName = vs.vs_strName;

        // Reset separators state
        bOnlySeparators = TRUE;
      }

    // Not a separator
    } else {
      bOnlySeparators = FALSE;
    }

    // Copy setting into the new tab
    if (pCur != NULL) {
      CVarSetting *pvsCopy = new CVarSetting(vs);
      pCur->lhVars.AddTail(pvsCopy->vs_lnNode);
    }
  }
};

void LoadVarSettings(const CTFileName &fnmCfg) {
  FlushVarSettings(FALSE);

  try {
    CTFileStream strm;
    strm.Open_t(fnmCfg);
    _ctLines = 0;
    _strFile = fnmCfg;

    // [Cecil] Start parsing the option config
    _cOpenedConfigs.Clear();
    _cOpenedConfigs.Add(&strm);

    StartConfigParsing(strm);

  } catch (char *strError) {
    CPrintF("%s (%d) : %s\n", (const char *)_strFile, _ctLines, strError);
  }

  // [Cecil] For each tab
  for (INDEX iTab = 0; iTab < _aTabs.Count(); iTab++)
  {
    FOREACHINLIST(CVarSetting, vs_lnNode, _aTabs[iTab].lhVars, itvs) {
      CVarSetting &vs = *itvs;

      // Skip if couldn't validate or it's a separator
      if (!vs.Validate() || vs.vs_eType == CVarSetting::E_SEPARATOR) {
        continue;
      }

      // Get value from the shell variable
      CTString strValue = _pShell->GetValue(vs.vs_strVar);

      // [Cecil] Different types
      switch (vs.vs_eType)
      {
        case CVarSetting::E_TOGGLE: {
          const INDEX ctValues = vs.vs_ctValues;

          // Custom value by default
          vs.vs_bCustom = TRUE;
          vs.vs_iOrgValue = -1;
          vs.vs_iValue = -1;

          // Search for the same value in the value list
          for (INDEX iValue = 0; iValue < ctValues; iValue++) {
            // If it matches this value
            if (strValue == vs.vs_astrValues[iValue]) {
              // Set index to the value in the list
              vs.vs_iOrgValue = vs.vs_iValue = iValue;
              vs.vs_bCustom = FALSE;
              break;
            }
          }
        } break;

        case CVarSetting::E_TEXTBOX: {
          // Set new string
          vs.vs_strValue = strValue;

          // Save hash value of the string
          vs.vs_iOrgValue = static_cast<ULONG>(strValue.GetHash());
        } break;
      }
    }
  }
}

void FlushVarSettings(BOOL bApply) {
  CStringStack astrScheduled;

  // [Cecil] For each tab
  for (INDEX iTab = 0; iTab < _aTabs.Count(); iTab++)
  {
    if (bApply) {
      FOREACHINLIST(CVarSetting, vs_lnNode, _aTabs[iTab].lhVars, itvs) {
        CVarSetting &vs = *itvs;

        // [Cecil] New value to set
        CTString strNewValue = "";
        BOOL bNewValue = FALSE;

        // [Cecil] Different types
        switch (vs.vs_eType)
        {
          case CVarSetting::E_TOGGLE: {
            // If selected some other value
            if (vs.vs_iValue != vs.vs_iOrgValue) {
              // Set to a new value from the list
              strNewValue = vs.vs_astrValues[vs.vs_iValue];
              bNewValue = TRUE;
            }
          } break;

          case CVarSetting::E_TEXTBOX: {
            // If typed in a different string
            ULONG ulOldHash = static_cast<ULONG>(vs.vs_iOrgValue);

            if (vs.vs_strValue.GetHash() != ulOldHash) {
              // Set to a new string
              strNewValue = vs.vs_strValue;
              bNewValue = TRUE;
            }
          } break;
        }

        // [Cecil] Add scheduled commands for all types
        BOOL bScheduledCommands = FALSE;

        // [Cecil] Try to set a new value and schedule its command if succeeded
        if (bNewValue) {
          // Retrieve shell symbol
          CShellSymbol *pssVar = _pShell->GetSymbol(vs.vs_strVar, TRUE);
          BOOL bCanSet = TRUE;

          // Execute pre-function to see if allowed to set new value
          if (pssVar != NULL && pssVar->ss_pPreFunc != NULL) {
            bCanSet = pssVar->ss_pPreFunc(pssVar->ss_pvValue);
          }

          if (bCanSet) {
            // Set shell symbol to it
            _pShell->SetValue(vs.vs_strVar, strNewValue);

            // Execute post-function
            if (pssVar != NULL && pssVar->ss_pPostFunc != NULL) {
              pssVar->ss_pPostFunc(pssVar->ss_pvValue);
            }

            bScheduledCommands = TRUE;
          }
        }

        // Schedule commands to execute afterwards
        if (bScheduledCommands && vs.vs_strSchedule != "") {
          // Check if it's not scheduled yet
          bScheduledCommands = FALSE;

          for (INDEX i = 0; i < astrScheduled.Count(); i++) {
            if (astrScheduled[i] == vs.vs_strSchedule) {
              bScheduledCommands = TRUE;
              break;
            }
          }

          if (!bScheduledCommands) {
            astrScheduled.Push() = vs.vs_strSchedule;
          }
        }
      }
    }

    {FORDELETELIST(CVarSetting, vs_lnNode, _aTabs[iTab].lhVars, itvs) {
      delete &*itvs;
    }}
  }

  // [Cecil] Clear tabs
  _aTabs.Clear();

  for (INDEX i = 0; i < astrScheduled.Count(); i++) {
    _pShell->Execute(astrScheduled[i]);
  }
}

CVarSetting::CVarSetting() {
  Clear();
}

void CVarSetting::Clear() {
  vs_iOrgValue = 0;
  vs_iValue = 0;
  vs_strValue.Clear(); // [Cecil]
  vs_ctValues = 0;
  vs_eType = E_TOGGLE; // [Cecil] Toggleable type by default
  vs_bCanChangeInGame = TRUE;
  vs_iSlider = 0;
  vs_strName.Clear();
  vs_strTip.Clear();
  vs_strVar.Clear();
  vs_strFilter.Clear();
  vs_strSchedule.Clear();
  vs_bCustom = FALSE;
  vs_bHidden = FALSE; // [Cecil]
}

BOOL CVarSetting::Validate(void) {
  // [Cecil] Specific types don't need validation
  switch (vs_eType) {
    case E_SEPARATOR:
    case E_TEXTBOX:
      return TRUE;
  }

  vs_ctValues = Min(vs_astrValues.Count(), vs_astrTexts.Count());

  if (vs_ctValues <= 0) {
    ASSERT(FALSE);
    return FALSE;
  }

  if (!vs_bCustom) {
    vs_iValue = Clamp(vs_iValue, 0L, vs_ctValues - 1L);
  }

  return TRUE;
}

// [Cecil] Copy constructor
CVarSetting::CVarSetting(const CVarSetting &vsOther) {
  if (&vsOther == this) return;

  vs_eType            = vsOther.vs_eType;
  vs_bCanChangeInGame = vsOther.vs_bCanChangeInGame;
  vs_iSlider          = vsOther.vs_iSlider;
  vs_strName          = vsOther.vs_strName;
  vs_strTip           = vsOther.vs_strTip;
  vs_strVar           = vsOther.vs_strVar;
  vs_strFilter        = vsOther.vs_strFilter;
  vs_strSchedule      = vsOther.vs_strSchedule;
  vs_iValue           = vsOther.vs_iValue;
  vs_strValue         = vsOther.vs_strValue;
  vs_ctValues         = vsOther.vs_ctValues;
  vs_iOrgValue        = vsOther.vs_iOrgValue;
  vs_bCustom          = vsOther.vs_bCustom;
  vs_bHidden          = vsOther.vs_bHidden;

  INDEX ct;
  CTString *pstr;

  // Copy texts
  ct = vsOther.vs_astrTexts.Count();

  if (ct > 0) {
    pstr = vs_astrTexts.Push(ct);

    while (--ct >= 0) {
      pstr[ct] = vsOther.vs_astrTexts[ct];
    }
  }

  // Copy values
  ct = vsOther.vs_astrValues.Count();

  if (ct > 0) {
    pstr = vs_astrValues.Push(ct);

    while (--ct >= 0) {
      pstr[ct] = vsOther.vs_astrValues[ct];
    }
  }
};
