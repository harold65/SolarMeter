boolean SD_OK;

void SetupSD()
{
    // make sure that the default chip select pin of SDI is set to
    // output, even if you don't use it:
    pinMode(10, OUTPUT);
    // see if the card is present and can be initialized:
    if (!SD.begin(4)) 
    {
        SD_OK = false;
    }
    SdFile::dateTimeCallback(dateTime); // set callback for SD file date
    SD_OK = true;
}

void OpenLogFile()
{
    if(SD_OK)
    {
        char fileName[20];
        int fileCounter=0;
        // scan card until file not found
        do
        {
            fileCounter++;
            sprintf(fileName,"%02d%02d_%03d.csv",month(),day(),fileCounter);
        } 
        while (SD.exists(fileName) && fileCounter<100);
        // now create the new filename
        logFile = SD.open(fileName, O_CREAT | O_WRITE); // do not use FILE_WRITE. It is 100x slower
        WriteDateToLog();
    }
}

void CloseLogFile()
{
    if(SD_OK)
    {
        logFile.close();
    }
}

void WriteDateToLog()
{
    if(SD_OK)
    {
        logFile << year() << "-" << month() << "-" << day() << ";";
        logFile << hour() << ":" << minute() << ":" << second() << ";";
        logFile.flush();
    }
}
   
// this function is called by SD if it wants to know the time
void dateTime(uint16_t* date, uint16_t* time) 
{
    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(year(), month(), day());
    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(hour(), minute(), second());
}   
