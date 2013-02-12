#ifdef USE_MINDERGAS

byte mgResponse;
unsigned long TotalGas;

void GetGasValue()
{
    #ifdef MG_USES_ANALOG_SENSOR
    // Update the total counter and store in eeprom
    TotalGas = eeprom_read_dword((uint32_t*) 0);  
    #endif

    #ifdef MG_USES_P1_SENSOR
    // get the gas counter from the P1 sensor
    TotalGas = MG_USES_P1_SENSOR.GasUsage;
    #endif
}    
    
// next function is called at midnight, just before the day counters are reset
void SendToMinderGas()
{
    GetGasValue();

    #ifdef MG_USES_ANALOG_SENSOR
    TotalGas += MG_USES_ANALOG_SENSOR.Today;
    eeprom_write_dword((uint32_t*) 0, TotalGas); 
    #endif

    EthernetClient mgClient;
    time_t t = now()-SECS_PER_DAY; // we want to upload the gas usage of yesterday so rewind the clock for 1 day
    // try to connect to minderGas
    if (mgClient.connect("mindergas.nl",80)) 
    {
        // create a string with the date and the meter value
        char dataString[80];
        sprintf(dataString,"{\"date\":\"%04d-%02d-%02d\",\"reading_l\":\"%ld\"}", year(t),month(t),day(t),TotalGas);
        // send headers
        mgClient << F("POST /you_less/gas_meter_readings?auth_token=" MG_KEY " HTTP/1.1") << endl;
        mgClient << F("Host: mindergas.nl") << endl;
        mgClient << F("User-Agent: Arduino") << endl;
        mgClient << F("Content-Type: application/json") << endl;
        mgClient << F("Accept: application/json") << endl;
        mgClient << F("Content-Length: ") << strlen(dataString) << endl << endl;
        // send the data
        mgClient << dataString << endl;
        // read the response code. Will be in the form of "HTTP 1.1 201"
        // 201 means ok
        mgClient.parseInt(); // skip the first 1
        mgClient.parseInt(); // skip the second 1
        mgResponse = mgClient.parseInt(); // we need the third number
        // close connection
        mgClient.stop();
    }
    else
    {
        // no response :-(
        mgResponse=0;
    }
}

#endif

