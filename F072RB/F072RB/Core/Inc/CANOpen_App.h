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
    SDO_RW_COMPLETE
} SDO_RW_State_t;

typedef struct
{
    bool write;
    bool read;

    SDO_RW_State_t read_state;
    SDO_RW_State_t write_state;

} SDO_RW_Control_t;




extern CANopenNodeSTM32 canopen;
extern CO_SDO_return_t sdo_return_state;
extern SDO_RW_State_t SDO_RW_State;
extern SDO_RW_Control_t sdo_rw;

void canoepn_app_init();
void nmt_operation_mode_command();
void sdo_write();
void sdo_read();





#endif
