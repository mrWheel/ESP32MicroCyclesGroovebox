/*** Last Changed: 2026-06-01 - 15:39 ***/
#include "uiSystemSettingsMenu.h"

#include "DisplayDriverClass.h"

#include <Arduino.h>

//-- System Settings menu entry count.
static const int systemSettingsEntryCount = 14;

//-- Visible line count used by list screens.
static const int menuVisibleLineCount = 9;

//-- Clip list item text to fit display row width.
static String fitSystemSettingsRowText(const String& text)
{
  const size_t listRowContentChars = 24;

  if (text.length() <= listRowContentChars)
  {
    return text;
  }

  return text.substring(0, listRowContentChars);

} //   fitSystemSettingsRowText()

//-- Keep selected menu item visible inside scrolling list.
static void updateSystemSettingsFirstVisibleIndex(int selectedIndex, int itemCount,
                                                  int& firstVisibleIndex)
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

} //   updateSystemSettingsFirstVisibleIndex()

//-- Draw System Settings main menu and update first visible index.
void uiSystemSettingsMenuDraw(int menuSelection, int& firstVisibleIndex, const String& ssidValue,
                              const String& ipValue, const String& macValue, const char* themeName,
                              int displayRotation, bool encoderReversed)
{
  String items[systemSettingsEntryCount];
  bool disabledItems[systemSettingsEntryCount] = {true,  true,  true,  false, false, false, false,
                                                  false, false, false, false, false, false, false};

  char cardStorageEntry[40];
  char themeEntry[32];
  char rotationEntry[40];
  char encoderOrderEntry[32];

  snprintf(cardStorageEntry, sizeof(cardStorageEntry), "Card Storage");
  snprintf(themeEntry, sizeof(themeEntry), "Set Theme (%s)", themeName);
  snprintf(rotationEntry, sizeof(rotationEntry), "Rotate Display (%d)", displayRotation);
  snprintf(encoderOrderEntry, sizeof(encoderOrderEntry), "Encoder Order (%s)",
           encoderReversed ? "B-A" : "A-B");

  items[0] = fitSystemSettingsRowText("SSID: " + ssidValue);
  items[1] = fitSystemSettingsRowText("IP: " + ipValue);
  items[2] = fitSystemSettingsRowText("MAC: " + macValue);
  items[3] = "Add Pattern";
  items[4] = "Delete Pattern";
  items[5] = fitSystemSettingsRowText(cardStorageEntry);
  items[6] = "Load Sample Set";
  items[7] = "Erase WiFi credentials";
  items[8] = "Start WiFiManager";
  items[9] = fitSystemSettingsRowText(themeEntry);
  items[10] = fitSystemSettingsRowText(rotationEntry);
  items[11] = fitSystemSettingsRowText(encoderOrderEntry);
  items[12] = "Restart Groovebox";
  items[13] = "Exit";

  updateSystemSettingsFirstVisibleIndex(menuSelection, systemSettingsEntryCount, firstVisibleIndex);

  display.drawListScreenWithDisabledItems("System Settings", items, systemSettingsEntryCount,
                                          menuSelection, firstVisibleIndex, disabledItems);

} //   uiSystemSettingsMenuDraw()
