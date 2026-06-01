/*** Last Changed: 2026-06-01 - 15:39 ***/
#ifndef UI_SEQUENCER_INPUT_H
#define UI_SEQUENCER_INPUT_H

//-- Wrap menu selection into valid range.
void uiNormalizeMenuSelection(int& selection, int itemCount);

//-- Keep selected item visible inside scrolling list.
void uiUpdateListFirstVisibleIndex(int selectedIndex, int itemCount, int& firstVisibleIndex);

#endif
