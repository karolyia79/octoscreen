#include "globals.h"
#include "network.h"
#include "ui.h"

unsigned long taskStartTime = 0;

void loadConfig() {
    if (!SD.exists("/config.txt")) {
        writeLog("Nincs config fájl az SD-n, alapértelmezettek betöltése.");
        
        wifi_ssid = "SSID";
        wifi_pass = "your ssid password";
        octo_ip = "your octoprint server ip";
        octo_apikey = "your octoprint server API code";
        lang = "en"; 
        color_theme = 0;
        sleep_enabled = false;
        sleep_timeout = 20;

        saveConfig(wifi_ssid, wifi_pass, octo_ip, octo_apikey, lang, color_theme, sleep_enabled, sleep_timeout);
        return;
    }

    File file = SD.open("/config.txt", FILE_READ);
    if (!file) {
        writeLog("Hiba: Nem sikerült megnyitni a config fájlt olvasásra.");
        return;
    }

    wifi_ssid = file.readStringUntil('\n'); wifi_ssid.trim();
    wifi_pass = file.readStringUntil('\n'); wifi_pass.trim();
    octo_ip = file.readStringUntil('\n'); octo_ip.trim();
    octo_apikey = file.readStringUntil('\n'); octo_apikey.trim();
    lang = file.readStringUntil('\n'); lang.trim();
    
    String themeStr = file.readStringUntil('\n'); themeStr.trim();
    color_theme = themeStr.toInt();

    String sleepStr = file.readStringUntil('\n'); sleepStr.trim();
    sleep_enabled = (sleepStr == "1" || sleepStr == "true");

    String timeoutStr = file.readStringUntil('\n'); timeoutStr.trim();
    sleep_timeout = timeoutStr.toInt();

    file.close();
    updateTheme();
    writeLog("Konfiguráció sikeresen betöltve az SD kártyáról.");
}

void saveConfig(String s, String p, String ip, String key, String l, int th, bool s_en, int s_time) {
    if (SD.exists("/config.txt")) {
        SD.remove("/config.txt");
    }

    File file = SD.open("/config.txt", FILE_WRITE);
    if (!file) {
        writeLog("Hiba: Nem sikerült megnyitni a config fájlt íráshoz az SD-n!");
        return;
    }

    file.println(s);
    file.println(p);
    file.println(ip);
    file.println(key);
    file.println(l);
    file.println(th);
    file.println(s_en ? "1" : "0");
    file.println(s_time);

    file.close();
    updateTheme();
    writeLog("Új konfiguráció mentve az SD kártyára.");
}

bool loadAndValidateConfig() {
    if (!SD.exists("/config.txt")) {
        writeLog("Nincs config fájl az SD-n, alapértelmezettek használata.");
        return false;
    }

    File file = SD.open("/config.txt", FILE_READ);
    if (!file) {
        writeLog("Hiba: Nem sikerült megnyitni a config fájlt olvasásra.");
        return false;
    }

    // Ideiglenes változók a validációhoz
    String t_ssid = file.readStringUntil('\n'); t_ssid.trim();
    String t_pass = file.readStringUntil('\n'); t_pass.trim();
    String t_ip = file.readStringUntil('\n'); t_ip.trim();
    String t_key = file.readStringUntil('\n'); t_key.trim();
    String t_lang = file.readStringUntil('\n'); t_lang.trim();
    
    String themeStr = file.readStringUntil('\n'); themeStr.trim();
    int t_theme = themeStr.toInt();

    String sleepStr = file.readStringUntil('\n'); sleepStr.trim();
    bool t_sleep = (sleepStr == "1" || sleepStr == "true");

    String timeoutStr = file.readStringUntil('\n'); timeoutStr.trim();
    int t_timeout = timeoutStr.toInt();

    file.close();

    // --- VALIDÁCIÓS SZABÁLYOK ---
    // 1. Az SSID nem lehet üres
    if (t_ssid.length() == 0 || t_ssid.length() > 32) {
        writeLog("Validációs hiba: Érvénytelen SSID a konfigban!");
        return false;
    }
    // 2. Az IP címnek tartalmaznia kell pontot (alapszintű ellenőrzés)
    if (t_ip.length() < 7 || t_ip.indexOf('.') == -1) {
        writeLog("Validációs hiba: Érvénytelen IP cím formátum!");
        return false;
    }
    // 3. Téma index ellenőrzése (pl. 0 és 5 között)
    if (t_theme < 0 || t_theme > 5) {
        writeLog("Validációs hiba: Ismeretlen téma index!");
        return false;
    }
    // 4. Sleep timeout ellenőrzése (pl. 5 és 600 másodperc között)
    if (t_timeout < 5 || t_timeout > 600) {
        writeLog("Validációs hiba: Érvénytelen alvó időzítés!");
        return false;
    }

    // Ha minden valid, átmentjük a globális változókba
    wifi_ssid = t_ssid;
    wifi_pass = t_pass;
    octo_ip = t_ip;
    octo_apikey = t_key;
    lang = t_lang;
    color_theme = t_theme;
    sleep_enabled = t_sleep;
    sleep_timeout = t_timeout;

    updateTheme();
    writeLog("Konfiguráció sikeresen betöltve és validálva az SD-ről.");
    return true;
}

