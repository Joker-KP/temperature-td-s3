
#include "data.h"
#include <ArduinoJson.h>         // https://github.com/bblanchon/ArduinoJson - https://arduinojson.org/
#include <HTTPClient.h>          // connecting with API

JsonDocument getData(HTTPClient& http, const String& url, const String& desc)
{
  JsonDocument doc;
  doc["result"] = "fail";
  if (!http.begin(url.c_str())) {
    Serial.println("Error: Could not begin HTTP connection for " + desc);
    return doc;
  }
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String payload = http.getString();
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) doc["result"] = "ok";
    else Serial.println("Error: Could not parse json result from " + desc);
  }
  else Serial.println("Error: Could not get result from " + desc);
  http.end();
  delay(200);
  return doc;
}

sourceData* getSourceData(const String& openWeatherUrl, const String& homeCenterUrl)
{
  static sourceData result = { 0, 0, 0, -50.0, -50.0, -50.0, -50, -50 };

  HTTPClient http;
  JsonDocument doc;

  doc = getData(http, openWeatherUrl, "Open Weather");  // NOTE: that uses free calls limit
  if (doc["result"] == "ok") {
    result.dataTimestamp = doc["dt"];            // time_t UTC weather data (epoch time)
    result.sunrise = doc["sys"]["sunrise"];      // time_t UTC sunrise
    result.sunset = doc["sys"]["sunset"];        // time_t UTC sunset
    result.feels = doc["main"]["feels_like"];    // float temperature
    result.humidity = doc["main"]["humidity"];   // int humidity in %
  }

  doc = getData(http, homeCenterUrl, "Home Center");
  if (doc["result"] == "ok") {
    result.inside = doc["inside"];     // float temperature
    result.outside = doc["outside"];   // float temperature   
    result.light = doc["light"];       // int light intensity
  }

  if (false) {  // create samples data for UI design
    result.dataTimestamp = 1734849797;
    result.sunrise = 1734849797;
    result.sunset = 1734878679;
    result.feels = -24.3;  
    result.humidity = 87;

    result.inside = 21.3;
    result.outside = -37.8;
    result.light = 55;

    // result.dataTimestamp = 0;
  }

  return &result;
}

void dumpSourceData(const sourceData& src)
{
    Serial.println("DataTimestamp " + String(src.dataTimestamp));
    Serial.println("Sunrise " + String(src.sunrise));
    Serial.println("Sunset " + String(src.sunset));
    Serial.println("Feels " + String(src.feels));
    Serial.println("Humidity " + String(src.humidity));

    Serial.println("Inside " + String(src.inside));
    Serial.println("Outside " + String(src.outside));
    Serial.println("Light " + String(src.light));
}
