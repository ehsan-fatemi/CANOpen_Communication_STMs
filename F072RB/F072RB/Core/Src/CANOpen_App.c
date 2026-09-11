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

void pdo_mapping()
{

}

void pdo_t()
{

}
//CO_TPDO_t* TPDO;
//CO_TPDO_t* TPDO_1 = {
//		.transmissionType = 0,
//
//};

//
//typedef struct {
//    uint16_t index;            /**< Object Dictionary index */
//    uint8_t subEntriesCount;   /**< Number of all sub-entries, including sub-entry at sub-index 0 */
//    uint8_t odObjectType;      /**< Type of the odObject, indicated by @ref OD_objectTypes_t enumerator. */
//    CO_PROGMEM void* odObject; /**< OD object of type indicated by odObjectType, from which @ref OD_getSub() fetches the
//                                  information */
//    OD_extension_t* extension; /**< Extension to OD, specified by application */
//} OD_entry_t;
//
//typedef struct {
//    CO_PDO_common_t PDO_common; /**< PDO common properties, must be first element in this object */
//    volatile void* CANrxNew[CO_RPDO_CAN_BUFFERS_COUNT]; /**< Variable indicates, if new PDO message received from CAN */
//    uint8_t CANrxData[CO_RPDO_CAN_BUFFERS_COUNT][CO_PDO_MAX_SIZE]; /**< CO_PDO_MAX_SIZE data bytes of the received
//                                                                      message. */
//    uint8_t receiveError; /**< Indication of RPDO length errors, use with CO_PDO_receiveErrors_t */
//#if (((CO_CONFIG_PDO)&CO_CONFIG_PDO_SYNC_ENABLE) != 0) || defined CO_DOXYGEN
//    CO_SYNC_t* SYNC;    /**< From CO_RPDO_init() */
//    bool_t synchronous; /**< True if transmissionType <= 240 */
//#endif
//#if (((CO_CONFIG_PDO)&CO_CONFIG_RPDO_TIMERS_ENABLE) != 0) || defined CO_DOXYGEN
//    uint32_t timeoutTime_us; /**< Maximum timeout time between received PDOs in microseconds. Configurable by OD
//                                variable RPDO communication parameter, event-timer. */
//    uint32_t timeoutTimer;   /**< Timeout timer variable in microseconds */
//#endif
//#if (((CO_CONFIG_PDO)&CO_CONFIG_FLAG_CALLBACK_PRE) != 0) || defined CO_DOXYGEN
//    void (*pFunctSignalPre)(void* object); /**< From CO_RPDO_initCallbackPre() or NULL */
//    void* functSignalObjectPre;            /**< From CO_RPDO_initCallbackPre() or NULL */
//#endif
//} CO_RPDO_t;
//CO_RPDO_t* RPDO;
//void pdo_r()
//{
//static ODR_t PDOconfigMap(CO_PDO_common_t* PDO, uint32_t map, uint8_t mapIndex, bool_t isRPDO, OD_t* OD)
//
//
//{0x1400, 0x04, ODT_REC, &ODObjs.o_1400_RPDOCommunicationParameter, NULL},
//	PDOconfigMap(RPDO.PDO_common , 0x1a01, 0, false, OD);
//	PDOconfigMap(TPDO.PDO_common , 0x1a01, 1, false, OD);
//
//	PDO_initMapping(CO_PDO_common_t* PDO, OD_t* OD, OD_entry_t* OD_PDOMapPar, bool_t isRPDO, uint32_t* errInfo,
//	                uint32_t* erroneousMap);
//
//}

