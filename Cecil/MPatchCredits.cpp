/* Copyright (c) 2023 Dreamy Cecil
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

#include "MPatchCredits.h"
#include "GUI/Menus/MenuPrinting.h"
#include "GUI/Menus/MenuStarters.h"

extern CMenuGadget *_pmgLastActivatedGadget;
extern void RunBrowser(const char *);

// Random quote index
static INDEX _iRandomQuote = 0;

// Label box for 5 names in a row
static FLOATaabbox2D BoxCredits5(FLOAT fRow, INDEX iCenter) {
  // Left
  if (iCenter == -2) {
    return FLOATaabbox2D(FLOAT2D(0.10f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.26f, 0.25f + (fRow + 1) * 0.04f));

  } else if (iCenter == -1) {
    return FLOATaabbox2D(FLOAT2D(0.26f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.42f, 0.25f + (fRow + 1) * 0.04f));

  // Right
  } else if (iCenter == +1) {
    return FLOATaabbox2D(FLOAT2D(0.58f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.74f, 0.25f + (fRow + 1) * 0.04f));

  } else if (iCenter == +2) {
    return FLOATaabbox2D(FLOAT2D(0.74f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.90f, 0.25f + (fRow + 1) * 0.04f));
  }

  // Center
  return FLOATaabbox2D(FLOAT2D(0.42f, 0.25f + (fRow + 0) * 0.04f),
                       FLOAT2D(0.58f, 0.25f + (fRow + 1) * 0.04f));
};

// Label box for 4 names in a row
static FLOATaabbox2D BoxCredits4(FLOAT fRow, INDEX iCenter) {
  // Left
  if (iCenter == -2) {
    return FLOATaabbox2D(FLOAT2D(0.1f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.3f, 0.25f + (fRow + 1) * 0.04f));

  } else if (iCenter == -1) {
    return FLOATaabbox2D(FLOAT2D(0.3f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.5f, 0.25f + (fRow + 1) * 0.04f));

  // Right
  } else if (iCenter == +1) {
    return FLOATaabbox2D(FLOAT2D(0.5f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.7f, 0.25f + (fRow + 1) * 0.04f));

  } else if (iCenter == +2) {
    return FLOATaabbox2D(FLOAT2D(0.7f, 0.25f + (fRow + 0) * 0.04f),
                         FLOAT2D(0.9f, 0.25f + (fRow + 1) * 0.04f));
  }

  // Center
  return BoxCredits5(fRow, 0);
};

// Click on some name
static void NameClick(void) {
  const INDEX iName = ((CMGButton *)_pmgLastActivatedGadget)->mg_iIndex;

  switch (iName) {
    case PCN_CECIL: {
      if (_iRandomQuote == 0) {
        RunBrowser("https://dreamycecil.itch.io/hl2-im");
      } else if (_iRandomQuote == 1) {
        RunBrowser("https://dreamycecil.itch.io/botmod");
      } else {
        RunBrowser("https://dreamycecil.github.io/");
      }
    } break;

    case PCN_NIKC75: RunBrowser("https://nikc75.newgrounds.com/"); break;
    case PCN_TIN: RunBrowser("https://youtu.be/o2W3jfvZU4w"); break;
    case PCN_ZDZICHU: RunBrowser("https://www.facebook.com/SeriousSamAlphaRemake"); break;
  }
};

// Click on source code link
static void SourceCodeClick(void) {
  RunBrowser(CLASSICSPATCH_URL_FULL);
};

// Initialize extras
void CPatchCreditsMenu::Initialize_t(void) {
  // Patch label
  gm_mgPatchName.SetText("Serious Sam Classics Patch v" + GetAPI()->GetVersion());
  gm_mgPatchName.mg_boxOnScreen = BoxMediumRow(0.0f);
  gm_mgPatchName.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgPatchName.mg_iCenterI = 0;
  gm_mgPatchName.mg_bEnabled = FALSE;
  gm_mgPatchName.mg_bLabel = TRUE;
  AddChild(&gm_mgPatchName);

  gm_mgPatchLink.SetText(CLASSICSPATCH_URL_FULL);
  gm_mgPatchLink.mg_strTip = TRANS("source code repository");
  gm_mgPatchLink.mg_boxOnScreen = BoxMediumRow(1.0f);
  gm_mgPatchLink.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgPatchLink.mg_iCenterI = 0;
  gm_mgPatchLink.mg_pActivatedFunction = &SourceCodeClick;
  gm_mgPatchLink.mg_pmgUp = &gm_amgNames[PCN_ZDZICHU];
  gm_mgPatchLink.mg_pmgDown = &gm_amgNames[PCN_CECIL];
  AddChild(&gm_mgPatchLink);

  const FLOAT fHeightLead = 4.0f;
  const FLOAT fHeightThanks = 9.0f;

  // Project lead
  gm_mgLead.SetText(TRANS("DESIGN AND PROGRAMMING"));
  gm_mgLead.mg_bfsFontSize = BFS_LARGE;
  gm_mgLead.mg_boxOnScreen = BoxCredits5(fHeightLead, 0);
  gm_mgLead.mg_iCenterI = 0;
  gm_mgLead.mg_bEnabled = FALSE;
  gm_mgLead.mg_bLabel = TRUE;
  AddChild(&gm_mgLead);

  gm_amgNames[PCN_CECIL].SetText("Dreamy Cecil");
  gm_amgNames[PCN_CECIL].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_CECIL].mg_boxOnScreen = BoxCredits5(fHeightLead + 2.0f, 0);
  gm_amgNames[PCN_CECIL].mg_pmgUp = &gm_mgPatchLink;
  gm_amgNames[PCN_CECIL].mg_pmgDown = &gm_amgNames[PCN_FREEKNIK];

  // Special thanks
  gm_mgThanks.SetText(TRANS("SPECIAL THANKS"));
  gm_mgThanks.mg_bfsFontSize = BFS_LARGE;
  gm_mgThanks.mg_boxOnScreen = BoxCredits5(fHeightThanks, 0);
  gm_mgThanks.mg_iCenterI = 0;
  gm_mgThanks.mg_bEnabled = FALSE;
  gm_mgThanks.mg_bLabel = TRUE;
  AddChild(&gm_mgThanks);

  gm_amgNames[PCN_FREEKNIK].SetText("FreekNik");
  gm_amgNames[PCN_FREEKNIK].mg_strTip = "^i#1 Nettie Fan";
  gm_amgNames[PCN_FREEKNIK].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_FREEKNIK].mg_boxOnScreen = BoxCredits4(fHeightThanks + 2.0f, -1);
  gm_amgNames[PCN_FREEKNIK].mg_pmgUp = &gm_amgNames[PCN_CECIL];
  gm_amgNames[PCN_FREEKNIK].mg_pmgDown = &gm_amgNames[PCN_NIKC75];
  gm_amgNames[PCN_FREEKNIK].mg_pmgLeft = &gm_amgNames[PCN_MAX512];
  gm_amgNames[PCN_FREEKNIK].mg_pmgRight = &gm_amgNames[PCN_MAX512];

  gm_amgNames[PCN_MAX512].SetText("M@x512 BLR");
  gm_amgNames[PCN_MAX512].mg_strTip = "^i\"The sufferings. The sweet sufferings.\"";
  gm_amgNames[PCN_MAX512].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_MAX512].mg_boxOnScreen = BoxCredits4(fHeightThanks + 2.0f, +1);
  gm_amgNames[PCN_MAX512].mg_pmgUp = &gm_amgNames[PCN_CECIL];
  gm_amgNames[PCN_MAX512].mg_pmgDown = &gm_amgNames[PCN_SUPERSNIPER98];
  gm_amgNames[PCN_MAX512].mg_pmgLeft = &gm_amgNames[PCN_FREEKNIK];
  gm_amgNames[PCN_MAX512].mg_pmgRight = &gm_amgNames[PCN_FREEKNIK];

  gm_amgNames[PCN_NIKC75].SetText("Nikc75");
  gm_amgNames[PCN_NIKC75].mg_strTip = "^i\"Where there's will, there's way.\"";
  gm_amgNames[PCN_NIKC75].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_NIKC75].mg_boxOnScreen = BoxCredits4(fHeightThanks + 3.0f, -1);
  gm_amgNames[PCN_NIKC75].mg_pmgUp = &gm_amgNames[PCN_FREEKNIK];
  gm_amgNames[PCN_NIKC75].mg_pmgDown = &gm_amgNames[PCN_TIN];
  gm_amgNames[PCN_NIKC75].mg_pmgLeft = &gm_amgNames[PCN_SUPERSNIPER98];
  gm_amgNames[PCN_NIKC75].mg_pmgRight = &gm_amgNames[PCN_SUPERSNIPER98];

  gm_amgNames[PCN_SUPERSNIPER98].SetText("Supersniper98");
  gm_amgNames[PCN_SUPERSNIPER98].mg_strTip = "^i\"Problems? FIGHT them!\"";
  gm_amgNames[PCN_SUPERSNIPER98].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_SUPERSNIPER98].mg_boxOnScreen = BoxCredits4(fHeightThanks + 3.0f, +1);
  gm_amgNames[PCN_SUPERSNIPER98].mg_pmgUp = &gm_amgNames[PCN_MAX512];
  gm_amgNames[PCN_SUPERSNIPER98].mg_pmgDown = &gm_amgNames[PCN_ZDZICHU];
  gm_amgNames[PCN_SUPERSNIPER98].mg_pmgLeft = &gm_amgNames[PCN_NIKC75];
  gm_amgNames[PCN_SUPERSNIPER98].mg_pmgRight = &gm_amgNames[PCN_NIKC75];

  gm_amgNames[PCN_TIN].SetText("Tin Kesaro");
  gm_amgNames[PCN_TIN].mg_strTip = "^iClick for leaked patch for Next Encounter";
  gm_amgNames[PCN_TIN].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_TIN].mg_boxOnScreen = BoxCredits4(fHeightThanks + 4.0f, -1);
  gm_amgNames[PCN_TIN].mg_pmgUp = &gm_amgNames[PCN_NIKC75];
  gm_amgNames[PCN_TIN].mg_pmgDown = &gm_mgPatchLink;
  gm_amgNames[PCN_TIN].mg_pmgLeft = &gm_amgNames[PCN_ZDZICHU];
  gm_amgNames[PCN_TIN].mg_pmgRight = &gm_amgNames[PCN_ZDZICHU];

  gm_amgNames[PCN_ZDZICHU].SetText("Zdzichu");
  gm_amgNames[PCN_ZDZICHU].mg_strTip = "^i\"Boys make the best girls.\"";
  gm_amgNames[PCN_ZDZICHU].mg_bfsFontSize = BFS_MEDIUM;
  gm_amgNames[PCN_ZDZICHU].mg_boxOnScreen = BoxCredits4(fHeightThanks + 4.0f, +1);
  gm_amgNames[PCN_ZDZICHU].mg_pmgUp = &gm_amgNames[PCN_SUPERSNIPER98];
  gm_amgNames[PCN_ZDZICHU].mg_pmgDown = &gm_mgPatchLink;
  gm_amgNames[PCN_ZDZICHU].mg_pmgLeft = &gm_amgNames[PCN_TIN];
  gm_amgNames[PCN_ZDZICHU].mg_pmgRight = &gm_amgNames[PCN_TIN];

  // Add all names
  for (INDEX i = 0; i < PCN_LAST; i++) {
    gm_amgNames[i].mg_iCenterI = 0;
    gm_amgNames[i].mg_iIndex = i;
    gm_amgNames[i].mg_pActivatedFunction = &NameClick;
    AddChild(&gm_amgNames[i]);
  }
};

// Start credits menu
void CPatchCreditsMenu::StartMenu(void) {
  // Randomize Cecil's quote
  static const CTString astrQuotes[] = {
    "^iAlso try Half-Life 2 Immersion Mod!",
    "^iAlso try Cecil's Bot Mod!",
    "^iI love this game",
    "^iMaking my own Serious Sam: Revolution one semicolon at a time",
    "^iI made this :)",
    "^iIf only Serious Engine 1.50 was open-source...",
    "^i100% no virus",
    "^iI bet others are jealous that I can have random and ^ce50000c^cff8d00o^cffee00l^c028121o^c004cffr^c770088f^cf5abb9u^c5bcffbl^C quotes",
    "^i\"You reap what you sow.\"",
    "^i\"Good things come to those who wait.\"",
    "^i\"If you play with fire, you will get burned.\"",
    "^i\"It's never too late to mend.\"",
    "^i\"People who live in glass houses shouldn't throw stones.\"",
    "^i\"Two wrongs don't make a right.\"",
    "^i\"Good things come to those who wait.\"",
    "^i\"If it ain't broke, don't fix it.\"",
  };

  _iRandomQuote = rand() % 16;
  gm_amgNames[PCN_CECIL].mg_strTip = astrQuotes[_iRandomQuote];

  CGameMenu::StartMenu();
};
