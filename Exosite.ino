#ifdef EXOSITE_KEY

int exResponse;

void SendToExosite()
{
    EthernetClient exoSiteClient;
    // try to connect to exoSite
    if (exoSiteClient.connect("m2.exosite.com",80)) 
    {
        long v[9];
        // fill the v-array with values defined in the exosite.h file
        #include "exosite.h"
        
        // compose a string with data to send
        String str = "";
        for(int i = 0; i < 10; i++)
        {
            if(i>0) str += '&';
            str += i;
            str += '=';
            str += v[i];
        }        
        // send the header
        exoSiteClient << F("POST /api:v1/stack/alias HTTP/1.1") << endl;
        exoSiteClient << F("Host: m2.exosite.com") << endl;
        exoSiteClient << F("X-Exosite-CIK: " EXOSITE_KEY) << endl;
        exoSiteClient << F("Content-Type: application/x-www-form-urlencoded; charset=utf-8") << endl;
        // send the length of the data
        exoSiteClient << F("Content-Length: ") << str.length() << endl << endl;
        // send the data itself
        exoSiteClient << str << endl;
        // skip the first part of the response ('HTTP/1.1 ')
        exoSiteClient.find(" ");
        // the characters after the space form the response number, read it
        exResponse = exoSiteClient.parseInt();
        // close the connection
        exoSiteClient.stop();
    }
    else
    {
        // unable to connect with exosite
        exResponse = 0;
    }
}

#endif
