#ifdef USE_MAIL
EthernetClient mailClient;

void SendMail()
{
    if(mailClient.connect(MAIL_SERVER,25))
    {
        if(checkResponse())
        {
            mailClient << F("HELO Arduino") << endl; // say hello
            if(checkResponse())
            {
                mailClient << F("MAIL FROM:" MAIL_FROM) << endl; // identify sender
                if(checkResponse())
                {
                    mailClient << F("RCPT TO:" MAIL_TO) << endl; // identify recipient
                    if(checkResponse())
                    {
                        mailClient << F("DATA") << endl;
                        if(checkResponse())
                        {
                            mailClient << F("Subject:SolarMeter ") << day() << endl << endl; // insert subject
                            for(byte i=0;i<NUMSENSORS;i++)
                            {
                              sensors[i]->Status(mailClient);
                            }
                            mailClient << F(".") << endl; // end of mail
                            mailClient << F("QUIT") << endl; // terminate connection
                            checkResponse();
                        }
                    }
                }
            }
        }
        mailClient.stop();
    }
}

boolean checkResponse()
{
    long timer=millis();
    while(millis()-timer < 5000)
    {
        if(mailClient.available())
        {
            char c = mailClient.read();
            if(c=='\n') return true;
        }
    }
    return false;
}

#endif
