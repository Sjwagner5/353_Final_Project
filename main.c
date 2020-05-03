// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"

//store the current position of each fruit
volatile uint16_t BANANA_X_COORD = 0;
volatile uint16_t BANANA_Y_COORD = 0;
volatile uint16_t APPLE_X_COORD = 0;
volatile uint16_t APPLE_Y_COORD = 0;
volatile uint16_t ORANGE_X_COORD = 0;
volatile uint16_t ORANGE_Y_COORD = 0;

//when true, we will move the fruit
volatile bool ALERT_APPLE = true;
volatile bool ALERT_BANANA = true;
volatile bool ALERT_ORANGE = true;

//these boolean variables will track if there is currently one of these fruits on the screen
volatile bool APPLE_PRESENT = false;
volatile bool BANANA_PRESENT = false;
volatile bool ORANGE_PRESENT = false;

static const uint16_t START_STATE = 0xACE7u;

static int score = 0;//this will store the current user's score (number of fruit destroyed)
static int numLives = 3;//this will store the number of lives the user has left. This will be displayed with the red LEDs at the top of the board
static uint8_t highscore = 0;
volatile int pixel_inc = 1;//this will determine how many pixels the fruit move at a time (1 for easy level, 2 for medium level, 3 for hard level)

volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;//the current direction of the joystick

volatile bool ALERT_BUTTON;//alert when a button gets pressed

//*****************************************************************************
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}

