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

// Patched function
class CStringPatch : public CTString {
  public:
    CTString P_Undecorated(void) const {
      CTString strResult = *this;
      const char *pchSrc = str_String;
      char *pchDst = strResult.str_String;

      while (pchSrc[0] != 0) {
        if (pchSrc[0] != '^') {
          *pchDst++ = *pchSrc++;
          continue;
        }

        switch (pchSrc[1]) {
          case 'c': pchSrc += 2 + FindZero((UBYTE *)pchSrc + 2, 6); break;
          case 'a': pchSrc += 2 + FindZero((UBYTE *)pchSrc + 2, 2); break;

          // [Cecil] 1 byte instead of 2
          case 'f': pchSrc += 2 + FindZero((UBYTE *)pchSrc + 2, 1); break;

          case 'b': case 'i': case 'r': case 'o':
          case 'C': case 'A': case 'F': case 'B': case 'I': pchSrc += 2; break;
          case '^': pchSrc++; *pchDst++ = *pchSrc++; break;

          default:
            *pchDst++ = *pchSrc++;
            break;
        }
      }
      *pchDst++ = 0;

      ASSERT(strResult.Length() <= Length());
      return strResult;
    };
};

extern void CECIL_ApplyUndecoratedPatch(void) {
  CTString (CTString::*pUndecorated)(void) const = &CTString::Undecorated;
  NEW_PATCH(pUndecorated, &CStringPatch::P_Undecorated, "CTString::Undecorated()");
};
