String inString = String("");

void ServeWebClients()
{
    EthernetClient client = server.available();
    if (client) 
    {
        inString = client.readStringUntil('\n');
        client << F("HTTP/1.1 200 OK") << endl;
        client << F("Content-Type: text/html") << endl << endl;
        int i=inString.indexOf("save");
        if(i!=-1) SaveValues();
        i=inString.indexOf("reset");
        if(i!=-1) ResetValues();
        i=inString.indexOf("?");
        if(i!=-1) ReadValue(inString,i);
        ShowStatus(client);
        client.stop();
    }
}

void ReadValue(String input,int i)
{
    // string format is "?0=12345"
    //                  "?3=-12345"
    //                  "?G=12345"
    long val=0;
    bool neg=false;
    int j=i+3;
    int address = input[i+1]-'0';
    #ifdef MG_USES_ANALOG_SENSOR
    if((address>=0 && address<NUMSENSORS) || address==23) 
    #else
    if(address>=0 && address<NUMSENSORS) 
    #endif
    {
        char c=input[j];
        if(c=='-')
        {
            neg=true;
            j++;
            c=input[j];
        }
        while(c>='0' && c<='9')
        {
            val = 10*val + (c-'0');
            j++;
            c=input[j];
        }
        if(neg) val=-val;
        #ifdef MG_USES_ANALOG_SENSOR
        if(address==23) // address = 'G' (gas)
        {
            TotalGas = val - MG_USES_ANALOG_SENSOR.Today; // calculate the value of last midnight
            eeprom_write_dword((uint32_t*) 0, TotalGas);  // store gas total in 0 
        }
        else
        #endif
        {
            // let the sensor write the value to eeprom
            sensors[address]->Update(val);
        }
    }
} 

void SaveValues()
{
    for(byte i=0;i<NUMSENSORS;i++)
    {
        sensors[i]->Save();
    }
}

void ResetValues()
{
    for(byte i=0;i<NUMSENSORS;i++)
    {
        sensors[i]->Reset();
    }
}

void ShowStatus(EthernetClient client)
{
    const char* br = "<br>";
    client << F("<html><style>td,th {padding:8;text-align:center;}</style>");
    client << F(VERSION) << br;
    client << DateTime(now()) << br;
    client << F("Uptime=") << upTime/24 << "d+" << upTime%24 << "h" << br;  
    client << F("<table border=\"1\" cellspacing=\"0\">");
    client << F("<tr><th>ID<th>SID<th>Type<th>Actual<th>Peak<th>Today<th>Factor<th>TodayCnt<th>EEprom<th>ppu<th>Pulse<th>Extra</tr>");

    for(int i=0;i<NUMSENSORS;i++)
    {  
        sensors[i]->CalculateActuals();
        client << F("<tr><td>") << i;
        sensors[i]->Status(client);
        client << F("</tr>");
    }
    client << F("</table>PvOutput response=") << pvResponse << br;
    client << F("DNS status=") << DnsStatus << br;
    client << F("Last NTP update=") << DateTime(lastTimeUpdate) << br;
    #ifdef MG_USES_ANALOG_SENSOR
    client << F("Mindergas=") << TotalGas << br;
    #endif
    #ifdef USE_MINDERGAS
    client << F("mgResponse=") << mgResponse << br;
    #endif
}




