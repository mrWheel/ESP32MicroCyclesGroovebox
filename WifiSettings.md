# WifiSettings

[Back to README](README.md)

## Purpose

WifiSettings is the compatibility name for the WiFi settings type owned by `WiFiManagerExt`.
The canonical definition lives inside [include/WiFiManagerExt.h](include/WiFiManagerExt.h) as `WiFiManagerExt::WifiSettings`.

## Fields

- `staSsid`
- `staPassword`
- `apSsid`
- `apPassword`
- `hostName`

## Usage

The struct is used by:
- settings storage
- WiFiManagerExt
- startup WiFi configuration in main.cpp

## Notes

- `WifiSettings` is re-exported by [include/WifiSettings.h](include/WifiSettings.h)
- `settingsStore` and startup code still use the alias for convenience
- `timerTypes.h` no longer defines `WifiSettings`

[Back to README](README.md)
