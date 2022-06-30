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

#ifndef CECIL_INCL_WINDOWMODES_H
#define CECIL_INCL_WINDOWMODES_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

// Window modes
enum EWindowModes {
  E_WM_WINDOWED   = 0, // Normal
  E_WM_BORDERLESS = 1,
  E_WM_FULLSCREEN = 2,
};

// Window mode names
extern CTString _astrWindowModes[3];

// Open the main application window in borderless mode
void OpenMainWindowBorderless(PIX pixSizeI, PIX pixSizeJ);

#endif
