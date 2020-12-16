/* ###################################################################
**     Filename    : main.c
**     Processor   : S32K1xx
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */

/* Including necessary module. Cpu.h contains other modules needed for compiling.*/
#include "Cpu.h"

volatile int exit_code = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */

#include "Config.h"

/*! 
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
  /* Write your local variable definition here */

	uint32_t i = 0;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

	/* Initialize and configure clocks */
	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
						g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

	/* Initialize pins */
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

	CAN_User_Init();/* initialize the CAN module as needed */

	/* send a message for test at first */
	CAN_SendBlocking(&can_pal1_instance, TX_MB, &CAN_TX_Buffer, 1000);

	/* start the RxFIFO CAN message receive */
	CAN_Receive(&can_pal1_instance,0,&CAN_RxFIFO_RX_Message_Buffer[CAN_RxFIFO_RX_BufferIndex]);

	/* start the individual MB1 CAN message receive */
	CAN_Receive(&can_pal1_instance,RX_MB1,&CAN_RX1_Message_Buffer[CAN_RX1_BufferIndex]);
	/* start the individual MB1 CAN message receive */
	CAN_Receive(&can_pal1_instance,RX_MB2,&CAN_RX2_Message_Buffer[CAN_RX2_BufferIndex]);

   while(1)
   {
   		/* check whether CAN RX is completed, and feedback the received CAN message */
   		if(true==CAN_RX1_Completed_Flag)
   		{
   			CAN_RX1_Completed_Flag = false;/* clean the flag */

   			/* the new receive data stored in previous RX buffer, get its index */
   			CAN_RxDataBufferIndex = (CAN_RX1_BufferIndex - 1)%CAN_RX_Buffer_MAX;

   			/* update TX MB to use the received cs */
   			CAN_MB_CS_Update(&can_pal1_instance, TX_MB, CAN_RX1_Message_Buffer[CAN_RxDataBufferIndex].cs);

   			/* send it back via CAN */
   			CAN_SendBlocking(&can_pal1_instance, TX_MB, &CAN_RX1_Message_Buffer[CAN_RxDataBufferIndex], 1000);

   			/* toggle the blue RGB LED */
   			PINS_DRV_TogglePins(RGB_LED_GPIO,(1<<RGB_BLUE_LED_PIN));
   		}
		
		/* check whether CAN RX is completed, and feedback the received CAN message */
   		if(true==CAN_RX2_Completed_Flag)
   		{
   			CAN_RX2_Completed_Flag = false;/* clean the flag */

   			/* the new receive data stored in previous RX buffer, get its index */
   			CAN_RxDataBufferIndex = (CAN_RX2_BufferIndex - 1)%CAN_RX_Buffer_MAX;

   			/* update TX MB to use the received cs */
   			CAN_MB_CS_Update(&can_pal1_instance, TX_MB, CAN_RX2_Message_Buffer[CAN_RxDataBufferIndex].cs);

   			/* send it back via CAN */
   			CAN_SendBlocking(&can_pal1_instance, TX_MB, &CAN_RX2_Message_Buffer[CAN_RxDataBufferIndex], 1000);

   			/* toggle the blue RGB LED */
   			PINS_DRV_TogglePins(RGB_LED_GPIO,(1<<RGB_BLUE_LED_PIN));
   		}

   		/* check whether CAN RxFIFO RX is completed, and feedback the received CAN message */
		if(true==CAN_RxFIFO_RX_Completed_Flag)
		{
			CAN_RxFIFO_RX_Completed_Flag = false;/* clean the flag */

			/* the new receive data stored in previous RX buffer, get its index */
			CAN_RxFIFO_RxDataBufferIndex = (CAN_RxFIFO_RX_BufferIndex - 1)%CAN_RxFIFO_RX_Buffer_MAX;

			/* update TX MB to use the received cs */
			CAN_MB_CS_Update(&can_pal1_instance, TX_MB, CAN_RxFIFO_RX_Message_Buffer[CAN_RxFIFO_RxDataBufferIndex].cs);

			/* send it back via CAN */
			CAN_SendBlocking(&can_pal1_instance, TX_MB, &CAN_RxFIFO_RX_Message_Buffer[CAN_RxFIFO_RxDataBufferIndex], 1000);

			/* toggle the red RGB LED */
			PINS_DRV_TogglePins(RGB_LED_GPIO,(1<<RGB_RED_LED_PIN));
		}

		/* restore the TX MB configure to send standard ID */
		CAN_TX_MB_Config.idType = CAN_MSG_ID_STD;
		CAN_ConfigTxBuff(&can_pal1_instance,TX_MB, &CAN_TX_MB_Config);
		
		(*(uint64_t *)&CAN_TX_Buffer.data[0])++;/* increase the data */
		
		/* periodical send the response message with increased data  */
   		CAN_SendBlocking(&can_pal1_instance, TX_MB, &CAN_TX_Buffer, 1000);

   		OSIF_TimeDelay(5);/* delay 5ms */
   	}

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the NXP S32K series of microcontrollers.
**
** ###################################################################
*/
