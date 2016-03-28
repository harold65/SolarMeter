#ifdef USE_MAIL
EthernetClient mailClient;

void SendMail()
{
    if(mailClient.connect(MAIL_SERVER,25))
    {
        if(CheckResponse())
        {
            mailClient << F("HELO Arduino") << endl; // say hello
            if(CheckResponse())
            {
                mailClient << F("MAIL FROM:" MAIL_FROM) << endl; // identify sender
                if(CheckResponse())
                {
                   mailClient << F("RCPT TO:" MAIL_TO) << endl; // identify recipient
                    if(CheckResponse())
                    {
                        mailClient << F("DATA") << endl;
                        if(CheckResponse())
                        {
                            mailClient << F("Subject:SolarMeter ") << day() << endl; // insert subject
                            mailClient << F("Content-type: text/html;") << endl << endl;

                            ShowStatus(mailClient);

                            mailClient << endl << F(".") << endl; // end of mail

		            if(CheckResponse())
                            {
                                mailClient << F("QUIT") << endl; // terminate connection
                            }
                        }
                    }
                }
            }
        }
        mailClient.stop();
    }
}

boolean CheckResponse()
{
  return mailClient.find((char*)"\n");
  
//    long timer = millis();
 //   while(millis() - timer < 5000)
  //  {
   //     if(mailClient.available())
    //    {
     //       char c = mailClient.read();
      //      if(c=='\n') return true;
       // }
   // }
   // return false;
}


#endif
