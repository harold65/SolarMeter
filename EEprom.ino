// These two functions are used to read and write long variables from and to eeprom
//

long readlong(int address)
{
    // use long address (groups of 4)
    address+=20;
    address *= 4;
    return eeprom_read_dword((uint32_t*) address);
}

void writelong(int address, long value)
{
    address+=20;
    address *= 4;
    eeprom_write_dword((uint32_t*) address, value);
}
