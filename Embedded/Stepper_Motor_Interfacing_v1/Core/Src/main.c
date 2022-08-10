/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Stepper Motor
#define FREQUENCY_TIM							167                        //83 = 1MHz , 167 = 500KHz
#define COUNTER_PERIOD							1200
#define MICROSTEP_RESOLUTION 					0.225
#define STEPS_IN_REVOLUTION 					1600 * 2
#define STEPPERMOTOR_TIMx						&htim2

#define TIMEOUT_TIMx							&htim3
#define TIMEOUT_DURATION						2

// Servo Motor
#define STAGING_SERVOMOTOR_TIMx					&htim1
#define STAGING_MOTOR_TIM_CHANNEL				TIM_CHANNEL_2
#define CONVEYOR_1_SERVO_CHANNEL				TIM_CHANNEL_3
#define STAGING_MOTOR_RUNTIME					5000					// in seconds


#define NO_PROVINCE								0
#define ON_PROVINCE								1
#define BC_PROVINCE								2
#define AB_PROVINCE								3

#define UNSORTED_MAILBOX						-135					// in degrees
#define ON_MAILBOX								-90
#define BC_MAILBOX								0
#define AB_MAILBOX								90

#define DEBUG_PROGRAM							1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
int32_t numOfSteps = 0;
uint8_t rxBuffer[256] = {'\0'};
uint8_t rxByte = 0;

int8_t startProgram = 5;
uint8_t rcvdAck = 0;
uint8_t retry = 0;

int8_t provinceFlag = -1;
uint8_t provinceRetry = 0;

