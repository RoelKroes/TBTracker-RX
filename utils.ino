/************************************************************************************
* some useful utilities
************************************************************************************/
  
  /************************************************************************************
  * Will return a string that is used to display the time since the last reception
  * of a valid packet.
  ************************************************************************************/
  String getDuration(unsigned long lastReceived)
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
       res.concat(" days ago)");  
       return res;
    }
    
    if (hours > 0)
    {
      res += hours;
      res.concat(" hours ago)");  
      return res;
    }
    
    if (minutes > 0)
    {
      res += minutes;
      res.concat(" minutes ago)");  
      return res;
    } 
    
    if (seconds > 0)
    {
      res += seconds;
      res.concat(" seconds ago)");
      return res;
    }

    return "";    
  }