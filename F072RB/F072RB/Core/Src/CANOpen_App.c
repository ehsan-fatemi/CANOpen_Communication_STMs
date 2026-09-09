#include "CANOpen_App.h"
#include "usart.h"


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

CO_SDO_return_t sdo_return_state;
//SDO_RW_State_t SDO_RW_State = {0};

SDO_RW_Control_t sdo_rw = {
		.read_state = SDO_RW_IDLE,
		.write_state = SDO_RW_IDLE,
};

void sdo_write()
{

	if(sdo_rw.write_state == SDO_RW_IDLE)
	{
		CO_SDOclient_setup(CO->SDOclient , 0x605 , 0x585, 0x05);

		CO_SDOclientDownloadInitiate(CO->SDOclient, 0x1017, 0x00 , 2 , 100 , false);


		uint8_t buffer[] = {0xE8 , 0x03};
		CO_SDOclientDownloadBufWrite(CO->SDOclient, buffer , 2);

		sdo_rw.write_state = SDO_RW_RUNNING;
	}

	if(sdo_rw.write_state == SDO_RW_RUNNING)
	{
		CO_SDO_abortCode_t SDOabortCode = CO_SDO_AB_NONE;
		size_t sizeTransferred = 0;

		sdo_return_state = CO_SDOclientDownload(CO->SDOclient, 1000 , false , false , &SDOabortCode, &sizeTransferred, NULL);

		if(sdo_return_state == CO_SDO_RT_ok_communicationEnd)
		{
			sdo_rw.write_state = SDO_RW_IDLE;

		}
	}

}


void sdo_read()
{
	if(sdo_rw.read_state == SDO_RW_IDLE)
	{
		sdo_return_state = CO_SDOclientUploadInitiate(CO->SDOclient, 0x1017, 0x00, 100, false);

		if(sdo_return_state == CO_SDO_RT_ok_communicationEnd)
		{
			sdo_rw.read_state = SDO_RW_RUNNING;

		}
	}

	if(sdo_rw.read_state == SDO_RW_RUNNING)
	{
		CO_SDO_abortCode_t SDOabortCode = CO_SDO_AB_NONE;
		size_t sizeIndicated = 0;
		size_t sizeTransferred = 0;

		sdo_return_state = CO_SDOclientUpload(CO->SDOclient, 1000, false, &SDOabortCode, &sizeIndicated, &sizeTransferred, NULL);

			if(sdo_return_state == CO_SDO_RT_ok_communicationEnd)
			{
				sdo_rw.read_state = SDO_RW_DONE;

			}
	}

	if(sdo_rw.read_state == SDO_RW_DONE)
	{
		uint8_t buffer[2] = {0};
		size_t size = CO_SDOclientUploadBufRead(CO->SDOclient, buffer, 2);

		char msg[30];
		sprintf(msg, "\r\nObject of 0x1017 is: 0x%.2x , 0x%.2x" , buffer[0] , buffer[1]);
		HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
		sdo_rw.read_state = SDO_RW_COMPLETE;
	}

}

