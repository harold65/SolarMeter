//*****************************************************************
//  userdefines  

#ifndef userdefs
#define userdefs

//*****************************************************************
// If you want the logging data to be written to the SD card, remove // from the next line:
//#define USE_LOGGING

// Mail variables. Uncomment the next line and a mail will be sent once a day
#define USE_MAIL
#define MAIL_TIME 21 // The default time to mail is 21:00 h
#define MAIL_TO "harold65@gmail.com" // fill in the destination mail address
#define MAIL_FROM "arduino@meterkast.nl" // any valid mail address will do here
#define MAIL_SERVER "smtp.upcmail.nl" // use the server address of your own provider

//*****************************************************************
// Enable watchdog only if you know the bootloader can handle this.
#define USE_WD

//*****************************************************************
// Network variables
static byte mac[]       = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address can be any number, as long as it is unique in your local network
static byte ip[]        = { 192, 168, 1, 99 };                    // IP of arduino
static byte dnsserver[] = { 192,168,1,1};                         // use the address of your gateway { 192, 168, 1, 1 } if your router supports this
                                                                  // or use the address of the dns server of your internet provider
                                                                  // or use { 8, 8, 8, 8 } as general DNS server from Google if you have no other option
static byte gateway[]   = { 192, 168, 1, 1 };
static byte subnet[]    = { 255, 255, 255, 0 };  

//*****************************************************************
#define EXOSITE_KEY "0646d32b0f1b82971e3425e69623ae167b10de21"

//*****************************************************************
#define USE_MINDERGAS
#define MG_USES_ANALOG_SENSOR G1
//#define MG_USES_P1_SENSOR P1 
#define MG_KEY "WJsdXKEqdjJQ6a9WJ8Vx"

//*****************************************************************
// You can find your api-key in the PvOutput settings page, under api-settings
#define PVOUTPUT_API_KEY "c8bddc0db5b36ce05ede305525ecfc3ffe21a10c"

//*****************************************************************
// The update interval must match what you have set in the PvOutput settings
// PvOutput->Settings->System->Live settings->Status interval
#define UPDATEINTERVAL 5

//*****************************************************************
// Sensor configuration
//*****************************************************************
// NUMSENSORS must match the number of sensors defined.
#define NUMSENSORS 4
S0Sensor  S1(2,1000,2812,2,1);
FerrarisSensor F1(A3,A4,250,2812,1);
AnalogSensor G1(A2,100,2812,8,1);
//Temperature T1("6275",2812,10);
DS_Temperature T1(6,2812,1);
//#define GRAADDAGEN

// the next list must be in the correct order and have the same length as NUMSENSORS
BaseSensor* sensors[] = {&S1,&F1,&G1,&T1};

#endif
