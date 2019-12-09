//E155 Final Project 
//Shivam Malpani 11/14/19

////////////////////////////////////////////////
// #includes
////////////////////////////////////////////////
#ifndef ACCELEROMETER
#define ACCELEROMETER

#include <stdio.h>
#include "SAM4S4B_lab7/SAM4S4B.h"

short resetPos= 0;
volatile unsigned char debug;
volatile short x,y,z, who;
char lastInp= 'u';
uint16_t readX[3]= {0xA9A9, 0xABAB, 0xADAD};

char getAccelInp(){
	//acclerometer logic
	//spiInit16(5, 0, 1);
	
	debug = spiSendReceive16(0x202F); //WHO AM I 
	who= spiSendReceive16(0x8F8F);
	x = spiSendReceive16(readX[0]);
	y = spiSendReceive16(readX[1]);
	z = spiSendReceive16(readX[2]);
	
	if(x>-3 && x<6 && y>-7 && y<6 && z>60 && z<69){
		resetPos=1;
		return lastInp;
	}

	//right y<-27
	else if(resetPos==1 && y<-27){
		resetPos= 0;
		lastInp= 'r';
		return 'r';
	}
	//left  y>30 
	else if(resetPos==1 && y>30){
		resetPos= 0;
		lastInp= 'l';
		return 'l';
	}
	//up x> 25 
	else if(resetPos==1 && x>25){
		resetPos= 0;
		lastInp= 'u';
		return 'u';
	}
	//down x<-25
	else if(resetPos==1 && x<-25){
		resetPos= 0;
		lastInp= 'd';
		return 'd';
	}
	
	return lastInp;
}

#endif
