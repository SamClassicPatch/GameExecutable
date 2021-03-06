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
#include <io.h>

CListHead _lhAutoDemos;
CListHead _lhAllLevels;
CListHead _lhFilteredLevels;
extern INDEX sam_bShowAllLevels;

CLevelInfo::CLevelInfo(void) {
  li_fnLevel = CTString("Levels\\Default.wld");
  li_strName = TRANS("<invalid level>");
  li_ulSpawnFlags = 0x0;
  li_eFormat = E_LF_SE100; // [Cecil]
}

CLevelInfo::CLevelInfo(const CLevelInfo &li) {
  li_fnLevel      = li.li_fnLevel;
  li_strName      = li.li_strName;
  li_ulSpawnFlags = li.li_ulSpawnFlags;
  li_eFormat      = li.li_eFormat; // [Cecil]
}

void CLevelInfo::operator=(const CLevelInfo &li) {
  li_fnLevel      = li.li_fnLevel;
  li_strName      = li.li_strName;
  li_ulSpawnFlags = li.li_ulSpawnFlags;
  li_eFormat      = li.li_eFormat; // [Cecil]
}

// Get level info for given filename
BOOL GetLevelInfo(CLevelInfo &li, const CTFileName &fnm) {
  try {
    // open the world file
    CTFileStream strm;
    strm.Open_t(fnm);

    // skip initial chunk ids
    strm.ExpectID_t("BUIV"); // 'build version'

    INDEX iDummy;
    strm >> iDummy; // the version number

    // [Cecil] Levels from other games
    if (iDummy != 10000) {
      li.li_eFormat = CLevelInfo::E_LF_SE150;
    }

    strm.ExpectID_t("WRLD"); // 'world'
    strm.ExpectID_t("WLIF"); // 'world info'

    if (strm.PeekID_t() == CChunkID("DTRS")) {
      strm.ExpectID_t("DTRS"); // 'world info'
    }

    // [Cecil] Prevent game from crashing from parsing SSR levels
    if (strm.PeekID_t() == CChunkID("LDRB")) {
      CTString strDummy;
      strm.ExpectID_t("LDRB");
      strm >> strDummy;
      li.li_eFormat = CLevelInfo::E_LF_SSR;
    }

    // read the name
    strm >> li.li_strName;

    // read the flags
    strm >> li.li_ulSpawnFlags;

    // [Cecil] Prevent game from crashing from parsing SSR levels
    if (strm.PeekID_t() == CChunkID("SpGM")) {
      strm.ExpectID_t("SpGM");
      li.li_eFormat = CLevelInfo::E_LF_SSR;
    }

    // translate name
    li.li_strName = TranslateConst(li.li_strName, 0);

    // if dummy name
    if (li.li_strName == "") {
      // use filename
      li.li_strName = fnm.FileName();
    }

    // remember filename
    li.li_fnLevel = fnm;

    // succeed
    return TRUE;

  // if failed
  } catch (char *strError) {
    (void)strError;
    //CPrintF("Invalid world file '%s': %s\n", (const char*) fnm, strError);

    // set dummy info
    li = CLevelInfo();

    // fail
    return FALSE;
  }
}

int qsort_CompareLevels(const void *elem1, const void *elem2) {
  const CLevelInfo &li1 = **(CLevelInfo **)elem1;
  const CLevelInfo &li2 = **(CLevelInfo **)elem2;
  return strcmp(li1.li_fnLevel, li2.li_fnLevel);
}

// Init level-info subsystem
void LoadLevelsList(void) {
  CPrintF(TRANS("Reading levels directory...\n"));

  // list the levels directory with subdirs
  CDynamicStackArray<CTFileName> afnmDir;
  MakeDirList(afnmDir, CTString("Levels\\"), "*.wld", DLI_RECURSIVE | DLI_SEARCHCD);

  // for each file in the directory
  for (INDEX i = 0; i < afnmDir.Count(); i++) {
    CTFileName fnm = afnmDir[i];

    CPrintF(TRANS("  file '%s' : "), (const char *)fnm);

    // try to load its info, and if valid
    CLevelInfo li;

    if (GetLevelInfo(li, fnm)) {
      CPrintF(TRANS("'%s' spawn=0x%08x\n"), li.li_strName, li.li_ulSpawnFlags);

      // create new info for that file
      CLevelInfo *pliNew = new CLevelInfo;
      *pliNew = li;

      // add it to list of all levels
      _lhAllLevels.AddTail(pliNew->li_lnNode);

    } else {
      CPrintF(TRANS("invalid level\n"));
    }
  }

  // sort the list
  _lhAllLevels.Sort(qsort_CompareLevels, offsetof(CLevelInfo, li_lnNode));
}

