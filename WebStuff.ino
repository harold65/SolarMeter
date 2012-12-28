
String inString = String("");
// storage for last page access

void ServeWebClients()
{
    EthernetClient client = server.available();
    if (client) 
    {
      inString = client.readStringUntil('\n');
      client << F("HTTP/1.1 200 OK") << endl;
      client << F("Content-Type: application/x-javascript") << endl << endl;
      ShowStatus(client);
      client.stop();
    }
}

void ShowStatus(EthernetClient client)
{
    client << F(VERSION) << endl;
    client << DateTime(now()) << endl;
    client << F("Uptime=") << upTime/24 << "d+" << upTime%24 << "h" << endl;  
    for(int i=0;i<NUMSENSORS;i++)
    {  
      client << endl;  
      sensors[i]->CalculateActuals();
      sensors[i]->Status(client);
    }
    client << endl << F("PvOutput response=") << pvResponse << endl;
    client << F("DNS status=") << DnsStatus << endl;
    client << F("Last NTP update=") << DateTime(lastTimeUpdate) << endl;
}
