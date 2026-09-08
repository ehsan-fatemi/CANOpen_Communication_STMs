#ifndef CANOPEN_APP_H
#define CANOPEN_APP_H

#include "CO_app_STM32.h"
#include "CANopen.h"
#include "main.h"
#include <stdio.h>

#include "CO_storageBlank.h"
#include "../CANopenNode/example/OD.h"

void CANOpen_App_Init();


extern CANopenNodeSTM32 canOpen;

#endif




