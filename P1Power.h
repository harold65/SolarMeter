/*
	P1Serial module tbv uitlezen slimme meter via P1 aansluiting.
	Gebaseerd op code van: http://zonnigbreda.blogspot.nl/
	Dit is de "P1 base + power sensor", die altijd gebruikt moet worden omdat de P1Gas sensor hier gebruik van maakt.
    HKO, 05-01-2013
*/

#ifndef P1Power_h
#define P1Power_h

#define BUFSIZE 60
#include "BaseSensor.h"
class P1Power : public BaseSensor
{
  public:
    P1Power(HardwareSerial* serIn, int sid, byte t, int f); 
    void Begin(byte i);
    void CalculateActuals();            // Convert P1 data to actual and day values
    void Status(EthernetClient client); // Dump status to ethernet    
    void Reset();                       // Reset day-counters
    void Loop(int m);                   // scan the P1 port in the main loop
    	
    // Smartmeter data:
    long  PowerUsage;	// Huidig afgegenomen vermogen, uit P1 meter (wordt in variabele Today gezet)
    long  PowerSolar;	// Huidig teruggeleverd vermogen, uit P1 meter (wordt negatief in variabele Today gezet als PowerUsage 0 is.)
    long  GasUsage;		// Gasverbruik, uit P1 meter
    long  m1; 			// Verbruik laagtarief in Wh, uit P1 meter
    long  m2; 			// Verbruik hoogtarief in Wh, uit P1 meter
    long  m3; 			// Teruggeleverd laagtarief in Wh, uit P1 meter
    long  m4;			// Teruggeleverd hoogtarief in Wh, uit P1 meter
  
  private:
    void ParseBuffer();
    HardwareSerial* serial;  
    char    buffer[BUFSIZE]; // storage for one line of data
    byte    bufpos;
    bool    readnextLine;    // used for gasmeter value
};

#endif
