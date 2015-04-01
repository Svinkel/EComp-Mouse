#include <p18cxxx.h>
#define SW_CS_PIN PORTCbits.RC2
#define TRIS_SW_CS_PIN TRISCbits.TRISC2

#define SW_DIO_PIN PORTBbits.RB7 // Data in/out
#define TRIS_SW_DIO_PIN TRISBbits.TRISB7

#define SW_SCK_PIN PORTBbits.RB6 // Clock
#define TRIS_SW_SCK_PIN TRISBbits.TRISB6

char ReadWithSPIHalfDuplex(char address)
{

    char Bit;
    char input;

    TRIS_SW_CS_PIN = 0;    // Make the CS pin an output
    TRIS_SW_SCK_PIN = 0; // Make the SCK pin an output
    TRIS_SW_DIO_PIN = 0; // Make the DIO pin an output
    SW_CS_PIN = 0; // Enable the mouse
    input = 0;

	for(Bit=7; Bit>=0;Bit--)
    {
        SW_SCK_PIN = 0; // Clear the SCK pin
        Nop(); // Pause a little bit
        SW_DIO_PIN = (address>>Bit)&1; // Set the Data line with a bit of the address
        SW_SCK_PIN = 1; // Set the SCK pin
        Nop(); // Pause a little bit
    }

    TRIS_SW_DIO_PIN = 1; // Make the DIO pin an input

    for(Bit=7; Bit>=0;Bit--)
    {
        SW_SCK_PIN = 0; // Clear the SCK pin
        Nop(); // Pause a little bit
        input = (input<<1) | SW_DIO_PIN; // Read data line
        SW_SCK_PIN = 1; // Set the SCK pin
        Nop(); // Pause a little bit
    }


    SW_CS_PIN = 1; // Disable the mouse
    return(input); // Return the received data
}

int WriteWithSPIHalfDuplex(char address, char Data)
{

    char Bit;
    char input;
	char address_w;
	int confirmation;

    TRIS_SW_CS_PIN = 0;    // Make the CS pin an output
    TRIS_SW_SCK_PIN = 0; // Make the SCK pin an output
    TRIS_SW_DIO_PIN = 0; // Make the DIO pin an output
    SW_CS_PIN = 0; // Enable the mouse
    input = 0;
	confirmation = 0;

	//envio do endereco do registo para o rato
	address_w = address;

    address_w = address_w | 0x80; 
	for(Bit=7; Bit>=0;Bit--)
    {
        SW_SCK_PIN = 0; // Clear the SCK pin
        Nop(); // Pause a little bit
        SW_DIO_PIN = (address_w>>Bit)&1; // Set the Data line with a bit of the address
        SW_SCK_PIN = 1; // Set the SCK pin
        Nop(); // Pause a little bit
    }
	


	//envio dos dados para o registo no rato
	for(Bit=7; Bit>=0;Bit--)
    {
        SW_SCK_PIN = 0; // Clear the SCK pin
        Nop(); // Pause a little bit
        SW_DIO_PIN = (Data>>Bit)&1; // Set the Data line with a bit of the address
        SW_SCK_PIN = 1; // Set the SCK pin
        Nop(); // Pause a little bit
    }

	input = ReadWithSPIHalfDuplex(address);
	
	
	if( input == (char) Data)
		confirmation = 1;

    SW_CS_PIN = 1; // Disable the mouse
    return(confirmation); // Return the received data
}