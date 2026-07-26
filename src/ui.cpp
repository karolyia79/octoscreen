#include "globals.h"
#include "ui.h"

void updateLEDAnimation() {
    if (millis() - lastLedUpdate < ledInterval) return;
    lastLedUpdate = millis();

    bool isDisconnected = (WiFi.status() != WL_CONNECTED || print_state == "Disconnected" || print_state == "Offline" || print_state == "Connecting...");
    bool isError = (print_state == "Error" || print_state.indexOf("Error") >= 0);
    bool isPrinting = (print_state == "Printing");

    int r = 0, g = 0, b = 0;
    bool pulse = false;

    if (isDisconnected) {
        r = 255; g = 180; b = 0;   
        pulse = true;
    } else if (isError) {
        r = 255; g = 0; b = 0;    
        pulse = true;
    } else if (isPrinting) {
        r = 0; g = 255; b = 0;    
        pulse = true;
    } else {
        r = 0; g = 255; b = 0;    
        pulse = false;
    }

    if (pulse) {
        float factor = (sin(millis() / 200.0) + 1.0) / 2.0; 
        int minVal = 15;
        int finalR = r > 0 ? (minVal + (int)((r - minVal) * factor)) : 0;
        int finalG = g > 0 ? (minVal + (int)((g - minVal) * factor)) : 0;
        int finalB = b > 0 ? (minVal + (int)((b - minVal) * factor)) : 0;

        strip.setPixelColor(0, strip.Color(finalR, finalG, finalB));
    } else {
        strip.setPixelColor(0, strip.Color(r, g, b));
    }
    strip.show();
}

void updateTheme() {
    if (color_theme == 0) { // Sötét
        COLOR_BG      = 0x10A2;
        COLOR_CARD    = 0x2124;
        COLOR_ACCENT  = 0x027B;
        COLOR_TEXT    = 0xFFFF;
        COLOR_SUBTEXT = 0xCE79;
        COLOR_GREEN   = 0x03E0;
        COLOR_RED     = 0xF800;
        COLOR_ORANGE  = 0xFD20;
    } else if (color_theme == 1) { // Világos
        COLOR_BG      = 0xFFFF;
        COLOR_CARD    = 0xDEFB;
        COLOR_ACCENT  = 0x001F;
        COLOR_TEXT    = 0x0000;
        COLOR_SUBTEXT = 0x528A;
        COLOR_GREEN   = 0x03E0;
        COLOR_RED     = 0xF800;
        COLOR_ORANGE  = 0xFD20;
    } else { // Színes / Neon
        COLOR_BG      = 0x000F;
        COLOR_CARD    = 0x0210;
        COLOR_ACCENT  = 0xF81F;
        COLOR_TEXT    = 0x07FF;
        COLOR_SUBTEXT = 0xFFE0;
        COLOR_GREEN   = 0x07E0;
        COLOR_RED     = 0xF800;
        COLOR_ORANGE  = 0xFD20;
    }
}

