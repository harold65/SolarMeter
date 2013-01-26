#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "BaseSensor.h"

#define TH 2000
#define TL 1500

class AnalogSensor : public BaseSensor
{
  public:
    AnalogSensor(int pin,int p, int sid, byte type, int f);
    void Begin(byte i);
    void CheckSensor();
  
  private:
    bool sensorIsOn;
    byte readCounter;
    int  sensorValue;
    byte pin;
};

#endif

