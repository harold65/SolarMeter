//
// Solar.cpp
// Count and store pulses from digital input
// Assuming 1000 pulses per kWh

#include "Arduino.h"
#include "Solar.h"


Solar::Solar(int pin, int sid)
{
    pinMode(pin, INPUT);      // initialize the selected io pin
    digitalWrite(pin,HIGH);
    _Pin = pin;
    SID = sid;
}

void Solar::begin()
{
    Total = 0;                // initialize all variables to default
    Actual = 0;
    Today = 0;
    Peak = 0;
    _PulseLength = 0;
    _LastMillis = 0;
}

void Solar::CheckSensor()
{
    // read the digital input
    bool solarInput = digitalRead(_Pin);
    // rising edge?
    if(solarInput && _SensorIsOn==false)
    {
        _SensorIsOn=true;
    }
    // falling edge?
    if(!solarInput && _SensorIsOn==true)
    {
        // use high resolution for accurate calculation
        _PulseLength = millis() - _LastMillis;
        _LastMillis = millis();
        // update counters
        Total++; // one edge is 1 Wh
        Today++;
        _SensorIsOn=false;
    }
}

void Solar::NewDay()
{
    Today=0;  // reset todays totals
}

void Solar::ResetPeak()
{
    Peak=0;
}

void Solar::CalculateActual()
{
    // Was the last solar pulse more than 3 minutes ago?
    if(millis()-_LastMillis > 180000) // <20W
    {
        Actual = 0;  // then we have no output
    }
    else
    {
        if(_PulseLength == 0) return;  // prevent division by zero
        // convert to W
        Actual = 3600000 / _PulseLength;
        if(Peak < Actual) Peak = Actual;
    }
}

