/*** Last Changed: 2026-06-01 - 15:12 ***/
#pragma once

#include "sequencer.h"

#include <Arduino.h>

//-- Build one Groovebox track row: left-aligned instrument, right-aligned 16 steps.
String uiGrooveboxScreenBuildTrackRowText(const char* trackName, const Track& track);
