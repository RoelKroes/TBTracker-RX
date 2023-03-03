/************************************************************************************
* All OLED SSD1306 related stuff
************************************************************************************/
#if defined(USE_SSD1306)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_DEFAULT 0
#define OLED_CHASE 1
#define OLED_GOD 2 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int oledMode = OLED_DEFAULT;

/************************************************************************************
* Initialize the SSD1306
************************************************************************************/
void setupSSD1306()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

/************************************************************************************
* This function is called about once per second
************************************************************************************/
void timedOledUpdate()
{
   // If time since last packet is one the screen, update the timing 
   switch(oledMode)
   {
     case OLED_GOD:
     case OLED_CHASE:
      // clear half of the first line 
      for (int y=0; y<=6; y++)
      {
       for (int x=0; x<64; x++)
       {
         display.drawPixel(x, y, BLACK); 
       }
      }
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print(getDuration(Telemetry.atmillis,true));
      display.setCursor(97, 0);
      display.print("#"); display.print(packetCounter);
      display.display();
     break;
   }
}


/************************************************************************************
* display a Text message on the OLED SSD1306
************************************************************************************/
void displayOled(int X, int Y, const char* str)
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(X, Y);
  // Display static text
  display.print(str);
  display.display(); 
}

/************************************************************************************
* clear the OLED display
************************************************************************************/
void displayClear()
{
  // Clear the buffer
  display.clearDisplay();
  display.display();
}

/************************************************************************************
* clear the OLED mode
************************************************************************************/
bool changeOLEDMode(int aMode)
{
  oledMode = aMode;
  displayUpdate();
  return true;
}

/************************************************************************************
* display packet data on the OLED display
************************************************************************************/
void displayUpdate()
{
  // flash the screen
  displayFlash();

  switch (oledMode)
  {
    case OLED_DEFAULT:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("IP: ");
      display.println(WiFi.localIP().toString().c_str());
      display.print("#"); display.println(packetCounter);
      display.print("       ID: "); display.println(Telemetry.payload_callsign);
      display.print("Frequency: "); display.println(Telemetry.frequency,3);
      display.print(" Altitude: "); display.println(Telemetry.alt,0);
      display.print(" Distance: "); display.println(Telemetry.distance,1);
      display.print("    Chase: "); display.println(Telemetry.compass); 
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
      display.print("TBTacker-RX "); display.print(TBTRACKER_VERSION);
      display.display();
    break; 
    case OLED_CHASE:
      drawCompass();
    break; 
    case OLED_GOD:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print(getDuration(Telemetry.atmillis,true));
      display.setCursor(97, 0);
      display.print("#"); display.print(packetCounter);
      display.drawLine(0, 10, 128, 10, WHITE);
      display.setCursor(0, 13);
      display.print(Telemetry.payload_callsign);
      display.setCursor(64, 13);
      display.print(Telemetry.frequency, 3); display.print("MHz");
      display.setCursor(0, 23);
      display.print(Telemetry.alt, 0); display.print("m");
      display.setCursor(64, 23);
      if (Telemetry.distance < 10)
        display.print(Telemetry.distance, 1); 
      else
        display.print(Telemetry.distance, 0); 
      display.print("km");
      display.setCursor(105, 23);
      display.print(Telemetry.compass);
      display.setCursor(0, 33);
      display.print(Telemetry.batt, 2); display.print("V");
      display.setCursor(64, 33);
      display.print(Telemetry.snr); display.print("dB");
      display.setCursor(0, 43);
      display.print(Telemetry.temp, 1); display.print((char)247); display.print("C");
      display.setCursor(64, 43);
      display.print(Telemetry.sats); display.print(" sats");
      display.setCursor(0, 53);
      display.print(Telemetry.lat, 6);
      display.setCursor(64, 53);
      display.print(Telemetry.lon, 6);
      display.display();    
    break; 
  }
}

/************************************************************************************
* flash the OLED screen to show packet was received
************************************************************************************/
void displayFlash()
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(10, 27);
  display.print("PACKET RX");
  display.display();
  display.invertDisplay(true);
  delay(400);
  display.invertDisplay(false);
}


