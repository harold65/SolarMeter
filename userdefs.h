//*****************************************************************
//*                                                                
//*  userdefines                                                   
//*****************************************************************

// If you want the logging data to be written to the SD card, remove // from the next line:
#define USE_LOGGING


// You can find your api-key in the settings page, under api-settings
#define PVOUTPUT_API_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

// The configuration of the first S0 sensor is done here
#define SID1 1234 // The system number can be found on the settings page, under systems
#define SID1_PIN 2 // the digital IO pin where the sensor is connected to
#define SID1_PLS 1000 // the number of pulses the sensor gives when 1kWh is measured

// The configuration of the second S0 sensor is done here
#define SID2 1234
#define SID2_PIN 3
#define SID2_PLS -1000 // a negative number will log the sensor as "consumption"

// The configuration of the third S0 sensor is done here (not used in this case)
//#define SID3 1235
//#define SID3_PIN 4
//#define SID3_PLS 1000

// Mail variables. Uncomment the next line and a mail will be sent once a day
//#define USE_MAIL
#define MAIL_TIME 21 // The default time to mail is 21:00 h
#define MAIL_TO "someone@somemail.com" // fill in the destination mail address
#define MAIL_FROM "arduino@meterkast.nl" // any valid mail address will do here
#define MAIL_SERVER "smtp.mymailprovider.nl" // use the server address of your own provider

// Network variables
static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address can be any number, as long as it is unique in your local network
static byte ip[] = { 192, 168, 1, 99 }; // IP of arduino
static byte dnsserver[] = { 8, 8, 8, 8 }; // general DNS server from Google