// Firmware frissítés SD kártyáról (/update.bin)
void performFirmwareUpdate() {
    if (!SD.exists("/update.bin")) {
        writeLog("FW Update hiba: /update.bin nem található az SD kártyán.");
        return;
    }

    File updateFile = SD.open("/update.bin", FILE_READ);
    if (!updateFile) {
        writeLog("FW Update hiba: Nem sikerült megnyitni a /update.bin fájlt.");
        return;
    }

    size_t updateSize = updateFile.size();
    if (updateSize == 0) {
        writeLog("FW Update hiba: A frissítőfájl üres.");
        updateFile.close();
        return;
    }

    writeLog("Firmware frissítés indítása az SD-ről...");
    
    if (!Update.begin(updateSize)) {
        writeLog("FW Update hiba: Nem elég memória a frissítéshez (Update.begin sikertelen).");
        updateFile.close();
        return;
    }

    size_t written = Update.writeStream(updateFile);
    if (written != updateSize) {
        writeLog("FW Update hiba: Nem sikerült minden bájtot kiírni.");
        updateFile.close();
        return;
    }

    updateFile.close();

    if (Update.end()) {
        if (Update.isFinished()) {
            writeLog("Sikeres firmware frissítés! Újraindítás 2mp múlva...");
            delay(2000);
            ESP.restart();
        } else {
            writeLog("FW Update hiba: A frissítés nem fejeződött be teljesen.");
        }
    } else {
        writeLog("FW Update hiba: Update.end hiba: " + String(Update.getError()));
    }
}

void handleRoot() {
    String html = "<html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:sans-serif;background:#111;color:#eee;padding:20px;max-width:500px;margin:auto;}";
    html += "input, select{width:100%;padding:10px;margin:5px 0 15px 0;background:#222;border:1px solid #444;color:#fff;border-radius:4px;box-sizing:border-box;}";
    html += "button{width:100%;padding:12px;background:#007bff;color:white;border:none;border-radius:4px;font-size:16px;cursor:pointer;}";
    html += ".checkbox-group{margin:10px 0 15px 0;display:flex;align-items:center;} .checkbox-group input{width:auto;margin-right:10px;}";
    html += "hr{border:0;border-top:1px solid #333;margin:25px 0;}</style></head>";
    html += "<body><h2>OctoScreen - Settings</h2>";
    html += "<form action='/save' method='POST'>";
    html += "WiFi SSID:<br><input type='text' name='ssid' value='" + wifi_ssid + "'><br>";
    html += "WiFi Password:<br><input type='password' name='pass' value='" + wifi_pass + "'><br>";
    html += "OctoPrint IP Address:<br><input type='text' name='ip' value='" + octo_ip + "'><br>";
    html += "OctoPrint API Key:<br><input type='text' name='apikey' value='" + octo_apikey + "'><br>";
    html += "Language / Nyelv:<br><select name='lang'>";
    html += "<option value='en' " + String(lang == "en" ? "selected" : "") + ">English</option>";
    html += "<option value='hu' " + String(lang == "hu" ? "selected" : "") + ">Magyar</option>";
    html += "</select><br>";
    html += "Color Theme / Színmód:<br><select name='theme'>";
    html += "<option value='0' " + String(color_theme == 0 ? "selected" : "") + ">Dark / Sötét</option>";
    html += "<option value='1' " + String(color_theme == 1 ? "selected" : "") + ">Light / Világos</option>";
    html += "<option value='2' " + String(color_theme == 2 ? "selected" : "") + ">Colorful / Színes</option>";
    html += "</select><br>";
    html += "<div class='checkbox-group'><input type='checkbox' name='sleep_en' value='1' " + String(sleep_enabled ? "checked" : "") + "> Sleep Screen / Alvó képernyő bekapcsolása</div>";
    html += "Sleep Timeout / Alvási idő:<br><select name='sleep_time'>";
    html += "<option value='10' " + String(sleep_timeout == 10 ? "selected" : "") + ">10 sec</option>";
    html += "<option value='20' " + String(sleep_timeout == 20 ? "selected" : "") + ">20 sec</option>";
    html += "<option value='30' " + String(sleep_timeout == 30 ? "selected" : "") + ">30 sec</option>";
    html += "</select><br>";
    html += "<button type='submit'>Save & Restart</button>";
    html += "</form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleSave() {
    if (server.hasArg("ssid") && server.hasArg("ip")) {
        int th = server.hasArg("theme") ? server.arg("theme").toInt() : color_theme;
        bool s_en = server.hasArg("sleep_en");
        int s_time = server.hasArg("sleep_time") ? server.arg("sleep_time").toInt() : sleep_timeout;
        saveConfig(server.arg("ssid"), server.arg("pass"), server.arg("ip"), server.arg("apikey"), server.arg("lang"), th, s_en, s_time);
        server.send(200, "text/html, charset=utf-8", "<h3>Settings saved! Rebooting device...</h3>");
        delay(2000);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "Parameters error!");
    }
}

