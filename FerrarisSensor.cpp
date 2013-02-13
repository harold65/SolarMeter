#include "FerrarisSensor.h"

FerrarisSensor::FerrarisSensor(byte p1, byte p2, int p, int sid, int f) : 
BaseSensor(p,sid,f)
{
    pin1 = p1;
    pin2 = p2;
    Type = 24;  // this sensor always logs to the consumption value
                // and the generation is added to create total consumption
}

void FerrarisSensor::Begin(byte i)
{
    BaseSensor::Begin(i);
    threshold1L = 314;   // actual switching threshold
    threshold1H = 393;   // will be adjusted every 10000 samples
    threshold2L = 203;
    threshold2H = 256;
    input1 = false;
    input2 = false;
    // variables for auto adjusting thresholds
    thresholdSampleCounter = 0;
    low1=5000;        
    low2=5000;
    sum1=0;
    sum2=0;
    state = 1;    // 1=on/on 2=off/on 2=off/off 4=on/off
    sensorValue1 = 0;
    sensorValue2 = 0;
    readCounter = 0;
}

void FerrarisSensor::CheckSensor()
{
    // measure the analog inputs 6 times to eliminate noise
    sensorValue1 += analogRead(pin1);
    sensorValue2 += analogRead(pin2);
    readCounter++;
    if(readCounter==6)
    {
        last1 = sensorValue1;
        last2 = sensorValue2;
        // determine the digital state
        if(sensorValue1 > threshold1H) input1=true;
        if(sensorValue1 < threshold1L) input1=false;
        if(sensorValue2 > threshold2H) input2=true;
        if(sensorValue2 < threshold2L) input2=false;
        // keep track of the range
        if(low1 > sensorValue1) low1 = sensorValue1;
        if(low2 > sensorValue2) low2 = sensorValue2;
        sum1 += sensorValue1;
        sum2 += sensorValue2;
        thresholdSampleCounter++;
        // depending on the level of the inputs go to the next state
        switch(state)
        {
        case 1:
            if(!input1 && input2)
            {
                // passed the sensors in positive direction
                todayCnt++;
                if(forward) pulseLength = millis() - lastMillis;
                else        pulseLength = 0;
                lastMillis = millis();
                forward = true;
                // goto next state
                state = 2;
            }
            if(input1 && !input2)
            {
                state = 4;
            }
            break;
        case 2:
            if(!input1 && !input2)
            {
                state = 3;
            }
            if(input1 && input2)
            {
                // passed the sensors in negative direction
                todayCnt--;
                if(forward) pulseLength = 0;
                else        pulseLength = lastMillis - millis(); // negative!
                lastMillis = millis();
                forward = false;
                state = 1;
            }
            break;                 
        case 3:
            if(input1 && !input2)
            {
                state = 4;
            }
            if(!input1 && input2)
            {
                state = 2;
            }
            break;
        case 4:
            if(input1 && input2)
            {
                state = 1;
            }
            if(!input1 && !input2)
            {
                state = 3;
            }
            break;     
        }   
        // ready for the next sample
        sensorValue1 = 0;
        sensorValue2 = 0;
        readCounter = 0;
    }
}

void FerrarisSensor::CalculateActuals()
{
    BaseSensor::CalculateActuals();
    if(thresholdSampleCounter > 10000)
    {
        // if the detected range is large enough we have seen bright and dark pulses
        // then setup the thresholds on 25% and 75% of the range
        // left sensor
        int range = (sum1 / thresholdSampleCounter) - low1;
        if(range>100)
        {
            threshold1L = low1 + range/4;
            threshold1H = low1 + range/2;
        }

        // same for right sensor
        range = (sum2 / thresholdSampleCounter) - low2;
        if(range>100)
        {
            threshold2L = low2 + range/4;
            threshold2H = low2 + range/2;
        }
        // start measuring new range
        low1 = 7000;
        low2 = 7000;
        sum1 = 0;
        sum2 = 0;
        thresholdSampleCounter = 0;
    }
}
void FerrarisSensor::Status(EthernetClient client)
{
    BaseSensor::Status(client);
    client << F("<td>C=") << thresholdSampleCounter;
    client << F(" 1=") << threshold1L << "-" << threshold1H << ":" << last1;
    client << F(" 2=") << threshold2L << "-" << threshold2H << ":" << last2;
}



