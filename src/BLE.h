#include <Arduino.h>
#include <Wire.h>
#include <BLEPeripheral.h>
#include "RTT.h"
#include <RTClib.h>
#include "PeaksModel.h"
#include "DataBase.h"

RTT rtt = RTT();

BLEPeripheral blePeripheral = BLEPeripheral();

BLEService settingService = BLEService("4303e3c0-df78-454a-8972-88fd0285cf8e");
BLEIntCharacteristic notifyStateCharacteristic = BLEIntCharacteristic("4303e3c1-df78-454a-8972-88fd0285cf8e", BLEWrite);

BLEService baseService = BLEService("1556b7b0-f1b6-4bc3-8880-035e1299a745");
BLEIntCharacteristic phaseFlowCharacteristic = BLEIntCharacteristic("1556b7b1-f1b6-4bc3-8880-035e1299a745", BLENotify | BLEWrite);
BLEIntCharacteristic tonicFlowCharacteristic = BLEIntCharacteristic("1556b7b2-f1b6-4bc3-8880-035e1299a745", BLENotify | BLEWrite);
BLECharacteristic timeCharacteristic = BLECharacteristic("1556b7b3-f1b6-4bc3-8880-035e1299a745", BLENotify | BLEWrite, 16);
BLECharacteristic dateCharacteristic = BLECharacteristic("1556b7b4-f1b6-4bc3-8880-035e1299a745", BLENotify | BLEWrite, 16);

BLEService dataService = BLEService("bacdabd0-ba2c-4e38-86ed-b35684fd3bb1");
BLEIntCharacteristic countMemoryCharacteristic = BLEIntCharacteristic("bacdabd1-ba2c-4e38-86ed-b35684fd3bb1", BLERead | BLENotify | BLEWrite);
BLECharacteristic timeBeginCharacteristic = BLECharacteristic("bacdabd2-ba2c-4e38-86ed-b35684fd3bb1", BLERead, 16);
BLECharacteristic timeEndCharacteristic = BLECharacteristic("bacdabd3-ba2c-4e38-86ed-b35684fd3bb1", BLERead, 16);
BLECharacteristic dateEndCharacteristic = BLECharacteristic("bacdabd4-ba2c-4e38-86ed-b35684fd3bb1", BLERead, 16);
BLEIntCharacteristic maxPeakValueCharacteristic = BLEIntCharacteristic("bacdabd5-ba2c-4e38-86ed-b35684fd3bb1", BLERead);
BLEIntCharacteristic tonicMemoryCharacteristic = BLEIntCharacteristic("bacdabd6-ba2c-4e38-86ed-b35684fd3bb1", BLERead | BLENotify);

String defaultTime = "00:00:00";
bool isTimeSync = false;
String defaultDate = "2022-08-12";
bool isDateSync = false;

DataBase *dataBase;

bool isNotify = false;

char *toCharArray(String str)
{
    char *buf = new char[str.length() + 1];
    str.toCharArray(buf, str.length() + 1);
    return buf;
}

void notifyStateCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
    rtt.println("notify state writen");
        int notifiState = notifyStateCharacteristic.value();
        if (notifiState == 0)
        {
            isNotify = false;
        }
        else
        {
            isNotify = true;
        }
    
}

void memoryCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
    rtt.println("memory writen");
    static int dbCounter = 0;
    if (dbCounter == (*dataBase).getSize())
    {
        dbCounter = 0;
        (*dataBase).allSent();
        countMemoryCharacteristic.setValueBE(0);
        return;
    }
    PeaksModel peak = (*dataBase).getPeaksByIndex(dbCounter);
    timeBeginCharacteristic.setValue(toCharArray(peak.getBeginTime()));
    timeEndCharacteristic.setValue(toCharArray(peak.getEndTime()));
    dateEndCharacteristic.setValue(toCharArray(peak.getDate()));
    maxPeakValueCharacteristic.setValueBE(peak.getMaxValue());
    tonicMemoryCharacteristic.setValueBE(peak.getTonic());
    countMemoryCharacteristic.setValueBE(1);
    dbCounter++;
}

void timeCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
    rtt.println("time writen");
    char(*timeValue) = (char *)timeCharacteristic.value();
    String syncTime = String(timeValue);
    syncTime.remove(8);
    defaultTime = syncTime;
    isTimeSync = true;
    rtt.println(defaultTime);
}

void dateCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
    rtt.println("date writen");
    char(*dateValue) = (char *)dateCharacteristic.value();
    String syncDate = String(dateValue);
    syncDate.remove(10);
    defaultDate = syncDate;
    isDateSync = true;
    rtt.println(defaultDate);
}

void bleConnectedEventHandler(BLECentral &central)
{
    rtt.print("Connect to ");
    rtt.println(central.address());
    delay(1000);
}

