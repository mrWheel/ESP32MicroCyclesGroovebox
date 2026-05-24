/*** Last Changed: 2026-05-24 - 16:40 ***/
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

//--- Build flag fallback values
#ifndef PIN_TFT_CS
#define PIN_TFT_CS 21
#endif

#ifndef PIN_TFT_DC
#define PIN_TFT_DC 22
#endif

#ifndef PIN_TFT_RST
#define PIN_TFT_RST 17
#endif

#ifndef PIN_TFT_BL
#define PIN_TFT_BL 16
#endif

#ifndef PIN_TFT_SCL
#define PIN_TFT_SCL 18
#endif

#ifndef PIN_TFT_SDA
#define PIN_TFT_SDA 23
#endif

#ifndef PIN_ENC_A
#define PIN_ENC_A 32
#endif

#ifndef PIN_ENC_B
#define PIN_ENC_B 33
#endif

#ifndef PIN_ENC_BTN
#define PIN_ENC_BTN 25
#endif

#ifndef PIN_KEY0
#define PIN_KEY0 26
#endif

#ifndef PIN_OUTPUT
#define PIN_OUTPUT 27
#endif

#ifndef PIN_TRIGGER
#define PIN_TRIGGER 34
#endif

#ifndef PIN_RESET
#define PIN_RESET 35
#endif

#ifndef PIN_SD_CS
#define PIN_SD_CS 13
#endif

#ifndef PIN_SD_SCK
#define PIN_SD_SCK 18
#endif

#ifndef PIN_SD_MISO
#define PIN_SD_MISO 19
#endif

#ifndef PIN_SD_MOSI
#define PIN_SD_MOSI 23
#endif

#ifndef TFT_WIDTH
#define TFT_WIDTH 320
#endif

#ifndef TFT_HEIGHT
#define TFT_HEIGHT 240
#endif

#ifndef DEFAULT_DISPLAY_ROTATION
#define DEFAULT_DISPLAY_ROTATION 3
#endif

#ifndef ENCODER_LONG_PRESS_MS
#define ENCODER_LONG_PRESS_MS 900
#endif

#ifndef ENCODER_SHORT_PRESS_MS
#define ENCODER_SHORT_PRESS_MS 40
#endif

#ifndef ENCODER_MEDIUM_PRESS_MS
#define ENCODER_MEDIUM_PRESS_MS 450
#endif

#ifndef BUTTON_LONG_PRESS_MS
#define BUTTON_LONG_PRESS_MS 900
#endif

#ifndef BUTTON_SHORT_PRESS_MS
#define BUTTON_SHORT_PRESS_MS 40
#endif

#ifndef BUTTON_MEDIUM_PRESS_MS
#define BUTTON_MEDIUM_PRESS_MS 450
#endif

#ifndef DEFAULT_AP_SSID
#define DEFAULT_AP_SSID "UTimer"
#endif

#ifndef DEFAULT_AP_PASSWORD
#define DEFAULT_AP_PASSWORD ""
#endif

#ifndef DEFAULT_WIFI_HOSTNAME
#define DEFAULT_WIFI_HOSTNAME "u-timer"
#endif

#ifndef DEFAULT_ON_TIME
#define DEFAULT_ON_TIME 5
#endif

#ifndef DEFAULT_OFF_TIME
#define DEFAULT_OFF_TIME 5
#endif

#ifndef DEFAULT_ON_UNIT
#define DEFAULT_ON_UNIT 1
#endif

#ifndef DEFAULT_OFF_UNIT
#define DEFAULT_OFF_UNIT 1
#endif

#ifndef DEFAULT_REPEAT_COUNT
#define DEFAULT_REPEAT_COUNT 0
#endif

#ifndef DEFAULT_TRIGGER_MODE
#define DEFAULT_TRIGGER_MODE 0
#endif

#ifndef DEFAULT_TRIGGER_EDGE
#define DEFAULT_TRIGGER_EDGE 1
#endif

#ifndef DEFAULT_OUTPUT_POLARITY
#define DEFAULT_OUTPUT_POLARITY 1
#endif

#ifndef DEFAULT_LOCK_INPUT_DURING_RUN
#define DEFAULT_LOCK_INPUT_DURING_RUN 0
#endif

#ifndef DEFAULT_AUTO_SAVE_LAST_PROFILE
#define DEFAULT_AUTO_SAVE_LAST_PROFILE 1
#endif

#endif
