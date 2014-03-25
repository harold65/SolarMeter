
#ifdef USE_MINDERGAS

unsigned long mgUploadTime;
byte GasCountdown;
long TotalGas;
int mgResponse;

// this function is called at midnight
// the actual total-gas value is saved and the countdown is started
void UpdateGas()
{
    // select a number between 1 and 59
    // this will be the minutes to wait before uploading
    GasCountdown = random(1,60);
}

// next function is called at midnight, just before the day counters are reset
void SendToMinderGas()
{
    // If the countdown is running, the value is > 0
    if(GasCountdown>0)
    { 
        GasCountdown--;
        if(GasCountdown==0)
        {
            // start the upload when the counter reaches 0
            EthernetClient mgClient;
            time_t t = now() - SECS_PER_DAY; // we want to upload the gas usage of yesterday so rewind the clock for 1 day
            // try to connect to minderGas
            int res = mgClient.connect("mindergas.nl",80); 
            if (res == 1) 
            {
                // create a string with the date and the meter value
                char dataString[80];
                sprintf(dataString,"{\"date\":\"%04d-%02d-%02d\",\"reading_l\":\"%ld\"}", year(t),month(t),day(t),MG_SENSOR.Midnight);
                // send headers
                mgClient << F("POST /you_less/gas_meter_readings?auth_token=" MG_KEY " HTTP/1.1") << endl;
                mgClient << F("Host: mindergas.nl") << endl;
                mgClient << F("User-Agent: Arduino") << endl;
                mgClient << F("Content-Type: application/json") << endl;
                mgClient << F("Accept: application/json") << endl;
                mgClient << F("Content-Length: ") << strlen(dataString) << endl << endl;
                // send the data
                mgClient << dataString << endl;
                mgUploadTime = now();
                mgClient.find(" ");
                mgResponse = mgClient.parseInt();
                // close connection
                mgClient.stop();
            }
            else
            {
                // no response :-(
                mgResponse = res;
            }
        }
    }
}

#endif


