/************************************************************************************
* some useful utilities
************************************************************************************/
  
  /************************************************************************************
  * Will return a string that is used to display the time since the last reception
  * of a valid packet.
  ************************************************************************************/
  String getDuration(unsigned long lastReceived, bool shortStr)
  {
    String res="";
    unsigned long durationMillis = millis() - lastReceived;
    unsigned long seconds = durationMillis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;
    res.concat("("); 
    if (days > 0)
    {
       res += days;
       if (shortStr)
         res.concat(" days)");  
       else  
         res.concat(" days ago)");  
       return res;
    }
    
    if (hours > 0)
    {
      res += hours;
      if (shortStr)
        res.concat("hr ago)");  
      else
        res.concat(" hours ago)");  
      return res;
    }
    
    if (minutes > 0)
    {
      res += minutes;
      if (shortStr)
         res.concat("m ago)");  
      else
        res.concat(" minutes ago)");  
      return res;
    } 
    
    if (seconds > 0)
    {
      res += seconds;
      if (shortStr)
         res.concat("s ago)");
      else
         res.concat(" seconds ago)");
      return res;
    }

    return "";    
  }

#if defined(FLASH_PIN)
  /************************************************************************************
  * Setup the flash pin if it was defined in the settings file
  ************************************************************************************/
  void setupFlashPin()
  {
    pinMode(FLASH_PIN, OUTPUT);
    digitalWrite(FLASH_PIN,LOW);
  }
  
  /************************************************************************************
  * enable the FLASH_PIN for 300ms
  ************************************************************************************/
  void flashPin()
  {
    digitalWrite(FLASH_PIN, HIGH); 
    pinMillis = millis();  
  }

  /************************************************************************************
  * disable the FLASH_PIN
  ************************************************************************************/
  void disablePin()
  {
      digitalWrite(FLASH_PIN, LOW); 
  }

#endif

//============================================================================
// Sends output to the serial console with 1ms delay between characters
//============================================================================
template <typename T>
void toSerialConsole(T data) {  
#if defined(ALLOWDEBUG)	
  // this will work for int, float, double, char, etc.
  String Str = String(data);
  
  // Send the String one char at the time
  for (unsigned int i = 0; i < Str.length(); i++) {
    Serial.write(Str[i]);
    delay(1); // 1ms delay for esp32s2 and c3 mcu with native USB comms.
  }
	Serial.flush();
#endif	
}

void toSerialConsole(const __FlashStringHelper* data) {
#if defined(ALLOWDEBUG)
  // Maak een pointer naar het flashgeheugen
  PGM_P p = reinterpret_cast<PGM_P>(data);
  
  // Lees de tekst karakter voor karakter direct uit het flashgeheugen
  char c;
  while ((c = pgm_read_byte(p++)) != 0) {
    Serial.write(c);
    delay(1); 
  }
  Serial.flush();
#endif
}

// version for floats where you can specify the number of decimals
void toSerialConsole(double data, int decimals) {
#if defined(ALLOWDEBUG)	
  String Str = String(data, decimals);
  for (unsigned int i = 0; i < Str.length(); i++) {
    Serial.write(Str[i]);
    delay(1);
  }
	Serial.flush();
#endif	
}


// print a number in HEX to the serial console
// version for floats where you can specify the number of decimals
void toSerialConsoleHEX(int lNumber) {
#if defined(ALLOWDEBUG)	
    if (lNumber < 16) Serial.print("0");
    Serial.print(lNumber, HEX);
	  Serial.flush();
#endif	
}

//============================================================================
// Evaluate the results of a call to the Radiolib library
// If the call does not succeed, wait 30 seconds, reset the esp32 and try again.
//============================================================================
void Radiolib_assert(int16_t lState) {
	if (lState >= RADIOLIB_ERR_NONE)  // Change this to (state == ERR_NONE) if you use an older radiolib library
	{
		toSerialConsole("done\n");
	} else 
	{
		toSerialConsole("failed, code ");
		toSerialConsole(lState); toSerialConsole("\n");
    toSerialConsole("Waiting 30 secs to reboot...");
		delay(30000);
		ESP.restart();
	}
}
