#ifndef Buienradar_h
#define Buienradar_h

#include "BaseSensor.h"

class Temperature : public BaseSensor
{
    public:
      Temperature(char* stn,int sid);    // constructor.
      void Begin(byte index);
      void NewHour();
      void CalculateActuals();
      void GetTemperature();
      float GetFactor(long Gas, int hr);
      void Status(EthernetClient client);
      float Actual;
      float Average;
      float Factor;
    private:
      char* weatherStation;
};

#endif
