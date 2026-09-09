#ifndef CANOPEN_APP_H
#define CANOPEN_APP_H
#include "CO_app_STM32.h"
#include "can.h"
#include "tim.h"
#include "../CANopenNode/301/CO_NMT_Heartbeat.h"

//typedef enum
//{
//	SDO_RW_IDLE,
//
//    SDO_READ_IDLE,
//    SDO_READ_RUNNING,
//    SDO_READ_DONE,
//    SDO_READ_ERROR,
//	SDO_READ_COMPLETE,
//
//	SDO_WRITE_IDLE,
//	SDO_WRITE_RUNNING,
//	SDO_WRITE_COMPLETE,
//	SDO_WRITE_ERROR
//
//
//} SDO_RW_State_t;

typedef enum
{
    SDO_RW_IDLE,
    SDO_RW_RUNNING,
    SDO_RW_DONE,
    SDO_RW_ERROR,
	SDO_RW_PRINTED
} SDO_RW_State_t;

typedef struct
{
    bool write;
    bool read;

    CO_SDO_return_t read_return_state;
    CO_SDO_return_t write_return_state;

    SDO_RW_State_t read_state;
    SDO_RW_State_t write_state;

    CO_SDO_abortCode_t read_SDOabortCode;
    CO_SDO_abortCode_t write_SDOabortCode;

} SDO_RW_Control_t;




extern CANopenNodeSTM32 canopen;
extern CO_SDO_return_t sdo_return_state;
extern SDO_RW_State_t SDO_RW_State;
extern SDO_RW_Control_t sdo_rw;

void canoepn_app_init();
void nmt_operation_mode_command();
void sdo_write(uint16_t object_index , uint8_t object_sub_index ,
	       uint8_t object_size , uint8_t *buffer , uint8_t buffer_size , uint8_t node_id);
void sdo_read(uint16_t object_index , uint8_t  object_sub_index, uint8_t *buffer , uint8_t buffer_size , uint8_t node_id);
void sdo_write_return_staus(uint8_t node_id);
void sdo_read_return_staus(uint8_t node_id);





#endif
