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

// Main game headers
#include "SeriousSam.h"
#include "GUI/Menus/Menu.h"

// [Cecil] Next argument in the symbol function call
#define NEXT_ARG(Type) (*((Type *&)pArgs)++)

// [Cecil] Extra functionality
#include "Cecil/CecilExtensions.h"
