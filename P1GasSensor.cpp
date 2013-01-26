#include "P1GasSensor.h"

P1GasSensor::P1GasSensor(P1Power* P1port, int sid, byte t, int f) : BaseSensor(1000,sid,f)
{
  P1 = P1port;
  Type = t;
}

void P1GasSensor::CalculateActuals()
{
  Today = P1->GasUsage - todayCnt;
  Actual = P1->GasUsage;
}

void P1GasSensor::Reset()
{
  todayCnt         = P1->GasUsage; // startwaarde teller om 0.00 uur
  Today            = 0;
  Save();
} 

