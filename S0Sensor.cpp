#include "S0Sensor.h"

S0Sensor::S0Sensor(byte pn, int p, int sid, byte v, int f) : BaseSensor(p,sid,f)
{
    pin = pn;
    pinMode(pin, INPUT_PULLUP);
    Type = v;
}

void S0Sensor::Begin(byte i)
{
    BaseSensor::Begin(i);
    sensorIsOn = false; 
}

void S0Sensor::CheckSensor()
{
    // read the digital input
    bool solarInput = digitalRead(pin);
    // rising edge?
    if(solarInput && sensorIsOn == false)
    {
        sensorIsOn=true;
    }
    // falling edge?
    if(!solarInput && sensorIsOn == true)
    {
        // store the time between the last two pulses
        pulseLength = millis() - lastMillis;
        //store the time for the next pulse
        lastMillis = millis();
        // update counters
        todayCnt++;
        sensorIsOn = false;
    }
}
