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

#ifndef SE_INCL_NODE_H
#define SE_INCL_NODE_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>

//! Class that represents object type that can form trees.
class CNode
{
  public:
    CNode *n_pParent;       // parent node
    CListNode n_lnInParent; // list node in the parent node
    CListHead n_lhChildren; // list of child entities
  
  public:
    //! Constructor.
    CNode();

    //! Returns pointer to parent node.
    inline CNode *GetParent(void)
    {
      return n_pParent;
    };

    //! Returns reference to list with children objects.
    inline CListHead &GetChildren()
    {
      return n_lhChildren;
    }

    //! Make this node parent of specified node.
    void AddChild(CNode *pChild);

    //! Sets parent of this node.
    virtual void SetParent(CNode *pNewParent);
    
    //! Get name of this node.
    virtual const CTString &GetName(void) const;
    
    //! Set name of this node.
    virtual void SetName(const CTString &strNew);
    
    //! Called when new child added.
    virtual void OnChildAdded(CNode *pNewChild) {};
};

#endif  /* include-once check. */