//uint8_t irSense = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	static uint32_t previousTime = 0;
	static uint32_t nowTime = 0;

	nowTime = HAL_GetTick();
	if((nowTime - previousTime) > 100){
		previousTime = HAL_GetTick();
		if(GPIO_Pin == B1_Pin){
			retry = 0;
			provinceFlag = -1;
			provinceRetry = 0;
			if(startProgram != 1){
				startProgram = 0;
			}
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	static uint8_t count = 0;
	if(htim == STEPPERMOTOR_TIMx){
		HAL_GPIO_TogglePin(STEP_GPIO_Port, STEP_Pin);											// toggling the step pin to create square wave
		numOfSteps--;
		if(numOfSteps <= 0){
			stepperMotorStop();
		}
	}
	else if(htim == TIMEOUT_TIMx){
		count++;
		if(count >= TIMEOUT_DURATION){
			count = 0;
			startProgram *= (-1);
			if(rcvdAck == 1){
				retry = 0;
				rcvdAck = 0;
				startProgram += 1;
			}
			else{
				retry++;
				if(retry > 3){
					startProgram = 5;
				}
			}
			HAL_TIM_Base_Stop_IT(TIMEOUT_TIMx);
		}
	}
}

void stepperMotorSetup(void){
	  HAL_GPIO_WritePin(FR_GPIO_Port, FR_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(PS_GPIO_Port, PS_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	  //HAL_GPIO_WritePin(MONI_GPIO_Port, MONI_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(VREF_GPIO_Port, VREF_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET);
	  HAL_TIM_Base_Init(STEPPERMOTOR_TIMx);
	  HAL_TIM_Base_Start_IT(STEPPERMOTOR_TIMx);
}

void stepperMotorStop(void){
	HAL_TIM_Base_Stop_IT(STEPPERMOTOR_TIMx);
}

void stepperMotionInDegreeC(int16_t angle){
	int32_t steps = 0;

	steps = (int32_t)(angle / MICROSTEP_RESOLUTION);
	numOfSteps = steps * 2;

	if(angle > 0){
		HAL_GPIO_WritePin(FR_GPIO_Port, FR_Pin, GPIO_PIN_RESET);
	}
	else if(angle < 0){
		HAL_GPIO_WritePin(FR_GPIO_Port, FR_Pin, GPIO_PIN_SET);
		numOfSteps *= (-1);
	}
	HAL_TIM_Base_Start_IT(STEPPERMOTOR_TIMx);
}

void runServoMotor(void){
	//htim1.Instance->CCR1 = 125;
	HAL_TIM_PWM_Start_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
	HAL_Delay(STAGING_MOTOR_RUNTIME);
	HAL_TIM_PWM_Stop_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
	//htim1.Instance->CCR1 = 0;
}

void provinceDelivery(uint8_t province){
	int16_t degrees = 0;

	if(province == NO_PROVINCE){
		degrees = UNSORTED_MAILBOX;
	}
	else if(province == ON_PROVINCE){
		degrees = ON_MAILBOX;
	}
	else if(province == BC_PROVINCE){
		degrees = BC_MAILBOX;
	}
	else if(province == AB_PROVINCE){
		degrees = AB_MAILBOX;
	}
	stepperMotionInDegreeC(degrees);
	while(numOfSteps > 0);
	HAL_Delay(500);
	runServoMotor();
	HAL_Delay(500);
	stepperMotionInDegreeC(degrees * (-1));
	while(numOfSteps > 0);
	HAL_Delay(50);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	static uint8_t bufferIndex = 0;

	if(huart->Instance == USART6){
		if((rxByte != '\n')){
			rxBuffer[bufferIndex] = rxByte;
			bufferIndex++;
		}
		else{
			if(startProgram == -1){
				if(strncmp((const char *)rxBuffer, (const char *)"ACK:OK", (size_t)6) == 0){
					rcvdAck = 1;
				}
//				else if(strncmp((const char *)rxBuffer, (const char *)"ACK:ERR", (size_t)7) == 0){
//					rcvdAck = 0;
//				}
			}
			if(startProgram == -2){
				if(strncmp((const char *)rxBuffer, (const char *)"PROVINCE:", (size_t)9) == 0){
					rcvdAck = 1;
					if((strncmp((const char *)&rxBuffer[9], (const char *)"ON", 2) == 0)){
						provinceFlag = ON_PROVINCE;
					}
					else if((strncmp((const char *)&rxBuffer[9], (const char *)"BC", (size_t)2) == 0)){
						provinceFlag = BC_PROVINCE;
					}
					else if((strncmp((const char *)&rxBuffer[9], (const char *)"AB", (size_t)2) == 0)){
						provinceFlag = AB_PROVINCE;
					}
					else if((strncmp((const char *)&rxBuffer[9], (const char *)"UN", (size_t)2) == 0)){
						provinceFlag = NO_PROVINCE;
					}
					else{
						#if DEBUG_PROGRAM
						HAL_UART_Transmit(&huart2, (uint8_t *)"ACK:ERR\n", 8, 2000);
						#endif
						HAL_UART_Transmit(huart, (uint8_t *)"ACK:ERR\n", 8, 2000);
						rcvdAck = 0;
					}
					if(rcvdAck == 1){
						startProgram *= -1;
						startProgram++;
					}
				}
				else{
					rcvdAck = 0;
					#if DEBUG_PROGRAM
					HAL_UART_Transmit(&huart2, (uint8_t *)"ACK:ERR\n", 8, 2000);
					#endif
					HAL_UART_Transmit(huart, (uint8_t *)"ACK:ERR\n", 8, 2000);
				}
			}
			#if DEBUG_PROGRAM
			HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, bufferIndex, 2000);
			HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 2000);
			#endif
			memset((char *)rxBuffer, '\0', (size_t)256 * sizeof(rxBuffer[0]));
			bufferIndex = 0;
		}
		HAL_UART_Receive_IT(huart, &rxByte, 1);
	}
}

void conOneServoStart(void){
//	htim1.Instance->CCR2 = 25;
//	htim1.Instance->CCR3 = 25;
	__HAL_TIM_SET_COMPARE(STAGING_SERVOMOTOR_TIMx, CONVEYOR_1_SERVO_CHANNEL, 25);
	__HAL_TIM_SET_COMPARE(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL, 25);
	HAL_TIM_Base_Start_IT(STAGING_SERVOMOTOR_TIMx);
	HAL_TIM_PWM_Start_IT(STAGING_SERVOMOTOR_TIMx, CONVEYOR_1_SERVO_CHANNEL);
	HAL_TIM_PWM_Start_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
	while(HAL_GPIO_ReadPin(IR_SENSOR_GPIO_Port, IR_SENSOR_Pin) == GPIO_PIN_SET);
	HAL_Delay(50);
	while(HAL_GPIO_ReadPin(IR_SENSOR_GPIO_Port, IR_SENSOR_Pin) == GPIO_PIN_RESET);
//	while(irSense != 1);
//	irSense = 0;
//	htim1.Instance->CCR3 = 0;
	__HAL_TIM_SET_COMPARE(STAGING_SERVOMOTOR_TIMx, CONVEYOR_1_SERVO_CHANNEL, 0);
	HAL_TIM_PWM_Stop_IT(STAGING_SERVOMOTOR_TIMx, CONVEYOR_1_SERVO_CHANNEL);
	HAL_Delay(1000);
//	htim1.Instance->CCR2 = 0;
	__HAL_TIM_SET_COMPARE(STAGING_SERVOMOTOR_TIMx, CONVEYOR_1_SERVO_CHANNEL, 0);
	HAL_TIM_PWM_Stop_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
	HAL_TIM_Base_Stop_IT(STAGING_SERVOMOTOR_TIMx);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart6, &rxByte, 1);
  stepperMotorSetup();
//  htim1.Instance->CCR2 = 0;
//  htim1.Instance->CCR3 = 0;
//  HAL_TIM_PWM_Start_IT(STAGING_SERVOMOTOR_TIMx, CONVEYOR_1_SERVO_CHANNEL);
//  HAL_TIM_PWM_Start_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
  HAL_Delay(250);
//  stepperMotionInDegreeC(-135);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(startProgram == 0){
		  conOneServoStart();
		  startProgram++;
	  }
	  else if(startProgram == 1){
		rcvdAck = 0;
		#if DEBUG_PROGRAM
		HAL_UART_Transmit(&huart2, (uint8_t *)"START\n", 6, 2000);
		HAL_Delay(10);
		#endif
		HAL_UART_Transmit(&huart6, (uint8_t *)"START\n", 6, 2000);
		startProgram = -1;
		HAL_TIM_Base_Start_IT(TIMEOUT_TIMx);
	  }
	  else if(startProgram == 2){
		#if DEBUG_PROGRAM
		HAL_UART_Transmit(&huart2, (uint8_t *)"Waiting for province\n", 21, 2000);
		#endif
		startProgram = -2;
	  }
	  else if(startProgram == 3){
		  if(provinceFlag == NO_PROVINCE){
			  provinceRetry++;
			  if(provinceRetry < 4){
				#if DEBUG_PROGRAM
				HAL_UART_Transmit(&huart2, (uint8_t *)"ACK:OK\n", 7, 2000);
				HAL_Delay(10);
				#endif
				HAL_UART_Transmit(&huart6, (uint8_t *)"ACK:OK\n", 7, 2000);
				HAL_TIM_Base_Start_IT(STAGING_SERVOMOTOR_TIMx);
				HAL_TIM_PWM_Start_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
				HAL_Delay(100);
				HAL_TIM_PWM_Stop_IT(STAGING_SERVOMOTOR_TIMx, STAGING_MOTOR_TIM_CHANNEL);
				HAL_TIM_Base_Stop_IT(STAGING_SERVOMOTOR_TIMx);
				provinceFlag = -1;
			  }
			  startProgram = 1;
		  }
		  else{
			#if DEBUG_PROGRAM
			HAL_UART_Transmit(&huart2, (uint8_t *)"ACK:OK\n", 7, 2000);
			HAL_Delay(10);
			#endif
			HAL_UART_Transmit(&huart6, (uint8_t *)"ACK:OK\n", 7, 2000);
			startProgram = 0;
		  }
		  HAL_Delay(2000);
	  }

	  if((provinceFlag != -1)){
		  if(provinceFlag == NO_PROVINCE){
			provinceRetry = 0;
			#if DEBUG_PROGRAM
			HAL_UART_Transmit(&huart2, (uint8_t *)"ACK:UN-------------\n", 20, 2000);
			HAL_Delay(10);
			#endif
			HAL_UART_Transmit(&huart6, (uint8_t *)"ACK:UN\n", 7, 2000);
		  }
		provinceDelivery(provinceFlag);
		provinceFlag = -1;
		provinceRetry = 0;
		startProgram = 0;
		HAL_Delay(2000);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1679;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 25;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  htim2.Init.Prescaler = FREQUENCY_TIM;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = COUNTER_PERIOD;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8399;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 10000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RST_Pin|OE_Pin|FR_Pin|PS_Pin
                          |MONI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, VREF_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STEP_GPIO_Port, STEP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RST_Pin OE_Pin FR_Pin PS_Pin
                           MONI_Pin */
  GPIO_InitStruct.Pin = RST_Pin|OE_Pin|FR_Pin|PS_Pin
                          |MONI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : VREF_Pin LD2_Pin */
  GPIO_InitStruct.Pin = VREF_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IR_SENSOR_Pin */
  GPIO_InitStruct.Pin = IR_SENSOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_SENSOR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : STEP_Pin */
  GPIO_InitStruct.Pin = STEP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STEP_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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

