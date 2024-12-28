#pragma once

#include <Arduino.h>

struct sourceData {
  time_t dataTimestamp;  // epoch time of last weather data update
  time_t sunrise;        // epoch time of sunrise
  time_t sunset;         // epoch time of sunset
  float feels;           // "feels like" temperature (OW)
  float outside;         // outside temperature (wall sensor)
  float inside;          // inside temperature (door sensor)
  int humidity;          // humidity (OW)
  int light;             // light intensity (kitchen sensor)
};

sourceData* getSourceData(const String& openWeatherUrl, const String& homeCenterUrl);

void dumpSourceData(const sourceData& src);