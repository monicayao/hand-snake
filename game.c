//E155 Final Project
//Shivam Malpani 11/23/19

#include <stdio.h>
#include <stdlib.h> 
#include "SAM4S4B_lab7/SAM4S4B.h"
#include "pixel.h"
#include "lose.h"

#define CS 15 //Chip select 

#define SELECT_LED 19
#define SELECT_ACC 20

#define MS 129
#define MS_1 128


Pixel board [32][32];
int snake[20][2];
int lenSnake; 
int food[2];
int eaten; 
char inpAcc;
void boardInit(){ 
	for(int i=0; i<32; ++i){ 
		for (int j = 0; j<32; ++j){
				if (i == 0 || i == 1 || i == 30 || i == 31 || j==0 || j==1 || j==30|| j==31){
					setWall(&board[i][j]);
				}
				else{
					setGround(&board[i][j]);
				}
		}
	}
}

/*
This method puts the snanke on the board.
It sets the board pixels to a isSnake for every 
unit of the snake. 
*/
void putSnakeOnBoard(){ 
	for(int i=0; i<lenSnake; ++i){
		setSnake(&board[snake[i][1]][snake[i][0]]); 
	}
} 

/*
This method removes the snake off the board. It does this by 
setting every pixel of the board that was previously a snake, 
to the ground. 
*/
void removeSnakeOnBoard(){
		for(int i=0; i<lenSnake; ++i){
		setGround(&board[snake[i][1]][snake[i][0]]); 
	}
}
/*
Initilizes a snake of length 5 in the middle of the board.
This method also places the snake on the board. 
*/
void snakeInit(){ 
	int initPos_X= 16;
	int initPos_Y= 16; 
	lenSnake= 5; 
	for(int i=0; i<lenSnake; ++i){
		snake[i][0]= initPos_X;
		snake[i][1]= initPos_Y+i;
	}
	putSnakeOnBoard();
}

/*
This method puts the food on the board by setting 
a pixel of the board to isFood. 
*/ 
void putFoodOnBoard(){
		setFood(&board[food[1]][food[0]]); 
}

/* This method removes the food off the board by setting the pixel of 
the board that was previously food to ground.*/ 
void removeFoodOnBoard(){
	setGround(&board[food[1]][food[0]]); 
}

/* This method initilizes food to a location on the upper half of the board. */
void foodInit(){ 
	food[0]=16; 
	food[1]=7; 
	putFoodOnBoard();
}

// UPDATE FUNCTIONS

/* It checks if the food was eaten. If it is eaten 
it updates the location of the food to a new random location 
on the board. */ 
void updateFood(){
	if(eaten){
		removeFoodOnBoard();
		do{
			food[0]= rand()%29;
			food[1]= rand()%29;
		}while(board[food[1]][food[0]].isSnake || food[0]<4 || food[1]<4);
		putFoodOnBoard();
		eaten=0;
	}
}

/* This method updates the position of the snake as it moves. The direction the snake moves 
   depends on user input which is passed in as a character. */
void updateSnake(char move){
	removeSnakeOnBoard();
	int nextPos_X= snake[0][0];
	int nextPos_Y= snake[0][1];

	if(move=='d'){
		++nextPos_Y;
	}
	else if(move=='r'){
		++nextPos_X;
	}
	else if(move=='l'){
		--nextPos_X;
	}
	else{ // 'u'
		--nextPos_Y;
	}
	if(nextPos_X==food[0] && nextPos_Y==food[1]){
		if(lenSnake!=20){
			++lenSnake;
		}
		eaten=1;
	}
	
	//shift 
	for(int i= lenSnake-2; i>=0; --i){
		snake[i+1][0]= snake[i][0];
		snake[i+1][1]= snake[i][1];
	}
	snake[0][0]=nextPos_X;
	snake[0][1]=nextPos_Y;
	putSnakeOnBoard();
}

/* This method checks if the snake has collided with a wall or itself */
bool checkCollsion(){
	volatile int i=0;
	volatile int j=0;
	for(i=0; i<lenSnake; ++i){
		//checks for wall collisions
		if(snake[i][0]<3 || snake[i][1]<3 || snake[i][0]>29 || snake[i][1]>29){
			return 1;
		}
		for(j=0; j<i; ++j){
			//
			if(i!=j && snake[i][0] ==snake[j][0] && snake[i][1]==snake[j][1]){
				return 1;
			}
		}
	}
	return 0;
}

/* this method calls other update methods to update the entire game */
int updateGame(){
	//char inp= uartRx();
	updateSnake(inpAcc);
	updateFood();
	bool lose= checkCollsion();
	return lose;
}	

/* This method sends the LED data one pixel at a time. It sends 35*2 bytes for every row since 32*2 bytes send pixel data 
   and 3*2 bytes are for shifting the bytes in. */
void sendLED()
{
	pioDigitalWrite(CS, 1);
	pioDigitalWrite(CS, 0);
	int i,j; 
	for (i = 0; i < 16; ++i){
		for (j = 0; j < 35; ++j){			
			uint16_t result;			
			if(j>=32){
				 result= 0x00;
			}
			else{
				char top, bot;
				if(i==snake[0][1] && j==snake[0][0]){
					top= 0x01;
				}
				else{
				 top = LEDToByte(&board[i][j]);
				}
				if(i+16==snake[0][1] && j==snake[0][0]){
					bot=0x01;
				}
				else{
			  bot = LEDToByte(&board[i+16][j]);
				}
			 result = (top<<3) | bot;
			}
		
			spiSend(result);
		}
	}		
}

/* Reads a char for user input from the acclerometer. This information is send either via wired UART or bluetooth UART */
void readAcc(){
	inpAcc= uartRx();
	while(inpAcc!='u' && inpAcc!='d' && inpAcc!='l' && inpAcc!='r'){
		inpAcc= uartRx();
	}
}

/* The main gets all the differnent pieces together and uses a timer to play the game at a decent pace. */
int main(void){
	samInit();
  pioInit();
	tcInit();
	tcDelayInit();
	boardInit(); 
	snakeInit();
	foodInit();
	
  spiInit16(5,0,1);
	uartInit(0,140);
	
	pioPinMode(CS, PIO_OUTPUT);
	pioPinMode(SELECT_LED, PIO_OUTPUT);
	pioPinMode(SELECT_ACC, PIO_OUTPUT);

	tcChannelInit(TC_CH0_ID,  TC_CLK2_ID, TC_MODE_UP); // mck/8 and counter just goes up with no comparison to RC
	tcChannelInit(TC_CH1_ID,  TC_CLK2_ID, TC_MODE_UP); 
	
	// start with making both SPIs NOT slaves 
	pioDigitalWrite(SELECT_LED, 1); 
	pioDigitalWrite(SELECT_ACC, 1);
	
	bool lose = 0;
	unsigned long end = MS*4000/8;
	unsigned long end1 = MS_1*4000/8;
	int temp=400;
	while(--temp){
		sendLED();
	} //To display the game for a few moments before the game starts. 
	while(!lose){
		lose= updateGame();
		sendLED();
		for(int c=0; c<3; ++c){
			tcResetChannel(TC_CH1_ID);
			while (tcReadChannel(TC_CH1_ID) < end1){
				tcResetChannel(TC_CH0_ID);
				while (tcReadChannel(TC_CH0_ID) < end){
					sendLED();
					readAcc();
					
				}
		}
	}
		
	}
	//The game ends if the player loses. 
	while(1){
		sendLoseScreen();
	}
	return 1;
}

