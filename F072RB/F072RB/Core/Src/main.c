/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "CANOpen_App.h"
#include "OD.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == canopen.timerHandle)
	{
		canopen_app_interrupt();
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
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim17);
  HAL_CAN_Start(&hcan);
  HAL_UART_Transmit(&huart1, "\r\nUART 1 Started!", sizeof("\r\nUART 1 Started!") , 100);
  HAL_UART_Receive_IT(&huart1, &Rx_buffer, 2);

  canoepn_app_init();

	nmt_operation_mode_command();


	uint32_t previous_time = HAL_GetTick();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint32_t current_time = HAL_GetTick();
//	  if(current_time - previous_time >= 300)
//	  {
//		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//		  previous_time = current_time;
//	  }

	  canopen_app_process();



	  if(sdo_rw.read_state !=  SDO_RW_RUNNING && sdo_rw.write_state != SDO_RW_DONE)
	  {
		  ////Heart beat
//		  uint8_t buffer[2] = {0x00 , 0x01};
//		  sdo_write(0x1017 , 0x00 , 2 , buffer , 2 , 0x05 );

		  // temperature
		  uint16_t object_index = 0x2000;
		  uint8_t buffer[4] = {0xEE , 0xEE , 0xEE , 0xEE};
		  sdo_write(object_index , 0x00 , 4 , buffer , 4 , 0x05 );

	  }

	  if(sdo_rw.write_state != SDO_RW_RUNNING && sdo_rw.read_state != SDO_RW_PRINTED)
	  {
//		  uint16_t object_index = 0x1017;
//		  uint8_t buffer[2] = {0};
//	  	sdo_read(object_index , 0x00 , buffer , 2 , 0x05);

		  // temperature
		  uint16_t object_index = 0x2000;
		  uint8_t buffer[4] = {0};
		  sdo_read(object_index , 0x00 , buffer , 4 , 0x05);

		  if(sdo_rw.read_state ==  SDO_RW_DONE )
			{
				char msg[80];
				uint8_t pos = 0;
				pos += snprintf(&msg[pos] , sizeof(msg) , "\r\nObject of 0x%x is:" , object_index);

				for(uint8_t i=0; i<sizeof(buffer); i++)
				{
					pos += snprintf(&msg[pos] , sizeof(msg) , " 0x%x " , buffer[i]);
				}
				HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
				sdo_rw.read_state = SDO_RW_PRINTED;
			}

	  }
	  if(current_time - previous_time > 1000)
	  {
		  char msg_2[80];
		  sprintf(msg_2 , "\r\n.x6000: %x " , OD_PERSIST_COMM.x6000_temp);
		  HAL_UART_Transmit(&huart1, msg_2, strlen(msg_2), 100);

		  previous_time = current_time;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
#ifdef USE_FULL_ASSERT
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
