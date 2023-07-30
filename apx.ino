/************************************************************************************
* AXP power management related stuff
************************************************************************************/

#include "XPowersLib.h"

#ifndef I2C_SDA
#define I2C_SDA         21
#endif

#ifndef I2C_SCL
#define I2C_SCL         22
#endif

// The middle button of your T-BEAM v1.1 or v1.2
#ifndef PMU_IRQ_BTN
#define PMU_IRQ_BTN     38
#endif


// Use the XPowersLibInterface standard to use the xpowers API
XPowersLibInterface *PMU = NULL;

bool  pmu_flag = 0;

const uint8_t i2c_sda = I2C_SDA;
const uint8_t i2c_scl = I2C_SCL;
const uint8_t pmu_irq_pin = PMU_IRQ_BTN;

void set_pmuFlag(void)
{
    pmu_flag = true;
}

/************************************************************************************
* Determine if a PMU that we support is present
************************************************************************************/
void XPowerInit()
{
    // Check for the AXP2101 (most common in the T-Beam's v1.1 and v1.2) 
    if (!PMU) 
    {
        PMU = new XPowersAXP2101(Wire, i2c_sda, i2c_scl);
        if (!PMU->init()) 
        {
            Serial.printf("Did not find AXP2101 power management\n");
            delete PMU;
            PMU = NULL;
        } else 
        {
            Serial.printf("AXP2101 PMU init succeeded, using AXP2101 PMU\n\n");
        }
    }

    // check  for the AXP192
    if (!PMU) 
    {
        PMU = new XPowersAXP192(Wire, i2c_sda, i2c_scl);
        if (!PMU->init()) 
        {
            Serial.printf("Did not find AXP192 power management\n");
            delete PMU;
            PMU = NULL;
        } else 
        {
            Serial.printf("AXP192 PMU init succeeded, using AXP192 PMU\n\n");
        }
    }
   
    // check for the axp202
    if (!PMU) 
    {
        PMU = new XPowersAXP202(Wire, i2c_sda, i2c_scl);
        if (!PMU->init()) 
        {
            Serial.printf("Did not find AXP202 power management\n");
            delete PMU;
            PMU = NULL;
        } else 
        {
            Serial.printf("AXP202 PMU init succeeded, using AXP202 PMU\n\n");
        }
    }

    // No supported PMU detected 
    if (!PMU) 
    {
        Serial.println("Continuing without APX support, which is no problem...\n\n"); 
    }
    else
    {
      // Set the correct power levels for the differents power rails in the detected PMU
      // The following AXP192 power supply setting voltage is based on esp32 T-beam
      if (PMU->getChipModel() == XPOWERS_AXP192) 
      {
        // lora radio power channel to 3.3V
        PMU->setPowerChannelVoltage(XPOWERS_LDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_LDO2);

        // oled module power channel to 3.3V
        // disable it will cause abnormal communication between boot and AXP power supply,
        // do not turn it off
        PMU->setPowerChannelVoltage(XPOWERS_DCDC1, 3300);
        // enable oled power
        PMU->enablePowerOutput(XPOWERS_DCDC1);

        // GPS module power channel to 3.3V
        PMU->setPowerChannelVoltage(XPOWERS_LDO3, 3300);
        //enable GPS power
        PMU->enablePowerOutput(XPOWERS_LDO3);

        //protected oled power source
        PMU->setProtectedChannel(XPOWERS_DCDC1);
        //protected esp32 power source
        PMU->setProtectedChannel(XPOWERS_DCDC3);

        //disable not use channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);

        //disable all axp chip interrupt
        PMU->disableIRQ(XPOWERS_AXP192_ALL_IRQ);

        //
        /*  Set the constant current charging current of AXP192
            opt:
            XPOWERS_AXP192_CHG_CUR_100MA,
            XPOWERS_AXP192_CHG_CUR_190MA,
            XPOWERS_AXP192_CHG_CUR_280MA,
            XPOWERS_AXP192_CHG_CUR_360MA,
            XPOWERS_AXP192_CHG_CUR_450MA,
            XPOWERS_AXP192_CHG_CUR_550MA,
            XPOWERS_AXP192_CHG_CUR_630MA,
            XPOWERS_AXP192_CHG_CUR_700MA,
            XPOWERS_AXP192_CHG_CUR_780MA,
            XPOWERS_AXP192_CHG_CUR_880MA,
            XPOWERS_AXP192_CHG_CUR_960MA,
            XPOWERS_AXP192_CHG_CUR_1000MA,
            XPOWERS_AXP192_CHG_CUR_1080MA,
            XPOWERS_AXP192_CHG_CUR_1160MA,
            XPOWERS_AXP192_CHG_CUR_1240MA,
            XPOWERS_AXP192_CHG_CUR_1320MA,
        */
        // Set the CC charge to 550mA
        PMU->setChargerConstantCurr(XPOWERS_AXP192_CHG_CUR_550MA);
      }
      // Set the correct power levels for the differents power rails in the detected AXP202 PMU
      // This chip is usually usid in esp32 watch
      else if (PMU->getChipModel() == XPOWERS_AXP202) 
      {
        PMU->disablePowerOutput(XPOWERS_DCDC2); //not elicited

        PMU->setPowerChannelVoltage(XPOWERS_LDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_LDO2);

        PMU->setPowerChannelVoltage(XPOWERS_LDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_LDO3);

        PMU->setPowerChannelVoltage(XPOWERS_LDO4, 3300);
        PMU->enablePowerOutput(XPOWERS_LDO4);

        //
        /*  Set the constant current charging current of AXP202
            opt:
            XPOWERS_AXP202_CHG_CUR_100MA,
            XPOWERS_AXP202_CHG_CUR_190MA,
            XPOWERS_AXP202_CHG_CUR_280MA,
            XPOWERS_AXP202_CHG_CUR_360MA,
            XPOWERS_AXP202_CHG_CUR_450MA,
            XPOWERS_AXP202_CHG_CUR_550MA,
            XPOWERS_AXP202_CHG_CUR_630MA,
            XPOWERS_AXP202_CHG_CUR_700MA,
            XPOWERS_AXP202_CHG_CUR_780MA,
            XPOWERS_AXP202_CHG_CUR_880MA,
            XPOWERS_AXP202_CHG_CUR_960MA,
            XPOWERS_AXP202_CHG_CUR_1000MA,
            XPOWERS_AXP202_CHG_CUR_1080MA,
            XPOWERS_AXP202_CHG_CUR_1160MA,
            XPOWERS_AXP202_CHG_CUR_1240MA,
            XPOWERS_AXP202_CHG_CUR_1320MA,
        */

        PMU->setChargerConstantCurr(XPOWERS_AXP202_CHG_CUR_550MA);
      }
      // The following AXP192 power supply voltage setting is based on esp32s3 T-beam
      else if (PMU->getChipModel() == XPOWERS_AXP2101) 
      {
        // lora radio power channel
        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        // gps module power channel
        PMU->setPowerChannelVoltage(XPOWERS_ALDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO3);

        // m.2 interface
        PMU->setPowerChannelVoltage(XPOWERS_DCDC3, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC3);

        // PMU->setPowerChannelVoltage(XPOWERS_DCDC4, 3300);
        // PMU->enablePowerOutput(XPOWERS_DCDC4);

        //not use channel
        PMU->disablePowerOutput(XPOWERS_DCDC2); //not elicited
        PMU->disablePowerOutput(XPOWERS_DCDC5); //not elicited
        PMU->disablePowerOutput(XPOWERS_DLDO1); //Invalid power channel, it does not exist
        PMU->disablePowerOutput(XPOWERS_DLDO2); //Invalid power channel, it does not exist
        PMU->disablePowerOutput(XPOWERS_VBACKUP);

        //disable all axp chip interrupt
        PMU->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);

        /*  Set the constant current charging current of AXP2101
            opt:
            XPOWERS_AXP2101_CHG_CUR_100MA,
            XPOWERS_AXP2101_CHG_CUR_125MA,
            XPOWERS_AXP2101_CHG_CUR_150MA,
            XPOWERS_AXP2101_CHG_CUR_175MA,
            XPOWERS_AXP2101_CHG_CUR_200MA,
            XPOWERS_AXP2101_CHG_CUR_300MA,
            XPOWERS_AXP2101_CHG_CUR_400MA,
            XPOWERS_AXP2101_CHG_CUR_500MA,
            XPOWERS_AXP2101_CHG_CUR_600MA,
            XPOWERS_AXP2101_CHG_CUR_700MA,
            XPOWERS_AXP2101_CHG_CUR_800MA,
            XPOWERS_AXP2101_CHG_CUR_900MA,
            XPOWERS_AXP2101_CHG_CUR_1000MA,
        */
        PMU->setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);
      }

      //Set up the charging voltage, AXP2101/AXP192 4.2V gear is the same
      // XPOWERS_AXP192_CHG_VOL_4V2 = XPOWERS_AXP2101_CHG_VOL_4V2
      PMU->setChargeTargetVoltage(XPOWERS_AXP192_CHG_VOL_4V2);

      // Set VSY off voltage as 2600mV , Adjustment range 2600mV ~ 3300mV
      PMU->setSysPowerDownVoltage(2600);

      // Set the time of pressing the button to turn off
      PMU->setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
      uint8_t opt = PMU->getPowerKeyPressOffTime();
      Serial.print("PowerKeyPressOffTime:");
      switch (opt) 
      {
        case XPOWERS_POWEROFF_4S: Serial.println("4 Second");
        break;
        case XPOWERS_POWEROFF_6S: Serial.println("6 Second");
        break;
        case XPOWERS_POWEROFF_8S: Serial.println("8 Second");
        break;
        case XPOWERS_POWEROFF_10S: Serial.println("10 Second");
        break;
        default:
        break;
      }

      // Set the button power-on press time
      PMU->setPowerKeyPressOnTime(XPOWERS_POWERON_128MS);
      opt = PMU->getPowerKeyPressOnTime();
      Serial.print("PowerKeyPressOnTime:");
      switch (opt) 
      {
        case XPOWERS_POWERON_128MS: Serial.println("128 Ms");
        break;
        case XPOWERS_POWERON_512MS: Serial.println("512 Ms");
        break;
        case XPOWERS_POWERON_1S: Serial.println("1 Second");
        break;
        case XPOWERS_POWERON_2S: Serial.println("2 Second");
        break;
        default:
        break;
      }

      Serial.println("===========================================================================");

       // It is necessary to disable the detection function of the TS pin on the board
       // without the battery temperature detection function, otherwise it will cause abnormal charging
      PMU->disableTSPinMeasure();

      // Enable internal ADC detection
      PMU->enableBattDetection();
      PMU->enableVbusVoltageMeasure();
      PMU->enableBattVoltageMeasure();
      PMU->enableSystemVoltageMeasure();

      /*
        The default setting is CHGLED is automatically controlled by the PMU.
       - XPOWERS_CHG_LED_OFF,
       - XPOWERS_CHG_LED_BLINK_1HZ,
       - XPOWERS_CHG_LED_BLINK_4HZ,
       - XPOWERS_CHG_LED_ON,
       - XPOWERS_CHG_LED_CTRL_CHG,
      */
      PMU->setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);


      // Not used yet in TBTracker but can be used for showing battery voltage for example.
      pinMode(pmu_irq_pin, INPUT);
      attachInterrupt(pmu_irq_pin, set_pmuFlag, FALLING);

       // Clear all interrupt flags
       PMU->clearIrqStatus();

      /*
      // call specific interrupt request

      uint64_t pmuIrqMask = 0;

      if (PMU->getChipModel() == XPOWERS_AXP192) 
      {

        pmuIrqMask = XPOWERS_AXP192_VBUS_INSERT_IRQ     | XPOWERS_AXP192_VBUS_REMOVE_IRQ |      //BATTERY
                     XPOWERS_AXP192_BAT_INSERT_IRQ      | XPOWERS_AXP192_BAT_REMOVE_IRQ  |      //VBUS
                     XPOWERS_AXP192_PKEY_SHORT_IRQ      | XPOWERS_AXP192_PKEY_LONG_IRQ   |      //POWER KEY
                     XPOWERS_AXP192_BAT_CHG_START_IRQ   | XPOWERS_AXP192_BAT_CHG_DONE_IRQ ;     //CHARGE
       } else 
       if (PMU->getChipModel() == XPOWERS_AXP2101) 
       {
        pmuIrqMask = XPOWERS_AXP2101_BAT_INSERT_IRQ     | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
                     XPOWERS_AXP2101_VBUS_INSERT_IRQ    | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
                     XPOWERS_AXP2101_PKEY_SHORT_IRQ     | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
                     XPOWERS_AXP2101_BAT_CHG_DONE_IRQ   | XPOWERS_AXP2101_BAT_CHG_START_IRQ;      //CHARGE
       }
       // Enable the required interrupt function
       PMU->enableIRQ(pmuIrqMask);

      */

      // Call the interrupt request through the interface class
      PMU->disableInterrupt(XPOWERS_ALL_INT);

      PMU->enableInterrupt(XPOWERS_USB_INSERT_INT |
                         XPOWERS_USB_REMOVE_INT |
                         XPOWERS_BATTERY_INSERT_INT |
                         XPOWERS_BATTERY_REMOVE_INT |
                         XPOWERS_PWR_BTN_CLICK_INT |
                         XPOWERS_CHARGE_START_INT |
                         XPOWERS_CHARGE_DONE_INT);
    }
}

