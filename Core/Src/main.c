/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint16_t ButtonMatrixState[2] = {0};
uint16_t ButtonMatrixHistory[12] = {0};  //memory last 11-bits
uint32_t ButtonMatrixTimestamp = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void ButtonMatrixUpdate();
void ButtonMatrixCheck();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  /* USER CODE BEGIN 2 */

  //set initial LED state OFF
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		ButtonMatrixUpdate();

		if (ButtonMatrixState[0]!=ButtonMatrixState[1] && ButtonMatrixState[1]==(uint16_t)0)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

			switch (ButtonMatrixState[0])
			{
				case 0b0000000000000001:
					ButtonMatrixHistory[0] = 7;
					break;

				case 0b0000000000000010:
					ButtonMatrixHistory[0] = 8;
					break;

				case 0b0000000000000100:
					ButtonMatrixHistory[0] = 9;
					break;

				case 0b0000000000001000:
					//clear history
					for (int i = 0; i < 12; i += 1) {
						ButtonMatrixHistory[i] = 0;
					}
					break;

				case 0b0000000000010000:
					ButtonMatrixHistory[0] = 4;
					break;

				case 0b0000000000100000:
					ButtonMatrixHistory[0] = 5;
					break;

				case 0b0000000001000000:
					ButtonMatrixHistory[0] = 6;
					break;

				case 0b0000000010000000:
					//backspace
					for (int i = 0; i < 11; i += 1) {
						if (i == 10) {
							ButtonMatrixHistory[i] = 0;
						}
						else {
							ButtonMatrixHistory[i] = ButtonMatrixHistory[i+2];
						}
					}
					break;

				case 0b0000000100000000:
					ButtonMatrixHistory[0] = 1;
					break;

				case 0b0000001000000000:
					ButtonMatrixHistory[0] = 2;
					break;

				case 0b0000010000000000:
					ButtonMatrixHistory[0] = 3;
					break;

				case 0b0001000000000000:
					ButtonMatrixHistory[0] = 0;
					break;

				case 0b1000000000000000:
					//press ok
					ButtonMatrixCheck();  //check
					for (int i = 0; i < 11; i += 1) {
						ButtonMatrixHistory[i] = ButtonMatrixHistory[i+1];
					}
					break;

				default:
					//pass
					for (int i = 0; i < 11; i += 1) {
						ButtonMatrixHistory[i] = ButtonMatrixHistory[i+1];
					}
					break;
			}
			//update history
			for (int i = 11; i > 0; i -= 1) {
				ButtonMatrixHistory[i] = ButtonMatrixHistory[i-1];
			}
		}

		ButtonMatrixState[1] = ButtonMatrixState[0];
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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA8 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA7 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

//input: PA10, PB3, PB5, PB4 / output: PA9, PC7, PB6, PA7
//index input: 0-3 / output: 4-7

GPIO_TypeDef *ButtonPort[8] =
{ GPIOA, GPIOB, GPIOB, GPIOB, GPIOA, GPIOC, GPIOB, GPIOA };

uint16_t ButtonPin[8] =
{ GPIO_PIN_10, GPIO_PIN_3, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_9, GPIO_PIN_7,
GPIO_PIN_6, GPIO_PIN_7 };

uint8_t Button_row = 0;

void ButtonMatrixUpdate()
{
	if (HAL_GetTick() - ButtonMatrixTimestamp >= 50)  //sampling 50ms (delay)
	{
		ButtonMatrixTimestamp = HAL_GetTick();  //update time

		for (int Button_column = 0; Button_column < 4; Button_column += 1)
		{
			GPIO_PinState PinState = HAL_GPIO_ReadPin(ButtonPort[Button_column], ButtonPin[Button_column]);

			if (PinState == GPIO_PIN_RESET)
			{
				ButtonMatrixState[0] |= (uint16_t) 1 << (Button_column + Button_row * 4);
			}
			else
			{
				ButtonMatrixState[0] &= ~((uint16_t) 1 << (Button_column + Button_row * 4));
			}
		}
		//Set current-Output pin
		HAL_GPIO_WritePin(ButtonPort[Button_row + 4], ButtonPin[Button_row + 4], GPIO_PIN_SET);

		//update new row
		Button_row = (Button_row + 1) % 4;

		//Reset next-Output pin
		HAL_GPIO_WritePin(ButtonPort[Button_row + 4], ButtonPin[Button_row + 4], GPIO_PIN_RESET);
	}
}

void ButtonMatrixCheck()
{
	if (ButtonMatrixHistory[11] == 6 &&
		ButtonMatrixHistory[10] == 2 &&
		ButtonMatrixHistory[9] == 3 &&
		ButtonMatrixHistory[8] == 4 &&
		ButtonMatrixHistory[7] == 0 &&
		ButtonMatrixHistory[6] == 5 &&
		ButtonMatrixHistory[5] == 0 &&
		ButtonMatrixHistory[4] == 0 &&
		ButtonMatrixHistory[3] == 0 &&
		ButtonMatrixHistory[2] == 0 &&
		ButtonMatrixHistory[1] == 2 )
	{
		//turn ON LED
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	}
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
