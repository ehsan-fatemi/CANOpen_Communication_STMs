/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
#include <stdint.h>

uint32_t can_error = HAL_CAN_ERROR_NONE;
uint8_t flag_can_error = 0;

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 3;
//  hcan.Init.Prescaler = 6;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspInit 0 */

  /* USER CODE END CAN_MspInit 0 */
    /* CAN clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN interrupt Init */
    HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
  /* USER CODE BEGIN CAN_MspInit 1 */

  /* USER CODE END CAN_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspDeInit 0 */

  /* USER CODE END CAN_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN interrupt Deinit */
    HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
  /* USER CODE BEGIN CAN_MspDeInit 1 */

  /* USER CODE END CAN_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t rxBuffer[2];

uint8_t rxData[2];

CAN_FilterTypeDef sFilterConfig;

bool can_msg_received_flag = false;

void can_send_data(void)
{
	uint32_t TxMailbox;
	uint8_t data[] = {0 , 1};



	TxHeader.IDE 				= CAN_ID_STD;
	TxHeader.StdId 				= 0x122;
	TxHeader.ExtId 				= 0x000;
	TxHeader.RTR 				= CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.DLC 				= sizeof(data);


//	HAL_CAN_AddTxMessage(&hcan, &TxHeader , &data, &TxMailbox);
	HAL_CAN_AddTxMessage(&hcan, &TxHeader, data, &TxMailbox);


}

void can_filter()
{


	sFilterConfig.FilterActivation 		= CAN_FILTER_ENABLE;
	sFilterConfig.FilterBank			= 0;
	sFilterConfig.FilterFIFOAssignment	= CAN_FILTER_FIFO0;
	sFilterConfig.FilterMode			= CAN_FILTERMODE_IDLIST;
	sFilterConfig.FilterScale			= CAN_FILTERSCALE_16BIT;

//	sFilterConfig.FilterIdLow			= 0x123 << 5;		// ID #1
//	sFilterConfig.FilterIdHigh			= 0x124 << 5;		// ID #2
//
//	sFilterConfig.FilterMaskIdHigh		= 0x125 << 5;		// ID #3
//	sFilterConfig.FilterMaskIdLow		= 0xFFF << 5;		// ID #4

	sFilterConfig.FilterMode			= CAN_FILTERMODE_IDMASK;

	sFilterConfig.FilterIdLow			= 0x123 << 5;		//
	sFilterConfig.FilterMaskIdLow		= 0x123 << 5;		//

	sFilterConfig.FilterIdHigh			= 0x200 << 5;		//
	sFilterConfig.FilterMaskIdHigh		= 0x7FF << 5;		//






}

void can_activate_interrupts()
{
	HAL_CAN_ActivateNotification(&hcan , CAN_IT_TX_MAILBOX_EMPTY |
										 CAN_IT_RX_FIFO0_MSG_PENDING |
										 CAN_IT_ERROR |
										 CAN_IT_LAST_ERROR_CODE |
										 CAN_IT_ERROR_WARNING |
										 CAN_IT_ERROR_PASSIVE |
										 CAN_IT_BUSOFF |
										 CAN_IT_SLEEP_ACK);
}

//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rxBuffer);
//	can_msg_received_flag = true;
//
//}






void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	can_error = HAL_CAN_GetError(&hcan);
	flag_can_error = 1;


}

void can_error_print()
{

	char msg[50];
	sprintf(msg , "\r\nCAN Error Code: %03x" , hcan.ErrorCode);
	HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);

	if(can_error == HAL_CAN_ERROR_NONE)
	{
		return;
	}

	if(can_error & HAL_CAN_ERROR_BOF)
	{
//		can_previous_error = can_error;
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nCAN Error: HAL_CAN_ERROR_BOF", sizeof("\r\nCAN Error: HAL_CAN_ERROR_BOF"), 100);
	}

	if(can_error & HAL_CAN_ERROR_ACK)
	{
//		can_previous_error = can_error;
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nCAN Error: HAL_CAN_ERROR_ACK", sizeof("\r\nCAN Error: HAL_CAN_ERROR_ACK"), 100);
	}

	if(can_error & HAL_CAN_ERROR_EPV)
	{
//		can_previous_error = can_error;
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nCAN Error: HAL_CAN_ERROR_EPV", sizeof("\r\nCAN Error: HAL_CAN_ERROR_EPV"), 100);
	}

	if(can_error & HAL_CAN_ERROR_EWG)
	{
//		can_previous_error = can_error;
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nCAN Error: HAL_CAN_ERROR_EWG", sizeof("\r\nCAN Error: HAL_CAN_ERROR_EWG"), 100);
	}

	if(can_error & HAL_CAN_ERROR_BD)
	{
//		can_previous_error = can_error;
		HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nCAN Error: HAL_CAN_ERROR_BD", sizeof("\r\nCAN Error: HAL_CAN_ERROR_BD"), 100);
	}

	if(can_error & HAL_CAN_ERROR_TX_ALST0)
	{
	    HAL_UART_Transmit(&huart1, (uint8_t *)"\r\nCAN Error: TX Arbitration Lost Mailbox 0", sizeof("\r\nCAN Error: TX Arbitration Lost Mailbox 0") - 1, 100);
	}

	can_error = HAL_CAN_ERROR_NONE;
}






/* USER CODE END 1 */
