/***********************************************************************************
 * SSDV related stuff
 ***********************************************************************************/
#define SSDV_MAX_CALLSIGN (6) /* Maximum number of characters in a callsign */

// Struct to hold LoRA settings
struct TSSDVSettings
{
   char callsign[SSDV_MAX_CALLSIGN+1];
   uint8_t imageID;
   uint16_t packetID;
} SSDVSettings;


/***********************************************************************************
* Process the received SSDV packet
 ***********************************************************************************/
 void processSSDVPacket(byte *buf)
 {
    uint32_t callsign_code;
    
    // Decode the callsign, which should be bytes 1, 2, 3 and 4 in BASE40 format. 
   callsign_code = (buf[1] * 256 * 256 * 256) + (buf[2] * 256 * 256) + (buf[3] * 256) + buf[4];
   decode_callsign(SSDVSettings.callsign, callsign_code);
   Telemetry.payload_callsign = SSDVSettings.callsign;
   // Decode the image ID
   SSDVSettings.imageID = buf[5];
   SSDVSettings.packetID = buf[6]*256 + buf[7];

   Telemetry.raw = "SSDV packet - callsign: ";
   Telemetry.raw += SSDVSettings.callsign;
   Telemetry.raw += " image ID: ";
   Telemetry.raw += SSDVSettings.imageID;
   Telemetry.raw += " packet ID: ";
   Telemetry.raw += SSDVSettings.packetID;

   Serial.println(Telemetry.raw);
 }

/***********************************************************************************
* Decodes the callsign in the packet. BASE40 decode
 ***********************************************************************************/
static char *decode_callsign(char *callsign, uint32_t code)
{
	char *c, s;
	
	*callsign = '\0';
	
	/* Is callsign valid? */
	if(code > 0xF423FFFF) return(callsign);
	
	for(c = callsign; code; c++)
	{
		s = code % 40;
		if(s == 0) *c = '-';
		else if(s < 11) *c = '0' + s - 1;
		else if(s < 14) *c = '-';
		else *c = 'A' + s - 14;
		code /= 40;
	}
	*c = '\0';
	
	return(callsign);
}


/************************************************************************************
// Put the SSDV packet into the SSDV queue
************************************************************************************/
void postSSDVinQueue( byte* buf)
{
  char packetBuf[256]; // Contains the hex encoded packet
     
  // Put the received SSDV packet into a temporary buffer and add a sync byte
  packetBuf[0] = 0x55;  // Sync Byte;
  for (int i = 1; i < 256; i++)
  {
    packetBuf[i] = buf[i-1];
  }
  // Add the packet to the queue. do not wait if thge queue is full
  if (ssdv_Queue != NULL)
  {
     if (xQueueSend(ssdv_Queue, packetBuf, 0) == pdPASS)
     {
       Telemetry.uploadResult = "Packet added to upload queue.";
     }
     else
     {
       Telemetry.uploadResult = "Could not upload. Queue is full.";
     }
  }
}  

