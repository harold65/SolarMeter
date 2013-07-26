#define VERSION "V11.3"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include <Time.h>
#include <FlashMini.h>
#include <MsTimer2.h>
#include <avr/wdt.h>
#include <utility/w5100.h>

#include "S0Sensor.h"
#include "P1GasSensor.h"
#include "P1Power.h"
#include "AnalogSensor.h"
#include "FerrarisSensor.h"
#include "Temperature.h"
#include "userdefs.h"

//#include <SD.h>

// global variables
byte   lastDayReset;
byte   lastHour;
byte   lastMinute;
int    upTime;              // the amount of hours the Arduino is running 
EthernetServer server(80);  // the web server is used to serve status calls
EthernetUDP Udp;
char   webData[70];
#ifdef USE_LOGGING
  File   logFile;
#endif
#define EE_RESETDAY 4

void setup()
{
    // initialize network
    Ethernet.begin(mac, ip, dnsserver, gateway, subnet);
    // set connect timeout to 2.5 seconds
    W5100.setRetransmissionTime(5000); // 500ms per try
    W5100.setRetransmissionCount(8);
    // initialize time server
    Udp.begin(8888);
    // Try to set the time 10 times
    for(byte i = 0; i < 10; i++)
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
    for(byte i = 0; i < NUMSENSORS; i++)
    {
        sensors[i]->Begin(i);
    }
    #ifdef USE_MINDERGAS
    GetGasValue();
    #endif
    
    // restore the last day on which the counters were reset
    lastDayReset = eeprom_read_byte((uint8_t*) EE_RESETDAY);
    // if the eeprom contains illegal data, set it to a useful value
    if(lastDayReset == 0 || lastDayReset > 31) lastDayReset = day();
    lastMinute = minute();
    lastHour = hour();
    upTime = 0;
    
    #ifdef USE_WD
      SetupWatchdog();
    #endif
    // start the timer interrupt
    MsTimer2::set(5, Every5ms); // 5ms period
    MsTimer2::start();
}

// check and update all counters every 5ms.
void Every5ms()
{
    for(byte i = 0; i < NUMSENSORS; i++)
    {
        sensors[i]->CheckSensor();
    }
    #ifdef USE_WD
      CheckWatchdog();
    #endif
}

void loop()
{   
    time_t nu = now();
    // reset counters when todays day is different from the last day the counters were reset
    if(day(nu) != lastDayReset)
    {
        busy(1);
        #ifdef USE_MINDERGAS
            // Calculate the new gas metervalue
            UpdateGas();
        #endif   
        for(byte i = 0; i < NUMSENSORS; i++)
        {
            sensors[i]->Reset();
        }
        #ifdef USE_LOGGING
            // create new logfile
            CloseLogFile();
            OpenLogFile();
        #endif
        lastDayReset = day(nu);
        // store today as the date of the last counter reset
        eeprom_write_byte((uint8_t*) EE_RESETDAY, lastDayReset);
    }

    if(hour(nu) != lastHour)
    {
        busy(2);
        lastHour = hour(nu);
        upTime++;
        // save the daily values every hour
        for(byte i = 0; i < NUMSENSORS; i++)
        {
            sensors[i]->Save();
        }
        // sync the time at fixed interval
        if(lastHour == 10 || lastHour == 22)
        {
            UpdateTime();
        } 
        #ifdef USE_MAIL
            if(lastHour == MAIL_TIME)
            {
                SendMail();
            }
        #endif
    }

    // update every minute
    if(minute(nu) != lastMinute)
    {
        busy(3);
        lastMinute = minute(nu);
        for(byte i = 0; i < NUMSENSORS; i++)
        {
            sensors[i]->CalculateActuals();
        }
        busy(31);

        #ifdef USE_MINDERGAS
            // this function will not do anything until the countdown timer is finished
            SendToMinderGas();
        #endif

        #ifdef USE_LOGGING
            WriteDateToLog();
            for(byte i = 0; i < NUMSENSORS; i++)
            {
                //sensors[i]->Status(&logFile);
                logFile << sensors[i]->Today << ";" << sensors[i]->Actual << ";" << endl;
            }
            logFile << endl;
            logFile.flush(); 
        #endif
        busy(32);
        // update every 5 minutes or whatever is set in userdefs
        if((lastMinute%UPDATEINTERVAL) == 0)
        {
            SendToPvOutput(sensors);
            busy(34);
            // reset the maximum for pvoutput
            for(byte i = 0; i < NUMSENSORS; i++)
            {
                sensors[i]->ResetPeak();
            }
        }
        busy(33);
        #ifdef EXOSITE_KEY
          SendToExosite();
        #endif
    }
    busy(4);
    // let all sensors do other stuff
    for(byte i = 0; i < NUMSENSORS; i++)
    {
      sensors[i]->Loop(lastMinute);
    }
    busy(5);
    // see if there are clients to serve
    ServeWebClients();
    busy(0);
    // give the ethernet shield some time to rest
    delay(50);
}