void setupWebServer() {
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.begin();
}

void startConfigPortal() {
    currentScreen = SCREEN_CONFIG_MODE;
    WiFi.mode(WIFI_AP);
    WiFi.softAP("octoscreen-Setup", "12345678");
    setupWebServer();

    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_ORANGE, COLOR_BG);
    tft.drawString("WIFI AP MODE", 60, 20, 2);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString("Connect to Wi-Fi:", 20, 60, 2);
    tft.drawString("AP: octoscreen-Setup", 20, 90, 2);
    tft.drawString("Pass: 12345678", 20, 120, 2);
    tft.drawString("Open browser: 192.168.4.1", 20, 160, 2);
}

bool fetchPrinterName() {
    if (WiFi.status() != WL_CONNECTED) return false;
    HTTPClient http;
    String url = String("http://") + octo_ip + "/api/printerprofiles";
    http.begin(url);
    http.addHeader("X-Api-Key", octo_apikey);
    http.setTimeout(3000);
    int httpResponseCode = http.GET();
    bool success = false;
    if (httpResponseCode > 0) {
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
            JsonObject profiles = doc["profiles"];
            for (JsonPair kv : profiles) {
                JsonObject profile = kv.value();
                if (!profile["name"].isNull()) {
                    printer_name = profile["name"].as<String>();
                    success = true;
                    break;
                }
            }
        }
    }
    http.end();
    return success;
}

void sendGCode(String gcode) {
    writeLog("GCode küldés: " + gcode);
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String("http://") + octo_ip + "/api/printer/command";
        http.begin(url);
        http.addHeader("X-Api-Key", octo_apikey);
        http.addHeader("Content-Type", "application/json");
        http.setTimeout(3000); 
        
        String body = "{\"command\": \"" + gcode + "\"}";
        int httpResponseCode = http.POST(body);
        writeLog("GCode válaszkód: " + String(httpResponseCode));
        
        if (httpResponseCode > 0 && httpResponseCode != 204) {
            String response = http.getString();
            if (response.length() > 0) writeLog("GCode válasz: " + response);
        }
        http.end();
    }
}

void sendGCodeBatch(String cmd1, String cmd2) {
    writeLog("GCode Batch küldés: " + cmd1 + " & " + cmd2);
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String("http://") + octo_ip + "/api/printer/command";
        http.begin(url);
        http.addHeader("X-Api-Key", octo_apikey);
        http.addHeader("Content-Type", "application/json");
        http.setTimeout(3000); 
        
        String body = "{\"commands\": [\"" + cmd1 + "\", \"" + cmd2 + "\"]}";
        int httpResponseCode = http.POST(body);
        writeLog("GCode Batch válaszkód: " + String(httpResponseCode));
        
        if (httpResponseCode > 0 && httpResponseCode != 204) {
            String response = http.getString();
            if (response.length() > 0) writeLog("GCode Batch válasz: " + response);
        }
        http.end();
    }
}

void sendOctoCommand(String endpoint, String jsonBody) {
    writeLog("OctoCmd küldés [" + endpoint + "]: " + jsonBody);
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String("http://") + octo_ip + endpoint;
        http.begin(url);
        http.addHeader("X-Api-Key", octo_apikey);
        http.addHeader("Content-Type", "application/json");
        http.setTimeout(3000); 
        int httpResponseCode = http.POST(jsonBody);
        writeLog("OctoCmd válaszkód: " + String(httpResponseCode));
        
        if (httpResponseCode > 0 && httpResponseCode != 204) {
            String response = http.getString();
            if (response.length() > 0) writeLog("OctoCmd válasz: " + response);
        }
        http.end();
    }
}

