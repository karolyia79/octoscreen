# CYDOctoScreen 🖨️✨

<div align="center">

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![ESP32](https://img.shields.io/badge/Platform-ESP32-orange.svg)](https://www.espressif.com/)
[![OctoPrint](https://img.shields.io/badge/OctoPrint-Compatible-red.svg)](https://octoprint.org/)
[![Language](https://img.shields.io/badge/Language-C++%2FArduino-blue.svg)](https://isocpp.org/)

*A sleek, modern, and responsive touch user interface for OctoPrint, powered by the ESP32-based Guition JC2432W328 capacitive display module.*

</div>

---

## 🌟 Overview

**CYDOctoScreen** is an open-source companion firmware designed to bring a dedicated, highly responsive physical touchscreen controller directly to your 3D printer. Running on the affordable and powerful **Guition JC2432W328** hardware board, it interfaces seamlessly with your OctoPrint server over Wi-Fi, giving you total control and real-time telemetry right at your printer enclosure without needing a bulky desktop or constantly pulling out your phone.

---

## 🛠️ Hardware Requirements & Pinout Specs

* **Display Controller:** Guition JC2432W328 (ESP32-WROOM-32 module with 3.2" TFT Capacitive Touchscreen).
* **Storage:** **MicroSD Card** (Required for storing UI assets, theme configurations, icons, and local logs).
* **Connectivity:** Onboard Wi-Fi (2.4 GHz) for seamless communication with your local OctoPrint instance.

---

## 🚀 Features

* **📊 Fancy Animated Print Feedback:** Enjoy a sleek, visually engaging animated print progress interface that depicts the print head in action alongside real-time telemetry, active filenames, precise percentage tracking, and live nozzle/bed temperatures. Instantly pause or cancel prints with a single tap.
* **🎛️ Comprehensive Main Menu & Control Hub:** Quick, intuitive access to manual axis movements, thermal preheats, and advanced system features.
* **📏 Advanced Calibration Routines:** Includes dedicated tools for precise printer tuning:
  * **Manual 5-Point Bed Leveling:** Step-by-step assistance for accurate first-layer setup across five key bed coordinates.
  * **E-Step Calibration:** Fine-tune your extruder's filament feed rate for exact volumetric precision.
  * **PID Tuning:** Stabilize hotend and bed temperatures seamlessly by running automated thermal optimization routines.
* **⚙️ Local Settings & Customization:** Directly tweak display options on-device, including language selection, light/dark themes, sleep mode toggles, adjustable sleep timeouts, and direct firmware update triggers.
* **🌐 Web Configuration Portal:** Features a built-in local web server supporting English and Hungarian. Easily provision Wi-Fi SSIDs, passwords, your OctoPrint server IP address, and your API key via browser without recompiling code.

---

## 📸 Screenshots & UI Preview

| Printing Status Screen | Main Menu |
| :---: | :---: |
| ![Printing Status](/screenshots/WIN_20260726_08_50_38_Pro.jpg) | ![Main Menu](/screenshots/WIN_20260726_08_50_51_Pro.jpg) |
| *Live thermal monitoring & progress tracking* | *Central hub for calibration & movement* |

| Device Settings | Web Configuration Panel |
| :---: | :---: |
| ![Settings Menu](/screenshots/WIN_20260726_08_50_57_Pro.jpg) | ![Web Interface](/screenshots/8e324cf9-8702-4f8e-85c3-16983efa802d.png) |
| *On-device preferences & themes* | *Browser-based provisioning & API setup* |

---

## 📦 SD Card Setup & Installation

Because the **Guition JC2432W328** relies on external storage for handling layout elements and assets, proper SD card preparation is critical:

1. Format a MicroSD card to **FAT32**.
2. Insert the MicroSD card into the slot on the back of the Guition module before powering it on.

---

## 💻 Flashing & Quick Start

1. Clone this repository or download the source code:
   ```bash
   git clone [https://github.com/karolyia79/cydos.git](https://github.com/karolyia79/cydoctoscreen.git)
   ```

2. Open the project in PlatformIO or the Arduino IDE (with ESP32 board support packages installed).
   Ensure the required libraries (TFT_eSPI, ArduinoJson, WebServer, etc.) are installed.

3. Initial Installation: For the very first installation, the firmware must be flashed directly onto the device via USB.
   Ensure a formatted MicroSD card is inserted into the module.

4. On first boot, use the configuration web portal to set up your Wi-Fi and OctoPrint API key, OctoPrint server IP address.

5. Enjoy!


🤝 Contributing
Contributions, feature requests, and bug reports are always welcome! Feel free to fork the repository, open an issue, or submit a pull request.
