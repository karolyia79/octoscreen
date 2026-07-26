#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <Adafruit_NeoPixel.h>
#include <FS.h>
#include <SD.h>
#include "CST820.h"

enum TaskState {
    TASK_IDLE,
    TASK_AUTOHOME_RUNNING,
    TASK_MESHBUILD_RUNNING
};

enum ScreenState {
    SCREEN_STATUS,
    SCREEN_MAIN_MENU,
    SCREEN_TEMP_MENU,
    SCREEN_MOVE_MENU,
    SCREEN_CALIB_MENU,
    SCREEN_LEVEL_MENU,
    SCREEN_SETTINGS_MENU,
    SCREEN_CONFIG_MODE,
    SCREEN_MANUAL_LEVEL,
    SCREEN_TUNING_MENU
};

extern Preferences preferences;
extern WebServer server;
extern Adafruit_NeoPixel strip;
extern TFT_eSPI tft;
extern CST820 touch;

// Színek
extern uint16_t COLOR_BG;
extern uint16_t COLOR_CARD;
extern uint16_t COLOR_ACCENT;
extern uint16_t COLOR_TEXT;
extern uint16_t COLOR_SUBTEXT;
extern uint16_t COLOR_GREEN;
extern uint16_t COLOR_RED;
extern uint16_t COLOR_ORANGE;

// Konfigurációs változók
extern String wifi_ssid;
extern String wifi_pass;
extern String octo_ip;
extern String octo_apikey;
extern String printer_name;
extern String lang;
extern int color_theme;

// Alvó mód változók
extern bool sleep_enabled;
extern int sleep_timeout;
extern unsigned long lastActivityTime;
extern bool screenAsleep;

// LED időzítő
extern unsigned long lastLedUpdate;
extern const long ledInterval;

// Állapotok
extern TaskState currentTask;
extern bool busyPhaseStarted;
extern bool meshFinishedGreen;
extern ScreenState currentScreen;

// Nyomtató adatok
extern String tool_temp;
extern String tool_target;
extern String bed_temp;
extern String bed_target;
extern String progress;
extern String print_state;
extern String current_file;

// Régi értékek változáskövetéshez
extern String old_print_state;
extern String old_progress;
extern String old_tool_temp;
extern String old_tool_target;
extern String old_bed_temp;
extern String old_bed_target;
extern String old_current_file;

extern unsigned long lastQueryTime;
extern const long queryInterval;

#define LED_PIN_R 17
#define LED_PIN_G 4
#define LED_PIN_B 16 // Ha 3 színű (RGB vagy 3 külön LED)

// PWM csatornák az ESP32-höz
#define LED_CH_R 0
#define LED_CH_G 1
#define LED_CH_B 2

// Segédfüggvények
inline String t(String hu, String en) {
    return (lang == "hu") ? hu : en;
}

inline void writeLog(String message) {
    Serial.println("[LOG] " + message);
}