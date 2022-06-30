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

#include "Cecil/CecilExtensions.h"
#include "Cecil/Patches.h"

// Don't listen to in-game sounds
BOOL _bNoListening = FALSE;

// Patched function
class CSoundLibPatch : public CSoundLibrary {
  public:
    void P_Listen(CSoundListener &sl)
    {
      // Ignore sound listener
      if (_bNoListening) return;

      // Original function code
      if (sl.sli_lnInActiveListeners.IsLinked()) {
        sl.sli_lnInActiveListeners.Remove();
      }

      sl_lhActiveListeners.AddTail(sl.sli_lnInActiveListeners);
    };
};

extern void CECIL_ApplySoundListenPatch(void) {
  void (CSoundLibrary::*pListen)(CSoundListener &) = &CSoundLibrary::Listen;
  NewPatch(pListen, &CSoundLibPatch::P_Listen, "CSoundLibrary::Listen(...)");
};
