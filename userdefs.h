//*****************************************************************
//*                                                                
//*  userdefines                                                   
//*****************************************************************
#ifndef userdefs_h
#define userdefs_h

#define PVOUTPUT_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxx"
// Every output has it's own system ID. Set to 0 if not used
static int SID1 = 0;
static int SID2 = 0;
static int SID3 = 0;

// Network variables
static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address
static byte ip[] = { 192, 168, 1, 99 }; // IP of arduino
static byte dnsserver[] = { 8, 8, 8, 8 };

#endif
