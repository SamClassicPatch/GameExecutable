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

#ifndef CECIL_INCL_WORLDENTITIES_H
#define CECIL_INCL_WORLDENTITIES_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Entities/Entity.h>
#include <Engine/World/World.h>

// Get current game world
inline CWorld *GetWorld(void) {
  return &_pNetwork->ga_World;
};

// Find entity in the world by its ID
CEntity *FindEntityByID(const ULONG ulEntityID);

// Find property by its ID and offset
CEntityProperty *FindProperty(CEntity *pen, const ULONG ulID, const SLONG slOffset, const INDEX iType);

// Get WorldSettingsController from the current world
CEntity *GetWSC(void);

// Get all entities of a specific class
void GetEntitiesOfClass(CDynamicContainer<CEntity> &cen, const CTString &strClass, BOOL bOnlyValid);

// Get pointers to local player entities
void GetLocalPlayers(CDynamicContainer<CPlayerEntity> &cen);

#endif