/************************************************************************************
// Update the OLED with the correct frequency
************************************************************************************/
void updateOLEDforFrequency(void)
{
  // Clear the display
  display.clearDisplay();
  // Set the Text size
  display.setTextSize(1);
  // Set the text color
  display.setTextColor(WHITE);
  // Set the cursor
  display.setCursor(0, 0);
  
  // Line 1
  display.print("IP: "); display.println(WiFi.localIP().toString().c_str());
  // Line 2
  display.println(); 
  // Line 3
  display.println();   
  // Line 4
  display.println("Waiting for packets");
  // Line 5
  display.print("on: ");
  display.print(String(LoRaSettings.Frequency,3).c_str());
  display.println(" MHz");
  // Line 6
  display.println();
  // Line 7
  display.println();
  // Invert the screen color
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  // Line 8
  display.print("TBTacker-RX "); display.println(TBTRACKER_VERSION);
  // Display everything
  display.display();
}

/************************************************************************************
// Draw a compass on the OLED
************************************************************************************/
void drawCompass() 
{
  static int armLength = 22;
  static int arrowLength = 15;
  static int cx = 86;
  static int cy = 32;
  int armX, armY, arrow1X, arrow1Y, arrow2X, arrow2Y;
  int bearing = Telemetry.bearing;

  //convert degree to radian
  float bearingRad = bearing/57.2957795;  // 1 Radian is 57.2957 degrees
  float arm1Rad = (bearing-15)/57.2957795;  // calculate for little arrow arms +/- a few degrees.
  float arm2Rad = (bearing+15)/57.2957795;

  armX = armLength*cos(bearingRad); // use trig to get x and y values. x=hypotenuse*cos(angle in Rads)
  armY = -armLength*sin(bearingRad); // y = hypotenuse*sin(angle in Rads)

  arrow1X = arrowLength*cos(arm1Rad); // x and y offsets to draw the arrow bits
  arrow1Y = -arrowLength*sin(arm1Rad);
  arrow2X = arrowLength*cos(arm2Rad); // x and y offsets to draw the rest of the arrow bits
  arrow2Y = -arrowLength*sin(arm2Rad);

  display.clearDisplay();
  // draw line, circle, and arrows
  display.drawLine(cx, cy, cx-armY, cy-armX, WHITE); // for some reason have to invert x and y to get correct compass heading
  //u8g2.drawLine(cx-armY, cy-armX, cx-arrow1Y, cy-arrow1X); // draw 1/2 of arrowhead
  //u8g2.drawLine(cx-armY, cy-armX, cx-arrow2Y, cy-arrow2X);
  display.drawTriangle(cx-armY, cy-armX, cx-arrow1Y, cy-arrow1X, cx-arrow2Y, cy-arrow2X, WHITE);
  //display.drawCircle(cx, cy, armLength, U8G2_DRAW_ALL, WHITE);
  display.drawCircle(cx, cy, armLength, WHITE);
  //display.drawCircle(cx, cy, 2, U8G2_DRAW_ALL);
  display.drawCircle(cx, cy, 2, WHITE);

  // Draw tick marks at each Compass point
  display.drawLine(cx, cy-(armLength-2), cx, cy-(armLength +2),WHITE); // North tick mark
  display.drawLine(cx, cy+(armLength-2), cx, cy+(armLength +2),WHITE); // South tick mark
  display.drawLine(cx-(armLength-2), cy, cx-(armLength+2), cy, WHITE); // West tick mark
  display.drawLine(cx+(armLength-2), cy, cx+(armLength+2), cy, WHITE); // East tick mark
  //u8g2.setFont(u8g_font_unifont);
  // display.setFont(u8g2_font_profont12_tf); //8 pixel font
  display.setTextColor(WHITE);
  
  // Label the Compass Directions 
  display.setTextSize(1);
  display.setCursor(cx-2, cy-(armLength+9)); 
  display.print("N");
  display.setCursor(cx-2, cy+(armLength+3)); 
  display.print("S");
  display.setCursor(cx+(armLength+6), cy-3);
  display.print("E");
  display.setCursor(cx-(armLength+9), cy-3);
  display.print("W");

  // display time since last packet
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(getDuration(Telemetry.atmillis,true));
  display.setCursor(97, 0);
  display.print("#"); display.print(packetCounter);
  display.display();

  // Display altitude
  display.setCursor(0, 34);
  display.print(Telemetry.alt,0); display.print("m");

  // Display the actual bearing in a larger font
  display.setTextSize(2);
  display.setCursor(0, 15);
  display.print(bearing,0); display.print((char)247);

  // display the distance 
  display.setCursor(0,48);
  // Add a decimal if the distance is < 10km
  if (Telemetry.distance < 10)
     display.print(Telemetry.distance, 1);
  else
     display.print(Telemetry.distance, 0); 
  display.print("km");     
  display.display();
}

#endif
