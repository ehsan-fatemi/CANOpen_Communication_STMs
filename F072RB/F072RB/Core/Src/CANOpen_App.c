#include "CANOpen_App.h"


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


void sdo_heartbeat_command()
{
asdas
// #if (((CO_CONFIG_SDO_CLI)&CO_CONFIG_SDO_CLI_ENABLE) != 0) || defined CO_DOXYGEN


	CO_SDOclient_setup(CO->SDOclient , 0x605 , 0x585, 0x05);

	CO_SDOclientDownloadInitiate(CO->SDOclient, 0x1017, 0x00 , 2 , 100 , false);
//
//	CO_SDOclientDownloadInitSize(CO_SDOclient_t* SDO_C, size_t sizeIndicated);

	uint8_t buffer[] = {0x10 , 0x27};
	CO_SDOclientDownloadBufWrite(CO->SDOclient, buffer , 2);

	CO_SDO_abortCode_t SDOabortCode = CO_SDO_AB_NONE;
	size_t sizeTransferred = 0;
	CO_SDOclientDownload(CO->SDOclient, 1000 , false , false , &SDOabortCode, sizeTransferred, NULL);
}

