#pragma once
#include <Arduino.h>

void loadConfig();
void saveConfig(String s, String p, String ip, String key, String l, int th, bool s_en, int s_time);
void setupWebServer();
void startConfigPortal();
bool fetchPrinterName();
void sendGCode(String gcode);
void sendOctoCommand(String endpoint, String jsonBody);
void queryOctoPrint();