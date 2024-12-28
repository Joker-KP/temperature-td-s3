#include "drawing.h"
#include "data.h"
#include "utils.hpp"
#include <TFT_eSPI.h>
#include "FreeSans10pt7b.h"      // True Type Fonts are converted with: https://rop.nl/truetype2gfx/
#include "FreeSansBold10pt7b.h"
#include "House_Icons86pt7b.h"   // House icons font: https://www.dafont.com/house-icons.font
#include "HPSimplified28pt7b.h"  // HP Simplified Font: https://fonnts.com/hp-simplified/
#include "HPSimplified34pt7b.h"
#include "WIFI16pt7b.h"          // several WiFi icons (free): https://www.dafont.com/wifi.font

#define TFT_VERYDARKGREY    0x2104   // 0x4208
#define TFT_BRIGHTTEAL      0x07D8   // 0x0595

// convert epoch time (UTC) to string "hh:mm" in local time
String hourMinLocalTime(time_t utcTime)   
{
  struct tm* cor = localtime(&utcTime);
  char buff[6];
  strftime(buff, sizeof(buff), "%R", cor);  // https://cplusplus.com/reference/ctime/strftime
  return buff;
}

void drawCelsiusIcon(uint32_t left, uint32_t top, uint32_t color, bool isLarge)
{
  sp.setTextDatum(TL_DATUM);
  sp.setTextColor(color);
  int size = isLarge ? 1 : 0;
  isLarge ? sp.setFreeFont(&FreeSansBold18pt7b): sp.setFreeFont(&FreeSansBold12pt7b); 
  sp.drawString("C", left + 14, top - 5);
  sp.fillCircle(left + 9, top - 3, 3 + size, color);
  sp.fillCircle(left + 9, top - 3, 1 + size, TFT_BLACK);
}

void drawThermometerIcon(uint32_t left, uint32_t top)
{
  sp.setTextDatum(TL_DATUM);
  sp.drawRoundRect(left + 3, top - 6, 5, 22, 3, TFT_CYAN);
  sp.fillRect(left + 4, top + 5, 3, 7, TFT_RED);
  sp.drawCircle(left + 5, top + 17, 5, TFT_CYAN);
  sp.fillCircle(left + 5, top + 17, 4, TFT_RED);
  sp.fillCircle(left + 5, top + 16, 2, TFT_WHITE);
}

void drawWaterDropIcon(uint32_t left, uint32_t top)
{
  sp.setTextDatum(TL_DATUM);
  sp.fillCircle(left, top + 21, 8, TFT_CYAN);
  sp.fillTriangle(left, top + 5, left - 8, top + 21, left + 8, top + 21, TFT_CYAN);
  sp.fillTriangle(left, top + 9, left - 4, top + 19, left + 4, top + 19, 0x0595);
  sp.drawSmoothArc(left, top + 20, 6, 5, 0, 110, TFT_WHITE, TFT_WHITE, true);
  sp.fillCircle(left, top + 21, 5, 0x0595);
  sp.fillCircle(left - 3, top + 18, 2, TFT_WHITE);
}

void drawSunIcon(uint32_t left, uint32_t top, uint32_t color)
{
  sp.setTextDatum(TL_DATUM);
  sp.drawLine(2 + left, top + 3, 18 + left, top + 19, color);
  sp.drawLine(2 + left, top + 19, 18 + left, top + 3, color);
  sp.drawLine(10 + left, top, 10 + left, top + 22, color);
  sp.drawLine(0 + left, top + 11, 20 + left, top + 11, color);
  sp.fillCircle(10 + left, top + 11, 7, color);
}

void drawSignalIcon(uint32_t left, uint32_t top, bool isConnected)
{
  sp.setTextDatum(TL_DATUM);
  sp.setTextColor(isConnected ? TFT_GREEN : TFT_RED);  // red color when the connection is lost
  sp.setFreeFont(&WIFI16pt7b);
  sp.drawString("b", left, top);
}