//*****************************************************************************
// Generates a random number
// https://en.wikipedia.org/wiki/Linear-feedback_shift_register  -- DO NOT MODIFY
//*****************************************************************************
uint16_t generate_random_x(uint8_t image_width)
{
    static uint16_t lfsr = START_STATE;
    uint16_t bit;
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);

		if (lfsr > (COLS - image_width/2)) {//if the image overlaps the right edge, set it to the right edge
			lfsr = COLS-image_width/2 - 1;
		} else if (lfsr < (0 + image_width/2)) {//if the image overlaps the left edge, set it to the left edge
			lfsr = 0 + image_width/2 + 1;
		}
    return lfsr;
}
void draw_banana(void) {
	if (!BANANA_PRESENT) {
		BANANA_X_COORD = generate_random_x(bananaWidthPixels);
		BANANA_Y_COORD = bananaHeightPixels/2;
		BANANA_PRESENT = true;
	}
	else {
		BANANA_Y_COORD = BANANA_Y_COORD + pixel_inc;
	}
	lcd_draw_image(BANANA_X_COORD, bananaWidthPixels, BANANA_Y_COORD, bananaHeightPixels, bananaBitmaps, LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
}

void draw_apple(void) {
	if (!APPLE_PRESENT) {
		APPLE_X_COORD = generate_random_x(appleWidthPixels);
		APPLE_Y_COORD = appleHeightPixels/2;
		APPLE_PRESENT = true;
	} else {
		APPLE_Y_COORD = APPLE_Y_COORD + pixel_inc;
	}
	lcd_draw_image(APPLE_X_COORD, appleWidthPixels, APPLE_Y_COORD, appleHeightPixels, appleBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
}

void draw_orange(void) {
	if (!ORANGE_PRESENT) {
		ORANGE_X_COORD = generate_random_x(orangeWidthPixels);
		ORANGE_Y_COORD = orangeHeightPixels/2;
		ORANGE_PRESENT = true;
	} else {
		ORANGE_Y_COORD = ORANGE_Y_COORD + pixel_inc;
	}
	lcd_draw_image(ORANGE_X_COORD, orangeWidthPixels, ORANGE_Y_COORD, orangeHeightPixels, orangeBitmaps, LCD_COLOR_BLACK, LCD_COLOR_ORANGE);
}

//Replace each fruit with an explosion when they are touched
void explode_fruit(uint16_t xCoord, uint16_t yCoord) {
	int i = 0;

	if (xCoord + explosionWidthPixels/2 >= COLS) {//if the explosion image will overlap the right edge of the screen, draw it at the edge
		xCoord = COLS - explosionWidthPixels/2 -1;

	} else if (xCoord - explosionWidthPixels/2 <= 0){//if the explosion will overlap the left edge of the screen, draw it at the left edge
		xCoord = 0+explosionWidthPixels/2 + 1;
	}
	//draw the explosion graphic in place of the fruit
	lcd_draw_image(xCoord, explosionWidthPixels, yCoord, explosionHeightPixels, explosionBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);

	for (i = 0; i <1000000; i++) {}//wait so that the explosion graphic is displayed for a little bit

	lcd_draw_rectangle(xCoord-explosionWidthPixels/2, explosionWidthPixels, yCoord-explosionHeightPixels/2, explosionHeightPixels, LCD_COLOR_BLACK);
	//clear the explosion graphic
}

//*****************************************************************************
// Turn on the LEDs based on how many lives are left and restart the game when
// a user loses a life
//*****************************************************************************
void life_lost(void) {
	io_expander_write_reg(MCP23017_GPIOA_R, 0xFF>>(8-numLives));
	lcd_clear_screen(LCD_COLOR_BLACK);//clear the screen of all fruit

	//set these to false so that the program redraws fruits in a random position at the top of the screen
	APPLE_PRESENT = false;
	BANANA_PRESENT = false;
	ORANGE_PRESENT = false;
}

//*****************************************************************************
// Check if the most recent touch event is on top of 1 of the fruits
//*****************************************************************************
void check_touch(void) {
	int xTouch = ft6x06_read_x();
	int yTouch = ft6x06_read_y();
	//get the coordinates of the latest x and y touch

	//check if the apple has been touched
	if (APPLE_X_COORD - appleWidthPixels/2 <= xTouch &&
			APPLE_X_COORD + appleWidthPixels/2 >= xTouch &&
			APPLE_Y_COORD - appleHeightPixels/2 <= yTouch &&
			APPLE_Y_COORD + appleHeightPixels/2 >= yTouch) {

			explode_fruit(APPLE_X_COORD, APPLE_Y_COORD);
			APPLE_PRESENT = false;
			score++;
	//check if the banana has been touched
	} else if (BANANA_X_COORD - bananaWidthPixels/2 <= xTouch &&
						 BANANA_X_COORD + bananaWidthPixels/2 >= xTouch &&
						 BANANA_Y_COORD - bananaHeightPixels/2 <= yTouch &&
						 BANANA_Y_COORD + bananaHeightPixels/2 >= yTouch) {

			explode_fruit(BANANA_X_COORD, BANANA_Y_COORD);
			BANANA_PRESENT = false;
			score++;

	//check if the orange has been touched
	} else if (ORANGE_X_COORD - orangeWidthPixels/2 <= xTouch &&
						 ORANGE_X_COORD + orangeWidthPixels/2 >= xTouch &&
						 ORANGE_Y_COORD - orangeHeightPixels/2 <= yTouch &&
						 ORANGE_Y_COORD + orangeHeightPixels/2 >= yTouch) {

			explode_fruit(ORANGE_X_COORD, ORANGE_Y_COORD);
			ORANGE_PRESENT = false;
			score++;
	}
}

void title_screen(int diff){

	int offset;
	int bitmapOff;
	int width;
	int length;
	int i;
	int j;
	char welcome[] = "WELCOME TO FRUIT ASSASSIN";
	char joystick[] = "USE JOYSTICK TO SELECT LEVEL";
	char difficulty[] = "EASY MED HARD";
	char button[] = "PUSH TO START";
	uint16_t gfg, gbg, yfg, ybg, rfg, rbg;

	// Title Screen Menu
	//lcd_clear_screen(LCD_COLOR_BLACK);
	printf("TITLE SCREEN\n");

	//eeprom_byte_write(I2C1_BASE, HS_ADDR, highscore); // Toggle this comment to initalize EEPROM to 0 at address HS_ADDR

	// Load Highscore
	eeprom_byte_read(I2C1_BASE, HS_ADDR, &highscore);
	printf("Highscore Loaded: %d\n", highscore);
	printf("Score: %d\n", score);
	printf("Highscore: %d\n", highscore);

	//turn on the first 3 LEDs to show that there are 3 lives left
	io_expander_write_reg(MCP23017_GPIOA_R, 0x07);

	// Display Welcome Text
	j = 20;
	length = strlen(welcome);
	for (i = 0; i < length; i++) {
		offset = welcome[i] - 'A';
		bitmapOff = vinerHandITC_14ptDescriptors[offset].offset;
		width = vinerHandITC_14ptDescriptors[offset].widthBits;
		if(i < 11){
			lcd_draw_image(15 + j, width, ROWS/5, 15, &vinerHandITC_14ptBitmaps[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
			if(i + 1 == 11)
				j = 45;
		}
		else if(i < 17){
			lcd_draw_image(15 + j, width, ROWS/3.75, 15, &vinerHandITC_14ptBitmaps[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
			if(i + 1 == 17)
				j = 20;
		}
		else {
			lcd_draw_image(15 + j, width, ROWS/3, 15, &vinerHandITC_14ptBitmaps[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
		}
		j = 20 + j;
	}

	// Display Joystick Text
	j = 0;
	length = strlen(joystick);
	for (i = 0; i < length; i++) {
		offset = joystick[i] - 'C';
		bitmapOff = courierNew_10ptDescriptors[offset].offset;
		width = courierNew_10ptDescriptors[offset].widthBits;
		if(i < 12){
			lcd_draw_image(10 + j, width, ROWS/2.25, 8, &courierNew_10ptBitmaps[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
			if(i + 1 == 12)
					j = -20;
		}
		else if(i < 22){
			lcd_draw_image(10 + j, width, ROWS/2, 8, &courierNew_10ptBitmaps[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
			if(i + 1 == 22)
					j = -20;
		}
		else {
			lcd_draw_image(10 + j, width, ROWS/1.8, 8, &courierNew_10ptBitmaps[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
		}

		j = 10 + j;
	}

	gfg = LCD_COLOR_GREEN;  gbg = LCD_COLOR_BLACK;
	yfg = LCD_COLOR_YELLOW; ybg = LCD_COLOR_BLACK;
	rfg = LCD_COLOR_RED;    rbg = LCD_COLOR_BLACK;
	switch(diff){
		case 0:
			gfg = LCD_COLOR_GRAY;
		  gbg = LCD_COLOR_GREEN;
		break;
		case 1:
			yfg = LCD_COLOR_GRAY;
		  ybg = LCD_COLOR_YELLOW;
		break;
	  case 2:
			rfg = LCD_COLOR_GRAY;
		  rbg = LCD_COLOR_RED;
		break;

	}

	// Display Difficulty Text
	j = COLS/2 + 25;
	length = strlen(difficulty);
	for (i = 0; i < length; i++) {
		offset = difficulty[i] - 'A';
		bitmapOff = courierNew_12ptDescriptors[offset].offset;
		width = courierNew_12ptDescriptors[offset].widthBits;
		if(i <= 4){
			lcd_draw_image(width + j, width, ROWS/2.25, 9, &courierNew_12ptBitmaps[bitmapOff], gfg, gbg);
			if(i == 4)
					j = COLS/2 + 8;
		}
		else if(i < 9){
			lcd_draw_image(width + j, width, ROWS/1.9, 9, &courierNew_12ptBitmaps[bitmapOff], yfg, ybg);
			if(i == 8)
					j = COLS/2 + 10;
		}
		else {
			lcd_draw_image(width + j, width, ROWS/1.65, 9, &courierNew_12ptBitmaps[bitmapOff], rfg, rbg);
		}

		j = 15 + j;
	}

	// Display Button Text
	j = 20;
	length = strlen(button);
	for (i = 0; i < length; i++) {
		offset = button[i] - 'A';
		bitmapOff = courierNew_10ptDescriptors2[offset].offset;
		width = courierNew_10ptDescriptors2[offset].widthBits;
		lcd_draw_image(width + j, width, ROWS - 40, 8, &courierNew_10ptBitmaps2[bitmapOff], LCD_COLOR_RED, LCD_COLOR_BLACK);
		j = 10 + j;
	}

	// Cornucopia
  ORANGE_X_COORD = orangeWidthPixels/2;
	ORANGE_Y_COORD = orangeHeightPixels/2;
	lcd_draw_image(ORANGE_X_COORD, orangeWidthPixels, ORANGE_Y_COORD, orangeHeightPixels, orangeBitmaps, LCD_COLOR_BLACK, LCD_COLOR_ORANGE);


  BANANA_X_COORD = bananaWidthPixels/2;
	BANANA_Y_COORD = COLS - 35 + bananaHeightPixels/2;
	lcd_draw_image(BANANA_X_COORD, bananaWidthPixels, BANANA_Y_COORD, bananaHeightPixels, bananaBitmaps, LCD_COLOR_YELLOW, LCD_COLOR_BLACK);


  APPLE_X_COORD = ROWS/2 + appleWidthPixels/2;
	APPLE_Y_COORD = COLS + appleHeightPixels/2;
	lcd_draw_image(APPLE_X_COORD, appleWidthPixels, APPLE_Y_COORD, appleHeightPixels, appleBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);

	lcd_draw_rectangle(ROWS/2 + 45, 100, COLS/2, 200, LCD_COLOR_BLACK);



	// Game Settings
	//TODO: add main menu to choose easy or medium or hard (select with joystick)
	//if easy: set pixel_inc to 1, if medium: set pixel_inc to 2, if hard: set pixel_inc to 3
	//add functionality to start game on any push button press
	pixel_inc = diff + 1;
}

void end_screen(bool newHighScore){
	// End Screen Menu
	lcd_clear_screen(LCD_COLOR_BLACK);

	// Store New Highscore
	if(newHighScore){
		highscore = score;
		eeprom_byte_write(I2C1_BASE, HS_ADDR, highscore);
		printf("Highscore Stored: %d\n", highscore);
	}
	printf("Score: %d\n", score);
	printf("Highschool: %d\n", highscore);


	printf("END SCREEN\n\n\n");
}

void game_main(void) {
	char lastKey;
	bool gameOver = false;
	int i, diff;

	printf("Running...\n");
	diff = 3000;
	//while(!ALERT_BUTTON) {
		for (i = 0; i < 100; i++) {
			if(PS2_DIR == PS2_DIR_UP){
				diff -= 1;
			}
			else if(PS2_DIR == PS2_DIR_DOWN) {
				diff += 1;
			}

			title_screen(diff % 3);
		}
		//ALERT_BUTTON = false;
	//}
	lcd_clear_screen(LCD_COLOR_BLACK);

	//main game loop
	while (!gameOver) {
		// UART0: Pause and resume when space bar is hit.
		lastKey = UART0_Rx_Buffer.array[(UART0_Rx_Buffer.produce_count - 1) % UART_BUFFER_SIZE];
		if(lastKey == ' ') {
			printf("Paused.\n");
			pc_buffer_add(&UART0_Rx_Buffer, '`');
			lastKey = UART0_Rx_Buffer.array[(UART0_Rx_Buffer.produce_count - 1) % UART_BUFFER_SIZE];
			while(lastKey != ' '){
				lastKey = UART0_Rx_Buffer.array[(UART0_Rx_Buffer.produce_count - 1) % UART_BUFFER_SIZE];
			}
			pc_buffer_add(&UART0_Rx_Buffer, '`');
			printf("Running...\n");
		}

		if (ALERT_APPLE) {//draw or move the apple when the timer is up
			ALERT_APPLE = false;
			draw_apple();
		}
		if (ALERT_BANANA) {//draw or move the banana when the timer is up
			ALERT_BANANA = false;
			draw_banana();
		}
		if (ALERT_ORANGE) {//draw or move the orange when the timer is up
			ALERT_ORANGE = false;
			draw_orange();
		}
		if (ft6x06_read_td_status() == 1 || ft6x06_read_td_status() == 2) {
			check_touch();
		}

		//if any of the fruits reach the bottom of the screen, the user loses a life
		if (((APPLE_Y_COORD + (appleHeightPixels/2)) >= ROWS) || ((BANANA_Y_COORD + (bananaHeightPixels/2)) >= ROWS) || ((ORANGE_Y_COORD + (orangeHeightPixels/2)) >= ROWS)) {
			numLives--;
			life_lost();//lose a life for the user and restart the game
		}
		if (numLives == 0) {//once the user reaches 0 lives left, the game is over
			gameOver = true;
		}
	}
  //TODO: add game over screen and show final score
  end_screen(score > highscore);

	//TODO: also need to add high score with eeprom
}

//*****************************************************************************
//*****************************************************************************
int
main(void)
{
		init_hardware();
		game_main();
    while(1){};
}
