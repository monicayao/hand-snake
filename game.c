//E155 Final Project
//Shivam Malpani 11/23/19

#include <stdio.h>
#include <stdbool.h>
#include "SAM4S4B_lab7/SAM4S4B.h"
#define RESET_PIN 15

typedef struct {
	bool isWall; 
	bool isGround;
	bool isSnake;
	bool isFood;
} Pixel;

typedef struct{
	
} Snake;
Pixel board [32][32];
int snake[20][2];
int lenSnake; 
int food[2];
int eaten; 
volatile unsigned char debug;
volatile short x,y,z, who;
short resetPos= 0;
char lastInp= 'u';
uint16_t readX[3]= {0xA9A9, 0xABAB,0xADAD};

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


char getAccelInp(){
	//acclerometer logic
	//spiInit16(5, 0, 1);
	/*
	debug = spiSendReceive16(0x202F); //WHO AM I 
	who= spiSendReceive16(0x8F8F);
	x = spiSendReceive16(readX[0]);
	y = spiSendReceive16(readX[1]);
	z = spiSendReceive16(readX[2]);
	*/
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




int updateGame(){
	char inp= getAccelInp();
	updateSnake(inp);
	updateFood();
	bool lose= checkCollsion();
	return lose;
}	


//Rxx- 0100- 0x04
//xGx- 0010- 0x02
//xxB- 0001- 0x01
char LEDToByte(Pixel * inputP)
{
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

void sendLED()
{
	pioDigitalWrite(RESET_PIN, 1);
	pioDigitalWrite(RESET_PIN, 0);
	int i,j; 
	for (i = 0; i < 16; ++i){
		for (j = 0; j < 35; ++j){
			
			char result= 0xFF;
			
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

int main(void){
	samInit();
  pioInit();
	tcInit();
	tcDelayInit();
	boardInit(); 
	snakeInit();
	foodInit();
	
  spiInit8(5,0,1);
	
	 pioPinMode(RESET_PIN, PIO_OUTPUT);


	bool lose =0;
		sendLED();
		while(!lose){
		lose= updateGame();
		sendLED();
		//volatile int i = (COUNTS_PER_MS*1000)/40; //one sec wait while reading accel
		//volatile int i = 3;
		//while(--i){
		//	char inp= getAccelInp();
		//}
	}
		while(1){
		sendLED();
	}
	
	return 1;
}



const char initialBoard[32][32] = {
 {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
	 '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},  // 0
 {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
	 '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},  // 1
 {'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 2
  {'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 3
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 4
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 5
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 6
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 7
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 8
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 9
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 10
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 11
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 12
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 13
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '#', '#', '#', '#', '-', '-', '-', '-', '#', '#'},  // 14
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '#', '#', '#', '#', '-', '-', '-', '-', '#', '#'},  // 15
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '#', '#', '#', '#', '-', '-', '-', '-', '#', '#'},  // 16
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '#', '#', '#', '#', '-', '-', '-', '-', '#', '#'},  // 17
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 18
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 19
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 20
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 21
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 22
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 23
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 24
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 25
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 26
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 27
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 28
	{'#', '#', '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-',
	 '-', '-', '-', '-', '-','-', '-', '-', '-', '-', '-', '-', '-', '-', '#', '#'},  // 29
	{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
	 '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},  // 30
  {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
	 '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},  // 31
};


char stringToByte(char inputChar)
{
	char result;
	switch(inputChar) {
		case '#': 
			result = 0x01; // WALL: 0000_0111
			break;
		case '-':
			result = 0x02; // GRASS: 0000_0010
			break;
	}
	return result;
}

/*
void sendLEDs()
{
	pioDigitalWrite(RESET_PIN, 1);
	pioDigitalWrite(RESET_PIN, 0);
	int i,j; 
	for (i = 0; i < 16; ++i){
		for (j = 0; j < 35; ++j){
			
			char result;
			
			if(j>=32){
				 result= 0x00;
			}
			else{
			 char top = stringToByte(initialBoard[i][j]);
			 char bot = stringToByte(initialBoard[i+16][j]);
			 result = (top<<3) | bot;
			}
		
			spiSend(result);
		 
		}
	}		

}
*/
/*
int main(void)
{
  samInit();
	pioInit();
	tcInit();
	tcDelayInit();
	boardInit(); 
	snakeInit();
	foodInit();
	// "clock divide" = master clock frequency / desired baud rate
  // the phase for the SPI clock is 1 and the polarity is 0
	spiInit8(5, 0, 1);
  
  pioPinMode(RESET_PIN, PIO_OUTPUT);

	while(1)
	{
		sendLED();
		//spiSendReceive(0x1F); // -> 00_011_111 		
		//spiSendReceive(0x12); // -> 00_010_010
    
		//spiSendReceive(0xFF);
		
	
	}
	//pioDigitalWrite(LOAD_PIN, 0);
}
*/
