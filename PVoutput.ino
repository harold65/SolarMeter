void SendToPvOutput(Solar S)
{
    EthernetClient pvout;
    if (pvout.connect("pvoutput.org",80))
    {
        pvoutputok = true;
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
        long x = millis() + 2000;
        // wait until one second passed or a character was received.
        while (!pvout.available() && millis() < x);  
        // dump the response to the log
        while(pvout.available());
        pvout.stop();
        // give pvoutput some time to process the request
        delay(200);
    }
    else
    {
        pvoutputok=false;
    }
}

