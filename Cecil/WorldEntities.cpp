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

#include "Cecil/WorldEntities.h"

// Find entity in the world by its ID
CEntity *FindEntityByID(const ULONG ulEntityID) {
  FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten) {
    CEntity *pen = &*iten;

    if (pen->GetFlags() & ENF_DELETED) {
      continue;
    }

    // Same ID
    if (pen->en_ulID == ulEntityID) {
      return pen;
    }
  }

  return NULL;
};

// Find property by its ID and offset
CEntityProperty *FindProperty(CEntity *pen, const ULONG ulID, const SLONG slOffset, const INDEX iType) {
  CDLLEntityClass *pdec = pen->en_pecClass->ec_pdecDLLClass;

  while (pdec != NULL) {
    // For each property
    for (INDEX iProp = 0; iProp < pdec->dec_ctProperties; iProp++) {
      CEntityProperty &ep = pdec->dec_aepProperties[iProp];

      // Only check the matching type
      if (ep.ep_eptType != iType) {
        continue;
      }

      // Matching ID or offset (ID is more likely to remain the same)
      if (ep.ep_ulID == ulID || ep.ep_slOffset == slOffset) {
        return &ep;
      }
    }

    // Next class in the hierarchy
    pdec = pdec->dec_pdecBase;
  }

  return NULL;
};

// Get WorldSettingsController from the current world
CEntity *GetWSC(void) {
  CEntity *penBack = GetWorld()->GetBackgroundViewer();

  if (penBack != NULL) {
    // Get property offset only once
    static SLONG slPointerOffset = -1;
    
    if (slPointerOffset == -1) {
      // Obtain entity pointer property
      CEntityProperty *pep = penBack->PropertyForName("World settings controller");

      // No entity pointer
      if (pep == NULL) return NULL;

      slPointerOffset = pep->ep_slOffset;
    }
    
    // Obtain WorldSettingsController
    return (CEntity *)ENTITYPROPERTY(penBack, slPointerOffset, CEntityPointer);
  }

  return NULL;
};