void bleDisconnectedEventHandler(BLECentral &central)
{
    rtt.print("Disconnect to ");
    rtt.println(central.address());
    delay(1000);
}

class BLE
{
private:
    bool timeCorrectFlag = false;
    void (*correctTimeCallback)(char *dateTime);

public:
    BLE(DataBase *db)
    {
        dataBase = db;
    }

    void setCorrectTimeCallback(void (*callback)(char *dateTime))
    {
        correctTimeCallback = callback;
    }

    void init()
    {
        blePeripheral.setLocalName("StressGadget_Zemnuhov");
        blePeripheral.setAdvertisedServiceUuid(baseService.uuid());

        blePeripheral.setEventHandler(BLEConnected, bleConnectedEventHandler);
        blePeripheral.setEventHandler(BLEDisconnected, bleDisconnectedEventHandler);

        blePeripheral.addAttribute(settingService);
        blePeripheral.addAttribute(notifyStateCharacteristic);
        notifyStateCharacteristic.setEventHandler(BLEWritten, notifyStateCharacteristicWritten);

        blePeripheral.addAttribute(baseService);
        blePeripheral.addAttribute(phaseFlowCharacteristic);
        blePeripheral.addAttribute(tonicFlowCharacteristic);
        blePeripheral.addAttribute(timeCharacteristic);
        blePeripheral.addAttribute(dateCharacteristic);
        timeCharacteristic.setEventHandler(BLEWritten, timeCharacteristicWritten);
        dateCharacteristic.setEventHandler(BLEWritten, dateCharacteristicWritten);

        blePeripheral.addAttribute(dataService);
        blePeripheral.addAttribute(countMemoryCharacteristic);
        blePeripheral.addAttribute(timeBeginCharacteristic);
        blePeripheral.addAttribute(timeEndCharacteristic);
        blePeripheral.addAttribute(dateEndCharacteristic);
        blePeripheral.addAttribute(maxPeakValueCharacteristic);
        blePeripheral.addAttribute(tonicMemoryCharacteristic);
        countMemoryCharacteristic.setEventHandler(BLEWritten, memoryCharacteristicWritten);
        blePeripheral.setTxPower(4);
        blePeripheral.begin();
    }

    void work()
    {
        if (isDateSync && isTimeSync)
        {
            timeCorrect();
        }
        blePeripheral.poll();
    }

    void timeCorrect()
    {
        if (isTimeSync && isDateSync)
        {
            rtt.println("time correct");
            String isoFormat = defaultDate + "T" + defaultTime;
            rtt.println(isoFormat);
            correctTimeCallback(toCharArray(isoFormat));
            isTimeSync = false;
            isDateSync = false;
            timeCharacteristic.setValue("true");
        }
    }

    bool getNotifiState()
    {
        return isNotify;
    }

    bool isCorrectTime()
    {
        return timeCorrectFlag;
    }

    void writeTonicPack(int *array, u_int8_t size)
    {
        if (blePeripheral.connected())
        {
            bool res = tonicFlowCharacteristic.setValueBE(array[size]);
        }
    }

    void writePhasePack(int *array, u_int8_t size)
    {

        if (blePeripheral.connected())
        {
            bool res = phaseFlowCharacteristic.setValueBE(array[size]);
        }
    }

    void writePhaseValue(int value)
    {
        if (blePeripheral.connected())
        {
            bool res = phaseFlowCharacteristic.setValueBE(value);
            rtt.println(value);
        }
    }

    void writeTonicValue(int value)
    {
        if (blePeripheral.connected())
        {
            bool res = tonicFlowCharacteristic.setValueBE(value);
            rtt.println(value);
        }
    }

    void writeMemoryTonicValue(int value)
    {
        if (blePeripheral.connected())
        {
            bool res = tonicMemoryCharacteristic.setValueBE(value);
            rtt.print("send tonic value :");
            rtt.println(value);
        }
    }

    void writeValues(int tonicValue, int phaseValue)
    {
        if (blePeripheral.connected())
        {
            tonicFlowCharacteristic.setValueBE(tonicValue);
            phaseFlowCharacteristic.setValueBE(phaseValue);
        }
    }

    void writePeak(PeaksModel peak)
    {
        rtt.println("send peak");
        timeBeginCharacteristic.setValue(toCharArray(peak.getBeginTime()));
        timeEndCharacteristic.setValue(toCharArray(peak.getEndTime()));
        dateEndCharacteristic.setValue(toCharArray(peak.getDate()));
        maxPeakValueCharacteristic.setValueBE(peak.getMaxValue());
        tonicMemoryCharacteristic.setValueBE(peak.getTonic());
        countMemoryCharacteristic.setValueBE(2);
    }

    bool getConnectState()
    {
        return blePeripheral.connected();
    }

    ~BLE() {}
};