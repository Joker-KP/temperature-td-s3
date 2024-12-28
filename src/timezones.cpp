#include <ctime>
#include "timezones.h"
#include "zones.h"
#include <WiFiManager.h>

const String sntpServer = "de.pool.ntp.org";

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("No time available (yet)");
        return;
    }
    const char* timeFormat = "Local timestamp: (%A) %Y-%m-%d %H:%M:%S";
    Serial.println(&timeinfo, timeFormat);
}

void timezoneConfig(const String& timezoneName)
{
  // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  String zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // default timezone
  for (unsigned i = 0; i < sizeof(zones) / sizeof(zones_t); ++i) {
    if (zones[i].name.equals(timezoneName)) {
      zone = zones[i].zones;
      break;
    }
  }
  configTzTime(zone.c_str(), sntpServer.c_str());  // set time zone & SNTP server
  
  Serial.println("Timezone name: " + timezoneName);
  Serial.println("Timezone definition: " + zone);
  printLocalTime();
}