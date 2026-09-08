#include "CANOpen_app.h"
#include "tim.h"
#include "can.h"
#include "CO_app_STM32.h"

CANopenNodeSTM32 canOpen;


void CANOpen_App_Init()
{
//	canOpen.timerHandle  	= &htim17;
//
//	canOpen.CANHandle 		= &hcan;
//	canOpen.desiredNodeID 	= 0x01;
////	canOpen.activeNodeID 	= 0x02;
//	canOpen.baudrate 		= 1000;

//	canopen_app_init(&canOpen);


	canOpen.CANHandle = &hcan;
	canOpen.HWInitFunction = MX_CAN_Init;
	canOpen.timerHandle = &htim17;
	canOpen.desiredNodeID = 0x05;
	canOpen.baudrate = 1000;
    canopen_app_init(&canOpen);
}












