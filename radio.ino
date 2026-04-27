/***********************************************************************************
 * Radio related stuff
 ***********************************************************************************/
// include the library
#include <RadioLib.h>   // https://github.com/jgromes/RadioLib

/************************************************************************************
* Globals
************************************************************************************/
// Pin numbers are defined in the settings file
// Create radio modules depending on what RF chip you use
#if defined(USE_SX127X)
SX1278 radio = new Module(PIN_NSS, PIN_DIO0, PIN_RESET, PIN_DIO1);
#else
#if defined(USE_SX1280PA)
SX1280 radio = new Module(PIN_NSS, PIN_DIO1, PIN_RESET, PIN_BUSY);
#endif
#endif

#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

uint8_t rawChar[PACKETLEN];

// Define the different packettypes
#define PACKETTYPE_SSDV 1
#define PACKETTYPE_TELEMETRY 2
#define PACKETTYPE_APRS 3
#define PACKETTYPE_UNKNOWN 99


/************************************************************************************
* Setup the Radio
************************************************************************************/
void setupLoRa()
{
  // Initialize the SX1278
  toSerialConsole(F("[LoRa] Initializing ... "));

  // First setup the mode
  // 0 = (normal for telemetry)      Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on 
  // 1 = (normal for SSDV)           Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off
  // 2 = (normal for repeater)       Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off
  // 3 = (normal for fast SSDV)      Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off
  // 5 = (normal for calling mode)   Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off
  // 99 = (LoRa APRS)                Explicit mode, Error coding 4:5, Bandwidth 125KHz,  SF 12 (experimental) - Only receiving, no igating, no uploading, experimental
  
  Radiolib_assert(radio.begin());
  
  switch (LoRaSettings.LoRaMode)
  {
    case 0: 
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 20.8;
      LoRaSettings.SpreadFactor = 11;
      LoRaSettings.SyncWord = 0x12;
      LoRaSettings.ModeString = "LoRa Mode 0";
      break;   

    case 1:
      LoRaSettings.CodeRate = 5;
      LoRaSettings.Bandwidth = 20.8;      
      LoRaSettings.SpreadFactor = 6;    
      LoRaSettings.SyncWord = 0x12;  
      LoRaSettings.implicitHeader = 255;
      LoRaSettings.ModeString = "LoRa Mode 1";
    break;   
    
    case 2:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 62.5;      
      LoRaSettings.SpreadFactor = 8;  
      LoRaSettings.SyncWord = 0x12; 
      LoRaSettings.ModeString = "LoRa Mode 2";   
      break;   

    case 3:
      LoRaSettings.CodeRate = 6;
      LoRaSettings.Bandwidth = 250;      
      LoRaSettings.SpreadFactor = 7;  
      LoRaSettings.SyncWord = 0x12;    
      LoRaSettings.ModeString = "LoRa Mode 3";      
      break;   

    case 5:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 41.7;      
      LoRaSettings.SpreadFactor = 11; 
      LoRaSettings.SyncWord = 0x12;
      LoRaSettings.ModeString = "LoRa Mode 5";           
    break;  

    case 99:
      // Experimental
      // Frequency should be set to 433.775 in settings.h 
      LoRaSettings.CodeRate = 5;
      LoRaSettings.Bandwidth = 125;      
      LoRaSettings.SpreadFactor = 12; 
      LoRaSettings.SyncWord = 0x12;   
      LoRaSettings.ModeString = "LoRa-APRS";        
    break;
  }

  // Set the radio to the correct settings  
  Radiolib_assert(radio.setFrequency(LoRaSettings.Frequency+LORA_FREQ_OFFSET));
  Radiolib_assert(radio.setBandwidth(LoRaSettings.Bandwidth));
  Radiolib_assert(radio.setSpreadingFactor(LoRaSettings.SpreadFactor));
  Radiolib_assert(radio.setCodingRate(LoRaSettings.CodeRate));
  Radiolib_assert(radio.setSyncWord(LoRaSettings.SyncWord));

  // Set the radio to LoRa mode specific settings
  // Add some extra radio parameters
  switch (LoRaSettings.LoRaMode)
  {
   case 0:
      // Low Data Rate Optimization 
      Radiolib_assert(radio.forceLDRO(true));
      Radiolib_assert(radio.explicitHeader());
      radio.setDio0Action(setFlag, RISING);  // As of RadioLib 6.0.0 all methods to attach interrupts no longer have a default level change direction
      Radiolib_assert(radio.startReceive());
   break; 
   case 1:
      // Mode 1 needs an implicit header with data length defined in advance 
      Radiolib_assert(radio.implicitHeader(LoRaSettings.implicitHeader));
      Radiolib_assert(radio.setCRC(true));
      Radiolib_assert(radio.autoLDRO());
      radio.setDio0Action(setFlag, RISING);
      Radiolib_assert(radio.startReceive(LoRaSettings.implicitHeader));
   break;  
   default:
      Radiolib_assert(radio.explicitHeader());
      Radiolib_assert(radio.autoLDRO());
      radio.setDio0Action(setFlag, RISING);
      Radiolib_assert(radio.startReceive());
   break;
  }

  toSerialConsole(F("success!\n"));
  toSerialConsole(F("[LoRa] Waiting for packets on: ")); toSerialConsole(LoRaSettings.Frequency,3); toSerialConsole(F(" MHz\n"));
  toSerialConsole(F("----------------------------\n"));
}

