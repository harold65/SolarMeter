IPAddress ip_pvoutput;
int DnsStatus;
int pvResponse;

// This function will contact the DNS server and ask for an IP address of PvOutput
// If successfull, this address will be used
// If not, keep using the previous found address
// In this way, we can still update to pvoutput if the dns timeouts.
void CheckIpPv()
{
  // Look up the host first
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(Ethernet.dnsServerIP());
  DnsStatus = dns.getHostByName("pvoutput.org", remote_addr);
  if (DnsStatus == 1)  ip_pvoutput = remote_addr; // if success, copy
}

// This function updates all registered sensors to pvoutput
// The sensors are listed in the 'S' array
void SendToPvOutput(BaseSensor** S)
{
  EthernetClient pvout;
  // create a total for each variable that can be used in pvoutput
  // !! The index in this array starts at 0 while the pvoutput vars start at 1
  float v[12]; // data sum
  bool b[12]; // data present flags
  // start with 0
  for(byte n = 0; n < 12; n++)
  { 
    v[n] = 0;
    b[n] = false;
  }
  
  CheckIpPv(); // update the ipaddress via DNS
  
  float production = 0;
  int sid = S[0]->SID;

  for(byte i = 0; i<NUMSENSORS; i++) // scan through the sensor array
  {
    byte type = S[i]->Type;
    switch(type)
    {
      // temperature
      case 5:   v[type-1] += (float)(S[i]->Actual) / S[i]->Factor;
                b[type-1] = true;
                break;
      //voltage
      case 6:   v[type-1] += (float)(S[i]->Today) / S[i]->Factor;
                b[type-1] = true;
                break;
      //ferraris or P1
      case 24:  // total consumption is production + net consumption
                v[2] = v[0] + (float)(S[i]->Today) / S[i]->Factor;
                v[3] = production + (float)(S[i]->Actual) / S[i]->Factor;
                // prevent negative consumption
                if(v[3]<0) v[3]=1;
                b[2] = true;
                b[3] = true;
                break;
      // production sensor. log the actual value. This is used by sensor type 24.
      case 2:   production += (float)(S[i]->Actual) / S[i]->Factor;
      // other sensors (including type 0). Log Peak and total
      default:  v[type-1] += (float)(S[i]->Peak) / S[i]->Factor;
                v[type-2] += (float)(S[i]->Today) / S[i]->Factor;
                b[type-1] = true;
                b[type-2] = true;
    }

    if(i == NUMSENSORS-1 || S[i+1]->SID != sid)
    {
      if(sid > 0) // only upload if the sid is valid
      {
        int res = pvout.connect(ip_pvoutput,80);
        if(res == 1) // connection successfull
        {
          pvout << F("GET /service/r2/addstatus.jsp");
          pvout << F("?key=" PVOUTPUT_API_KEY);
          pvout << F("&sid=") << sid;
          sprintf(webData, "&d=%04d%02d%02d", year(),month(),day());
          pvout << webData;
          sprintf(webData, "&t=%02d:%02d", hour(),minute());
          pvout << webData;
          for(byte i = 0; i < 12; i++)
          {
            #ifdef GRAADDAGEN
              // replace voltage(v6) by factor
              if(i==5)
              {
                pvout << "&v6=" << T1.GetFactor(G1.Today,hour());
              }
              else
            #endif
            if(b[i])  // only send data if present
            {
              pvout << "&v" << i+1 << "=" << v[i];
            }
          }
          pvout << endl << F("Host: pvoutput.org") << endl << endl;
          // read the response code. 200 means ok
          pvResponse = pvout.parseInt();
          pvout.stop();
          // give pvoutput some time to process the request
          delay(200);
        }
        else // cannnot connect
        {
          pvResponse=res;
        }
      }
      // reset the counters for the next round
      for(byte n = 0; n < 12; n++)
      {
        v[n] = 0;
        b[n] = false;
      }
      production = 0;
      if(i<NUMSENSORS) sid=S[i+1]->SID;
    }
  }
}

