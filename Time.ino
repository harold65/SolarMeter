const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
time_t  lastTimeUpdate = 0;
byte ntpRetry;

void UpdateTime()
{
    // initialize time server
    // Try to set the time 10 times
    for(byte i = 1; i <= 10; i++)
    {
        Udp.begin(8888);
        unsigned long newTime = getNtpTime();
        Udp.stop();
        if (newTime>0) // got a valid time, keep it.
        {
            setTime(newTime);
            lastTimeUpdate = newTime;
            ntpRetry = i;
            break;
        }
    }
}

char dt[20];
char* DateTime(time_t t)
{
    int y = year(t) - 2000;
    if(y < 0) sprintf(dt,"Never");
    else      sprintf(dt, "%02d.%02d.%02d %02d:%02d:%02d", day(t),month(t),y,hour(t),minute(t),second(t));
    return dt;
}

unsigned long getNtpTime()
{
    while(Udp.parsePacket())
    {
      Udp.flush(); // make sure udp buffer is empty
    }
    sendNTPpacket();
    delay(1000);
    if(Udp.parsePacket())
    {
        Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
        // the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words:
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
        // check for invalid response
        if(highWord == 0 || lowWord == 0) return 0;
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        unsigned long now = secsSince1900 - 2208988800UL;  // GMT
        // DST == DaySavingTime == Zomertijd
        boolean dst = false;
        int m = month(now);
        int previousSunday = day(now) - weekday(now) + 1;  // add one since weekday starts at 1
        dst = !(m < 3 || m > 10); // between october and march
        if (dst) 
        {
            if (m == 3) 
            {
                //  starts last sunday of march
                dst = previousSunday >= 25;
            } 
            else if (m == 10) 
            {
                // ends last sunday of october
                dst = previousSunday < 25;
            }
        }
        now +=(dst ? 7200 : 3600); // CEST or CET
        now += TIME_OFFSET;
        Udp.flush();
        return now;
    }
    return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address 
void sendNTPpacket()
{
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE); 
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49; 
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp: 		   
    Udp.beginPacket(NTP_SERVER, 123); //NTP requests are to port 123
    Udp.write(packetBuffer,NTP_PACKET_SIZE);
    Udp.endPacket(); 
}

