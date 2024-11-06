/*
 * game.c
 *
 *  Created on: Mar 20, 2024
 *      Author: dylanlim
 */

#include "main.h"

#include "game.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

//ADC_HandleTypeDef hadc1;
//DMA_HandleTypeDef hdma_adc1;
//
extern I2C_HandleTypeDef hi2c1;
//
//TIM_HandleTypeDef htim2;
//
//UART_HandleTypeDef huart2;
//

extern uint16_t Xaxis;
extern uint16_t Yaxis;
//extern uint16_t rawValues[2];
//extern uint16_t game_option = 0;
//extern uint32_t previousMillis = 0;
//extern uint32_t currentMillis = 0;
//extern char msg[20];


static const uint8_t LEDMAT_ADD = 0x75 << 1;
static const uint8_t PAGE_1 = 0x00;
//static const uint8_t PAGE_2 = 0x01;
//static const uint8_t PAGE_3 = 0x02;
//static const uint8_t PAGE_4 = 0x03;
//static const uint8_t PAGE_5 = 0x04;
//static const uint8_t PAGE_6 = 0x05;
//static const uint8_t PAGE_7 = 0x06;
//static const uint8_t PAGE_8 = 0x07;
static const uint8_t FUN_REG = 0x0B;
static const uint8_t COM_REG = 0xFD;
static const uint8_t MAT_ROW[11] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x01, 0x03, 0x05, 0x07, 0x09};
static const uint8_t MAT_COL[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};



uint8_t screenstatus[11];
uint8_t data[2];
uint8_t new_head_y, new_head_x;
uint8_t r_size = 11;
uint8_t c_size = 7;
int score = 0;
int fruit_x = 6;
int fruit_y = 3;
int keep_playing = 1;
int snakeX[4] = {0,0,0,0};
int snakeY[4] = {0,0,1,2};


// 0 = low speed, 1 = high speed, 2 = pause, 3 = play;
int button_state;
/* Number specifies the direction, 1=left, 2=right, 3=up, 4=down*/
int direction = 2;
char msg[20];

void clearscreen(){
  		for(uint8_t r = 0; r <11; r++){
			  data[0] = COM_REG;
			  data[1] = PAGE_1;
			  HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
			  //HAL_Delay(10);
			  data[0] = MAT_ROW[r];
			  data[1] = 0x00;
			  HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
			  //HAL_Delay(10);
			  for(uint8_t r = 0; r <11; r++){
					screenstatus[r] = 0;
  			  }
  		  }
  }

  void turnoffscreen(){
  	  data[0] = COM_REG;
  	  data[1] = FUN_REG;
  	  HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
  	  //HAL_Delay(10);
  	  data[0] = 0x0A; //shutdown on/off
  	  data[1] = 0x00; //off
  	  HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
  }

  void turnonscreen(){
  	  data[0] = COM_REG;
  	  data[1] = FUN_REG;
  	  HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
  	 // HAL_Delay(10);
  	  data[0] = 0x0A; //shutdown on/off
  	  data[1] = 0x01; //on
  	  HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
  }

  void addpixel(uint8_t r, uint8_t c){
    	screenstatus[r] |= MAT_COL[c];
    	data[0] = COM_REG;
    	data[1] = PAGE_1;
    	HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
    	data[0] = MAT_ROW[r];
    	data[1] = screenstatus[r];
    	HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
    }

  void pwm_pixel_pause(){
    	data[0] = COM_REG;
    	data[1] = PAGE_1;
    	HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);

    	for(uint8_t i = 0x54; i< 0x5B; i ++){
        	data[0] = i;
        	data[1] = 0x0A;
        	HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
    	}

    	data[0] = COM_REG;
    	data[1] = PAGE_1;
    	HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);

    	for(uint8_t j = 0x3C; j< 0x43; j ++){
			data[0] = j;
			data[1] = 0x0A;
			HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
    	}
  }

	void removepixel(uint8_t r, uint8_t c){
		screenstatus[r] &= ~MAT_COL[c];
		data[0] = COM_REG;
		data[1] = PAGE_1;
		HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
		data[0] = MAT_ROW[r];
		data[1] = screenstatus[r];
		HAL_I2C_Master_Transmit(&hi2c1, LEDMAT_ADD, data, 2, HAL_MAX_DELAY);
	}



	void display_end(){
		addpixel(2, 0); addpixel(8, 0);
		addpixel(3, 1); addpixel(7, 1);
		addpixel(4, 2); addpixel(6, 2);
		addpixel(5, 3); addpixel(5, 3);
		addpixel(6, 4); addpixel(4, 4);
		addpixel(7, 5); addpixel(3, 5);
		addpixel(8, 6); addpixel(2, 6);
	}

	void display_snake(){
		addpixel(snakeX[1], snakeY[1]);
		addpixel(snakeX[2], snakeY[2]);
		addpixel(snakeX[3], snakeY[3]);
		addpixel(fruit_x, fruit_y);
	}

	void display_pause(){
		addpixel(3, 0); addpixel(7, 0);
		addpixel(3, 1); addpixel(7, 1);
		addpixel(3, 2); addpixel(7, 2);
		addpixel(3, 3); addpixel(7, 3);
		addpixel(3, 4); addpixel(7, 4);
		addpixel(3, 5); addpixel(7, 5);
		addpixel(3, 6); addpixel(7, 6);
		pwm_pixel_pause();
	}

	void change_coordinate(int x_pos[4], int y_pos[4], int new_x_point, int new_y_point){
	    for (int i = 0; i < 3; i++) {
	        x_pos[i] = x_pos[i + 1];
	        y_pos[i] = y_pos[i + 1];
	    }
	    x_pos[3] = new_x_point;
	    y_pos[3] = new_y_point;
	}

	int move_direction(int dir){
		if((Xaxis < 500) & (new_head_x > 0) & (dir != 2)){
		  return 1; // Left
		}

		else if ((Xaxis > 3500) & (new_head_x < r_size - 1) & (dir != 1)){
		  return 2; // right
		}

		else if ((Yaxis < 500) & (new_head_y < c_size - 1) & (dir != 4)){
		  return 3; // up
		}

		else if ((Yaxis > 3500) & (new_head_y > 0) & (dir != 3)){
		  return 4; //down
		}

		else{
			return dir;
		}
	}

	int new_x_coordinates(int new_head_x){
		// Move left
		if((direction == 1)  & (new_head_x > 0)){
			return new_head_x - 1;
		}

		// move right
		else if((direction == 2) & (new_head_x < r_size - 1)){
			return new_head_x + 1;
		}

		else{
			return new_head_x;
		}
	}

	int new_y_coordinates(int new_head_y){
		// move up
		if((direction == 3) & (new_head_y < c_size - 1)){
			return new_head_y + 1;
		}

		// move down
		else if((direction == 4) & (new_head_y > 0)){
			return new_head_y - 1;
		}

		else{
			return new_head_y;
		}
	}

	int check_keep_playing(){
		if((snakeX[3] == snakeX[2]) && (snakeY[3] == snakeY[2])){
			return 0;
		}
		else{
			return 1;
		}

	}
