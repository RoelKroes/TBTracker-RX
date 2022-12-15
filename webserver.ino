/************************************************************************************
* All webserver related stuff
************************************************************************************/
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request);

/************************************************************************************
* This is the code for the HTML page as a RAW literal. You can change this if you want
* Basically it is a template that contains variables which will be replaced by actual
* values when sent to the client.
* 
* The variables in the templates look like %VARIABLE% and will be replaced with actual 
* values in the function "processor"
************************************************************************************/
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<meta charset="utf-8">
  
<style type="text/css">
.myTable { background-color:#FFFFE0;border-collapse:collapse; }
.myTable th { background-color:#3F4DE4;color:white;width:50%% }
.myTable {font-family:verdana;}  
.myTable td, .myTable th { padding:5px;border:1px solid #000;  }
.myBody{font-family:verdana;}   
</style>  
  
<title>TBTracker-RX</title>
<meta http-equiv="refresh" content="20">
</head>
<body class="myBody" >
<table class="myTable" border="0" width="100%%" height="50">
  <tbody>
    <tr>
      <th align="center">%TBTRACKERRX%</th>
    </tr>
  </tbody>
</table>

<table class="myTable" border="0" width="100%%" height="50">
  <tbody>
    <tr>
    <td>Frequency:</td><td>%FREQUENCY%</td>
    <td><form action="/get"><input type="text" name="frequency"><input type="submit" value="change"></form></td> 
    </tr>
    <tr>
    <td>Your callsign:</td><td>%CALLSIGN%</td><td>&nbsp;</td>
    </tr>
    <tr>
    <td>Your location:</td><td><a href="https:www.google.com/maps/place/%LOCATION%">Google Maps</a></td><td>&nbsp;</td>
    </tr>
    <tr>
    <td>Upload to <a href="https:amateur.sondehub.org/">Sondehub:</a></td><td>%SONDEHUB%</td>
    <td><form action="/get2"><input type="submit" value="toggle"></form></td>
    </tr>
  </tbody>
</table>
 
<table class="myTable" border="0" width="100%%" height="50">
  <tbody>
    <tr>
      <th align="center">LATEST PACKET</th>
    </tr>
  </tbody>
</table>
  
<table class="myTable" border="0" width="100%%" height="50">
  <tbody>
    <tr>
    <td>Telemetry:</td><td style="white-space:nowrap">%TELEMETRY%</td>
    </tr>
    <tr>
    <td>Payload ID:</td><td>%PAYLOADID%</td>
    </tr>
    <tr>
    <td>Location:</td><td><a href="https:www.google.com/maps/place/%PAYLOADLOCATION%">Google Maps</a></td>
    </tr>
    <tr>
    <td>Altitude:</td><td>%ALTITUDE%</td>
    </tr>
    <tr>
    <td>Distance (km):</td><td>%DISTANCE%</td>
    </tr>    
    <tr>
    <td>Bearing (degr.):</td><td>%BEARING% - travel <B>%COMPASS%</B> to chase</td>
    </tr>    
  </tbody>
</table>  
<p>
(Page will autoload every 20 seconds. If not, press the button)
</p>  
<input type="button" value="Refresh Page" onClick="location.href=location.href">
</body>
</html>)rawliteral";

/************************************************************************************
* The function that replaces the variables from the template with real values
************************************************************************************/
String processor(const String& var)
{ 
  if (var == "TBTRACKERRX")
    return "TBTRACKER-RX " + String(TBTRACKER_VERSION);
  else if (var == "FREQUENCY")
    return  String(LoRaSettings.Frequency,3);
  else if (var == "CALLSIGN")
    return String(CALLSIGN);
  else if (var == "TELEMETRY")
    return Telemetry.raw;
  else if (var == "PAYLOADID")
    return Telemetry.payload_callsign;
  else if (var == "PAYLOADLOCATION")
    return String(Telemetry.lat,5) + "," + String(Telemetry.lon,5);
  else if (var == "ALTITUDE")
    return String(Telemetry.alt,0);
  else if (var == "DISTANCE")
    return String(Telemetry.distance,1);
  else if (var == "BEARING")
    return String(Telemetry.bearing,0);    
  else if (var == "COMPASS")
    return String(Telemetry.compass);        
  else if (var == "LOCATION")
  {
#if defined(USE_GPS)    
    processGPSData();
    return String(Telemetry.uploader_position[0],5) + "," + String(Telemetry.uploader_position[1],5);
#else
   return String(UPL_LAT) + "," + String(UPL_LON);
#endif
  }
  else if (var == "SONDEHUB")
    if (Telemetry.uploadSondehub)
      return String("Yes");
    else
      return String("No");
   
  return String();
 
}


/************************************************************************************
* HTML template for when the user requests a non existing page
************************************************************************************/
void notFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "Not found");
}

/************************************************************************************
* Setting up the webserver with logic when a specific page is requested.
************************************************************************************/
void setupWebserver()
{
    
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", index_html, processor);
    });

    server.on("/get2", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      if (Telemetry.uploadSondehub)
        Telemetry.uploadSondehub = false;
      else
        Telemetry.uploadSondehub = true;
        request->send(200, "text/html", "Upload to Sondehub was changed.<br><a href=\"/\">Return to Home Page</a>");
    });


    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
      String inputMessage;
      String inputParam;
  
      // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
      if (request->hasParam("frequency")) 
      {
        inputMessage = request->getParam("frequency")->value();
        inputParam = "frequency";
      }
      Serial.print("Frequency change to: ");
      Serial.println(inputMessage); 
      // Try to change the frequency
      if ( changeFrequency(inputMessage) ) 
      {
         request->send(200, "text/html", "Frequency changed to " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");
      }
      else
      {
        request->send(200, "text/html", "ERROR changing frequency to " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");
      }
    });
 
    server.onNotFound(notFound);
    server.begin();
}
