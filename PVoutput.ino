IPAddress ip_pvoutput;
int DnsStatus;
char pvResponse[80];
time_t pvResponseTime;
float previous = -1;


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
  DnsStatus = dns.getHostByName((char*)"pvoutput.org", remote_addr);
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
  
  unsigned int sid = S[0]->SID;

  for(byte i = 0; i<NUMSENSORS; i++) // scan through the sensor array
  {
    byte type = S[i]->Type;
    float actual = S[i]->Actual / S[i]->Factor;
    float peak = S[i]->Peak / S[i]->Factor;
    float today = S[i]->Today / S[i]->Factor;

    switch(type)
    {
      // temperature
      case 5:   v[type-1] += actual;
                b[type-1] = true;
                break;
      //voltage
      case 6:   v[type-1] += today;
                b[type-1] = true;
                break;
      //ferraris or P1
      case 24:  // total consumption is production + net consumption
                v[2] = v[0] + today;
                
                if(v[1] == 0) // no production, use data from type 24 directly
                {
                    v[3] = actual;
                }
                else
                {
                    // actual power is energy since previous upload divided by number of uploads per hour
                    // using this method because actual values of production and consumption sensors have different sampling rates, causing actual to be unreliable.
                    if(previous >=0 && previous < v[2]) 
                    {
                      v[3] = (v[2] - previous) * 60 / UPDATEINTERVAL;
                    }
                }
                previous = v[2];
                b[2] = true;
                b[3] = true;
                break;
      // other sensors (including type 0). Log Peak and total
      default:  v[type-1] += peak;
                v[type-2] += today;
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
          pvout << F(" HTTP/1.1") << endl;
          pvout << F("Host: pvoutput.org") << endl << endl;
          // give pvoutput some time to process the request
          delay(500);
		      // skip the first part of the reply, which is "HTTP/1.1 "
		      pvout.readBytes(webData, 9);
          // read the response code. 200 means ok. 0 means that there is no response yet
          byte lastResponse = pvout.parseInt();
          if(lastResponse == 0)
          {
            sprintf(pvResponse,"Response timeout\0");
            pvResponseTime = now();
          }
          else if(lastResponse != 200)
          { 
            sprintf(pvResponse, "%03d",lastResponse);
            size_t numchars = pvout.readBytes(pvResponse+3, 80); 
            pvResponse[numchars+3] = 0; // terminate the string
            pvResponseTime = now();
          }
          pvout.stop();
        }
        else // cannnot connect
        {
          sprintf(pvResponse,"No connection\0");
          pvResponseTime = now();
        }
      }
      // reset the counters for the next round
      for(byte n = 0; n < 12; n++)
      {
        v[n] = 0;
        b[n] = false;
      }
      if(i < NUMSENSORS) sid = S[i+1]->SID;
    }
  }
}