void drawPrintingAnimation() {
    static int nozzleX = 90;
    static int direction = 2;
    static unsigned long lastAnimTime = 0;

    // Kb. 30 FPS frissítés a vajpuha mozgásért (33 ms)
    if (millis() - lastAnimTime < 33) return; 
    lastAnimTime = millis();

    int boxX = 60;
    int boxY = 102;
    int boxW = 200;
    int boxH = 90;

    // Az animációs területetjebb visszatevék, hogy alul mozogjon a felső sáv alatt
    int innerX = boxX + 8;
    int innerY = boxY + 28; // Hagyunk helyet a felső százalék kiírásnak
    int innerW = boxW - 16;
    int innerH = boxH - 34;

    // Csak a belső animációs terület törlése (nincs villogás)
    tft.fillRect(innerX, innerY, innerW, innerH, COLOR_BG);
    
    // Build plate (nyomtatóasztal) vonala alul
    tft.drawFastHLine(innerX, innerY + innerH - 2, innerW, 0x7BEF);

    // A nyomtatott tárgy (rétegek) épülése alulról felfelé
    int maxObjectH = innerH - 12;
    int progInt = progress.toInt();
    int currentObjH = (maxObjectH * progInt) / 100;
    
    if (currentObjH > 0) {
        int objTopY = (innerY + innerH - 2) - currentObjH;
        // Tárgy teste
        tft.fillRect(innerX + 30, objTopY, innerW - 60, currentObjH, COLOR_ACCENT);
        
        // Stílusos réteg-csíkozás a tárgyon
        for (int h = 4; h < currentObjH; h += 5) {
            tft.drawFastHLine(innerX + 32, (innerY + innerH - 2) - h, innerW - 64, COLOR_BG);
        }
    }

    // Nyomtatófej X mozgás logika
    nozzleX += direction;
    int minX = innerX + 16;
    int maxX = innerX + innerW - 16;
    if (nozzleX > maxX || nozzleX < minX) {
        direction = -direction; // Irányváltás a széleken
    }

    int tipY = (innerY + innerH - 2) - currentObjH;
    if (tipY < innerY + 10) tipY = innerY + 10;

    // --- KOMPLEX ÉS SZÍNES 3D NYOMTATÓFEJ (TOOLHEAD) ---
    
    // 1. Sötétszürke hátsó kocsi lemez
    tft.fillRect(nozzleX - 10, tipY - 18, 20, 4, 0x4208);

    // 2. Ezüstös hűtőborda (bordázott mintával)
    tft.fillRect(nozzleX - 8, tipY - 14, 16, 7, 0xD69A);
    tft.drawFastHLine(nozzleX - 8, tipY - 12, 16, 0x4208);
    tft.drawFastHLine(nozzleX - 8, tipY - 10, 16, 0x4208);

    // 3. Élénk kék hűtőburkolat / ventilátorház oldalt
    tft.fillRect(nozzleX - 12, tipY - 14, 4, 8, 0x001F); 
    tft.fillRect(nozzleX + 8, tipY - 14, 4, 8, 0x001F);  

    // 4. Narancssárga fűtőblokk középen
    tft.fillRect(nozzleX - 6, tipY - 7, 12, 5, COLOR_ORANGE); 

    // 5. Sárgaréz fúvóka csúcs (Nozzle)
    tft.fillRect(nozzleX - 2, tipY - 2, 4, 3, 0xFFE0);

    // 6. Kinyomtatott izzó filament szál a fúvóka alól a tárgyra terülve
    tft.fillRect(nozzleX - 4, tipY + 1, 9, 2, COLOR_ACCENT);
}

