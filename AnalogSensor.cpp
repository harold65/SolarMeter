#include "AnalogSensor.h"

AnalogSensor::AnalogSensor(int pn, int p, int sid, byte type, int f) : BaseSensor(p,sid,f)
{
    pin = pn;
    Type = type;
}

void AnalogSensor::Begin(byte i)
{
    BaseSensor::Begin(i);
    readCounter = 0;
    sensorValue = 0;
    sensorIsOn = false; 
}

void AnalogSensor::CheckSensor()
{
    // read the analog input 10 times before evaluating
    sensorValue += analogRead(pin); 
    readCounter++;
    if(readCounter == 10)
    {
        // rising edge?
        if(sensorValue > TH && sensorIsOn == false)
        {
            pulseLength = millis() - lastMillis;
            lastMillis = millis();
            todayCnt++;
            sensorIsOn = true;
        }
        // falling edge?
        if(sensorValue < TL && sensorIsOn == true)
        {
            sensorIsOn = false;
        }

        readCounter = 0;
        sensorValue = 0;
    }
}

