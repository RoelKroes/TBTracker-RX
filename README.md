# TBTracker-RX
A cheap, mobile LoRa High Altitude Balloon receiver for Arduino based on esp32 and sx1278 with support for GPS, a web interface and an OLED display. 

TBTracker-RX is a sketch for receiving LoRa transmissions from high altitude balloons. It will receive, decode and upload those transmissions.
It is designed to upload telemetry data in the correct format to https://amateur.sondehub.org

# Hardware needed
The sketch is designed to compile in the Arduino IDE and work with a TTGO T-Beam board but it will also work with seperate hardware modules.

You will need at least:
- esp32 based board
- LoRa radio module (SX127x or RFM9x module)

Optional components:
- GPS module (Ublox, ATGM or any compatible module)
- SSD1306 LCD display (or compatible)

# Libraries needed
The sketch uses several libraries. Some will probably already be installed in your Arduino IDE but if not, follow the directions below:

- ArduinoJson library (install from the library manager)
- Adafruit_SSD1306 library (install from the library manager)
- Adafruit_GFX library (install from the library manager)
- Adafruit_BusIO library (install from the library manager)
- Radiolib library (install from the library manager)
- TinyGPSPlus library ((install from the library manager)
- ESPAsyncWebServer library (download: https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip )
- AsyncTCP library (download: https://github.com/me-no-dev/AsyncTCP/archive/master.zip )

Install the last two libraries from the IDE menu: Sketch -> Include Library -> Add .ZIP library

# settings.h
You will need to change the values in settings.h before you can use the sketch.
Some important settings:

- DEVFLAG: Set this to true if you want to test uploading to Sondehub but don't want to store the data in the Sondehub database. The telemetry will not appear on the map. You can still check the Serial Monitor to check the upload status of your data. Recommended for testing.
- WiFi settings: You can add up to three wifi networks to the sketch. The software will automatically select the strongest network. Good choices are probalby your home network and the hotspot network from you mobile device.
- CALLSIGN: Change your callsign. It will be shown on sondehub
- UPL_LAT,UPL_LON,UPL_ALT: change these to your current location. It will be used when the software cannot get a valid GPS position
- UPLOAD_YOUR_POSITION: if set to true, the software will upload your position periodically to Sondehub, so it will be show on the map.
- UPLOAD_PAYLOAD_PACKET: if set to true, the software will upload received telemetry to sondehub
- LORA SETTINGS: Change the frequency and LoRa mode (currently I only tested mode 2)

# Compile and run
>> Before you hit the compile button, be sure to select an ESP32 board in the Arduino IDE. Otherwise you will get compile errors! <<

Compile, upload and run the sketch. Use the Serial Monitor to monitor the software. It will try to connect to one of your specified WiFi networks and show the IP-number it got from DHCP. The IP-number will also show on the OLED display (if you have one of those installed on your board). 

# Using the web interface
Just enter the ip-number in a browser and the web interface will show. From the web interface you can change the RX frequency and toggle the option to upload telemetry to Sondehub. The main webpage will autoload every 20 seconds and will show you which direction you need to go if you want to chase your balloon.

# Work in progress
This software is a work in progress. I made it to track my own balloons. Use it at your own risk, there are no guarantees. Let me know if you find it useful or not and as always don't forget to have fun.

Roel.

![TBTracker-RX-001](https://user-images.githubusercontent.com/58561387/208243067-bfdd5e9e-8f6b-4190-9626-1636de4a8068.png)
![TBTracker-RX-002](https://user-images.githubusercontent.com/58561387/208243079-900dfd50-ce42-46ea-b731-e743e1de91d0.png)
![TBTracker-RX-003](https://user-images.githubusercontent.com/58561387/208243093-8fb9749e-5dee-47d9-8347-a0649a3a4bc4.png)
![TBTracker-RX-004](https://user-images.githubusercontent.com/58561387/208243096-233fc4e0-eeb6-426f-93d8-f40e25134dd9.png)

