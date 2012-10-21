// Sensor.h

#ifndef Solar_h
#define Solar_h

#include "Arduino.h"

class Solar
{
  public:
    Solar(int pin, int sid, int ppkwh);   // constructor. pin is the used io pin
    void begin();              // initialize all variables
    void CheckSensor();        // check the input and update counters
    void CalculateActuals();   // Convert all counters to W and Wh
    void NewDay();             // Reset day-counters
    void ResetPeak();          // reset peak so new peak measurement can start
    
    long Today;                // Total production for today. Reset at midnight
    long Actual;               // Actual generated power
    long Peak;                 // Peak power of the last period
    int  SID;                  // System id

  private:
    long TodayCnt;             // today's pulse count
    int  ppkwh;                // the pulses per kWh for this counter
    long _LastMillis;          // the time of the last pulse (ms)
    long _PulseLength;         // the time between the last two pulses (ms)
    long _MinPulse;            // the minimum time between two pulses since the last ResetPeak
    bool _SensorIsOn;          // keeps track of the last input status
    int  _Pin;
};

#endif


