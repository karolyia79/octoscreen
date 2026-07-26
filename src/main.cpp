#include "globals.h"
#include "network.h"
#include "ui.h"

bool loadAndValidateConfig();
void performFirmwareUpdate();
void drawPrintingAnimation();

void setup() {
    Serial.begin(115200);
    
    strip.begin();
    strip.setBrightness(50);
    strip.show();

    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH); 

    ledcSetup(LED_CH_R, 5000, 8);
    ledcSetup(LED_CH_G, 5000, 8);
    ledcSetup(LED_CH_B, 5000, 8);

    ledcAttachPin(LED_PIN_R, LED_CH_R);
    ledcAttachPin(LED_PIN_G, LED_CH_G);
    ledcAttachPin(LED_PIN_B, LED_CH_B);

    Wire.begin(33, 32);
    Wire.setTimeout(50); 

    tft.init();
    tft.invertDisplay(true);
    tft.setRotation(1);
    
    touch.begin();

    if (!SD.begin()) {
        writeLog("SD kártya csatolás sikertelen vagy nincs kártya.");
    } else {
        writeLog("SD kártya sikeresen inicializálva.");
    }

    writeLog("--- Octoscreen32 indulás ---");

    loadConfig();

    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString("Connecting to WiFi...", 10, 50, 2);
    
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        writeLog("WiFi connection failed, starting AP mode.");
        startConfigPortal();
    } else {
        setupWebServer();
        writeLog("WiFi connected successfully. IP: " + WiFi.localIP().toString());

        if (!fetchPrinterName()) {
            printer_name = t("Csatlakozz az OctoPrint szerverhez", "Please connect to OctoPrint server");
        }

        currentScreen = SCREEN_STATUS;
        drawStatusScreen();
    }

    lastActivityTime = millis();
}

void updateStatusLED() {
    static unsigned long lastBlinkTime = 0;
    static bool ledState = false;
    unsigned long interval = 500; 

    int brightness = 128;
    bool isBlinking = false;
    
    int r = 0, g = 0, b = 0;

    // --- ÁLLAPOTOK ÉRTÉKELÉSE ---

    // 1. Nyomtatási hiba -> Piros, 100% fényerő (255), gyors villogás
    if (print_state == "Error" || print_state == "Cancelled") {
        brightness = 255; // 100% fényerő
        r = brightness;
        g = 0;
        b = 0;
        isBlinking = true;
        interval = 250;
    }
    // 2. Hálózati vagy API hiba -> Sárga (Piros + Zöld), 50% fényerő, villog
    // (Írd át a te kódodban lévő pontos hiba/kapcsolat változókra, ha szükséges)
    else if (WiFi.status() != WL_CONNECTED) { 
        brightness = 128; // 50% fényerő
        r = brightness;
        g = brightness / 2; // Sárgás keverék
        b = 0;
        isBlinking = true;
        interval = 500;
    }
    // 3. Fut a nyomtatás -> Zöld, 50% fényerő, villog
    else if (print_state == "Printing") {
        brightness = 128; // 50% fényerő
        r = 0;
        g = brightness;
        b = 0;
        isBlinking = true;
        interval = 1000;
    }
    // 4. Minden oké (Készenlét) -> Zöld, 50% fényerő, folyamatos
    else {
        brightness = 128; // 50% fényerő
        r = 0;
        g = brightness;
        b = 0;
        isBlinking = false;
    }

    // --- VILLOGÁS ÉS KIÍRÁS LOGIKA ---
    if (isBlinking) {
        if (millis() - lastBlinkTime >= interval) {
            ledState = !ledState;
            lastBlinkTime = millis();
        }
        
        if (ledState) {
            ledcWrite(LED_CH_R, r);
            ledcWrite(LED_CH_G, g);
            ledcWrite(LED_CH_B, b);
        } else {
            ledcWrite(LED_CH_R, 0);
            ledcWrite(LED_CH_G, 0);
            ledcWrite(LED_CH_B, 0);
        }
    } else {
        // Folyamatos világítás
        ledcWrite(LED_CH_R, r);
        ledcWrite(LED_CH_G, g);
        ledcWrite(LED_CH_B, b);
    }
}