void queryOctoPrint() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        String url = String("http://") + octo_ip + "/api/job";
        http.begin(url);
        http.addHeader("X-Api-Key", octo_apikey);
        http.setTimeout(3000);
        
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (!error) {
                print_state = doc["state"].as<String>();
                float progVal = doc["progress"]["completion"].as<float>();
                progress = String(progVal, 1);
                
                const char* fileName = doc["job"]["file"]["name"];
                if (fileName != nullptr) {
                    current_file = String(fileName);
                } else {
                    current_file = "";
                }
            }
        } else {
            printer_name = t("Csatlakozz az OctoPrint szerverhez", "Please connect to OctoPrint server");
            print_state = "Disconnected";
            http.end();
            return;
        }
        http.end();

        url = String("http://") + octo_ip + "/api/printer";
        http.begin(url);
        http.addHeader("X-Api-Key", octo_apikey);
        http.setTimeout(3000);
        httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (!error) {
                if (!doc["temperature"]["tool0"]["actual"].isNull()) {
                    tool_temp = String(doc["temperature"]["tool0"]["actual"].as<float>(), 1);
                }
                if (!doc["temperature"]["tool0"]["target"].isNull()) {
                    tool_target = String(doc["temperature"]["tool0"]["target"].as<float>(), 1);
                }
                if (!doc["temperature"]["bed"]["actual"].isNull()) {
                    bed_temp = String(doc["temperature"]["bed"]["actual"].as<float>(), 1);
                }
                if (!doc["temperature"]["bed"]["target"].isNull()) {
                    bed_target = String(doc["temperature"]["bed"]["target"].as<float>(), 1);
                }

                // API flag-ek kiolvasása a stabil állapotkövetéshez
                bool isReady = true;
                if (!doc["state"]["flags"]["ready"].isNull()) {
                    isReady = doc["state"]["flags"]["ready"].as<bool>();
                } else if (!doc["state"]["flags"]["operational"].isNull()) {
                    isReady = doc["state"]["flags"]["operational"].as<bool>();
                }

                // Feladatkezelés (AutoHome és MeshBuild API alapokon)
                if (currentTask != TASK_IDLE) {
                    if (taskStartTime == 0) {
                        taskStartTime = millis();
                        drawStatusScreen(); // Azonnal sárgára váltja a gombot indításkor!
                    }

                    unsigned long elapsed = millis() - taskStartTime;

                    if (currentTask == TASK_AUTOHOME_RUNNING) {
                        static bool homeStarted = false;
                        if (!homeStarted) {
                            sendGCode("G28");
                            homeStarted = true;
                        }
                        // Ha elindult, várunk amíg a gép újra ready lesz, vagy letelik a biztonsági idő (>7s)
                        if ((!isReady && elapsed > 2000) || elapsed > 10000) {
                            if (isReady || elapsed > 10000) {
                                writeLog("AutoHome kész.");
                                currentTask = TASK_IDLE;
                                taskStartTime = 0;
                                homeStarted = false;
                                drawStatusScreen();
                            }
                        }
                    } 
                    else if (currentTask == TASK_MESHBUILD_RUNNING) {
                        static bool meshCommandSent = false;
                        static bool meshWasBusy = false;

                        if (!meshCommandSent) {
                            sendGCodeBatch("G29", "M500");
                            meshCommandSent = true;
                            meshWasBusy = false;
                            writeLog("MeshBuild elindítva, válaszra várva...");
                        }

                        // Amikor a G29 elindul, a gép elfoglalt lesz (!isReady)
                        if (!isReady) {
                            meshWasBusy = true;
                        }

                        // Ha már volt elfoglalt és újra kész, vagy túllépte a maximális időt (60mp), akkor kész
                        if ((meshWasBusy && isReady) || elapsed > 60000) {
                            writeLog("MeshBuild és M500 kész.");
                            currentTask = TASK_IDLE;
                            taskStartTime = 0;
                            meshCommandSent = false;
                            meshWasBusy = false;
                            meshFinishedGreen = true;
                            drawStatusScreen();
                        }
                    }
                } else {
                    taskStartTime = 0;
                }
            }
        }
        http.end();

        if (printer_name == "" || printer_name == "Csatlakozz az OctoPrint szerverhez" || printer_name == "Please connect to OctoPrint server") {
            if (!fetchPrinterName()) {
                printer_name = t("Csatlakozz az OctoPrint szerverhez", "Please connect to OctoPrint server");
            }
        }
    } else {
        printer_name = t("Csatlakozz az OctoPrint szerverhez", "Please connect to OctoPrint server");
        print_state = "Offline";
    }
}
