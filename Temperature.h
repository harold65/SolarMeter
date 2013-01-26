#ifndef Buienradar_h
#define Buienradar_h

#include "BaseSensor.h"

class Temperature : public BaseSensor
{
    public:
      Temperature(char* ws,int sid, int f);    // constructor.
      void  Begin(byte index);
      void  Loop(int m);
      void  CalculateActuals();
      void  GetTemperature();
      float GetFactor(long Gas, int hr);
      void  Status(EthernetClient client);
	private:
      float actual;
      float average;
      float gdFactor;
      char* weatherStation;
	  bool needUpdate;
};

#endif
