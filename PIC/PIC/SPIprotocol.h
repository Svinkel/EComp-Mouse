#ifndef SPIPROTOCOL_H_INCLUDED
#define SPIPROTOCOL_H_INCLUDED

char ReadWithSPIHalfDuplex(char address);
int WriteWithSPIHalfDuplex(char address, char Data);

#endif // SPIPROTOCOL_H_INCLUDED