/************************************************************************************
* this function is called when a complete packet is received by the radio module
* IMPORTANT: this function MUST be 'void' type and MUST NOT have any arguments!
************************************************************************************/
void setFlag(void) 
{
  // we got a packet, set the flag
  receivedFlag = true;
  start = millis();
}

/************************************************************************************
* Start receiving next packet
************************************************************************************/
void startReceive()
{
  if (LoRaSettings.LoRaMode == 1) {
    Radiolib_assert(radio.startReceive(LoRaSettings.implicitHeader));
  } else {
    Radiolib_assert(radio.startReceive());
  }
}

/************************************************************************************
* Process a received LoRa packet. 
************************************************************************************/
void receiveLoRa()
{
    // reset the data received flag
    receivedFlag = false;

    // Read data from the radio
    int state;

    // Buffer to hold the received data from the radio
    byte buf[PACKETLEN];
    // Init the buffer to zeros
    // memset(buf,0x00,sizeof(buf));

    // Grab the data from the radio module
    switch(LoRaSettings.LoRaMode)
    {
       case 1:  // Implicit header, so tell the radio how many bytes to read
          state = radio.readData(buf,LoRaSettings.implicitHeader);
       break;
       default: 
          state = radio.readData(buf,0);
      break;
    }
    
    if (state == RADIOLIB_ERR_NONE) 
    {
      // A LoRa packet was successfully received
      // Now process it.
      // 1. Get as much metadata from the radio as possible
      Telemetry.rxPacketLen = radio.getPacketLength();
      Telemetry.rssi = radio.getRSSI();
      Telemetry.snr = radio.getSNR();
      Telemetry.frequency_error = radio.getFrequencyError();

      // Reset the AFC counter
      AFCCounter = millis();

      // Get the frequency error and retune
      // but only do this if AFC is enabled
      if (radio_afc > 0)
      {
        LoRaSettings.Frequency = LoRaSettings.Frequency - (Telemetry.frequency_error / 1000000);
        Radiolib_assert(radio.setFrequency(LoRaSettings.Frequency+LORA_FREQ_OFFSET));
      }

      // Then get back to receiving
      startReceive();

      // Flash the OLED display 
#if defined(USE_SSD1306)
      displayFlash();
#endif  

    // Flash the LED
#if defined(FLASH_PIN)
    flashPin();
#endif
          
      // Print lots of data
      toSerialConsole(F("\n"));
      
      // Print the first 10 hex chars of the packet
      toSerialConsole(F("[RADIO] first 10 hex chars:\t"));
      for (int i = 0; i < 10; i++)
      {
        toSerialConsoleHEX(buf[i]);
        toSerialConsole(F(" "));
      }
      toSerialConsole(F("\n"));

      // Get the time from the ESP, so we have a timestamp
      formatLocalTime();
      Telemetry.atmillis = millis();

      // Process datapacket from the radio, print it to the serial port and store it in the telemetry struct
      toSerialConsole(F("[RADIO] Received packet:\t"));
      toSerialConsole(Telemetry.time_received);
      toSerialConsole(F("\n"));
     
      // Length of the latest packet that was received
      toSerialConsole(F("[RADIO] Packet length:\t\t"));
      toSerialConsole(Telemetry.rxPacketLen);
      toSerialConsole(F("\n"));

      // print RSSI (Received Signal Strength Indicator)
      toSerialConsole(F("[RADIO] RSSI:\t\t\t"));
      toSerialConsole(Telemetry.rssi);
      toSerialConsole(F(" dBm\n"));

      // print SNR (Signal-to-Noise Ratio)
      toSerialConsole(F("[RADIO] SNR:\t\t\t"));
      toSerialConsole(Telemetry.snr);
      toSerialConsole(F(" dB\n"));

      // print frequency error
      toSerialConsole(F("[RADIO] Frequency error:\t"));
      toSerialConsole(Telemetry.frequency_error);
      toSerialConsole(F(" Hz \n"));
      Telemetry.frequency = LoRaSettings.Frequency;
   
      // 2. Check the type of packet
      // SSDV ?
      if ( ((buf[0] & 0x7F) == 0x66) || ((buf[0] & 0x7F) == 0x67) ||		// SSDV 
			     ((buf[0] & 0x7F) == 0x68) || ((buf[0] & 0x7F) == 0x69)
        )
      {
          LoRaSettings.packetType = PACKETTYPE_SSDV ;
      }  
      else if (buf[0] == '$' && buf[1]=='$')  // Telemetry
      {
        LoRaSettings.packetType = PACKETTYPE_TELEMETRY ;
      }
      else if (buf[0] == '<' && buf[1] == 0xff && buf[2] == 0x01) // APRS
      {
        LoRaSettings.packetType = PACKETTYPE_APRS ;
      }
      else
      {
        LoRaSettings.packetType = PACKETTYPE_UNKNOWN ; // UNKNOWN OR corrupted
      }

      // Print the packet type
      toSerialConsole("[RADIO] Packet type:\t\t");
      switch (LoRaSettings.packetType)
      {
        case PACKETTYPE_SSDV: 
          toSerialConsole(F("SSDV\n")); 
          processSSDVPacket(buf);
          // Put the SSDV packet into the upload queue
          if (Telemetry.uploadSondehub) postSSDVinQueue(buf);    
          addToLog();
        break;
        case PACKETTYPE_TELEMETRY: 
          toSerialConsole(F("TELEMETRY\n")); 
          processTelemetryPacket(buf);
          addToLog();
        break;
        case PACKETTYPE_APRS:
          toSerialConsole(F("LORA-APRS\n")); 
          // Valid APRS packet
          parseAPRSPacket(buf);
          addToLog();
        break;

        default: toSerialConsole(F("UNKNOWN\n")); break;
      }
      toSerialConsole(F("\n"));
    } 
    else if (state == RADIOLIB_ERR_CRC_MISMATCH) 
    {
      startReceive(); // This packet is a dud, so start listening for the next one
      // packet was received, but is malformed
      toSerialConsole(F("[RADIO] CRC error - maybe adjust frequency a bit?\n"));
    } 
    else 
    {
      startReceive(); // This packet is a dud, so start listening for the next one
      // some other error occurred
      toSerialConsole(F("[RADIO] Failed, code "));
      toSerialConsole(state); toSerialConsole(F("\n"));
      Telemetry.raw = "Invalid Packet";
    }
}


