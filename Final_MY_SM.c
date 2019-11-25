#include "SAM4S4B/SAM4S4B.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define LED_PIN PIO_PA0
#define BUFF_LEN 32

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////


// Sends a null terminated string of arbitrary length
void sendString(char* str) {
	char* ptr = str;
	while (*ptr) uartTx(*ptr++);
}

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}



int main(void)
{
  samInit();
	pioInit();
	tcInit();
	tcDelayInit();
	
	// "clock divide" = master clock frequency / desired baud rate
  // the phase for the SPI clock is 1 and the polarity is 0
	spiInit(MCK_FREQ/244000, 0, 1);
  
	pioPinMode(LED_PIN, PIO_OUTPUT);
	pioDigitalWrite(LED_PIN, PIO_LOW);
	
	//uartInit(4,26); // no parity, a baud rate of 9600
	uartInit(4,2); // 115200 
	while(1)
	{
		spiSendReceive(0x12); // -> 01_001_111 
	
	
		char test = 0x42;
		sendString(&test);
	//sendString("$$$\n");
	//sendString("testing\n");
	
	}
}
