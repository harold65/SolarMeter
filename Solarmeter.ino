#define VERSION "V9.1"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include <Time.h>
#include <FlashMini.h>
#include <MsTimer2.h>
#include "S0Sensor.h"
#include "AnalogSensor.h"
#include "FerrarisSensor.h"
#include "Temperature.h"
#include "userdefs.h"

//#include <SD.h>

// global variables
int    lastDay;
int    lastHour;
int    lastMinute;
int    upTime;              // the amount of hours the Arduino is running 
int    pvResponse;
EthernetServer server(80);  // the web server is used to serve status calls
EthernetUDP Udp;
char   webData[70];
#ifdef USE_LOGGING
  File   logFile;
#endif

void setup()
{
    // initialize network
    Ethernet.begin(mac, ip, dnsserver);
    // initialize time server
    Udp.begin(8888);
    // wait until time is set
    while(!UpdateTime());
    #ifdef USE_LOGGING
        // initialize SD card
        SetupSD();
        OpenLogFile();
    #endif
    // start listening
    server.begin();
    // initialize the sensors
    for(byte i=0;i<NUMSENSORS;i++)
    {
        sensors[i]->Begin(i);
    }
    lastDay = day();
    lastMinute = minute();
    lastHour = hour();
    upTime = 0;
    // start the timer interrupt
    MsTimer2::set(5, Every5ms); // 5ms period
    MsTimer2::start();
}

// check and update all counters every 5ms.
void Every5ms()
{
    for(byte i=0;i<NUMSENSORS;i++)
    {
        sensors[i]->CheckSensor();
    }
}

void loop()
{   
    // reset counters at midnight
    if(day()!=lastDay && lastHour==23)
    {
        lastDay=day();
        for(byte i=0;i<NUMSENSORS;i++)
        {
            sensors[i]->NewDay();
        }
        #ifdef USE_LOGGING
            // create new logfile
            CloseLogFile();
            OpenLogFile();
        #endif
    }

    if(hour()!=lastHour)
    {
        lastHour=hour();
        upTime++;
        // save the daily values every hour
        for(byte i=0;i<NUMSENSORS;i++)
        {
            sensors[i]->NewHour();
        }
        // sync the time at fixed interval
        if(lastHour==10 || lastHour==22)
        {
            UpdateTime();
        }
        #ifdef USE_MAIL
            if(lastHour==MAIL_TIME)
            {
                SendMail();
            }
        #endif
    }

    // update every minute
    if(minute()!=lastMinute)
    {
        lastMinute=minute();
        for(byte i=0;i<NUMSENSORS;i++)
        {
            sensors[i]->CalculateActuals();
        }

        #ifdef USE_LOGGING
            WriteDateToLog();
            for(byte i=0;i<NUMSENSORS;i++)
            {
                //sensors[i]->Status(&logFile);
                logFile << sensors[i]->Today << ";" << sensors[i]->Actual << ";" << endl;
            }
            logFile << endl;
            logFile.flush(); 
        #endif
        
        // update every 5 minutes or whatever is set in userdefs
        if((lastMinute%UPDATEINTERVAL)==0)
        {
            SendToPvOutput(sensors);
            // reset the maximum for pvoutput
            for(byte i=0;i<NUMSENSORS;i++)
            {
                sensors[i]->ResetPeak();
            }
        }
    }
    // see if there are clients to serve
    ServeWebClients();
    delay(50);
}


