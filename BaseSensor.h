// Sensor.h

#ifndef BaseSensor_h
#define BaseSensor_h

#include "Arduino.h"
#include <avr/eeprom.h>
#include <EthernetClient.h>
#include <FlashMini.h>

class BaseSensor
{
  public:
    BaseSensor(int ppu, int sid);                // constructor
    virtual void Begin(byte index);              // initialize all variables
    virtual void CheckSensor();                  // check the input and update counters
    virtual void CalculateActuals();             // Convert all counters according to the pulses per unit
    virtual void Status(EthernetClient client);  // Dump status to ethernet
    void NewDay();                               // Reset day-counters
    void NewHour();                              // Save day-counters
    void ResetPeak();                            // reset peak so new peak measurement can start
    long Today;                                  // Total for today in correct units. Reset at midnight
    long Actual;                                 // Actual measured value in correct units
    long Peak;                                   // Peak value of the last period
    int  SID;                                    // System id where this sensor logs to
    byte Type;                                   // Variable of PvOutput to log to. See userdefs.h for explanation  
  protected:
    long todayCnt;                               // today's pulse count
    int  ppu;                                    // the pulses per unit (kWh or m3) for this counter
    long lastMillis;                             // the time of the last pulse (ms)
    long pulseLength;                            // the time between the last two pulses (ms)
    byte ee;                                     // the address to store the day counter in case of a reset
};

#endif


