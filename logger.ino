/************************************************************************************
* Functions to log the packets in a FIFO queue
************************************************************************************/

// change this if you want more packets logged in the web interface
#define  MAXLOGS 10
 
String logger[MAXLOGS];
unsigned long millisLogger[MAXLOGS];

/************************************************************************************
* Add the received packet to the queue for display on the web interface
************************************************************************************/
void addToLog()
{
  // Bubble the log to most recent packet first. 
  // purge the oldest record when the log is full
  for(int i=MAXLOGS-1; i > 0; i--)
  {
    logger[i]=logger[i-1];
    millisLogger[i]=millisLogger[i-1];
  }
  logger[0]=Telemetry.raw;
  millisLogger[0]=Telemetry.atmillis;
}


/************************************************************************************
* Deliver the whole queue in HTML format so it can be displayed on the web interface
************************************************************************************/
String getLogs()
{
  String res="";

  for(int i=0; i<MAXLOGS; i++)
  {
    if (logger[i] != "")
    {
      // Start of row
      res += "<tr>";
      
      // Time since
      res +="<td style=""white-space:nowrap"">";
      res += getDuration(millisLogger[i],false);
      res += "</td>";
      // Telemetry string
      res +="<td style=""white-space:nowrap"">";
      res += logger[i];
      res += "</td>";

      // End of row
      res += "</tr>";
    }
  }
  return res;
}