/************************************************************************************
* Process a telemetry packet
************************************************************************************/
void processTelemetryPacket(byte *buf)
{
   bool validPacket = true;
   int i =0;
     // Check if it is telemetry whenever the LoRaMode is not APRS
  if (LoRaSettings.LoRaMode < 99)
  {  
    while (i < Telemetry.rxPacketLen-2  && buf[i] != '\n')
    {
      if (buf[i] < ' ' || buf[i] > '~')
      {
        validPacket = false;
        Telemetry.raw = "Received a telemetry packet but it is corrupted.";
        toSerialConsole(Telemetry.raw);
        toSerialConsole(F("\n"));
      }
      i++;
    }
    if (validPacket)
    {
      packetCounter++;   
      getMetafromRaw((char *) buf);
      parseRawData((char *) buf);
    }
  }
  else
  {
    // APRS telemetry packet
     // Check if it is a valid LoRa-APRS packet
     if (buf[0] != '<' || buf[1] != 0xff || buf[2] != 0x01)
     {
       // not a LoRa-APRS packet
       Telemetry.raw = "Received a LoRa-APRS packet but it is not valid";
     }
     // Extra check for validity of package: Read until end of package. It should all be printable ASCII
     for (i=3 ; i < Telemetry.rxPacketLen-1; i++)
     {
       if (buf[i] < ' ' || buf[i] > '~' )
       {
         Telemetry.raw = "Received a LoRa-APRS packet but it is corrupted.";
         toSerialConsole(Telemetry.raw);
         toSerialConsole(F("\n"));
       }
      }
  }
}


