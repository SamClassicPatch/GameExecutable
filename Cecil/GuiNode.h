/* Copyright (c) 2022-2023 Dreamy Cecil
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

#ifndef CECIL_INCL_GUINODE_H
#define CECIL_INCL_GUINODE_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Base/ListIterator.inl>

// Class that allows nesting GUI elements of the game menu
class CGuiNode : public CListNode {
  protected:
    CListHead m_lhChildren; // Sub-menus or gadgets of a menu
    CGuiNode *m_pParent; // Parent node class

  public:
    // Constructor
    CGuiNode() : m_pParent(NULL)
    {
    };

    // Get list of child nodes
    inline const CListHead &GetChildren(void) const {
      return m_lhChildren;
    };

    // Get parent class from the list node
    inline CGuiNode *GetParent(void) const {
      return m_pParent;
    };

    // Add new child node
    void AddChild(CGuiNode *pChild) {
      // No child or already parented
      if (pChild == NULL || pChild->GetParent() == this) {
        return;
      }

      pChild->SetParent(this);
    };

    // Set new parent of this node
    void SetParent(CGuiNode *pParent) {
      // Unparent from existing parent
      if (IsLinked()) {
        Remove();
      }

      // No new parent
      if (pParent == NULL) return;

      // Go through parents of a new parent
      for (CGuiNode *pCheck = pParent; pCheck != NULL; pCheck = pCheck->GetParent())
      {
        // Abort if encountered this node, which shouldn't be a parent
        if (pCheck == this) {
          ASSERT(FALSE);
          return;
        }
      }

      // Add this node as a child to the new parent
      m_pParent = pParent;
      m_pParent->m_lhChildren.AddTail(*this);
    };
};

// Iteration through GUI nodes
#define FOREACHNODE(Class, List, Iterator) \
  for (CListIter<Class, 4> Iterator(List); !Iterator.IsPastEnd(); Iterator.MoveToNext())

#endif
