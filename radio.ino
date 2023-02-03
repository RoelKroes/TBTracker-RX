/***********************************************************************************
 * Radio related stuff
 ***********************************************************************************/
// include the library
#include <RadioLib.h>   // https://github.com/jgromes/RadioLib

/************************************************************************************
* Globals
************************************************************************************/
// Pin numbers are defined in the settings file
SX1278 radio = new Module(PIN_NSS, PIN_DIO0, PIN_RESET, PIN_DIO1);

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

// MAX possible length for a packet
#define PACKETLEN 255

/************************************************************************************
* Setup the Radio
************************************************************************************/
void setupLoRa()
{
  // Initialize the SX1278
  Serial.print(F("[LoRa] Initializing ... "));

  // First setup the mode
  // 0 = (normal for telemetry)      Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on 
  // 1 = (normal for SSDV)           Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off
  // 2 = (normal for repeater)       Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off
  // 3 = (normal for fast SSDV)      Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off
  // 5 = (normal for calling mode)   Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off
  // 99 = (LoRa APRS)                Explicit mode, Error coding 4:5, Bandwidth 125KHz,  SF 12 (experimental) - Only receiving, no igating, no uploading, experimental
  switch (LoRaSettings.LoRaMode)
  {
    case 0: 
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 20.8;
      LoRaSettings.SpreadFactor = 11;
      break;   

    case 1:
      LoRaSettings.CodeRate = 5;
      LoRaSettings.Bandwidth = 20.8;      
      LoRaSettings.SpreadFactor = 6;      
    break;   
    
    case 2:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 62.5;      
      LoRaSettings.SpreadFactor = 8;      
      break;   

    case 3:
      LoRaSettings.CodeRate = 6;
      LoRaSettings.Bandwidth = 250;      
      LoRaSettings.SpreadFactor = 7;            
      break;   

    case 5:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 41.7;      
      LoRaSettings.SpreadFactor = 11;            
    break;  

    case 99:
      // Experimental
      // Frequency should be set to 433.775 in settings.h 
      LoRaSettings.CodeRate = 5;
      LoRaSettings.Bandwidth = 125;      
      LoRaSettings.SpreadFactor = 12;            
    break;
  }
  
  int16_t state = radio.begin
  (
    LoRaSettings.Frequency,
    LoRaSettings.Bandwidth,
    LoRaSettings.SpreadFactor,
    LoRaSettings.CodeRate,
    LoRaSettings.SyncWord,
    LoRaSettings.Power,
    LoRaSettings.PreambleLength, 
    LoRaSettings.Gain
  );

  // set the function that will be called
  // when a new packet is received
  radio.setDio0Action(setFlag);

  // Add some extra radio parameters
  switch (LoRaSettings.LoRaMode)
  {
   case 0:
      // Low Data Rate Optimization 
      radio.forceLDRO(true);
   break; 
   case 1:
      // Mode 1 needs an implicit header with data length defined in advance 
      radio.implicitHeader(PACKETLEN);
      radio.setCRC(true);
   break;  
   default:
      radio.explicitHeader();
   break;
  }

  // start listening for LoRa packets
  if (LoRaSettings.LoRaMode==1)
  {
    // Listen for mode 1 packets  
    state = radio.startReceive(PACKETLEN);
  }
  else
  {
    state = radio.startReceive();
  }
  
  if (state == RADIOLIB_ERR_NONE) 
  {
    Serial.println(F("success!"));
    Serial.print(F("[LoRa] Waiting for packets on: ")); Serial.print(LoRaSettings.Frequency,3); Serial.println(F(" MHz"));
    Serial.println(F("----------------------------"));
  } 
  else 
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

/************************************************************************************
* this function is called when a complete packet is received by the radio module
* IMPORTANT: this function MUST be 'void' type and MUST NOT have any arguments!
************************************************************************************/
void setFlag(void) 
{
  // check if the interrupt is enabled
  if(!enableInterrupt) 
  {
    return;
  }
  // we got a packet, set the flag
  receivedFlag = true;
}


/************************************************************************************
* Process a received LoRa packet. 
************************************************************************************/
void receiveLoRa()
{
  // check if the flag is set
  if(receivedFlag) 
  {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;
    // reset flag
    receivedFlag = false;

    // Read data from the radio
    int state;
    
    // Get the received packet length
    Telemetry.rxPacketLen = radio.getPacketLength();

    switch(LoRaSettings.LoRaMode)
    {
       case 1: 
          radio.readData(Telemetry.raw,PACKETLEN);
      break;
       default: radio.readData(Telemetry.raw); break;
    }
    
    if (state == RADIOLIB_ERR_NONE) 
    {

      Serial.println();

      // Get the time from the ESP, so we have a timestamp
      formatLocalTime();
      Telemetry.atmillis = millis();

      // Process datapacket from the radio, print it to the serial port and store it in the telemetry struct
      Serial.print(F("[RADIO] Received packet:\t"));
      Serial.println(Telemetry.time_received);
     
      // Length of the latest packet that was received
       Serial.print(F("[RADIO] Packet length:\t\t"));
       Serial.println(Telemetry.rxPacketLen);

      // print RSSI (Received Signal Strength Indicator)
       Serial.print(F("[RADIO] RSSI:\t\t\t"));
       Telemetry.rssi = radio.getRSSI();
       Serial.print(Telemetry.rssi);
       Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[RADIO] SNR:\t\t\t"));
      Telemetry.snr = radio.getSNR();
      Serial.print(Telemetry.snr);
      Serial.println(F(" dB"));

      // print frequency error
      Serial.print(F("[RADIO] Frequency error:\t"));
      Telemetry.frequency_error = radio.getFrequencyError();
      Serial.print(Telemetry.frequency_error);
      Serial.println(F(" Hz (Radio will be retuned)"));
      Serial.println();
      LoRaSettings.Frequency = LoRaSettings.Frequency - (Telemetry.frequency_error / 1000000);
      Telemetry.frequency = LoRaSettings.Frequency;


      // packet was successfully received, determine if it is a HAB packet by checking for "$$"
      // as the first two characters of the LoRa packet the radio received.
      if ( !checkIfHABPacket())
      {
        // not a HAB package. display it on the Serial interface for debugging or analyzing
        Serial.println(Telemetry.raw);
      }
      else
      { 
        // Valid telemetry packet!  
        packetCounter++;   
        if (LoRaSettings.LoRaMode < 99)
        {
          // Parse the RAW payload data, create a JSON and post it to SondeHub
          getMetafromRaw(Telemetry.raw);
          parseRawData(Telemetry.raw);
        }
        else
        {
          // Valid APRS packet
          parseAPRSPacket(Telemetry.raw.substring(3));
        }
        // Add the packet to the log
        addToLog();
      }
    } 
    else if (state == RADIOLIB_ERR_CRC_MISMATCH) 
    {
      // packet was received, but is malformed
      Serial.println(F("[RADIO] CRC error!"));
      Telemetry.raw = "Invalid Packet - CRC Error";
    } 
    else 
    {
      // some other error occurred
      Serial.print(F("[RADIO] Failed, code "));
      Serial.println(state);
      Telemetry.raw = "Invalid Packet";
    }

    setupLoRa();
    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
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
        Serial.println(Telemetry.raw);
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
         Serial.println(Telemetry.raw);
         Telemetry.raw = "Received a packet but it is corrupted.";
         return false;
       }
      }
  }
  
  return true;
}

/************************************************************************************
* Change the RX frequency.  
************************************************************************************/
bool changeFrequency(String newFrequency)
{
  int str_len = newFrequency.length() + 1;
  char char_array[str_len];
  newFrequency.toCharArray(char_array, str_len);
  LoRaSettings.Frequency = atof(char_array);
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

  
