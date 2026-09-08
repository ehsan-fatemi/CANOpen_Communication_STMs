#ifndef CANOPEN_APP_H
#define CANOPEN_APP_H
#include "CO_app_STM32.h"
#include "can.h"
#include "tim.h"
#include "../CANopenNode/301/CO_NMT_Heartbeat.h"

extern CANopenNodeSTM32 canopen;

void canoepn_app_init();
void nmt_operation_mode_command();
void nmt_heartbeat_command();


#endif
