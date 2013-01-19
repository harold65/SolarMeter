
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
    int i=inString.indexOf("save");
    if(i!=-1) SaveValues();
    i=inString.indexOf("?");
    if(i!=-1) ReadValue(inString,i);
    ShowStatus(client);
    client.stop();
  }
}

void ReadValue(String input,int i)
{
  long val=0;
  bool neg=false;
  int j=i+3;
  byte ee;
  int address = input[i+1]-'0';

  if(address>=0 && address<NUMSENSORS)
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
    // write value to eeprom
    sensors[address]->Update(val);
  }
} 

void SaveValues()
{
  for(byte i=0;i<NUMSENSORS;i++)
  {
    sensors[i]->Save();
  }
}

void ShowStatus(EthernetClient client)
{
  int ee;
  long eepromValue;
  client << F(VERSION) << endl;
  client << DateTime(now()) << endl;
  client << F("Uptime=") << upTime/24 << "d+" << upTime%24 << "h" << endl;  
  for(int i=0;i<NUMSENSORS;i++)
  {  
    client << endl;  
    sensors[i]->CalculateActuals();
    sensors[i]->Status(client);
    ee = (i+20)*4; // the eeprom address of this sensor where the last value is saved
    eepromValue = eeprom_read_dword((uint32_t*) ee); 
    client << F(" SensorId=") << i;
    client << F(", eeprom value=") << eepromValue << endl;
  }
  client << endl << F("PvOutput response=") << pvResponse << endl;
  client << F("DNS status=") << DnsStatus << endl;
  client << F("Last NTP update=") << DateTime(lastTimeUpdate) << endl;
}

