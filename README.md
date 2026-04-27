# TBTracker-RX

TBTracker-RX is an affordable, mobile LoRa High Altitude Balloon (HAB) receiver for Arduino, based on the ESP32 and SX1278. It features support for GPS, a web interface, and an OLED display. As of version 0.0.9, it also supports SSDV.

This sketch is designed to receive, decode, and upload LoRa transmissions from high-altitude balloons. It automatically uploads telemetry data in the correct format to [amateur.sondehub.org](https://amateur.sondehub.org).

## Hardware Needed

The sketch is designed to compile in the Arduino IDE and is optimized for the **TTGO T-Beam** board, though it also works with separate hardware modules.

**Minimum requirements:**

- **ESP32-based dual-core board:** T-Beam is recommended, but most ESP32-based boards should work.

- **LoRa radio module:** SX127x or RFM9x module.

**Optional components:**

- **GPS module:** Ublox, ATGM, or any compatible module.

- **OLED display:** SSD1306 or compatible.

## Libraries Needed

The sketch uses several libraries. Ensure the following are installed in your Arduino IDE via the Library Manager:

- **ArduinoJson** by Benoit Blanchon

- **Adafruit SSD1306** by Adafruit (including all dependencies)

- **Adafruit GFX library** by Adafruit

- **Adafruit BusIO** by Adafruit

- **Radiolib** by Jan Gromes

- **TinyGPSPlus** by Mikal Hart

- **XPowersLib** by Lewis He (needed as of release V0.0.11)

- **ESP Async WebServer** by ESP32Async

- **Async TCP** by ESP32Async

## Configuration (settings.h)

You must modify the values in `settings.h` before using the sketch. Key settings include:

- **Pin Numbers:** You must define the pin numbers used for your specific board.

- **DEVFLAG:** Set this to `true` if you want to test uploading to Sondehub without storing data in their database. The telemetry will not appear on the public map, but you can check the upload status in the Serial Monitor.

- **WiFi Settings:** You can add up to three WiFi networks. The software automatically selects the strongest available network, such as your home network or a mobile hotspot.

- **Callsign:** Change this to your unique callsign to be displayed on Sondehub.

- **Station Location (UPL_LAT, UPL_LON, UPL_ALT):** Set these to your current location. These coordinates are used if the software cannot obtain a valid GPS fix.

- **UPLOAD_YOUR_POSITION:** If set to `true`, the software periodically uploads your receiver's position to Sondehub.

- **UPLOAD_PAYLOAD_PACKET:** If set to `true`, the software uploads received telemetry to Sondehub.

- **LoRa Settings:** Configure the frequency and LoRa mode (Mode 2 is currently the most thoroughly tested).

## Memory & Compilation Issues

Because this sketch offers extensive functionality, it may not compile on some older boards due to memory limitations in the default Arduino IDE partitioning.

**Recommended Compilation Settings:**

- Identify the MCU on your board and select the corresponding **"DEV" board** in the IDE (e.g., "ESP32S3 Dev Module").

- In the board options, set the **Partition Scheme** to **"Huge APP"**.

- Ensure you map the correct pin numbers for your specific board in `settings.h`.

## Compile and Run

1. Compile and upload the sketch to your board.

2. Use the **Serial Monitor (115200 baud)** to monitor the software.

3. The device will attempt to connect to your specified WiFi and display the assigned IP address. This IP will also appear on the OLED display if one is installed.

## Using the Web Interface

Enter the device's IP address into a web browser to access the interface. From here, you can:

- Change the RX frequency.

- Toggle telemetry uploads to Sondehub.

- View chasing directions; the main page auto-reloads every 20 seconds to show you which way to go to find your balloon.

---

## Versions

### V0.1.1

- **20-Apr-2026:** Added a frequency offset macro for specific RF chip deviations.

- **20-Apr-2026:** Made AFC flexible; the radio adjusts to the balloon frequency for 1-3 minutes before reverting.

- **18-Apr-2026:** Added a GPS communication check.

- **18-Apr-2026:** Added manual SPI parameter settings in `settings.h` to support "ESP32 DEV" boards on older T-Beams.

- **12-Apr-2026:** Replaced webserver and TCP libraries with `ESP32Async` versions.

- **12-Apr-2026:** Disabled Bluetooth on compatible boards to save resources.

### v0.0.13

- **20-Feb-2026:** Updated code to maintain compatibility with all libraries.

### v0.0.12

- **15-May-2024:** Added a manual position upload button to the web interface.

- **14-May-2024:** Resolved a bug regarding the 30-minute position upload interval.

### v0.0.11

- **28-Jul-2023:** Added support for the APX power management chip (found in T-BEAM v1.1 and v1.2).

- **29-Jun-2023:** Verified compatibility with Radiolib 6.0.1.

- **28-Jun-2023:** Fixed incorrect LoRa mode text in uploads and added a GPS debugging option.

### v0.0.10

- **24-May-2023:** Re-enabled OLED Flash and Flash Pin on packet reception.

- **22-May-2023:** Updated for RadioLib 6.0.0 and optimized radio listening uptime.

### v0.0.9

- **07-Apr-2023:** Temporarily disabled certain OLED/Pin flashing features.

- **20-Mar-2023:** Migrated uploading to a background thread and queue system.

- **15-Mar-2023:** Added support for SSDV.

- **03-Mar-2023:** Updated Serial port baud rate to 115200.

### v0.0.8

- **24-Feb-2023:** Added support for a "FLASH PIN" (HIGH for 300ms upon packet receipt).

- **23-Feb-2023:** Added multiple OLED visual modes (Default, All, Chase).

### v0.0.7

- **03-Feb-2023:** Enabled functionality without WiFi (Serial/OLED only) and updated web links.

### v0.0.6

- **29-Jan-2023:** Added an APRS packet parser for all interfaces.

### v0.0.5

- **21-Jan-2023:** Added LoRa-APRS support and a packet log trail in the web interface.

- **21-Jan-2023:** Improved packet validation and Serial monitor readability.

### v0.0.4

- **20-Jan-2023:** Added radio autotune based on calculated frequency error.

- **20-Jan-2023:** Added support for LoRa Modes 0 and 1; enabled mode switching via web interface.

- **14-Jan-2023:** Added support for LoRa Modes 3 and 5.

### v0.0.3

- **06-Jan-2023:** Added SNR, RSSI, and "time since last packet" to the web interface.

### v0.0.2

- **23-Dec-2022:** Added upload results to the web interface and improved packet identification.

### v0.0.1

- **19-Dec-2022:** Initial versioned release; added frequency updates to OLED and web-based DEVFLAG toggling.

---

## Work in Progress

This software is a work in progress, originally created to track my own balloons. It is provided "as-is" without any guarantees. If you find it useful, please let me know—and most importantly, have fun!

Roel.
