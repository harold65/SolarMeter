#include "BaseSensor.h"

BaseSensor::BaseSensor(int p,int sid, int f)
{
    ppu = p;
    SID = sid;
    Factor = f;
}

void BaseSensor::Begin(byte i)
{
    Actual = 0;
    Peak = 0;
    Today = 0;
    ee = (i+20) * 4; // the eeprom address of this sensor where the last value is saved
    todayCnt = eeprom_read_dword((uint32_t*) ee); 
    pulseLength = 0;
    lastMillis = 0;
}

void BaseSensor::CheckSensor()
{
    // Check sensor must be done by the derived sensor
}

void BaseSensor::Loop(int m)
{
    // Derived sensors can execute non time critical actions here
}

void BaseSensor::Save()
{
    eeprom_write_dword((uint32_t*) ee, todayCnt); 
}

void BaseSensor::Update(long Value)
{
    todayCnt = Value;
    Save();
}

void BaseSensor::Reset()
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
        if(pulseLength != 0)  // prevent division by zero
        {
            // convert to W
            Actual = 3600000000 / ppu;
            Actual /= pulseLength;
            if(Peak < abs(Actual)) Peak = Actual;
        }
    }
    Today = todayCnt * 1000 / ppu;
}

void BaseSensor::Status(Print& client)
{
    const char* td = "<td>";
    client << td << SID;
    client << td << Type;
    client << td << Actual;
    client << td << Peak;
    client << td << Today;
    client << td << Factor;
    client << td << todayCnt;
    client << td << (long)eeprom_read_dword((uint32_t*) ee);
    client << td << ppu;
    client << td << pulseLength;
}



