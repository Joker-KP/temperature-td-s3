#pragma once

#include "data.h"
#include <TFT_eSPI.h>

extern TFT_eSprite sp;


void drawHeader(const sourceData& src);

void drawMain(const sourceData& src);

void drawHome(const sourceData& src, bool isConnected, bool autoBrightness);

void drawFooter(const sourceData& src);

void drawClock(const sourceData& src);