/************************************************************************************
* Check if received packet is a LoRa HAB packet.
* Should be updated later for all the different HAB packets
************************************************************************************/
bool checkIfHABPacket()
{
  int i;
  int j;
  // Check if it is telemetry whenever the LoRaMode is not APRS
  if (LoRaSettings.LoRaMode < 99)
  {
    if (Telemetry.raw.indexOf("$$") != 0)
    {
      // not a HAB package
      Telemetry.raw = "Received a packet but it is not a HAB telemetry packet.";
      return false;
    }
    i = 0;
    j = 2;
    // Extra check for validity of package: Read until end of package. It should all be printable ASCII
    while (i < Telemetry.rxPacketLen-j  && Telemetry.raw[i] != '\n')
    {
      if (Telemetry.raw[i] < ' ' || Telemetry.raw[i] > '~')
      {
        toSerialConsole(Telemetry.raw); toSerialConsole(F("\n"));
        Telemetry.raw = "Received a packet but it is corrupted.";
        return false;
      }
      i++;
    }
  }
  else
  {
     // Check if it is a LoRa-APRS packet
     if (Telemetry.raw.substring(0, 3) != "<\xff\x01")
     {
       // not a LoRa-APRS packet
       Telemetry.raw = "Received a LoRa-APRS packet but it is not valid";
       return false;
     }
     i=3;
     j=1;
     // Extra check for validity of package: Read until end of package. It should all be printable ASCII
     for (i ; i < Telemetry.rxPacketLen-j; i++)
     {
       if (Telemetry.raw[i] < ' ' || Telemetry.raw[i] > '~' )
       {
         toSerialConsole(Telemetry.raw); toSerialConsole(F("\n"));
         Telemetry.raw = "Received a packet but it is corrupted.";
         return false;
       }
      }
  }
  
  return true;
}

/************************************************************************************
* Change the RX frequency or one of the scanner frequencies
************************************************************************************/
bool changeFrequency(String newFrequency, int nr)
{
  int str_len = newFrequency.length() + 1;
  char char_array[str_len];
  newFrequency.toCharArray(char_array, str_len);
  TBScanner.scanFreq[nr] = atof(char_array);
  if (nr == 0)
  {
    LoRaSettings.Frequency = TBScanner.scanFreq[0];
    // Also change the initial frequency as set by the user
    initial_Frequency = LoRaSettings.Frequency;
    AFCCounter = millis();
  } 
  setupLoRa();
#if defined(USE_SSD1306)
  updateOLEDforFrequency();
#endif
  return true;
}

bool changeFrequency(float newFrequency)
{
   LoRaSettings.Frequency = newFrequency;
   setupLoRa();
#if defined(USE_SSD1306)
  updateOLEDforFrequency();
#endif
  return true;
}


/************************************************************************************
* Change LoRa Mode
************************************************************************************/
bool changeLoRaMode(int newMode)
{
  if (newMode >= 0 && newMode != LoRaSettings.LoRaMode)
  {
      LoRaSettings.LoRaMode = newMode;
      setupLoRa(); 
      return true;
  }
  else
  {
     return false; 
  }
}


/************************************************************************************
* Change AFC Mode
************************************************************************************/
bool changeradioAFC(int newMode)
{
  if (newMode >= 0 && newMode != radio_afc)
  {
      radio_afc = newMode;
      AFCCounter = millis();
      return true;
  }
  else
  {
     return false; 
  }
}
  
