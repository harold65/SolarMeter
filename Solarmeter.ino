#define VERSION "V7"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Time.h>
#include <SD.h>
#include <FlashMini.h>
#include <MsTimer2.h>
#include <avr/eeprom.h>

#include "Userdefs.h"
#include "Solar.h"

// define eeprom addresses
#define EE_S1_TOTAL 0
#define EE_S1_TODAY 1
#define EE_S2_TOTAL 2
#define EE_S2_TODAY 3
#define EE_S3_TOTAL 4
#define EE_S3_TODAY 5

// Construct a class for each sensor
Solar S1(2,SID1);
Solar S2(3,SID2);
Solar S3(4,SID3);

// global variables
int    lastDay;
int    lastHour;
int    lastMinute;
bool   pvoutputok = false;
EthernetServer server(80);              // the web server is used to serve status calls
char   webData[70];
File   logFile;
EthernetUDP Udp;

void setup()
{
    // initialize network
    Ethernet.begin(mac, ip, dnsserver);
    // initialize time server
    Udp.begin(8888);
    // wait until time is set
    while(!UpdateTime());
    // initialize SD card
    SetupSD();
    OpenLogFile();
    // start listening
    server.begin();
    // initialize counters
    S1.begin();
    S2.begin();
    S3.begin();
    lastDay = day();
    lastMinute = minute();
    lastHour = hour();
    // restore the last saved values
    S1.Total = readlong(EE_S1_TOTAL);
    S1.Today = readlong(EE_S1_TODAY);
    // Total was saved at midnight so we have to add todays production
    S1.Total += S1.Today;
    
    S2.Total = readlong(EE_S2_TOTAL);
    S2.Today = readlong(EE_S2_TODAY);
    S2.Total += S2.Today;
    
    S3.Total = readlong(EE_S3_TOTAL);
    S3.Today = readlong(EE_S3_TODAY);
    S3.Total += S3.Today;
    
    // start the timer interrupt
    MsTimer2::set(5, Every5ms); // 5ms period
    MsTimer2::start();
}

// check and update all counters every 5ms.
void Every5ms()
{
    S1.CheckSensor();
    S2.CheckSensor();
    S3.CheckSensor();
}

void loop()
{   
    // reset counters at midnight
    if(day()!=lastDay && lastHour==23)
    {
        lastDay=day();
        // save the total counters
        writelong(EE_S1_TOTAL,S1.Total);
        writelong(EE_S2_TOTAL,S2.Total);
        writelong(EE_S3_TOTAL,S3.Total);
        S1.NewDay();
        S2.NewDay();
        S3.NewDay();
        // create new logfile
        CloseLogFile();
        OpenLogFile();
    }

    if(hour()!=lastHour)
    {
        lastHour=hour();
        // save the daily values every hour
        writelong(EE_S1_TODAY,S1.Today);
        writelong(EE_S2_TODAY,S2.Today);
        writelong(EE_S3_TODAY,S3.Today);
	// sync the time at fixed interval
	if(lastHour==10 || lastHour==22)
        {
          UpdateTime();
        }
    }

    // update every minute
    if(minute()!=lastMinute)
    {
        //S1.Dummy(); for testing outputs without connected sensor
        lastMinute=minute();
        S1.CalculateActual();
        S2.CalculateActual();
        S3.CalculateActual();
        
        WriteDateToLog();
        logFile << S1.Total << ";" << S1.Actual << ";" << endl;
        logFile << S2.Total << ";" << S2.Actual << ";" << endl;
        logFile << S3.Total << ";" << S3.Actual << ";" << endl;
        
        if((lastMinute%5)==0)
        {
            // PvOutput can handle 1 event per 5 minutes (per system)
            SendToPvOutput(S1);
            SendToPvOutput(S2);
            SendToPvOutput(S3);
            // reset the maximum for pvoutput
            S1.ResetPeak();
            S2.ResetPeak();
            S3.ResetPeak();  
            logFile.flush();          
        }
    }
   
    // see if there are clients to serve
    ServeWebClients();
    delay(50);
}





