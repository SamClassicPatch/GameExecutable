/* Copyright (c) 2023 Dreamy Cecil
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

#include "MLevelCategories.h"
#include "GUI/Menus/MenuPrinting.h"
#include "GUI/Menus/MenuStuff.h"
#include "LevelInfo.h"

// Select levels from this category
void CMGLevelCategory::OnActivate(void) {
  extern void StartSelectLevelFromCategory(INDEX);
  StartSelectLevelFromCategory(mg_iCategory);

  extern CSoundData *_psdPress;
  PlayMenuSound(_psdPress);

  CMGButton::OnActivate();
};

// Categories with level lists
static INDEX _ctCats = 0;

#define MAX_CUSTOM_CATEGORIES 10

// Initialize categories
void CLevelCategoriesMenu::Initialize_t(void) {
  gm_mgTitle.SetName(TRANS("CHOOSE CATEGORY"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  // Load category lists
  CFileList aCategories;
  IFiles::ListGameFiles(aCategories, "Data\\ClassicsPatch\\LevelCategories\\", "*.lst", (_fnmMod != "") ? IFiles::FLF_ONLYMOD : 0);

  // Create new categories
  _ctCats = Min(aCategories.Count(), INDEX(MAX_CUSTOM_CATEGORIES));

  // No categories
  if (_ctCats == 0) return;

  _aLevelCategories.New(_ctCats);
  gm_amgCategories.New(_ctCats + 1); // One more for other levels

  for (INDEX i = 0; i < _ctCats; i++) {
    // Load level list from this category file
    CFileList &aList = _aLevelCategories[i];
    IFiles::LoadStringList(aList, aCategories[i]);

    // Use first line from the list as the category name
    AddCategory(i, aList[0]);
    aList[0] = CTString("");
  }

  // Add extra category with all other levels at the end
  AddCategory(_ctCats, TRANS("OTHER LEVELS"));
};

// Add category under a specific index
void CLevelCategoriesMenu::AddCategory(INDEX i, const CTString &strName) {
  CMGLevelCategory &mg = gm_amgCategories[i];
  mg.mg_iCategory = i;

  mg.SetText(strName);
  mg.mg_bfsFontSize = BFS_LARGE;
  mg.mg_boxOnScreen = BoxBigRow(i - 1);
  mg.mg_pActivatedFunction = NULL;

  const INDEX ct = _ctCats + 1;
  mg.mg_pmgUp = &gm_amgCategories[(i + ct - 1) % ct];
  mg.mg_pmgDown = &gm_amgCategories[(i + 1) % ct];

  AddChild(&mg);
};
