/* Copyright (c) 2023-2024 Dreamy Cecil
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

#include "UpdateCheck.h"
#include "GUI/Menus/MenuManager.h"

#include <CoreLib/Networking/HttpRequests.h>

// Version of the latest release
static PatchVer_t _ulLatestVersion = 0;
static CTString _strLatestVersion;

// Open web page with the latest release
static void OpenLatestUpdate(void) {
  RunBrowser(CLASSICSPATCH_URL_LATESTRELEASE);
};

// Postpone the notification for another week
static void PostponeNotification(void) {
  time_t iTime;
  time(&iTime);
  tm *tmLocal = localtime(&iTime);

  // Reset the clock and add a week
  tmLocal->tm_hour = 0;
  tmLocal->tm_min = 0;
  tmLocal->tm_sec = 0;
  tmLocal->tm_mday += 7;

  sam_iUpdateReminder = (INDEX)mktime(tmLocal);
};

// Check if should be notified about new updates
static BOOL NotifyAboutUpdates(void) {
  // Notifications disabled
  if (!IConfig::global[k_EConfigProps_NotifyAboutUpdates]) return FALSE;

  // Get current time
  time_t iTime;
  time(&iTime);

  // Notify if current time is past the reminder
  return (ULONG)iTime > (ULONG)sam_iUpdateReminder;
};

// Prompt to download the latest release
static void DownloadUpdatePrompt(void) {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &OpenLatestUpdate;
  gmCurrent._pConfimedNo = &PostponeNotification;

  const CTString strPrompt(0, TRANS(
    "A new version of Classics Patch is available!\n"
    "   Your version: ^cff3f3f%s^r      Latest version: ^c5fff5f%s^r\n\n"
    "- \"Remind me later\" will show this again in a week.\n"
    "- Use '^cffffffNotifyAboutUpdates^r' config property to toggle it."
  ), ClassicsCore_GetVersionName(), _strLatestVersion);

  gmCurrent.SetText(strPrompt, TRANS("DOWNLOAD"), TRANS("REMIND ME LATER"));
  gmCurrent.BeSmall(0.3f);

  if (pgmCurrentMenu != &gmCurrent) {
    gmCurrent.SetParentMenu(pgmCurrentMenu);
  }

  ChangeToMenu(&gmCurrent);
};

// Display notification about a new release
void NotifyAboutNewVersion(void) {
  // No latest version
  if (_ulLatestVersion == 0 || !NotifyAboutUpdates()) return;

  // Can be updated
  if (_ulLatestVersion > ClassicsCore_GetVersion())
  {
    CPrintF(TRANS("New release is available: %s\nDownload it here: %s\n"),
            _strLatestVersion, CLASSICSPATCH_URL_LATESTRELEASE);

    DownloadUpdatePrompt();
  }

  _ulLatestVersion = 0;
};

// Check for patch updates
void QueryPatchUpdates(void) {
  // Check only once
  static BOOL bChecked = FALSE;

  if (bChecked) return;
  bChecked = TRUE;

  if (!NotifyAboutUpdates()) return;

  // See if the version has been checked for recently
  ULONG ulLastCheck = 0;
  char strLastVersion[32];

  if (sam_strLastVersionCheck.ScanF("%X:'%32[^']'", &ulLastCheck, &strLastVersion) == 2) {
    time_t iTime;
    time(&iTime);

    // Return last checked version if the check happened within the last 6 hours
    ULONG ulInterval = 60 * 60 * 6;

    if (ulLastCheck < iTime + ulInterval) {
      _strLatestVersion = strLastVersion;

      // Compose a version number out of the version tag
      ULONG ulRelease = 0;
      ULONG ulUpdate = 0;
      ULONG ulPatch = 0;
      _strLatestVersion.ScanF("%u.%u.%u", &ulRelease, &ulUpdate, &ulPatch);

      _ulLatestVersion = MakeVersion(ulRelease, ulUpdate, ulPatch);
      return;
    }
  }

  // Request latest release from GitHub
  CPutString(TRANS("Checking for updates...\n"));

  CHttpResponse str = HttpRequest(L"api.github.com", L"GET", CLASSICSPATCH_URL_HTTPREQUEST, TRUE, NULL);

  // Find version tag
  static const CTString strTagKey = "\"tag_name\":\"";
  INDEX iTag = str.FindSubstr(strTagKey);

  if (iTag == -1) return;

  // Trim everything before the tag
  str.TrimLeft(str.Length() - iTag - strTagKey.Length());

  // Trim everything after the tag
  iTag = str.FindSubstr("\"");
  str.TrimRight(iTag);

  _strLatestVersion = str;

  // Compose a version number out of the version tag
  ULONG ulRelease = 0;
  ULONG ulUpdate = 0;
  ULONG ulPatch = 0;
  str.ScanF("%u.%u.%u", &ulRelease, &ulUpdate, &ulPatch);

  _ulLatestVersion = MakeVersion(ulRelease, ulUpdate, ulPatch);

  // Remember which version was checked for and when
  time_t iTime;
  time(&iTime);

  sam_strLastVersionCheck.PrintF("%X:'%s'", (ULONG)iTime, str);
};