void drawStatusScreen() {
    tft.fillScreen(COLOR_BG);

    String headerText = printer_name;
    if (current_file != "") {
        headerText += " | " + current_file;
    }
    if (headerText.length() > 32) {
        headerText = headerText.substring(0, 29) + "...";
    }
    tft.setTextColor(COLOR_SUBTEXT, COLOR_BG);
    tft.drawString(headerText, 10, 8, 1);

    // MENU gomb (átlátszó háttérrel a keret elkerülésére)
    tft.fillRoundRect(240, 5, 72, 24, 6, COLOR_ACCENT);
    tft.setTextColor(COLOR_TEXT);
    tft.drawString("MENU", 260, 10, 1);

    tft.drawFastHLine(0, 35, 320, COLOR_SUBTEXT);

    // 1. Sor: Aktuális hőmérsékletek és státusz
    tft.fillRoundRect(10, 42, 300, 28, 6, COLOR_CARD);
    tft.setTextColor(COLOR_ORANGE, COLOR_CARD);
    tft.drawString("E:" + tool_temp + "C", 15, 48, 1);
    tft.setTextColor(COLOR_ACCENT, COLOR_CARD);
    tft.drawString("B:" + bed_temp + "C", 100, 48, 1);
    tft.setTextColor(COLOR_TEXT, COLOR_CARD);
    tft.drawString(print_state + " (" + progress + "%)", 175, 48, 1);

    // 2. Sor: Célhőmérsékletek
    tft.fillRoundRect(10, 74, 300, 24, 6, COLOR_CARD);
    tft.setTextColor(COLOR_SUBTEXT, COLOR_CARD);
    tft.drawString("Target E: " + tool_target + "C", 20, 80, 1);
    tft.drawString("Target B: " + bed_target + "C", 170, 80, 1);

    // Középső Státusz doboz
    tft.fillRoundRect(60, 102, 200, 90, 8, COLOR_CARD);
    if (print_state == "Printing") {
        // Cím és százalék fixen a felső sávban (nem zavarja az animációt)
        tft.setTextColor(COLOR_SUBTEXT, COLOR_CARD);
        tft.drawString(t("NYOMTATÁS", "PRINTING"), 72, 108, 1);
        
        tft.setTextColor(COLOR_TEXT, COLOR_CARD);
        tft.drawString(progress + "%", 170, 106, 2);
    } else {
        // Készenléti állapotban a szokásos szöveg
        tft.setTextColor(COLOR_SUBTEXT, COLOR_CARD);
        tft.drawString(t("Nyomtato keszenletben", "Printer Ready"), 75, 137, 1);
    }

    // Alsó gombok (átlátszó text colorrel, hogy ne legyen keretük)
    if (print_state == "Printing") {
        tft.fillRoundRect(10, 200, 95, 30, 6, COLOR_ACCENT);
        tft.setTextColor(COLOR_TEXT);
        tft.drawString(t("Tuning", "Tune"), 35, 208, 1);

        tft.fillRoundRect(112, 200, 95, 30, 6, COLOR_ORANGE);
        tft.setTextColor(COLOR_TEXT);
        tft.drawString(t("Szunet", "Pause"), 140, 208, 1);

        tft.fillRoundRect(214, 200, 95, 30, 6, COLOR_RED);
        tft.setTextColor(COLOR_TEXT);
        tft.drawString(t("Megszak.", "Cancel"), 238, 208, 1);
    } else {
        uint16_t autoHomeColor = (currentTask == TASK_AUTOHOME_RUNNING) ? COLOR_ORANGE : COLOR_CARD;
        tft.fillRoundRect(10, 200, 95, 30, 6, autoHomeColor);
        tft.setTextColor(COLOR_TEXT);
        tft.drawString("AutoHome", 22, 208, 1);

        tft.fillRoundRect(112, 200, 95, 30, 6, COLOR_CARD);
        tft.setTextColor(COLOR_TEXT);
        tft.drawString(t("Motor Off", "Steppers Off"), 120, 208, 1);

        uint16_t meshColor = COLOR_CARD;
        if (currentTask == TASK_MESHBUILD_RUNNING) {
            meshColor = COLOR_ORANGE;
        } else if (meshFinishedGreen) {
            meshColor = COLOR_GREEN;
        }
        tft.fillRoundRect(214, 200, 95, 30, 6, meshColor);
        tft.setTextColor(COLOR_TEXT);
        tft.drawString(t("Mesh Epites", "MeshBuild"), 222, 208, 1);
    }
}

void drawTuningMenu() {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    tft.drawString(t("TUNING & FINOMHANGOLAS", "TUNING MENU"), 45, 8, 1);
    tft.drawFastHLine(0, 32, 320, COLOR_SUBTEXT);

    tft.fillRoundRect(10, 45, 300, 35, 6, COLOR_CARD);
    tft.setTextColor(COLOR_TEXT);
    tft.drawString(t("Sebesseg: [- 1% / + 1%]", "Speed: [- 1% / + 1%]"), 50, 54, 1);

    tft.fillRoundRect(10, 85, 300, 35, 6, COLOR_CARD);
    tft.drawString(t("Hotend: [- 5C / + 5C]", "Hotend: [- 5C / + 5C]"), 65, 94, 1);

    tft.fillRoundRect(10, 125, 300, 35, 6, COLOR_CARD);
    tft.drawString(t("Bed: [- 5C / + 5C]", "Bed: [- 5C / + 5C]"), 85, 134, 1);

    tft.fillRoundRect(10, 165, 300, 35, 6, COLOR_CARD);
    tft.drawString(t("Z-Offset: [- 0.05 / + 0.05]", "Z-Offset: [- 0.05 / + 0.05]"), 45, 174, 1);

    tft.fillRoundRect(10, 205, 300, 28, 6, COLOR_RED);
    tft.setTextColor(COLOR_TEXT);
    tft.drawString(t("< VISSZA", "< BACK"), 135, 212, 1);
}

