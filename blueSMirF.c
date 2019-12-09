//E155 Final Project 
//Shivam Malpani 11/18/19

////////////////////////////////////////////////
// #includes
////////////////////////////////////////////////
/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SAM4S4B_lab7/SAM4S4B.h"

#define LED_PIN PIO_PA25
#define BUFF_LEN 32
#define COUNTS_PER_MS 40000
void sendString(char* str) {
	char* ptr = str;
	while (*ptr) uartTx(*ptr++);
}

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}
void delayLoop(int ms){ 
		volatile int i = COUNTS_PER_MS*ms; 
		while(--i); 
}

int main(void)
{
  samInit();
	pioInit();
	tcInit();
	tcDelayInit();
	
	pioPinMode(LED_PIN, PIO_OUTPUT);
	pioDigitalWrite(LED_PIN, PIO_LOW);
	
	uartInit(4,260);
	
	char request[BUFF_LEN] = "                  ";
	int charIndex = 0;
	while(1){
		sendString("$");
		sendString("$");
		sendString("$");
		delayLoop(100);
		sendString("+");
		sendString("IQ");
		sendString("SM,1");
		sendString("SU,9600");
		//get the address of slave
		//enable bonding using SX,1	
		//set the address to connect to using SR 
		sendString("D");
		while(inString(request, "\n") == -1)
		{
			// Wait for a complete request to be transmitted before processing
			while(!uartRxReady());
			request[charIndex++] = uartRx();
		}
}
}
*/
