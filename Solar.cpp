//
// Solar.cpp
// Count and store pulses from digital input

#include "Arduino.h"
#include "Solar.h"


Solar::Solar(int pin, int sid, int p)
{
    pinMode(pin, INPUT);      // initialize the selected io pin
    digitalWrite(pin,HIGH);
    _Pin = pin;
    SID = sid;
    if(p<0)
    {
      _ppkwh=-p;
      Consumption=true;
    }
    else
    {
      _ppkwh=p;
      Consumption=false;
    }
}

void Solar::begin()
{
    Actual = 0;
    Peak = 0;
    Today = 0;
    _TodayCnt=0;
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
        // store the time between the last two pulses
        _PulseLength = millis() - _LastMillis;
        //store the time for the next pulse
        _LastMillis = millis();
        // update counters
        _TodayCnt++;
        _SensorIsOn=false;
    }
}

void Solar::NewDay()
{
    Today=0;  // reset todays totals
    _TodayCnt=0;
}

void Solar::ResetPeak()
{
    Peak=0;
}

void Solar::CalculateActuals()
{
    // Was the last solar pulse more than 3 minutes ago?
    if(millis()-_LastMillis > 180000)
    {
        Actual = 0;  // then we have no output
    }
    else
    {
        if(_PulseLength == 0) return;  // prevent division by zero
        // convert to W
        Actual = (3600000000 / _ppkwh) / _PulseLength;
        if(Peak < Actual) Peak = Actual;
    }
    Today = _TodayCnt * 1000 / _ppkwh;
}

