#include "BaseSensor.h"

BaseSensor::BaseSensor(int p,int sid)
{
   ppu=p;

   SID=sid;
}

void BaseSensor::Begin(byte i)
{
    Actual = 0;
    Peak = 0;
    Today = 0;
    ee = (i+20)*4; // the eeprom address of this sensor where the last value is saved
    todayCnt = eeprom_read_dword((uint32_t*) ee); 
    if(todayCnt < 0) todayCnt = 0; // prevent invalid eeprom values
    pulseLength = 0;
    lastMillis = 0;
}

void BaseSensor::CheckSensor()
{
}

void BaseSensor::NewHour()
{
    eeprom_write_dword((uint32_t*) ee, todayCnt); 
}

void BaseSensor::NewDay()
{
    todayCnt=0;
    Today=0;
}

void BaseSensor::ResetPeak()
{
    Peak=0;
}

void BaseSensor::CalculateActuals()
{
    // Was the last BaseSensor pulse more than 5 minutes ago?
    if(millis()-lastMillis > 300000)
    {
        Actual = 0;  // then we have no output
    }
    else
    {
        if(pulseLength == 0)
       {
          return;  // prevent division by zero
       }
        // convert to W
        Actual = 3600000000 / ppu;
        Actual /= pulseLength;
        if(Peak < abs(Actual)) Peak = Actual;
    }
    Today = todayCnt * 1000 / ppu;
}

void BaseSensor::Status(EthernetClient client)
{
    client << F("SID=") << SID ;
    client << F(" Type=") << Type;
    client << F(" Today=") << Today;
    client << F(" TodayCnt=") << todayCnt;
    client << F(" Actual=") << Actual;
    client << F(" Peak=") << Peak;
    client << F(" Pulse=") << pulseLength;
    client << F(" PPU=") << ppu << endl;    
}