void drawMainMenu() {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString(t("FOMENU", "MAIN MENU"), 120, 8, 2);
    tft.drawFastHLine(0, 32, 320, COLOR_SUBTEXT);

    tft.fillRoundRect(10, 42, 145, 45, 6, COLOR_CARD);
    tft.setTextColor(COLOR_TEXT);
    tft.drawString(t("1. Homerseklet", "1. Temperature"), 20, 57, 1);

    tft.fillRoundRect(165, 42, 145, 45, 6, COLOR_CARD);
    tft.drawString(t("2. Mozgatas", "2. Move"), 185, 57, 1);

    tft.fillRoundRect(10, 95, 145, 45, 6, COLOR_CARD);
    tft.drawString(t("3. Kalibracio", "3. Calibration"), 20, 110, 1);

    tft.fillRoundRect(165, 95, 145, 45, 6, COLOR_CARD);
    tft.drawString("4. BLTouch Mesh", 180, 110, 1);

    tft.fillRoundRect(10, 148, 145, 45, 6, COLOR_CARD);
    tft.drawString(t("5. Beallitasok", "5. Settings"), 20, 163, 1);

    tft.fillRoundRect(165, 148, 145, 45, 6, 0x8200);
    tft.drawString(t("M500 Mentes", "M500 Save"), 185, 163, 1);

    tft.fillRoundRect(10, 200, 300, 30, 6, COLOR_RED);
    tft.setTextColor(COLOR_TEXT);
    tft.drawString(t("< VISSZA A FOOLDALRA", "< BACK TO HOME"), 70, 208, 1);
}

void drawSettingsMenu() {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString(t("BEALLITASOK", "SETTINGS"), 105, 5, 2);
    tft.drawFastHLine(0, 28, 320, COLOR_SUBTEXT);

    // 1. Nyelv választó
    tft.fillRoundRect(10, 33, 300, 26, 6, COLOR_CARD);
    tft.setTextColor(COLOR_TEXT);
    String langStr = (lang == "hu") ? "Magyar" : "English";
    tft.drawString(t("Nyelv: ", "Language: ") + langStr, 18, 38, 1);

    // 2. Téma / Színmód váltás
    tft.fillRoundRect(10, 63, 300, 26, 6, COLOR_CARD);
    String themeStr = "Dark";
    if (color_theme == 1) themeStr = "Light";
    else if (color_theme == 2) themeStr = "Colorful";
    tft.drawString(t("Szinmod: ", "Theme: ") + themeStr, 18, 68, 1);

    // 3. Alvó mód
    tft.fillRoundRect(10, 93, 300, 26, 6, COLOR_CARD);
    tft.drawString(t("Alvo mod: ", "Sleep Mode: ") + (sleep_enabled ? t("Be (On)", "On") : t("Ki (Off)", "Off")), 18, 98, 1);

    // 4. Alvási idő
    tft.fillRoundRect(10, 123, 300, 26, 6, COLOR_CARD);
    tft.drawString(t("Alvasi ido: ", "Sleep Timeout: ") + String(sleep_timeout) + " sec", 18, 128, 1);

    // 5. Konfiguráció újratöltése / validálása (ÚJ)
    tft.fillRoundRect(10, 153, 145, 26, 6, COLOR_ACCENT);
    tft.drawString(t("Konfig Betolt", "Load Config"), 18, 158, 1);

    // 6. Firmware frissítés SD-ről (ÚJ)
    tft.fillRoundRect(165, 153, 145, 26, 6, COLOR_ORANGE);
    tft.drawString(t("FW Frissites", "FW Update"), 178, 158, 1);

    // 7. Vissza gomb
    tft.fillRoundRect(10, 188, 300, 28, 6, COLOR_RED);
    tft.setTextColor(COLOR_TEXT);
    tft.drawString(t("< VISSZA A FOMENUBE", "< BACK TO MAIN MENU"), 65, 194, 1);
}