// Cleanup level-info subsystem
void ClearLevelsList(void) {
  // delete list of levels
  FORDELETELIST(CLevelInfo, li_lnNode, _lhAllLevels, itli) {
    delete &itli.Current();
  }
}

// Find all levels that match given flags
void FilterLevels(ULONG ulSpawnFlags) {
  // delete list of filtered levels
  {FORDELETELIST(CLevelInfo, li_lnNode, _lhFilteredLevels, itli) {
    delete &itli.Current();
  }}

  // for each level in main list
  FOREACHINLIST(CLevelInfo, li_lnNode, _lhAllLevels, itli) {
    CLevelInfo &li = *itli;

    // initially, the level is not visible in list
    BOOL bVisible = FALSE;

    // if all levels are shown, it is visible
    if (sam_bShowAllLevels) {
      bVisible = TRUE;

    // if it satisfies the spawn flags
    } else if (li.li_ulSpawnFlags & ulSpawnFlags) {
      // if spawn flags include single player
      if (ulSpawnFlags & SPF_SINGLEPLAYER) {
        // visibile only if visited already
        bVisible = FileExists(li.li_fnLevel.NoExt() + ".vis");

      // if not single player
      } else {
        // it is visibile
        bVisible = TRUE;
      }
    }

    // if visible
    if (bVisible) {
      // make a copy
      CLevelInfo *pliNew = new CLevelInfo;
      *pliNew = li;

      // add it to the list of filtered levels
      _lhFilteredLevels.AddTail(pliNew->li_lnNode);
    }
  }
}

// If level doesn't support given flags, find one that does
void ValidateLevelForFlags(ULONG ulSpawnFlags) {
  // for each level in main list
  {FOREACHINLIST(CLevelInfo, li_lnNode, _lhAllLevels, itli) {
    CLevelInfo &li = *itli;

    // if found
    if (li.li_fnLevel == GetGameAPI()->GetCustomLevel()) {
      // if it satisfies the flags
      if (li.li_ulSpawnFlags & ulSpawnFlags) {
        // all ok
        return;
      }
    }
  }}

  // for each level in main list
  {FOREACHINLIST(CLevelInfo, li_lnNode, _lhAllLevels, itli) {
    CLevelInfo &li = *itli;

    // if it satisfies the flags
    if (li.li_ulSpawnFlags & ulSpawnFlags) {
      // use that one
      GetGameAPI()->SetCustomLevel(li.li_fnLevel);
      return;
    }
  }}

  // if nothing found, use default invalid level
  GetGameAPI()->SetCustomLevel(CLevelInfo().li_fnLevel);
}

// get level info for its filename
CLevelInfo FindLevelByFileName(const CTFileName &fnm) {
  // for each level in main list
  FOREACHINLIST(CLevelInfo, li_lnNode, _lhAllLevels, itli) {
    CLevelInfo &li = *itli;

    // if found
    if (li.li_fnLevel == fnm) {
      // return it
      return li;
    }
  }

  // if none found, return dummy
  return CLevelInfo();
}

int qsort_CompareDemos(const void *elem1, const void *elem2) {
  const CLevelInfo &li1 = **(CLevelInfo **)elem1;
  const CLevelInfo &li2 = **(CLevelInfo **)elem2;
  return strcmp(li1.li_fnLevel, li2.li_fnLevel);
}

// Init list of autoplay demos
void LoadDemosList(void) {
  CPrintF(TRANS("Reading demos directory...\n"));

  // list the levels directory with subdirs
  CDynamicStackArray<CTFileName> afnmDir;
  MakeDirList(afnmDir, CTString("Demos\\"), "Demos\\Auto-*.dem", DLI_RECURSIVE);

  // for each file in the directory
  for (INDEX i = 0; i < afnmDir.Count(); i++) {
    CTFileName fnm = afnmDir[i];

    // create new info for that file
    CLevelInfo *pli = new CLevelInfo;
    pli->li_fnLevel = fnm;

    CPrintF("  %s\n", (const char *)pli->li_fnLevel);

    // add it to list
    _lhAutoDemos.AddTail(pli->li_lnNode);
  }

  // sort the list
  _lhAutoDemos.Sort(qsort_CompareDemos, offsetof(CLevelInfo, li_lnNode));

  // add the intro to the start
  if (sam_strIntroLevel != "") {
    CLevelInfo *pli = new CLevelInfo;
    pli->li_fnLevel = sam_strIntroLevel;

    CPrintF("  %s\n", (const char *)pli->li_fnLevel);
    _lhAutoDemos.AddHead(pli->li_lnNode);
  }
}

// Clear list of autoplay demos
void ClearDemosList(void) {
  // delete list of levels
  FORDELETELIST(CLevelInfo, li_lnNode, _lhAllLevels, itli) {
    delete &itli.Current();
  }
}
