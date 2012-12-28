//
// Temperature
// Retrieves the actual temperatue and calculates average and 'Graaddagen factor'

#include "Temperature.h"

Temperature::Temperature(char* stn, int sid) : BaseSensor(1000,sid) // 1000 is a dummy value
{
    weatherStation = stn;
    Type=5; // temperature graph of PvOutput
}

void Temperature::Begin(byte i)
{
    todayCnt=0;
    GetTemperature(); // get the temperature at startup
}

void Temperature::CalculateActuals()
{
  // replace the basefunction by an empty one
}

void Temperature::NewHour()
{
    GetTemperature();
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
                Actual = buienradarClient.parseFloat();
                BaseSensor::Actual = Actual;
                // Calculate new average
                Average = (todayCnt * Average) + Actual;
                todayCnt++;
                Average /= todayCnt;
            }
        }
        buienradarClient.stop();
    }
}

float Temperature::GetFactor(long Gas, int hr)
{
    // Calculate 'graaddagen' = 18 - average temperature of today
    float gd = 18 - Average;
    if (gd<0 || hr==0)
    { 
        gd=0;
        Factor = 0;
    }
    else
    {
        // Factor is gas usage per 'graaddag' extrapolated to the end of the day (*24/Hr)
        Factor = 2.4 * (Gas / gd) / hr ;
    }  
    return Factor;
}

void Temperature::Status(EthernetClient client)
{
    BaseSensor::Status(client);
    client << F("+Actual:") << Actual;
    client << F(" Average:") << Average;
    client << F(" Factor:") << Factor << endl;
}

