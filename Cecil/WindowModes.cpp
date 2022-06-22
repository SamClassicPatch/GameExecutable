/* Copyright (c) 2022 Dreamy Cecil
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

#include "Cecil/WindowModes.h"
#include "MainWindow.h"

#define RADIOTRANS(String) ("ETRS" String)

// Window mode names
CTString _astrWindowModes[3] = { "", "", "" };

// Open the main application window in borderless mode
void OpenMainWindowBorderless(PIX pixSizeI, PIX pixSizeJ) {
  ASSERT(_hwndMain == NULL);

  // Create an invisible window
  _hwndMain = CreateWindowExA(WS_EX_APPWINDOW, APPLICATION_NAME, "",
    WS_POPUP, 0, 0, pixSizeI, pixSizeJ, NULL, NULL, _hInstance, NULL);

  // Can't create the window
  if (_hwndMain == NULL) {
    FatalError(TRANS("Cannot open main window!"));
  }

  SE_UpdateWindowHandle(_hwndMain);

  // Set window title and show it
  sprintf(_achWindowTitle, TRANS("Serious Sam (Borderless %dx%d)"), pixSizeI, pixSizeJ);
  SetWindowTextA(_hwndMain, _achWindowTitle);

  _pixLastSizeI = pixSizeI;
  _pixLastSizeJ = pixSizeJ;

  ResetMainWindowNormal();
};
