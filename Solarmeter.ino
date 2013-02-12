#define VERSION "V11"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <LiquidCrystal.h>
#include <Dns.h>
#include <Time.h>
#include <FlashMini.h>
#include <MsTimer2.h>
#include "S0Sensor.h"
#include "P1GasSensor.h"
#include "P1Power.h"
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
EthernetServer server(80);  // the web server is used to serve status calls
EthernetUDP Udp;
char   webData[70];
#ifdef USE_LOGGING
  File   logFile;
#endif

void setup()
{
    // initialize network
    Ethernet.begin(mac, ip, dnsserver, gateway, subnet);
    // initialize time server
    Udp.begin(8888);
    // Try to set the time 10 times
    for(byte i=0;i<10;i++)
    {
        if(UpdateTime()) break;
    }
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
    #ifdef USE_MINDERGAS
    GetGasValue();
    #endif
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
        #ifdef USE_MINDERGAS
        SendToMinderGas();
        #endif   
        lastDay=day();
        for(byte i=0;i<NUMSENSORS;i++)
        {
            sensors[i]->Reset();
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
            sensors[i]->Save();
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
            #ifdef EXOSITE_KEY
            SendToExosite();
            #endif
            // reset the maximum for pvoutput
            for(byte i=0;i<NUMSENSORS;i++)
            {
                sensors[i]->ResetPeak();
            }
        }
    }
    // let all sensors do other stuff
    for(byte i=0;i<NUMSENSORS;i++)
    {
      sensors[i]->Loop(lastMinute);
    }
    // see if there are clients to serve
    ServeWebClients();
    // give the thernet shield some time to rest
    delay(50);
}


