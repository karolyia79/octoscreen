#include "globals.h"

Preferences preferences;
WebServer server(80);
Adafruit_NeoPixel strip(1, 4, NEO_RGB + NEO_KHZ800);
TFT_eSPI tft = TFT_eSPI();
CST820 touch(33, 32, 25, 21);

uint16_t COLOR_BG, COLOR_CARD, COLOR_ACCENT, COLOR_TEXT, COLOR_SUBTEXT, COLOR_GREEN, COLOR_RED, COLOR_ORANGE;

String wifi_ssid = "";
String wifi_pass = "";
String octo_ip = "";
String octo_apikey = "";
String printer_name = "";
String lang = "en";
int color_theme = 0;

bool sleep_enabled = false;
int sleep_timeout = 20;
unsigned long lastActivityTime = 0;
bool screenAsleep = false;

unsigned long lastLedUpdate = 0;
const long ledInterval = 30;

TaskState currentTask = TASK_IDLE;
bool busyPhaseStarted = false;
bool meshFinishedGreen = false;
ScreenState currentScreen = SCREEN_STATUS;

String tool_temp = "0.0";
String tool_target = "0.0";
String bed_temp = "0.0";
String bed_target = "0.0";
String progress = "0.0";
String print_state = "Connecting...";
String current_file = "";

String old_print_state = "";
String old_progress = "";
String old_tool_temp = "";
String old_bed_temp = "";
String old_tool_target = "";
String old_bed_target = "";
String old_current_file = "";
String old_x_pos = "";
String old_y_pos = "";
String old_z_pos = "";

unsigned long lastQueryTime = 0;
const long queryInterval = 3000;
