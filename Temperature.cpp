//
// Temperature
// Retrieves the actual temperatue and calculates average and 'Graaddagen factor'

#include "Temperature.h"

Temperature::Temperature(char* ws, int sid, int f) : BaseSensor(1000,sid, f) // 1000 is a dummy value
{
    weatherStation = ws;
    Type = 5; // temperature graph of PvOutput
}

void Temperature::Begin(byte i)
{
    BaseSensor::Begin(i); // to initialize other variables
    todayCnt = 0;
}

void Temperature::CalculateActuals()
{
    // Actual is the temperature in 0.1 deg
    Actual = actual * 10;
}
void Temperature::Loop(int m)
{
    // limit the temperature reading to once every 10 minutes 
    if(m%10==0)
    {
        if(needUpdate)
        {
            GetTemperature();
            needUpdate = false;
        }
    }
    else needUpdate = true;
}

void Temperature::GetTemperature()
{
    // Get temperature from buienradar
    EthernetClient buienradarClient;
    if(buienradarClient.connect("xml.buienradar.nl",80)) 
    {
        buienradarClient << F("GET / HTTP/1.1") << endl;
        buienradarClient << F("Host: xml.buienradar.nl") << endl << endl;
        if(buienradarClient.find(weatherStation)) //6275 = Arnhem
        {
            if(buienradarClient.find("<temperatuurGC>"))
            {
                actual = buienradarClient.parseFloat();
                // Calculate new average
                average = (todayCnt * average) + actual;
                todayCnt++;
                average /= todayCnt;
            }
        }
        buienradarClient.stop();
    }
}

float Temperature::GetFactor(long Gas, int hr)
{
    // Calculate 'graaddagen' = 18 - average temperature of today
    float gd = 18 - average;
    if (gd < 0 || hr == 0)
    { 
        gd = 0;
        gdFactor = 0;
    }
    else
    {
        // Factor is gas usage per 'graaddag' extrapolated to the end of the day (*24/Hr)
        gdFactor = 2.4 * (Gas / gd) / hr ;
    }  
    return gdFactor;
}

void Temperature::Status(Print& client)
{
    BaseSensor::Status(client);
    client << F("<td>avg=") << average;
    client << F(" gdFactor=") << gdFactor;
}


