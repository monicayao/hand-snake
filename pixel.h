#ifndef PIXEL
#define PIXEL

#include <stdbool.h>

/* Definition of a pixel. At any a point a pixel can be the wall, snake or food. */
typedef struct {
	bool isWall; 
	bool isGround;
	bool isSnake;
	bool isFood;
} Pixel;

/* --------- Setter methods ---------- */ 

void setWall(Pixel * inputP){
	inputP->isWall = true; 
	inputP->isGround = false;
	inputP->isSnake = false;
	inputP->isFood = false;
}

void setGround(Pixel * inputP){
	inputP->isWall = false; 
	inputP->isGround = true;
	inputP->isSnake = false;
	inputP->isFood = false;
}

void setSnake(Pixel * inputP){
	inputP->isWall = false; 
	inputP->isGround = false;
	inputP->isSnake = true;
	inputP->isFood = false;
}

void setFood(Pixel * inputP){
	inputP->isWall = false; 
	inputP->isGround = false;
	inputP->isSnake = false;
	inputP->isFood = true;
}


/* This method replies with a character that will indicate the right color for the pixel. 
   Snake is Red, Food is Green, Wall is White. */
char LEDToByte(Pixel * inputP)
{
	//Rxx- 0100- 0x04
	//xGx- 0010- 0x02
	//xxB- 0001- 0x01
	char result;
	if (inputP->isWall){
		result = 0x07;
	}
	else if(inputP->isFood){
		result= 0x06; 
	}
	else if(inputP->isSnake){
		result= 0x04; 
	}
	else {
		result= 0x00;
	}
		
	return result;
}

#endif
