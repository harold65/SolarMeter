#ifdef EXOSITE_KEY

String exResponse;

void SendToExosite()
{
    EthernetClient exoSiteClient;
    // try to connect to exoSite
    if (exoSiteClient.connect("m2.exosite.com",80)) 
    {
        long v[9];
        v[0]=S1.Actual;
        v[1]=S1.Today;
        v[2]=F1.Actual;
        v[3]=F1.Today;
        v[4]=G1.Actual;
        v[5]=G1.Today;
        v[6]=T1.Actual;
        v[7]=100*T1.gdFactor;
        
        String str="";
        for(int i=0;i<9;i++)
        {
            if(i>0) str += '&';
            str += i;
            str += '=';
            str += v[i];
        }        
        exoSiteClient << F("POST /api:v1/stack/alias HTTP/1.1") << endl;
        exoSiteClient << F("Host: m2.exosite.com") << endl;
        exoSiteClient << F("X-Exosite-CIK: " EXOSITE_KEY) << endl;
        exoSiteClient << F("Content-Type: application/x-www-form-urlencoded; charset=utf-8") << endl;
        exoSiteClient << F("Content-Length: ") << str.length() << endl << endl;
        exoSiteClient << str << endl;
        exResponse = exoSiteClient.readStringUntil('\n');
        exoSiteClient.stop();
    }
    else
    {
        exResponse = "No";
    }
}

#endif
