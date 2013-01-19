#ifndef P1GasSensor_h
#define P1GasSensor_h

#include "BaseSensor.h"
#include "P1Power.h"

class P1GasSensor : public BaseSensor
{
  public:
    P1GasSensor(P1Power* P1port , int sid, byte t);
    void CalculateActuals();
    void Reset();
  
  private:
    P1Power* P1;
};

#endif