void drawHeader(const sourceData& src)
{
  int topMargin = 3;
  int degRight = sp.width();
  int humColor = src.dataTimestamp > 0 ? TFT_WHITE : TFT_VERYDARKGREY;
  int tempColor = src.dataTimestamp > 0 ? TFT_BRIGHTTEAL : TFT_VERYDARKGREY;
  String tempStr = stringFormat("%.1f", src.feels);
  String humStr = stringFormat("%d%%", src.humidity);

  // humidity
  drawWaterDropIcon(10, topMargin - 8);
  sp.setTextDatum(TC_DATUM);
  sp.setFreeFont(&FreeSansBold12pt7b);
  sp.setTextColor(humColor);
  sp.drawString(humStr, sp.width()/4 + 10, topMargin);

  // temperature
  sp.setTextDatum(TR_DATUM);
  sp.setFreeFont(&FreeSansBold12pt7b);
  sp.setTextColor(tempColor);
  sp.drawString(tempStr, degRight - 25, topMargin);  
  sp.fillCircle(degRight - 18, topMargin + 4, 3, tempColor);
  sp.fillCircle(degRight - 18, topMargin + 4, 1, TFT_BLACK);
  sp.setFreeFont(&FreeSansBold10pt7b);
  sp.drawString("C", degRight, topMargin + 1);    
  sp.setTextDatum(TL_DATUM);

  // lines
  sp.drawFastHLine(0, topMargin + 24, sp.width(), TFT_DARKGREY);
  sp.drawFastVLine(sp.width() / 2, 0, topMargin + 24, TFT_DARKGREY);  
}

void drawMain(const sourceData& src)
{
  drawThermometerIcon(70, 54);
  drawCelsiusIcon(86, 56, TFT_SKYBLUE, true);

  int color = src.outside > -50 ? TFT_SKYBLUE : TFT_VERYDARKGREY;
  sp.setTextColor(color);
  sp.setFreeFont(&HPSimplified34pt7b);
  sp.setTextDatum(TC_DATUM);
  sp.drawFloat(src.outside, 1, sp.width()/2, 90);
  sp.setTextDatum(TL_DATUM);
}

void drawHome(const sourceData& src, bool isConnected, bool autoBrightness)
{
  // walls and roof of a house
  sp.setTextColor(TFT_WHITE);
  sp.setFreeFont(&House_Icons86pt7b);
  sp.setTextDatum(TC_DATUM);
  sp.drawString("<", sp.width()/2, 182);  // house icon
  sp.fillRoundRect(26, 230, 114, 62, 0, TFT_BLACK);  // "removing" door and window

  // value
  int color = src.inside > -50 ? TFT_YELLOW : TFT_VERYDARKGREY;
  sp.setTextColor(color);
  sp.setFreeFont(&HPSimplified28pt7b);
  sp.drawFloat(src.inside, 1, sp.width()/2, 234);
  sp.setTextDatum(TL_DATUM);

  if (autoBrightness) sp.fillCircle(15, 200, 3, TFT_WHITE);
  drawCelsiusIcon(70, 214, TFT_YELLOW, false);
  drawSignalIcon(128, 176, isConnected);
}

void drawFooter(const sourceData& src)
{
  constexpr uint32_t bottomLine = 292;
  sp.fillRoundRect(0, bottomLine, sp.width(), 25, 0, TFT_BLACK);
  sp.drawFastHLine(0, bottomLine, sp.width(), TFT_DARKGREY);

  int color = src.dataTimestamp > 0 ? TFT_WHITE : TFT_VERYDARKGREY;
  sp.setTextColor(color);
  sp.setFreeFont(&FreeSans10pt7b);
  sp.drawString(hourMinLocalTime(src.sunrise), 30, bottomLine + 6);
  sp.drawString(hourMinLocalTime(src.sunset), 122, bottomLine + 6);
  drawSunIcon(0, bottomLine + 3, TFT_YELLOW);
  drawSunIcon(94, bottomLine + 3, TFT_ORANGE);
}

void drawClock(const sourceData& src)
{
  struct tm tInfo;
  getLocalTime(&tInfo);
  sp.setTextDatum(TL_DATUM);
  sp.setTextColor(TFT_WHITE);
  sp.drawCentreString("Local Time", sp.width() / 2, 50, 4);
  sp.setFreeFont(&HPSimplified28pt7b);
  sp.setTextColor(TFT_CYAN);
  sp.setCursor(0, 120);
  sp.printf("%02d:%02d", tInfo.tm_hour, tInfo.tm_min);
  sp.setFreeFont(&FreeSansBold12pt7b);
  sp.setCursor(140, 120);
  sp.printf("%02d", tInfo.tm_sec);
  sp.setTextColor(TFT_BROWN);
  sp.drawCentreString("Last weather update", sp.width() / 2, 180, 2);
  sp.drawCentreString(hourMinLocalTime(src.dataTimestamp), sp.width() / 2, 200, 4);
  sp.setTextColor(TFT_WHITE);
  String lux = stringFormat("Light: %d lux", src.light);
  sp.drawCentreString(lux, sp.width() / 2, 250, 4);
}
