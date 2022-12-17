/***********************************************************************************
 * 
 * SETTING UP RADIO
 * 
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


/************************************************************************************
* Setup the Radio
* Currently only LoRa mode 2 is supported.
************************************************************************************/
void setupLoRa()
{
  // Initialize the SX1278
  Serial.print(F("[LoRa] Initializing ... "));

  // First setup the mode
  // 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on - NOT IMPLEMENTED YET
  // 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off - NOT IMPLEMENTED YET
  
  // 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off - Should work correctly
  // 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off - Should work correctly
  
  switch (LORA_MODE)
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
  // Add a CRC. some receiver networks like tinyGS use CRC
  radio.setCRC(true);

  // start listening for LoRa packets
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) 
  {
    Serial.println(F("success!"));
    Serial.print("[LoRa] Waiting for packets on: "); Serial.print(LoRaSettings.Frequency,3); Serial.println(" MHz");
    Serial.println(F("----------------------------"));
  } 
  else 
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // Upload telemetry?
  if (UPLOAD_PAYLOAD_PACKET)
    Telemetry.uploadSondehub = true;
  else
    Telemetry.uploadSondehub = false;
}

/************************************************************************************
// this function is called when a complete packet is received by the radio module
// IMPORTANT: this function MUST be 'void' type and MUST NOT have any arguments!
************************************************************************************/
void setFlag(void) 
{
  // check if the interrupt is enabled
  if(!enableInterrupt) {
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
    int state = radio.readData(Telemetry.raw);

    if (state == RADIOLIB_ERR_NONE) 
    {
      // packet was successfully received

      // Get the time from the ESP, so we have a timestamp
      formatLocalTime();

      // Process datapacket from the radio, print it to the serial port and store it in the telemetry struct
      Serial.print(F("[SX1278] Received packet:\t"));
      Serial.println(Telemetry.time_received);

      // print data of the packet
      Serial.print(F("[SX1278] Raw Data:\t\t"));
      Serial.println(Telemetry.raw);

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1278] RSSI:\t\t\t"));
      Telemetry.rssi = radio.getRSSI();
      Serial.print(Telemetry.rssi);
      Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1278] SNR:\t\t\t"));
      Telemetry.snr = radio.getSNR();
      Serial.print(Telemetry.snr);
      Serial.println(F(" dB"));

      // print frequency error
      Serial.print(F("[SX1278] Frequency error:\t"));
      Telemetry.frequency_error = radio.getFrequencyError();
      Serial.print(Telemetry.frequency_error);
      Serial.println(F(" Hz"));
      Serial.println();
      Telemetry.frequency = LoRaSettings.Frequency + (Telemetry.frequency_error / 1000000);
      
      // Parse the RAW payload data and post it to SondeHub
      packetCounter++;
      getMetafromRaw(Telemetry.raw);
      parseRawData(Telemetry.raw);

    } 
    else if (state == RADIOLIB_ERR_CRC_MISMATCH) 
    {
      // packet was received, but is malformed
      Serial.println(F("[SX1278] CRC error!"));
    } 
    else 
    {
      // some other error occurred
      Serial.print(F("[SX1278] Failed, code "));
      Serial.println(state);

    }

    // put radio back to listen mode
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }
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
  return true;
}

  