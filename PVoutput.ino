void SendToPvOutput(Solar S)
{
    if(S.SID > 0) // only output to enabled System
    {
        pvoutput(S.SID, S.Today, S.Peak);
    }
}

void pvoutput(int sid, long today, long peak)
{
    EthernetClient pvout;
    if (pvout.connect("pvoutput.org",80))
    {
        pvoutputok = true;
        logFile << sid << ": ";
        pvout << F("GET /service/r2/addstatus.jsp");
        pvout << F("?key=" PVOUTPUT_KEY);
        pvout << F("&sid=") << sid;
        sprintf(webData, "&d=%04d%02d%02d", year(),month(),day());
        pvout << webData;
        sprintf(webData, "&t=%02d:%02d", hour(),minute());
        pvout << webData;
        pvout << F("&v1=") << today;
        pvout << F("&v2=") << peak << endl;
        pvout << F("Host: pvoutput.org") << endl << endl;
        long x = millis() + 2000;
        // wait until one second passed or a character was received.
        while (!pvout.available() && millis() < x);  
        // dump the response to the log
        while(pvout.available())
        {
            logFile.write(pvout.read());
        }
        pvout.stop();
        // give pvoutput some time to process the request
        delay(200);
    }
    else
    {
        logFile << F("Cannot connect") << endl;
        pvoutputok=false;
    }
}

