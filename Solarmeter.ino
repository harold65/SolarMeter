#define VERSION "V8.4"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Time.h>
#include <FlashMini.h>
#include <MsTimer2.h>
#include <avr/eeprom.h>

#include "Userdefs.h"
#include "Solar.h"

#ifdef USE_LOGGING
 #include <SD.h>
#endif

// define eeprom addresses
#define EE_S1_TODAY 1
#define EE_S2_TODAY 3
#define EE_S3_TODAY 5

// global variables
int    lastDay;
int    lastHour;
int    lastMinute;
int    pvResponse;
EthernetServer server(80);              // the web server is used to serve status calls
char   webData[70];
#ifdef USE_LOGGING
File   logFile;
#endif
EthernetUDP Udp;
// Construct a class for each sensor, if available
#ifdef SID1
Solar S1(SID1_PIN,SID1,SID1_PLS);
#endif
#ifdef SID2
Solar S2(SID2_PIN,SID2,SID2_PLS);
#endif
#ifdef SID3
Solar S3(SID3_PIN,SID3,SID3_PLS);
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
  // initialize counters
  #ifdef SID1
    S1.begin();
  #endif
  #ifdef SID2
    S2.begin();
  #endif
  #ifdef SID3
    S3.begin();
  #endif
  lastDay = day();
  lastMinute = minute();
  lastHour = hour();

  // restore the last saved values
  #ifdef SID1
    S1.Today = readlong(EE_S1_TODAY);
  #endif
  #ifdef SID2
    S2.Today = readlong(EE_S2_TODAY);
  #endif
  #ifdef SID3
    S3.Today = readlong(EE_S3_TODAY);
  #endif

  // start the timer interrupt
  MsTimer2::set(5, Every5ms); // 5ms period
  MsTimer2::start();
}

// check and update all counters every 5ms.
void Every5ms()
{
  #ifdef SID1
    S1.CheckSensor();
  #endif
  #ifdef SID2
    S2.CheckSensor();
  #endif
  #ifdef SID3
    S3.CheckSensor();
  #endif
}

void loop()
{   
  // reset counters at midnight
  if(day()!=lastDay && lastHour==23)
  {
    lastDay=day();
    #ifdef SID1
        S1.NewDay();
    #endif
    #ifdef SID2
        S2.NewDay();
    #endif
    #ifdef SID3
        S3.NewDay();
    #endif
    #ifdef USE_LOGGING
        // create new logfile
        CloseLogFile();
        OpenLogFile();
    #endif
  }

  if(hour()!=lastHour)
  {
    lastHour=hour();
    // save the daily values every hour
    #ifdef SID1
        writelong(EE_S1_TODAY,S1.Today);
    #endif
    #ifdef SID2
        writelong(EE_S2_TODAY,S2.Today);
    #endif
    #ifdef SID3    
        writelong(EE_S3_TODAY,S3.Today);
    #endif
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
    //S1.Dummy(); for testing outputs without connected sensor
    lastMinute=minute();
    #ifdef SID1
        S1.CalculateActuals();
    #endif
    #ifdef SID2
        S2.CalculateActuals();
    #endif
    #ifdef SID3
        S3.CalculateActuals();
    #endif

    #ifdef USE_LOGGING
        WriteDateToLog();
        #ifdef SID1
        logFile << S1.Today << ";" << S1.Actual << ";" << endl;
        #endif
        #ifdef SID2
        logFile << S2.Today << ";" << S2.Actual << ";" << endl;
        #endif
        #ifdef SID3
        logFile << S3.Today << ";" << S3.Actual << ";" << endl;
        #endif
        logFile << endl;
        logFile.flush(); 
    #endif

    if((lastMinute%5)==0)
    {
      // PvOutput can handle 1 event per 5 minutes (per system)
      #ifdef SID1
            SendToPvOutput(S1);
      #endif
      #ifdef SID2
            SendToPvOutput(S2);
      #endif
      #ifdef SID3
            SendToPvOutput(S3);
      #endif
      // reset the maximum for pvoutput
      #ifdef SID1
            S1.ResetPeak();
      #endif
      #ifdef SID2
            S2.ResetPeak();
      #endif
      #ifdef SID3
            S3.ResetPeak();  
      #endif
    }
  }
  // see if there are clients to serve
  ServeWebClients();
  delay(50);
}

