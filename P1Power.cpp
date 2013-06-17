#include "P1Power.h"

P1Power::P1Power(HardwareSerial *serIn, int sid, byte t, int f) : BaseSensor(1000,sid,f) // 1000 is dummy. not used for P1
{
  Type = t; // default = 24, same as ferraris sensor: log net consumption
  serial = serIn;
}

void P1Power::Begin(byte i)
{
  BaseSensor::Begin(i);
  serial->begin(9600);
}

void P1Power::Reset()
{
  todayCnt         = (m1+m2) - (m3+m4); // startwaarde tellers om 0.00 uur
  Today            = 0;
  Save();
} 

void P1Power::CalculateActuals()
{
  Actual = PowerUsage - PowerSolar;
  if(Peak < abs(Actual)) Peak = Actual; 
  Today = ((m1+m2)-(m3+m4)) - todayCnt;
}

void P1Power::Loop(int m)
{
  while(serial->available())
  {
    char c = serial->read(); 

    if (c == '\n' || bufpos == BUFSIZE-1)
    {
      Serial.println(bufpos);
      buffer[bufpos] = '\0';
      bufpos = 0;
      ParseBuffer();
    }
    else 
    {
      buffer[bufpos++] = c&127;
    }
  }
} 

void P1Power::ParseBuffer()
{
  // buffer contains one line of text
  long tl,tld;
  int i,j;
  if (readnextLine)
  {
    // gas actual counter: (00127.969)
    if (sscanf(buffer,"(%ld.%ld)" , &tl, &tld) == 2)
    { 
      GasUsage =  tl * 1000 + tld;
      readnextLine = false;
    }
  }
  else
  {
    // Consumption: 1-0:1.8.1(00391.000*kWh)
    //              1-0:1.8.2(00391.000*kWh)
    // Production:  1-0:2.8.1(00391.000*kWh)
    //              1-0:2.8.2(00391.000*kWh)
    if (sscanf(buffer,"1-0:%d.8.%d(%ld.%ld" , &i, &j, &tl, &tld)==4)
    { 
      tl *= 1000;
      tl += tld;
      if(i == 1)
      {
        if(j == 1) m1 = tl;
        if(j == 2) m2 = tl;
      }
      if(i == 2)
      {
        if(j == 1) m3 = tl;
        if(j == 2) m4 = tl;
      }
    }
    // gas definition: 0-1:24.3.0
    if (sscanf(buffer,"0-1:24.%d.%d",&i,&j) == 2)
    {
	  if(i == 3 && j == 0) readnextLine = true; // the actual gas counter is found on the next line
    }
  
    // Actual Consumption: 1-0:1.7.0(0000.50*kW)
    // Actual Generation:  1-0:2.7.0(0001.20*kW)
    if (sscanf(buffer,"1-0:%d.7.0(%ld.%ld" , &i, &tl , &tld) == 3)
    { 
      tl *= 1000;
      tl += tld * 10;
      if(i == 1) PowerUsage = tl;
      if(i == 2) PowerSolar = tl;
    }
  }
}

void P1Power::Status(Print& client)
{
  BaseSensor::Status(client);
  client << F("<td>PowerUsage=") << PowerUsage;
  client << F(" PowerSolar=") << PowerSolar;
  client << F(" GasUsage=") << GasUsage;
  client << F(" M1=") << m1;
  client << F(" M2=") << m2;
  client << F(" M3=") << m3;
  client << F(" M4=") << m4;
}
