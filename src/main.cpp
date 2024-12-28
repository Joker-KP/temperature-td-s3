// sketch for LilyGo_T_Display_S3 - https://www.lilygo.cc/products/t-display-s3?variant=42585826558133
// screen size is 170 x 320 px

#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager/
#include <TFT_eSPI.h>            // https://github.com/Bodmer/TFT_eSPI

#include "data.h"
#include "drawing.h"
#include "secrets.h"
#include "timezones.h"
#include "utils.hpp"

WiFiManager myWiFi;
TFT_eSPI tft = TFT_eSPI();           // User_Setup_Select.h: Setup206_LilyGo_T_Display_S3.h
TFT_eSprite sp = TFT_eSprite(&tft);  // sprite to avoid flickering

enum Buttons {
  BUTTON_FLASH = 0,
  BUTTON_RIGHT = 14  // button opposite flash button
};

uint8_t lcdBacklight = 255;  // display backlight initial brightness
bool autoBrightness = true;
int luxMin = 5;
int luxMax = 25;
int lcdMin = 40;
int lcdMax = 255;

const String owUrl = "https://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longitude + "&appid=" + apiKey + "&units=metric";
const String homeUrl = homeAddress;

struct sourceData src;  // gathered data kept between loop() calls

constexpr uint8_t SCREEN_ORIENTATION = 0;
constexpr uint8_t INIT_SCREEN_ORIENTATION = 3;

unsigned long updateInterval = 3 * 60 * 1000;
unsigned long lastUpdateAction = 0xFFFFFFFF - updateInterval;
unsigned long clockInterval = 7 * 1000;
unsigned long clockShowBegin = 0xFFFFFFFF - clockInterval;

const String timezone = "Europe/Warsaw";  // See "zones.h"

void itemChecked(unsigned line, int color, const char* msg)
{
    const unsigned x = 170;
    const unsigned h = 25;
    tft.setTextColor(color);
    tft.drawString(msg, x, line * h, 4);
}

void noConnectionCallback(WiFiManager* myWiFi)
{
  const char* msg[] = { "WiFi status: No connection.", "To enable WiFi:",
                        "- connect to hotspot 'TD-S3',", "- open a web browser,",
                        "- go to 192.168.4.1,", "- enter SSID and password."};
  tft.fillScreen(TFT_NAVY);
  tft.setTextColor(TFT_SKYBLUE);
  for (uint8_t i = 0; i < 6; ++i) tft.drawString(msg[i], 0, 2 + i * 28, 4);
}

void displayInit()
{
  const char* conn_Msg[] = { "Connecting...", "", "- WiFi", "- Time Sync", "- Weather API" };
  tft.init();
  tft.setRotation(INIT_SCREEN_ORIENTATION);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setTextFont(4);
  tft.fillRectVGradient(0, 0, tft.width(), tft.height(), TFT_BLACK, TFT_NAVY);
  for (uint8_t i = 0; i < 5; i++) tft.drawString(conn_Msg[i], 0, i * 25, 4);
}

void displayReconfigure()
{
  tft.setRotation(SCREEN_ORIENTATION);
  sp.createSprite(tft.width(), tft.height());  // sprite for faster rendering
  sp.setTextWrap(false);
}

void setup()
{
  pinMode(BUTTON_FLASH, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  Serial.begin(115200);
  displayInit();

  // reset settings - for testing
  //myWiFi.resetSettings();

  myWiFi.setAPCallback(noConnectionCallback);  // called when WiFi connection fails
  bool success = myWiFi.autoConnect("TD-S3");  // if not connected, start access point named "TD-S3"
  if (success) itemChecked(2, TFT_GREEN, "[OK]");

  timezoneConfig(timezone);

  if (WiFi.isConnected()) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      itemChecked(3, TFT_RED, "[FAIL]");
      return;
    }
    String timeStr = stringFormat("[OK] %02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    itemChecked(3, TFT_GREEN, timeStr.c_str());
  }

  displayReconfigure();
}

void changeBrightness(int diff)   // button pressed: increase or reduce brightness
{
  autoBrightness = false;
  if (lcdBacklight + diff > 255) {
    if (lcdBacklight == 255) lcdBacklight = 0;
    else lcdBacklight = 255;
  } else {
    if (lcdBacklight + diff < 0) {
      if (lcdBacklight == 0) lcdBacklight = 255;
      else lcdBacklight = 0;
    } else {
      lcdBacklight = (lcdBacklight + diff) % 255;
    }
  }
  analogWrite(TFT_BL, lcdBacklight);
  delay(500);
}

void loop() {
  sp.fillSprite(TFT_BLACK);

  unsigned long currentTicks = millis();

  if (currentTicks - lastUpdateAction > updateInterval) {
    lastUpdateAction = currentTicks;
    Serial.println("Retrieve new data: " + String(currentTicks));
    sourceData* recDataPtr = getSourceData(owUrl, homeUrl);  // retrieve new data
    if (recDataPtr != nullptr) src = *recDataPtr;
    dumpSourceData(src);
  }

  if (currentTicks - clockShowBegin > clockInterval) {
    drawHeader(src);
    drawMain(src);
    drawHome(src, WiFi.isConnected(), autoBrightness);
    drawFooter(src);
  } else {
    drawClock(src);
  }

  sp.pushSprite(0, 0);

  if (false && autoBrightness) {  // disabled for now
    int lux = src.light;
    float fraction = (lux - luxMin)/float(luxMax - luxMin);
    if (fraction < 0) fraction = 0;
    int lcd = lcdMin + fraction * (lcdMax - lcdMin);
    if (lcd > 255) lcd = 255;
    analogWrite(TFT_BL, lcd);
  }

  if (!digitalRead(BUTTON_FLASH)) changeBrightness(-64);
  if (!digitalRead(BUTTON_RIGHT)) {
    autoBrightness = true;
    clockShowBegin = currentTicks;
  }

  delay(100);
}
