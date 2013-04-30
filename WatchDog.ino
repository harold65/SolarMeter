byte busyState;
int busyCounter;

#define TIMEOUT 12000 // * 5ms = 1 minute
#define EE_CTR 78
#define EE_STATE 79

void SetupWatchdog()
{
    #ifdef USE_WD
    // set watchdog timeout to 4 seconds
    wdt_enable(WDTO_4S);
    #endif
    // initialize counters
    busyCounter = 0;
    busyState = 0;
}

void busy(byte function)
{
    if(function==0)
    {
        busyCounter=0;
    }
    if(busyState!=function)
    {
        busyState = function;
    }
}

// this is called every 5ms to keep the watchdog from resetting the board
void CheckWatchdog()
{
    // increment the counter as long as we are executing a function
    busyCounter++;
    
    // keep resetting the watchdog until busycounter gets really big
    // 12000 * 5ms + 4s = 64 seconds of inactivity before a reset occurs
    // A .connect() function can take up to 15 seconds because of DNS
    // so a normal watchdog would not be long enough.
    if(busyCounter<TIMEOUT)
    {
          // reset watchdog counter  
          wdt_reset();        
    }
    else
    {
        // while waiting for the reset-by watchdog, save the current busystate
        eeprom_write_byte ((uint8_t*)EE_STATE, busyState);
        // increment the watchdog reset counter
        byte ctr = eeprom_read_byte ((uint8_t*)EE_CTR) + 1;
        eeprom_write_byte ((uint8_t*)EE_CTR, ctr);
        // save all counters
        SaveValues();
        // wait for the reset to come
        while(1); 
    }
}

