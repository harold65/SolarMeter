#include "P1GasSensor.h"

P1GasSensor::P1GasSensor(P1Power* P1port, int sid, byte t, int f) : BaseSensor(1000,sid,f)
{
  P1 = P1port;
  Type = t;
}

void P1GasSensor::CalculateActuals()
{
  Today = Midnight - todayCnt;
}

void P1GasSensor::Reset()
{
  Today=0;
  Midnight = P1->GasUsage; // Get counter value
  BaseSensor::Reset(); // store it
} 

