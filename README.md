# TBTracker-RX
A cheap, mobile LoRa High Altitude Balloon receiver for Arduino based on esp32 and sx1278 with support for GPS, a web interface and an OLED display. As of v0.0.9 it supports ssdv.

TBTracker-RX is a sketch for receiving LoRa transmissions from high altitude balloons. It will receive, decode and upload those transmissions.
It is designed to upload telemetry data in the correct format to https://amateur.sondehub.org

# Notes about v0.0.9 (pre release)
v0.0.9 is the first version to support SSDV (tested in LoRa mode 1, no fec packets). However there are a few points to consider:
- As SSDV packets are sent one after another very fast, I had to rewrite a lot of the code to keep up with the received packets. 
- Packets that need uploading are now sent to a queue and uploaded in a seperate thread on another core of the esp32
- As updating the OLED display with "time since last packet" and flashing the "flashpin" takes up a lot of processing time, I temporary disabled these features in v0.0.9. Hopefully I find a good solution to make this faster and enable it again in future versions.
- v0.0.9 is only marginally tested. I would appreciate it if you can give me feedback and testing results

If v0.0.9 is not working to your liking, just revert to v0.0.8.

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

# Versions
v0.0.9:
v0.0.9 pre-release
- 03-MAR-2023: Serial port baudrate to 115200
- 15-MAR-2023: Added support for SSDV
- 20-MAR-2023: changed uploading part of the code. uploading will now take place from a queue and in a seperate thread
- 07-APR-2023: disabled temporary OLED flashing, time since last packet on the OLED display and flashing a pin when a new packets is reveived

v0.0.8:
- 23-FEB-2023: Added support for different visual modes for the OLED (default, all, chase)
- 24-FEB-2023: Added support for a "FLASH PIN" which will set HIGH for 300ms when a packet is received (new entry in settings file!)
 
 Many thanks to Star Holden, Luc Bodson and Eelco de Graaff for testing and suggesting improvements

- Example of the three OLED modes

![IMG-6557](https://user-images.githubusercontent.com/58561387/222697528-747ce37b-25bc-49ef-a2cb-feca527335bf.JPG)
![IMG-6559](https://user-images.githubusercontent.com/58561387/222697536-9b1211c6-e4a5-4414-aa3b-89f98305b998.JPG)
![IMG-6555](https://user-images.githubusercontent.com/58561387/222697542-d7b8b98e-4abe-41ea-9730-17eccb419490.JPG)
 
v0.0.7:
- 03-FEB-2023: The link to Sondehub in the web interface now opens in a new window
- 03-FEB-2023: Software now works also without WiFi (data on Serial output or OLED display)

v0.0.6
- 29-JAN-2023: Added a parser for the APRS packets to display on the Serial interface, webinterface and SSD1306 display

v0.0.5
- 21-JAN-2023: Print length of received packet in the Serial monitor
- 21-JAN-2023: Check if the received packet is indeed a HAB telemetry packet
- 21-JAN-2023: Removed the RAW telemetry string from the Serial monitor to avoid double info and unreadable characters
- 21-JAN-2023: Added support for LoRa-APRS packets
- 21-JAN-2023: Added a packet Log trail in the web interface

v0.0.4:
- 14-JAN-2023: Added support for LoRa Mode 5 (Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off)
- 14-JAN-2023: Added support for LoRa Mode 3 (Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off)
- 20-JAN-2023: Added support for LoRa Mode 0 (Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on)
- 20-JAN-2023: Added support for LoRa Mode 1 (Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off)
- 20-JAN-2023: Added support for showing and changing the LoRa Mode in the webinterface
- 20-JAN-2023: Solved several bugs
- 20-JAN-2023: Added autotune to the radio (based on the frquency error calculated by the radio)

v0.0.3:
- 06-JAN-2023: Added SNR, RSSI to the web interface
- 06-JAN-2023: Added time since latest packet to the web interface
- 06-JAN-2023: Made the Google Maps links open in a new window
- 06-JAN-2023: Solved several reported bugs / unexpected behaviour
- 07-JAN-2023: Changed some UI language. (I apologize, English is not my native language)

v0.0.2:
- 23-DEC-2022: Added upload result to the web interface
- 23-DEC-2022: "comment" string is now disabled by default
- 23-DEC-2022: Try to determine if the packet received is an actual HAB packet or an invalid or unknown packet
- 23-DEC-2022: Changed the wording of "Test mode" in the webinterface
 
v0.0.1: 
- 19-DEC-2022: Changed to x.y.z version numbering
- 19-DEC-2022: Moved version number to TBTracker-rx.ino from settings.h
- 19-DEC-2022: The OLED display will now show frequency updates
- 19-DEC-2022: Added a way to change the DEVFLAG in the webinterface 

v0.1ß:
- 18-DEC-2022: Initial version, released in the Facebook HAB-NL group

# Work in progress
This software is a work in progress. I made it to track my own balloons. Use it at your own risk, there are no guarantees. Let me know if you find it useful or not and as always don't forget to have fun.

Roel.

![TBTracker-RX-001](https://user-images.githubusercontent.com/58561387/208243067-bfdd5e9e-8f6b-4190-9626-1636de4a8068.png)
![TBTracker-RX-002](https://user-images.githubusercontent.com/58561387/208243079-900dfd50-ce42-46ea-b731-e743e1de91d0.png)
![TBTracker-RX-003](https://user-images.githubusercontent.com/58561387/208243093-8fb9749e-5dee-47d9-8347-a0649a3a4bc4.png)
![tb-tracker_v003](https://user-images.githubusercontent.com/58561387/211216791-2b8a34f7-5c6b-442a-bf04-dfc4209e8bcd.png)

