#include "CANOpen_App.h"
#include "usart.h"
//#include <string.h>


CANopenNodeSTM32 canopen;

void canoepn_app_init()
{
	canopen.CANHandle 		= &hcan;
	canopen.HWInitFunction	= MX_CAN_Init;
	canopen.timerHandle		= &htim17;
	canopen.desiredNodeID 	= 0x02;
	canopen.baudrate     	= 1000;

	canopen_app_init(&canopen);
}


void nmt_operation_mode_command()
{
	CO_NMT_sendCommand(CO->NMT, CO_NMT_ENTER_PRE_OPERATIONAL , 0x05);

}

//CO_SDO_return_t sdo_return_state;

SDO_RW_Control_t sdo_rw = {
		.read_state = SDO_RW_IDLE,
		.write_state = SDO_RW_IDLE,
};

void sdo_write(uint16_t object_index , uint8_t object_sub_index ,
		       uint8_t object_size , uint8_t *buffer , uint8_t buffer_size  , uint8_t node_id)
{

	if(sdo_rw.write_state == SDO_RW_IDLE)
	{

		sdo_rw.write_return_state = CO_SDOclient_setup(CO->SDOclient , 0x600 + node_id , 0x580 + node_id , node_id);
		sdo_write_return_staus(node_id);

		sdo_rw.write_return_state = CO_SDOclientDownloadInitiate(CO->SDOclient, object_index, object_sub_index , object_size , 100 , false);
		sdo_write_return_staus(node_id);

//		uint8_t buffer[] = {0xE8 , 0x03};
		CO_SDOclientDownloadBufWrite(CO->SDOclient, buffer , buffer_size);

		sdo_rw.write_state = SDO_RW_RUNNING;
	}

	if(sdo_rw.write_state == SDO_RW_RUNNING)
	{
		sdo_rw.write_SDOabortCode = CO_SDO_AB_NONE;
		size_t sizeTransferred = 0;

		sdo_rw.write_return_state = CO_SDOclientDownload(CO->SDOclient, 1000 , false , false , &sdo_rw.write_SDOabortCode, &sizeTransferred, NULL);
		sdo_write_return_staus(node_id);

		if(sdo_rw.write_return_state == CO_SDO_RT_ok_communicationEnd)
		{
			sdo_rw.write_state = SDO_RW_DONE;

		}
	}

}

void sdo_read(uint16_t object_index , uint8_t  object_sub_index, uint8_t *buffer , uint8_t buffer_size , uint8_t node_id)
{
	if(sdo_rw.read_state == SDO_RW_IDLE)
	{
		sdo_rw.read_return_state = CO_SDOclient_setup(CO->SDOclient , 0x600 + node_id , 0x580 + node_id , node_id);
		sdo_read_return_staus(node_id);

		sdo_rw.read_return_state = CO_SDOclientUploadInitiate(CO->SDOclient, object_index, object_sub_index, 100, false);
		sdo_read_return_staus(node_id);

		if(sdo_rw.read_return_state == CO_SDO_RT_ok_communicationEnd)
		{
			sdo_rw.read_state = SDO_RW_RUNNING;

		}
	}

	if(sdo_rw.read_state == SDO_RW_RUNNING)
	{
		sdo_rw.read_SDOabortCode = CO_SDO_AB_NONE;
		size_t sizeIndicated = 0;
		size_t sizeTransferred = 0;

		sdo_rw.read_return_state = CO_SDOclientUpload(CO->SDOclient, 1000, false, &sdo_rw.read_SDOabortCode, &sizeIndicated, &sizeTransferred, NULL);
		sdo_read_return_staus(node_id);

			if(sdo_rw.read_return_state == CO_SDO_RT_ok_communicationEnd)
			{
				sdo_rw.read_state = SDO_RW_DONE;

			}
	}

	if(sdo_rw.read_state == SDO_RW_DONE)
	{

		size_t size = CO_SDOclientUploadBufRead(CO->SDOclient, buffer, buffer_size);

	}

}
void sdo_write_return_staus(uint8_t node_id)
{
	static CO_SDO_return_t previous_write_return_state = 100;

	if( sdo_rw.write_return_state != previous_write_return_state)
	{

		  char sdo_msg[50];
		  if(sdo_rw.write_return_state < 0 )
		  {
			sprintf(sdo_msg , "\r\nThe SDO Write for node 0x%02x Error: %d" , node_id , sdo_rw.write_return_state);
			sdo_rw.write_state = SDO_RW_ERROR;
		  }
		  else if(sdo_rw.write_return_state > 0 )
		  {
			sprintf(sdo_msg , "\r\nThe SDO Write for node 0x%02x is in process: %d" , node_id ,sdo_rw.write_return_state);
		  }
		  if(sdo_rw.write_return_state == 0 )
		  {
			sprintf(sdo_msg , "\r\nThe SDO Write for node 0x%02x successfully transmitted: %d" , node_id , sdo_rw.write_return_state);
		  }

		  HAL_UART_Transmit(&huart1, sdo_msg , strlen(sdo_msg), 100);

	}
	previous_write_return_state = sdo_rw.write_return_state;

}
void sdo_read_return_staus(uint8_t node_id)
{
	static CO_SDO_return_t previous_read_return_state = 100;

	if( sdo_rw.read_return_state != previous_read_return_state)
	{
		  char sdo_msg[50];
		  if(sdo_rw.read_return_state < 0 )
		  {
			sprintf(sdo_msg , "\r\nThe SDO Read for node 0x%02x Error: %d" , node_id , sdo_rw.read_return_state);
			sdo_rw.read_state = SDO_RW_ERROR;
		  }
		  else if(sdo_rw.read_return_state > 0 )
		  {
			sprintf(sdo_msg , "\r\nThe SDO Read for node 0x%02x is in process: %d" , node_id , sdo_rw.read_return_state);
		  }
		  if(sdo_rw.read_return_state == 0 )
		  {
			sprintf(sdo_msg , "\r\nThe SDO Read for node 0x%02x successfully transmitted: %d" , node_id , sdo_rw.read_return_state);
		  }

		  HAL_UART_Transmit(&huart1, sdo_msg , strlen(sdo_msg), 100);

	}

	previous_read_return_state = sdo_rw.read_return_state;

}



