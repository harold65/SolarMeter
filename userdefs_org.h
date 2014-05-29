#ifndef userdefs
#define userdefs

//*****************************************************************
// If you want the logging data to be written to the SD card, remove // from the next line:
// also remove the // from the solarmeter.ino line "//#include <SD.h>"
//#define USE_LOGGING

//*****************************************************************
// Mail variables. Uncomment the next line and a mail will be sent once a day
//#define USE_MAIL
#define MAIL_TIME 21 // The default time to mail is 21:00 h
#define MAIL_TO "mymail@gmail.com" // fill in the destination mail address
#define MAIL_FROM "arduino@meterkast.nl" // any valid mail address will do here
#define MAIL_SERVER "smtp.upcmail.nl" // use the server address of your own provider

//*****************************************************************
// Network variables
static byte mac[]       = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address can be any number, as long as it is unique in your local network
static byte ip[]        = { 192, 168, 1, 99 };                    // IP of arduino
static byte dnsserver[] = {192,168,1,1};                          // use the address of your gateway { 192, 168, 1, 1 } if your router supports this
                                                                  // or use the address of the dns server of your internet provider
                                                                  // or use { 8, 8, 8, 8 } as general DNS server from Google if you have no other option
static byte gateway[]   = { 192, 168, 1, 1 };
static byte subnet[]    = { 255, 255, 255, 0 };  
//*****************************************************************
#define NTP_SERVER "nl.pool.ntp.org"                             // If you are having problems with the time synchonisation, try a different NTP server

//*****************************************************************
//#define EXOSITE_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
//#define EXOSITEUPDATEINTERVAL 1

//*****************************************************************
//#define USE_MINDERGAS
//#define MG_SENSOR G1
#define MG_SENSOR P1Gas
#define MG_KEY "xxxxxxxxxxxxxxxxxxxx"

//*****************************************************************
// You can find your api-key in the PvOutput settings page, under api-settings
#define PVOUTPUT_API_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

//*****************************************************************
// The update interval must match what you have set in the PvOutput settings
// PvOutput->Settings->System->Live settings->Status interval
// Default is 5 minutes
#define UPDATEINTERVAL 5

//*****************************************************************
// The actual time can be shifted to move the time of uploading to pvoutput and exosite
// This is to prevent missing uploads because everyone is uploading at exactly the same time
// Offset is in seconds, positive numbers will upload earlier
// A negative number will delay the upload by the amount of seconds set here
#define TIME_OFFSET 0

//*****************************************************************
// Enable the watchdog only if you know the bootloader can handle this.
#define USE_WD

//*****************************************************************
// Sensor configuration
//*****************************************************************
// NUMSENSORS must match the number of sensors defined.
#define NUMSENSORS 3
//*****************************************************************
// S0 sensors have 5 parameters: 
//   1: The digital pin to which they are connected.
//   2: The number of pulses for 1 kWh
//   3: The System ID of the corresponding pvOutput graph
//   4: The number of the variable to log to (see software manual)
//   5: The x-factor. The actual and total values will be divided by this number before sending to pvoutput
S0Sensor  S1(2,1000,2222,2,1);   // S0 sensor connected to pin 2, logging to variable 1 & 2 (production) of sid 2222
//S0Sensor  S2(3,2000,2222,2,1);   // S0 sensor connected to pin 3, logging to variable 1 & 2 (production) of sid 2222. This will be added to S1
//S0Sensor  S3(4,1000,3333,4,1);   // S0 sensor connected to pin 4, logging to variable 3 & 4 (consumption) of sid 3333
//*****************************************************************
// Analog Sensors have 5 parameters: 
//   1: The analog pin to which they are connected
//   2: The number of pulses for unit
//   3: The SID
//   4: The number of the variable to log to (see software manual)
//   5: The x-factor. The actual and total values will be divided by this number before sending to pvoutput
//AnalogSensor G1(A2,100,2222,6,1000);    // gas sensor connected to analog 2, measuring 100 pulses per m3, showing on SID 2222 variable 6 (voltage) sending to pvoutput in m3

// Example:
// AnalogSensor W1(A1,1000,2222,8,1);  // water meter sensor connected to analog 1, measuring 10000 pulses per m3.
//                                     Daily water usage (liter) is on v7, actual (liter/h) on v8 (donation mode)
//*****************************************************************
// Temperature 'sensor' Gets its value from an online weatherstation at buienradar.nl
// Parameters:
//   1: The number of the weatherstation to get the temperature from
//      Find the nearest weatherstation on:  http://gratisweerdata.buienradar.nl/#Station
//   2: The SID
//   3: The factor to convert to degrees
//   Temperature is logged to v5
//  Temperature T1("6275",2222,10);
// if you want to log the gas usage per 'graaddag' in stead of the temperature, enable the next line
//#define GRAADDAGEN
//*****************************************************************
// Ferrarissensors have 5 parameters: 
//   1: The analog input of the left sensor
//   2: The analog input of the right sensor
//   3: The number of revolutions of the disc for 1kWh
//   4: The SID
//   5: The factor for correction
//   This sensor always logs to variable 3 and 4
//FerrarisSensor F1(A3,A4,250,2222,1);
//*****************************************************************
// Smartmeter Power sensor has 4 parameters: 
//   1: Serial object. Mega2560: on Serial1. Uno etc: Serial watch out: Serial not working together with usb!
//   2: The SID
//   3: The variable to log to. See software manual
//   4: The x-factor. The actual and total values will be divided by this number before sending to pvoutput
P1Power P1(&Serial,2222, 24, 1);
//*****************************************************************
// Smartmeter Gas sensors have 4 parameter: 
//   1: Smartmeter Serial object. Default: P1
//   2: The SID
//   3: The variable to log to. See software manual
//   4: The x-factor. The actual and total values will be divided by this number before sending to pvoutput
P1GasSensor P1Gas(&P1, 2222, 6, 1);
//*****************************************************************
// the next list must be in the correct order and have the same length as NUMSENSORS
BaseSensor* sensors[] = {&S1,&P1,&P1Gas};

#endif
