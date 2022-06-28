/* Copyright (c) 2021-2022 by ZCaliptium.

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

#include <Core/Base/CTString.h>
#include <Core/Objects/Node.h>

CNode::CNode()
{
  n_pParent = NULL;
}

void CNode::SetParent(CNode *pNewParent)
{
  if (GetParent() != NULL) {
    n_lnInParent.Remove();
    n_pParent = NULL;
  }
  
  if (pNewParent == NULL) {
    return;
  }
  
  // For each predecesor (parent) entity in the chain
  for (CNode *pPred = pNewParent; pPred != NULL; pPred = pPred->GetParent()) 
  {
    // If self then refuse to set parent
    if (pPred == this) {
      return;
    }
  }
  
  // Set new parent
  n_pParent = pNewParent;
  pNewParent->n_lhChildren.AddTail(n_lnInParent);
  n_pParent->OnChildAdded(this);
}

void CNode::AddChild(CNode *pChild)
{
  if (pChild == NULL || pChild->GetParent() == this) {
    return;
  }

  pChild->SetParent(this);
}

const CTString &CNode::GetName(void) const
{
  static const CTString strDummyName("");
  return strDummyName;
}

void CNode::SetName(const CTString &strNew)
{
}