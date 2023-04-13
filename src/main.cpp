#include <Arduino.h>
#include <Wire.h>
#include <BLEPeripheral.h>
#include "nrf_soc.h"
#include "nrf_nvic.h"
#include "nrf52.h"
#include "nrf52840_peripherals.h"
#include "core_cm4.h"
#include "BLE.h"
#include "Time.h"
#include "DataBase.h"
#include "GsrSignal.h"
#include "RTT.h"

Time realTime = Time();
DataBase db = DataBase();
BLE ble = BLE(&db);
GsrSignal gsrSignal = GsrSignal(19, &realTime, &db);

uint32_t lastSent = 0;
uint32_t lastTonicSent = 0;
uint32_t gsrlastWork = 0;

uint32_t gsrSignalWorkInterval = 40;



void wdt_init(void)
{
	NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.  b) Keep the watchdog running while the CPU is sleeping.
	NRF_WDT->CRV = 3*32768;             //ca 3 sek. timout
	NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
	
	// Enable WDT interrupt:
  NVIC_EnableIRQ(WDT_IRQn);
  NRF_WDT->INTENSET = WDT_INTENSET_TIMEOUT_Msk;	
	
	NRF_WDT->TASKS_START = 1;           //Start the Watchdog timer
}


void writePeaks(PeaksModel peak)
{
  if (!ble.getNotifiState())
  {
    ble.writePeak(peak);
  }
}

void powerControl()
{
  if (ble.getConnectState())
  {
    sd_app_evt_wait();
    sd_nvic_ClearPendingIRQ(SWI2_IRQn);
  }
  __WFI();
}

void correctTime(char *dateTime)
{
  realTime.correct(dateTime);
}

void sendData()
{
  if (ble.getConnectState())
  {
    if (ble.getNotifiState())
    {
      if (millis() - lastSent >= gsrSignalWorkInterval)
      {
        lastSent = millis();
        ble.writeValues(gsrSignal.getTonicValue(), gsrSignal.getPhaseValue());
      }
    }
    else
    {
      if (millis() - lastTonicSent >= 30000)
      {
        lastTonicSent = millis();
        ble.writeMemoryTonicValue(gsrSignal.getTonicValue());
      }
    }
  }
}

void intervalGsrWork()
{
  if (millis() - gsrlastWork >= gsrSignalWorkInterval)
  {
    gsrlastWork = millis();
    gsrSignal.work();
  }
}

void setup()
{
  ble.init();
  realTime.init();
  ble.setCorrectTimeCallback(correctTime);
  gsrSignal.registerWritePeakCallBack(writePeaks);
  analogReadResolution(12);
  sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
  wdt_init();
}

void work()
{
  ble.work();
  realTime.work();
  intervalGsrWork();
  sendData();
  db.setIsWriting(!ble.getConnectState());

}

void loop()
{
  work();
  powerControl();
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}