IPAddress ip_pvoutput;
int DnsStatus;

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
  long v[12]; // data sum
  bool b[12]; // data present flags
  // start with 0
  for(byte i=0;i<12;i++)
  { 
    v[i]=0;
    b[i]=false;
  }

  int sid=S[0]->SID;

  for(byte i=0;i<NUMSENSORS;i++) // scan through the sensor array
  {
    byte type = S[i]->Type;
    if(type==5)       //temperature
    {
      v[type-1] += S[i]->Actual;
      b[type-1] = true;
    }
    else if(type==6)  //voltage
    {
      v[type-1] += S[i]->Today;
      b[type-1] = true;
    }
    else if(type==24)  //ferraris
    {
      // total consumption is production + net consumption
      v[2] = v[0] + S[i]->Today;
      v[3] = v[1] + S[i]->Actual;
      b[2] = true;
      b[3] = true;
    }
    else  // S0 sensors
    {
      v[type-1]+=S[i]->Peak;
      v[type-2]+=S[i]->Today;
      b[type-1] = true;
      b[type-2] = true;
    }

    if(i==NUMSENSORS-1 || S[i+1]->SID!=sid)
    {
      CheckIpPv(); // update the ipaddress via DNS
      int res = pvout.connect(ip_pvoutput,80);
      if(res==1) // connection successfull
      {
        pvout << F("GET /service/r2/addstatus.jsp");
        pvout << F("?key=" PVOUTPUT_API_KEY);
        pvout << F("&sid=") << sid;
        sprintf(webData, "&d=%04d%02d%02d", year(),month(),day());
        pvout << webData;
        sprintf(webData, "&t=%02d:%02d", hour(),minute());
        pvout << webData;
        for(byte i=0;i<12;i++)
        {
            #ifdef GRAADDAGEN
                // replace temperature by factor
                if(i==4 && b[i])
                {
                  pvout << "&v5=" << T1.GetFactor(G1.Today,hour());
                }
                else
            #endif
            if(b[i])  // only send extended data if present
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
      // reset the counters for the next round
      for(byte i=0;i<12;i++)
      {
        v[i]=0;
        b[i]=false;
      }
      if(i<NUMSENSORS) sid=S[i+1]->SID;
    }
  }
}