void showToast(String msg, uint16_t borderColor) {
    // Felugró kártya a képernyő közepén (320x240-es felbontáshoz)
    tft.fillRoundRect(50, 95, 220, 50, 8, COLOR_CARD);
    tft.drawRoundRect(50, 95, 220, 50, 8, borderColor);
    tft.setTextColor(COLOR_TEXT, COLOR_CARD);
    tft.drawString(msg, 70, 112, 2);
    delay(1000);
}

void loop() {
    updateLEDAnimation();

    if (currentScreen == SCREEN_CONFIG_MODE) {
        server.handleClient();
        return;
    }

    server.handleClient();

    if (sleep_enabled && !screenAsleep && currentScreen != SCREEN_CONFIG_MODE) {
        if (millis() - lastActivityTime > (unsigned long)(sleep_timeout * 1000)) {
            screenAsleep = true;
            digitalWrite(27, LOW); 
            tft.fillScreen(0x0000); 
        }
    }

    if (currentScreen == SCREEN_STATUS && !screenAsleep) {
        if (print_state == "Printing") {
            drawPrintingAnimation();
        }

        if (millis() - lastQueryTime > queryInterval) {
            queryOctoPrint();
            
            if (print_state != old_print_state || progress != old_progress || tool_temp != old_tool_temp || tool_target != old_tool_target || bed_temp != old_bed_temp || bed_target != old_bed_target || current_file != old_current_file) {
                drawStatusScreen();

                old_progress = progress;
                old_tool_temp = tool_temp;
                old_tool_target = tool_target;
                old_bed_temp = bed_temp;
                old_bed_target = bed_target;
                old_current_file = current_file;
                old_print_state = print_state;
            }
            lastQueryTime = millis();
        }
    }

    uint16_t raw_x, raw_y;
    uint8_t gesture;
    if (touch.getTouch(&raw_x, &raw_y, &gesture)) {
        uint16_t x = raw_y;
        uint16_t y = 240 - raw_x;

        delay(300);

        if (screenAsleep) {
            screenAsleep = false;
            digitalWrite(27, HIGH); 
            lastActivityTime = millis();
            if (currentScreen == SCREEN_STATUS) drawStatusScreen();
            else if (currentScreen == SCREEN_MAIN_MENU) drawMainMenu();
            else if (currentScreen == SCREEN_SETTINGS_MENU) drawSettingsMenu();
            return;
        }

        lastActivityTime = millis(); 

        switch (currentScreen) {
            case SCREEN_STATUS:
                if (x >= 200 && x <= 320 && y >= 0 && y <= 40) {
                    writeLog("MENU gomb megnyomva!");
                    currentScreen = SCREEN_MAIN_MENU;
                    drawMainMenu();
                }
                else if (y >= 200 && y <= 230) {
                    if (print_state == "Printing") {
                        if (x >= 10 && x <= 105) {
                            currentScreen = SCREEN_TUNING_MENU;
                            drawTuningMenu();
                        } else if (x >= 112 && x <= 207) {
                            sendOctoCommand("/api/job", "{\"command\": \"pause\", \"action\": \"toggle\"}");
                        } else if (x >= 214 && x <= 309) {
                            sendOctoCommand("/api/job", "{\"command\": \"cancel\"}");
                        }
                    } else {
                        if (x >= 10 && x <= 105) {
                            currentTask = TASK_AUTOHOME_RUNNING;
                            busyPhaseStarted = false;
                            sendGCode("G28");
                            drawStatusScreen();
                        } else if (x >= 112 && x <= 207) {
                            sendGCode("M18");
                        } else if (x >= 214 && x <= 309) {
                            currentTask = TASK_MESHBUILD_RUNNING;
                            busyPhaseStarted = false;
                            meshFinishedGreen = false;
                            sendGCode("G29");
                            drawStatusScreen();
                        }
                    }
                }
                break;

            case SCREEN_TUNING_MENU:
                if (x >= 10 && x <= 310) {
                    if (y >= 45 && y <= 80) {
                        if (x < 160) sendGCode("M220 S95");
                        else sendGCode("M220 S105");
                    }
                    else if (y >= 85 && y <= 120) {
                        if (x < 160) sendGCode("M104 S190");
                        else sendGCode("M104 S210");
                    }
                    else if (y >= 125 && y <= 160) {
                        if (x < 160) sendGCode("M140 S50");
                        else sendGCode("M140 S65");
                    }
                    else if (y >= 165 && y <= 200) {
                        if (x < 160) sendGCode("M290 Z-0.05");
                        else sendGCode("M290 Z0.05");
                    }
                    else if (y >= 205 && y <= 233) {
                        currentScreen = SCREEN_STATUS;
                        drawStatusScreen();
                    }
                }
                break;

            case SCREEN_MAIN_MENU:
                if (x >= 10 && x <= 310) {
                    if (y >= 42 && y <= 87) {
                        if (x < 155) { currentScreen = SCREEN_TEMP_MENU; }
                        else { currentScreen = SCREEN_MOVE_MENU; }
                    }
                    else if (y >= 95 && y <= 140) {
                        if (x < 155) { currentScreen = SCREEN_CALIB_MENU; }
                        else { currentScreen = SCREEN_LEVEL_MENU; }
                    }
                    else if (y >= 148 && y <= 193) {
                        if (x < 155) { 
                            currentScreen = SCREEN_SETTINGS_MENU; 
                            drawSettingsMenu();
                        }
                        else { sendGCode("M500"); }
                    }
                    else if (y >= 200 && y <= 230) {
                        currentScreen = SCREEN_STATUS;
                        drawStatusScreen();
                    }
                }
                break;

            case SCREEN_SETTINGS_MENU:
                if (x >= 10 && x <= 310) {
                    if (y >= 33 && y <= 59) { 
                        // 1. Nyelv váltás (hu <-> en)
                        lang = (lang == "hu") ? "en" : "hu";
                        saveConfig(wifi_ssid, wifi_pass, octo_ip, octo_apikey, lang, color_theme, sleep_enabled, sleep_timeout);
                        drawSettingsMenu();
                    }
                    else if (y >= 63 && y <= 89) { 
                        // 2. Téma / Színmód váltás
                        color_theme = (color_theme + 1) % 3;
                        updateTheme();
                        saveConfig(wifi_ssid, wifi_pass, octo_ip, octo_apikey, lang, color_theme, sleep_enabled, sleep_timeout);
                        drawSettingsMenu();
                    }
                    else if (y >= 93 && y <= 119) { 
                        // 3. Alvó mód be/ki
                        sleep_enabled = !sleep_enabled;
                        saveConfig(wifi_ssid, wifi_pass, octo_ip, octo_apikey, lang, color_theme, sleep_enabled, sleep_timeout);
                        drawSettingsMenu();
                    }
                    else if (y >= 123 && y <= 149) { 
                        // 4. Alvási idő
                        if (sleep_timeout == 10) sleep_timeout = 20;
                        else if (sleep_timeout == 20) sleep_timeout = 30;
                        else sleep_timeout = 10;
                        saveConfig(wifi_ssid, wifi_pass, octo_ip, octo_apikey, lang, color_theme, sleep_enabled, sleep_timeout);
                        drawSettingsMenu();
                    }
                    else if (y >= 153 && y <= 179) {
                        // 5. & 6. Új gombok az alsó sorban (két oszlop)
                        if (x < 160) {
                            // Bal gomb: Konfiguráció betöltése és validálása
                            if (loadAndValidateConfig()) {
                                writeLog("Konfig sikeresen újratöltve kézi parancsra.");
                                showToast(t("Konfig betöltve!", "Config Loaded!"), COLOR_ACCENT);
                            } else {
                                writeLog("Hiba a konfig betöltésekor!");
                                showToast(t("Konfig hiba!", "Config Error!"), COLOR_RED);
                            }
                            drawSettingsMenu();
                        } else {
                            // Jobb gomb: Firmware frissítés SD-ről
                            showToast(t("FW Frissités...", "Updating FW..."), COLOR_ORANGE);
                            performFirmwareUpdate();
                            drawSettingsMenu();
                        }
                    }
                    else if (y >= 188 && y <= 216) { 
                        // 7. Vissza gomb
                        currentScreen = SCREEN_MAIN_MENU;
                        drawMainMenu();
                    }
                }
                break;

            default:
                break;
        }
    }
    updateStatusLED();
}
