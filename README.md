# TBTracker-RX
A cheap, mobile LoRa High Altitude Balloon receiver for Arduino based on esp32 and sx1278 with support for GPS, a web interface and a LCD display. 

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

- WiFi settings: You can add up to three wifi networks to the sketch. The software will automatically select the strongest network. Good choices are probalby your home network and the hotspot network from you mobile device.
- CALLSIGN: Change your callsign. It will be shown on sondehub
- UPL_LAT,UPL_LON,UPL_ALT: change these to your current location. It will be used when the software cannot get a valid GPS position
- UPLOAD_YOUR_POSITION: if set to true, the software will upload your position periodically to Sondehub, so it will be show on the map.
- UPLOAD_PAYLOAD_PACKET: if set to true, the software will upload received telemetry to sondehub
- LORA SETTINGS: Change the frequency and LoRa mode (currently I only tested mode 2)

# Compile and run
Compile, upload and run the sketch. Use the Serial Monitor to monitor the software. It will try to connect to one of your specified WiFi networks and show the IP-number it got. The IP-number will also show on the LCD display (if you have one of those installed on your board). 

# Using the web interface
Just enter the ip-number in a browser and the web interface will show. From the web interface you can change the RX frequency and toggle the option to upload telemetry to Sondehub. The main webpage will autoload every 20 seconds and will show you which direction you need to go if you want to chase your balloon.

# Work in progress
This software is a work in progress. I made it to track my own balloons. Use it at your own risk, there are no guarantees. Let me know if you find it useful or not and as always don't forget to have fun.

Roel.


