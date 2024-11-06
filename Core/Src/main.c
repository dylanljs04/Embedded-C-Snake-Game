/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "game.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <time.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint16_t Xaxis = 0;
uint16_t Yaxis = 0;
uint16_t rawValues[2];
uint16_t game_option = 0;
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
//char msg[20];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

////the code below is an interrupt that is triggered on an Update event (i.e. when the timer reaches its ARR value)
////if you look at TIM2 in the .ioc file (the GUI tab) you will see it has a source of 84MHz, an ARR of 84000-1 and
////a prescaler of 10, meaning this interrupt will occur every 10 ms.
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim2)
  {
	////this updates the X and Y axes of my joystick
	  for(uint8_t i = 0; i<hadc1.Init.NbrOfConversion; i++){
		  Xaxis = (uint16_t) rawValues[0];
		  Yaxis = (uint16_t) rawValues[1];
	  }
	 ////This allows me to write to the serial monitor similar to the serial monitor on Arduino
	 ////Note STM32CUBEIDE does not have a serial monitor. You will need to download PuTTY to view
	 ////your serial output.
//	  sprintf(msg, "X axis: %hu \r\t\t", Xaxis);
//	  HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
//
//	  sprintf(msg, "Y axis: %hu \r\n", Yaxis);
//	  HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
  }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	  currentMillis = HAL_GetTick();
	  if ((currentMillis - previousMillis > 300) & (game_option < 2))
	  {
	    game_option++; //For testing only
	    previousMillis = currentMillis;
	  }

	  else if(currentMillis - previousMillis > 300){
		  game_option = 0;
		  previousMillis = currentMillis;
	  }
}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//	HAL_StatusTypeDef ret;
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
//
//  // 0 = low speed, 1 = high speed, 2 = pause, 3 = play;
  int button_state;
//  /* Number specifies the direction, 1=left, 2=right, 3=up, 4=down*/
  int direction = 2;
  char msg[20];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  ////This begins the process of storing our ADC readings into the DMA. The DMA can be thought of a temporary storage location.
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) rawValues, 2);
  ////This begins our timer 2
  HAL_TIM_Base_Start_IT(&htim2);

  // Functions for the LED Screen matrix Demo
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
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

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
	sprintf(msg, "\nPlayer Score: %d", score);
	HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if( HAL_I2C_IsDeviceReady(&hi2c1, LEDMAT_ADD, 2, HAL_MAX_DELAY) == HAL_OK){
		  keep_playing = check_keep_playing();
		  if((keep_playing == 1) & (game_option == 0 || (game_option == 1))){
			turnonscreen();
			clearscreen();
			display_snake();
			direction = move_direction(direction);
			new_head_x = new_x_coordinates(snakeX[3]);
			new_head_y = new_y_coordinates(snakeY[3]);
			change_coordinate(snakeX, snakeY, new_head_x, new_head_y);

			if((fruit_x == snakeX[3]) & (fruit_y == snakeY[3])){
				score++;
				fruit_x = rand() % 11;
				fruit_y = rand() % 7;
				sprintf(msg, "\nPlayer Score: %d", score);
				HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
				}

			if(game_option == 0){
				HAL_Delay(300);
			}

			if(game_option == 1){
				HAL_Delay(100);
			}

		  }

		  else if((keep_playing == 1) & (game_option == 2)){
			display_pause();
			HAL_Delay(300);
		  }

		  else{
			display_end();
			HAL_Delay(300);
		  }

	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  ////Joystick wiring-
	  ////+5V goes to 3V3 on the NUCLEO <-------- VERY IMPORTANT!!!! Do not connect to 5V as this will damage your board.
	  ////GND goes to GND on the NUCLEO
	  ////VRX goes to A0 on the NUCLEO
	  ////VRY goes to A1 on the NUCLEO


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 10;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 84000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : analog_button_Pin */
  GPIO_InitStruct.Pin = analog_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(analog_button_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
