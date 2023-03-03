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
      delay(300);
      digitalWrite(FLASH_PIN, LOW); 
  }
#endif