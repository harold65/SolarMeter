
String inString = String("");
// storage for last page access

void ServeWebClients()
{
    EthernetClient client = server.available();
    int i;
    if (client) 
    {
      inString = client.readStringUntil('\n');
      client << F("HTTP/1.1 200 OK") << endl;
      client << F("Content-Type: application/x-javascript") << endl << endl;
      i=inString.indexOf("?");
      if(i!=-1) readValue(inString,i);
      showStatus(client);
      client.stop();
    }
}

void readValue(String input,int i)
{
    long val=0;
    bool neg=false;
    int j=i+3;
    int address = input[i+1]-'0';
    if(address>=0 && address<10)
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
        writelong(address,val);
    }
}

void showStatus(EthernetClient client)
{
    int i;
    client << DateTime(now()) << endl;
    client << F(VERSION) << endl << endl;
#ifdef SID1
    solarStatus(client, S1);
#endif
#ifdef SID2
    solarStatus(client, S2);
#endif
#ifdef SID3    
    solarStatus(client, S3);
#endif
    
    client << F("Eeprom:") << endl;
    for(i=0;i<10;i++)
    {
      client << i << " : " << readlong(i) << endl;
    }
    client << F("Time updated:") << DateTime(lastTimeUpdate) << endl;
    client << F("PvOutput response: ") << pvResponse;
}

void solarStatus(EthernetClient client, Solar S)
{
    client << S.SID << endl;
    if(S.Consumption) client << "C";
    else              client << "P";
    client << endl;
    client << F("Today: ") << S.Today << endl;
    client << F("Actual: ") << S.Actual << endl;
    client << F("Peak: ") << S.Peak << endl << endl;
}

char* DateTime(time_t t)
{
    int y = year(t)-2000;
    if(y < 0) sprintf(webData,"Invalid");
    else      sprintf(webData, "%02d.%02d.%02d %02d:%02d:%02d", day(t),month(t),y,hour(t),minute(t),second(t));
    return webData;
}
