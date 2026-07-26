# OctoScreen 🖨️✨

<div align="center">

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![ESP32](https://img.shields.io/badge/Platform-ESP32-orange.svg)](https://www.espressif.com/)
[![OctoPrint](https://img.shields.io/badge/OctoPrint-Compatible-red.svg)](https://octoprint.org/)
[![Language](https://img.shields.io/badge/Language-C++%2FArduino-blue.svg)](https://isocpp.org/)

*A sleek, modern, and responsive touch user interface for OctoPrint, powered by the ESP32-based Guition JC2432W328 capacitive display module.*

</div>

---

## 🌟 Overview

**OctoScreen** is an open-source companion firmware designed to bring a dedicated, highly responsive physical touchscreen controller directly to your 3D printer. Running on the affordable and powerful **Guition JC2432W328** hardware board, it interfaces seamlessly with your OctoPrint server over Wi-Fi, giving you total control and real-time telemetry right at your printer enclosure without needing a bulky desktop or constantly pulling out your phone.

---

## 🛠️ Hardware Requirements & Pinout Specs

* **Display Controller:** Guition JC2432W328 (ESP32-WROOM-32 module with 3.2" TFT Capacitive Touchscreen).
* **Storage:** **MicroSD Card** (Required for storing UI assets, theme configurations, icons, and local logs).
* **Connectivity:** Onboard Wi-Fi (2.4 GHz) for seamless communication with your local OctoPrint instance.

---

## 🚀 Key Features

* **📊 Real-Time Print Telemetry:** Monitor active filenames, real-time nozzle & bed temperatures (current vs. target), and dynamic print progress bars with percentage tracking. Instantly pause or cancel prints with a single tap.
* **🎛️ Comprehensive Main Menu:** Quick, intuitive access to manual axis movements, thermal preheats, advanced calibration routines, and BLTouch mesh bed leveling visualization.
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
2. Copy all required UI icon packs, configuration templates, and font assets into the root directory of the SD card.
3. Insert the MicroSD card into the slot on the back of the Guition module before powering it on.

---

## 💻 Flashing & Quick Start

1. Clone this repository or download the source code:
   ```bash
   git clone [https://github.com/karolyia79/octoscreen.git](https://github.com/karolyia79/octoscreen.git)
