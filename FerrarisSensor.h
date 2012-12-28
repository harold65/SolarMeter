#ifndef Ferraris_h
#define Ferraris_h

#include "BaseSensor.h"

class FerrarisSensor : public BaseSensor
{
  public:
    FerrarisSensor(byte pin1, byte pin2, int p, int sid); // Class constructor
    void Begin(byte i);            // Initialises the variables in the class
    void SetupThresholds();        // Calculate new thresholds, determined by min/max values
    void CheckSensor();
    void CalculateActuals();
    void Status(EthernetClient client); // Dump status to ethernet

  private:
    bool forward;		   // Current spinning direction
    byte pin1;                     // The location of analog input connected to left sensor
    byte pin2;                     // The location of analog input connected to right sensor
    byte readCounter;              // Counter keeps track of the number of sensor reads
    // left sensor
    int  sensorValue1;             // Sum of 6xanalog input value
    int  threshold1H;              // Above this value the sensor is "on"
    int  threshold1L;              // Below this value the sensor is "off"
    int  low1;                     // minimum of read values. Used to update the threshold
    long sum1;                     // total of read sensorvalues, used for average
    int  last1;                    // last value of sensor
    bool input1;                   
    // right sensor
    int  sensorValue2;             // same as left
    int  threshold2H;
    int  threshold2L;
    int  low2;
    long sum2;
    int  last2;
    bool input2;                   // last value of sensor
    byte state;                    // combined state of the two sensors
    int  thresholdSampleCounter;   // Counter keeps track when it is time to update the threshold
};

#endif

