//E155 Final Project
//Shivam Malpani and Monica Yao 12/11/19

#include <stdio.h>
#include "SAM4S4B_lab7/SAM4S4B.h"
#include "pixel.h"

#define RESET_PIN 15

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

//Red Snake 
void putSnakeOnBoard(){ 
	for(int i=0; i<lenSnake; ++i){
		setSnake(&board[snake[i][1]][snake[i][0]]); 
	}
} 
void removeSnakeOnBoard(){
		for(int i=0; i<lenSnake; ++i){
		setGround(&board[snake[i][1]][snake[i][0]]); 
	}
}

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

//blue food
void putFoodOnBoard(){
		setFood(&board[food[1]][food[0]]); 
}
void removeFoodOnBoard(){
	setFood(&board[food[1]][food[0]]); 
}

void foodInit(){ 
	food[0]=3; 
	food[1]=3; 
	putFoodOnBoard();
}

// UPDATE FUNCTIONS
void updateFood(){
	if(eaten){
		removeFoodOnBoard();
		++food[0];
		++food[1];
		if(food[0]>29){
			food[0]=3;
			food[1]=3;
		}
		putFoodOnBoard();
		eaten=0;
	}
}

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

bool checkCollsion(){
	volatile int i=0;
	volatile int j=0;
	for(i=0; i<lenSnake; ++i){
		if(snake[i][0]<3 || snake[i][1]<3 || snake[i][0]>29 || snake[i][1]>29){
			return 1;
		}
		for(j=0; j<i; ++j){
			if(i!=j && snake[i][0] ==snake[j][0] && snake[i][1]==snake[j][1]){
				return 1;
			}
		}
	}
	return 0;
}


int updateGame(){
	//char inp= uartRx();
	updateSnake(inpAcc);
	updateFood();
	bool lose= checkCollsion();
	return lose;
}	

void sendLED()
{
	pioDigitalWrite(RESET_PIN, 1);
	pioDigitalWrite(RESET_PIN, 0);
	int i,j; 
	for (i = 0; i < 16; ++i){
		for (j = 0; j < 35; ++j){			
			uint16_t result;			
			if(j>=32){
				 result= 0x00;
			}
			else{
			 char top = LEDToByte(&board[i][j]);
			 char bot = LEDToByte(&board[i+16][j]);
			 result = (top<<3) | bot;
			}
		
			spiSend(result);
		}
	}		
}

void readAcc(){
	inpAcc= uartRx();
	while(inpAcc!='u' && inpAcc!='d' && inpAcc!='l' && inpAcc!='r' ){
		inpAcc= uartRx();
	}
}

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
	
	pioPinMode(RESET_PIN, PIO_OUTPUT);
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
	while(!lose){
		lose= updateGame();
		sendLED();
		
		tcResetChannel(TC_CH1_ID);
		while (tcReadChannel(TC_CH1_ID) < end1){
			tcResetChannel(TC_CH0_ID);
			while (tcReadChannel(TC_CH0_ID) < end){
				sendLED();
				readAcc();
				
			}
	}
		
	}
	while(1){
		sendLED();
	}
	return 1;
}
