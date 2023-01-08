/************************************************************************************
* All LCD SSD1306 related stuff
************************************************************************************/
#if defined(USE_SSD1306)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
* display packet data on the OLED display
************************************************************************************/
void displayUpdate()
{
  displayFlash();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("IP: ");
  display.println(WiFi.localIP().toString().c_str());
  display.println();
  display.print("       ID: "); display.println(Telemetry.payload_callsign);
  display.print("Frequency: "); display.println(Telemetry.frequency,3);
  display.print(" Altitude: "); display.println(Telemetry.alt,0);
  display.print(" Distance: "); display.println(Telemetry.distance,1);
  display.print("    Chase: "); display.println(Telemetry.compass); 
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.print(packetCounter);
  display.print(" TBTacker-RX "); display.print(TBTRACKER_VERSION);
  display.display();
}

/************************************************************************************
* flash the screen to show packet was received
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

#endif
