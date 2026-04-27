//============================================================================
// TBTracker-RX - roel@kroes.com
// 
//  FIRST THING YOU NEED TO DO IS ADJUST THE SETTINGS IN settings.h
//  
//  Have FUN!
//============================================================================
#define TBTRACKER_VERSION_NUM (((TBTRACKER_VERSION_MAJOR) << 24) | ((TBTRACKER_VERSION_MINOR) << 16) | ((TBTRACKER_VERSION_PATCH) << 8) | (TBTRACKER_VERSION_EXTRA))

void write_version_info() 
{
   toSerialConsole(F("\n\n\n=======================================================================\n"));
   toSerialConsole(F("Welcome to TBTracker-RX by Roel Kroes\n"));
   toSerialConsole(F("Get the latest version at: https://github.com/RoelKroes/TBTracker-RX\n\n"));
   toSerialConsole(F("TBTracker-RX version: "));
   toSerialConsole(TBTRACKER_VERSION_MAJOR); toSerialConsole(F("."));
   toSerialConsole(TBTRACKER_VERSION_MINOR); toSerialConsole(F("."));
   toSerialConsole(TBTRACKER_VERSION_PATCH); toSerialConsole(F("\n"));
   toSerialConsole(F("Radiolib version: "));
   toSerialConsole(RADIOLIB_VERSION_MAJOR); toSerialConsole(F("."));
   toSerialConsole(RADIOLIB_VERSION_MINOR); toSerialConsole(F("."));
   toSerialConsole(RADIOLIB_VERSION_PATCH); toSerialConsole(F("\n"));
   toSerialConsole(F("=======================================================================\n\n"));
}