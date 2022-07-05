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

// [Cecil] Include the core library
#include <CoreLib/Core.h>

// [Cecil] Moved from many individual files
#include <Engine/Base/KeyNames.h>
#include <LCDDrawing.h> // [Cecil] Use local instead of Game's

// [Cecil] Choose value based on configuration
#ifdef SE1_TFE
  #define CHOOSE_FOR_GAME(_TFE105, _TSE105, _TSE107) _TFE105
#elif SE1_VER == 105
  #define CHOOSE_FOR_GAME(_TFE105, _TSE105, _TSE107) _TSE105
#else
  #define CHOOSE_FOR_GAME(_TFE105, _TSE105, _TSE107) _TSE107
#endif

// Main game headers
#include "SeriousSam.h"
#include "GUI/Menus/Menu.h"

// [Cecil] Next argument in the symbol function call
#define NEXT_ARG(Type) (*((Type *&)pArgs)++)

// [Cecil] Extra functionality
#include "Cecil/CecilExtensions.h"

// [Cecil] Query manager
#include "Query/QueryMgr.h"

// [Cecil] CSessionProperties byte container (replacement for CUniversalSessionProperties)
typedef UBYTE CSesPropsContainer[NET_MAXSESSIONPROPERTIES];
