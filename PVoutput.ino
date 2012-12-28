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

void SendToPvOutput(Solar S)
{
    EthernetClient pvout;
    CheckIpPv(); // update the ipaddress via DNS
    int res = pvout.connect(ip_pvoutput,80);
    if(res==1) // connection successfull
    {
        pvout << F("GET /service/r2/addstatus.jsp");
        pvout << F("?key=" PVOUTPUT_API_KEY);
        pvout << F("&sid=") << S.SID;
        sprintf(webData, "&d=%04d%02d%02d", year(),month(),day());
        pvout << webData;
        sprintf(webData, "&t=%02d:%02d", hour(),minute());
        pvout << webData;
        if(S.Consumption)  // consumption counter
        {
          pvout << F("&v3=") << S.Today;
          pvout << F("&v4=") << S.Peak << endl;
        }
        else  // production counter
        {
          pvout << F("&v1=") << S.Today;
          pvout << F("&v2=") << S.Peak << endl;
        }
        pvout << F("Host: pvoutput.org") << endl << endl;
        // read the response code. 200 means ok
        pvResponse = pvout.parseInt();
        pvout.flush();
        pvout.stop();
        // give pvoutput some time to process the request
        delay(200);
    }
    else
    {
        pvResponse=res;
    }
}

