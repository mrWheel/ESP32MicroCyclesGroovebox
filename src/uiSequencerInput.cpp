/*** Last Changed: 2026-06-01 - 15:39 ***/
#include "uiSequencerInput.h"

//-- Visible line count used by list screens.
static const int menuVisibleLineCount = 9;

//-- Wrap menu selection into valid range.
void uiNormalizeMenuSelection(int& selection, int itemCount)
{
  if (itemCount <= 0)
  {
    selection = 0;
    return;
  }

  while (selection < 0)
  {
    selection += itemCount;
  }

  while (selection >= itemCount)
  {
    selection -= itemCount;
  }

} //   uiNormalizeMenuSelection()

//-- Keep selected item visible inside scrolling list.
void uiUpdateListFirstVisibleIndex(int selectedIndex, int itemCount, int& firstVisibleIndex)
{
  if (itemCount <= menuVisibleLineCount)
  {
    firstVisibleIndex = 0;
    return;
  }

  if (selectedIndex < firstVisibleIndex)
  {
    firstVisibleIndex = selectedIndex;
  }
  else if (selectedIndex >= firstVisibleIndex + menuVisibleLineCount)
  {
    firstVisibleIndex = selectedIndex - menuVisibleLineCount + 1;
  }

  int maxFirstVisible = itemCount - menuVisibleLineCount;

  if (firstVisibleIndex > maxFirstVisible)
  {
    firstVisibleIndex = maxFirstVisible;
  }

  if (firstVisibleIndex < 0)
  {
    firstVisibleIndex = 0;
  }

} //   uiUpdateListFirstVisibleIndex()
