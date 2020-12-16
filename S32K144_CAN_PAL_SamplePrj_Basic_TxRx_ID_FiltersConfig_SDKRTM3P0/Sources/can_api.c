/*
 * can_api.c
 *
 *  Created on: Oct 22, 2020
 *      Author: nxa07657
 */

#include "Config.h"

/* include the following header file to use FlexCAN PD layer driver API and data type */
#include "flexcan_driver.h"
/* include the following header file to use FlexCAN hardware access layer driver API and data type */
#include "flexcan_hw_access.h"

uint32_t CAN_RX1_BufferIndex = 0;    /*buffer index for CAN RX MB1*/
uint32_t CAN_RX2_BufferIndex = 0;    /*buffer index for CAN RX MB2*/
uint32_t CAN_RxDataBufferIndex = 0; /*the latest CAN message data buffer index */

bool CAN_RX1_Completed_Flag = false; /* CAN RX MB1 flag used for sync between CAN TXRX ISR and application*/
bool CAN_RX2_Completed_Flag = false; /* CAN RX MB2 flag used for sync between CAN TXRX ISR and application*/

can_message_t CAN_RX1_Message_Buffer[CAN_RX_Buffer_MAX];/*circle buffer for CAN RX MB1*/
can_message_t CAN_RX2_Message_Buffer[CAN_RX_Buffer_MAX];/*circle buffer for CAN RX MB2*/

uint32_t CAN_RxFIFO_RX_BufferIndex = 0;    /*buffer index for CAN RxFIFO RX*/
uint32_t CAN_RxFIFO_RxDataBufferIndex = 0; /*the latest RxFIFO CAN message data buffer index */
bool CAN_RxFIFO_RX_Completed_Flag = false; /*flag used for sync between CAN TXRX ISR and application*/
can_message_t CAN_RxFIFO_RX_Message_Buffer[CAN_RxFIFO_RX_Buffer_MAX];/*circle buffer for CAN RxFIFO RX*/

bool BusOff_Flag = false; /* the bus off flag */

/* CAN hardware MB(Message Buffer) configuration for TX MB */
can_buff_config_t CAN_TX_MB_Config = {
  .enableFD = false,
  .enableBRS = false,
  .fdPadding = 0xCC,
  .idType = CAN_MSG_ID_STD,
  .isRemote = false
 };
 
 /* CAN hardware MB(Message Buffer) configuration for RX MB1 */
can_buff_config_t CAN_RX_MB1_Config = {
  .enableFD = false,
  .enableBRS = false,
  .fdPadding = 0xCC,
  .idType = CAN_MSG_ID_STD,
  .isRemote = false
 };
 
  /* CAN hardware MB(Message Buffer) configuration for RX MB2 */
can_buff_config_t CAN_RX_MB2_Config = {
  .enableFD = false,
  .enableBRS = false,
  .fdPadding = 0xCC,
  .idType = CAN_MSG_ID_STD,
  .isRemote = false
 };

/* CAN message transmit buffer*/
can_message_t CAN_TX_Buffer ={
		.id = 0x100,
		.data = {0},
		.length = 15
};

/*
 * configure the RxFIFO filter acceptance ID element table to receive the follow specified 16 ID messages
 * standard data frame with ID = 0x011,0x022,0x033,0x044,0x055,0x066,0x077,0x088, 0x111,0x222,0x333,0x444,0x555,0x666,0x777 and 0x788
 *
 * user should assign this table to can_pal component's extension idFilterTable member if initialize configuration structure
 * before call can_pal init() API
 *
 * for example
 *
 * ///@brief User configuration structure
 *  can_user_config_t can_pal1_Config0;
 *
 * in application code:
 * {
 *     ....
 * 		//assign the RxFIFIO filter acceptance ID table to rx_fif0 extension of can_pal config structure
 *      can_pal1_rx_fifo_ext0.idFilterTable = (flexcan_id_table_t *)RxFIFO_FilterAcceptanceID_Table;
 *
 * 		// initialize the CAN module
 * 		CAN_Init(&can_pal1_instance, &can_pal1_Config0);
 * 		....
 * }
 */
const flexcan_id_table_t RxFIFO_FilterAcceptanceID_Table[16] =
{
		{    /*RxFIFO ID table element 0*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x011
		},
		{	 /*RxFIFO ID table element 1*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x022
		},
		{    /*RxFIFO ID table element 2*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x033
		},
		{	 /*RxFIFO ID table element 3*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x044
		},
		{	 /*RxFIFO ID table element 4*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x055
		},
		{	 /*RxFIFO ID table element 5*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x066
		},
		{	 /*RxFIFO ID table element 6*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x077
		},
		{	 /*RxFIFO ID table element 7*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x088
		},
		{    /*RxFIFO ID table element 8*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x111
		},
		{	 /*RxFIFO ID table element 9*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x222
		},
		{    /*RxFIFO ID table element 10*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x333
		},
		{	 /*RxFIFO ID table element 11*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x444
		},
		{	 /*RxFIFO ID table element 12*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x555
		},
		{	 /*RxFIFO ID table element 13*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x666
		},
		{	 /*RxFIFO ID table element 14*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x777
		},
		{	 /*RxFIFO ID table element 15*/
			.isRemoteFrame = false,
			.isExtendedFrame = false,
			.id = 0x788
		}
};

/* Table of base addresses for CAN instances. */
static CAN_Type * const g_flexcanBasePtr[] = CAN_BASE_PTRS;

void FLEXCAN_BOFFREC_Config(uint32_t instance, bool enable);

/*******************************************************************
 * The CAN TX and RX completed interrupt ISR callback
 *  1) use the new buffer to continue CAN message receive
 *  2) set the RX_Completed_Flag for application handle
 *
 *******************************************************************/
void CAN_TX_RX_Callback(uint32_t instance, can_event_t eventType, uint32_t objIdx, void *driverState)
{
	/*check and handle the TX complete event*/
	if(CAN_EVENT_TX_COMPLETE==eventType)
	{
		/*add your TX complete event handle codes here if needed*/

	}

	/*check and handle the RX complete event*/
	if(CAN_EVENT_RX_COMPLETE==eventType)
	{
		/* check whether the RxFIFO is enabled */
		if((0==objIdx)&&(NULL != can_pal1_Config0.extension))
		{
			CAN_RxFIFO_RX_BufferIndex++; /*increase the RxFIFO RX buffer index*/

			/*use next RxFIFO circle buffer for the new CAN message receive*/
			CAN_Receive(&can_pal1_instance,objIdx,&CAN_RxFIFO_RX_Message_Buffer[CAN_RxFIFO_RX_BufferIndex%CAN_RxFIFO_RX_Buffer_MAX]);

			CAN_RxFIFO_RX_Completed_Flag = true;/*set the RxFIFO RX completed flag*/
		}
		else
		{
			switch (objIdx)
			{
				case RX_MB1:
					{
						CAN_RX1_BufferIndex++; /*increase the RX buffer index*/

						/*use next circle buffer for the new CAN message receive*/
						CAN_Receive(&can_pal1_instance,objIdx,&CAN_RX1_Message_Buffer[CAN_RX1_BufferIndex%CAN_RX_Buffer_MAX]);

						CAN_RX1_Completed_Flag = true;/*set the RX completed flag*/

						break;
					}
				case RX_MB2:
					{
						CAN_RX2_BufferIndex++; /*increase the RX buffer index*/

						/*use next circle buffer for the new CAN message receive*/
						CAN_Receive(&can_pal1_instance,objIdx,&CAN_RX2_Message_Buffer[CAN_RX2_BufferIndex%CAN_RX_Buffer_MAX]);

						CAN_RX2_Completed_Flag = true;/*set the RX completed flag*/

						break;
					}
				default: break;
			}
		}
	}
}
/*************************************************************************************
 * the CAN error ISR callback
 * user can add handle code for :
 *
 * FlexCAN Error, Error Fast, Tx Warning, Rx Warning and Bus Off as well as Bus Off Done event
 *
 * Note: Here only add the bus-off recovery code for demonstration
 *************************************************************************************/
void CAN_Error_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState)
{
	/* temp variable to store ESR1 register value */
	uint32_t temp_ESR1_RegVal = 0;

	DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

	CAN_Type * base = g_flexcanBasePtr[instance];

	if(FLEXCAN_EVENT_ERROR==eventType)
	{
		/*get the error flags for further process */
		temp_ESR1_RegVal = FLEXCAN_DRV_GetErrorStatus(instance);


		if(CAN_ESR1_BOFFINT_MASK&temp_ESR1_RegVal)
		{
			/* record the bus off event */
			BusOff_Flag = true;

#ifdef CAN_BUSOFF_RECOVERY_MANUAL
			/* abort current MB transmit */
			CAN_AbortTransfer(&can_pal1_instance,TX_MB);
#endif  /* end of CAN_BUSOFF_RECOVERY_MANUAL */

			/* use can add their network management handle codes here  */
			PINS_DRV_ClearPins(RGB_LED_GPIO,(1<<RGB_GREEN_LED_PIN)); /*turn ON green RGB LED*/

			/*get the error flags again in case bus-off recovery have done */
			temp_ESR1_RegVal = FLEXCAN_DRV_GetErrorStatus(instance);
		}

		if(CAN_ESR1_BOFFDONEINT_MASK&temp_ESR1_RegVal)
		{
#ifdef CAN_BUSOFF_RECOVERY_MANUAL
			/* the bus off recovery condition(the Tx Error Counter (TXERRCNT) has finished counting 128 occurrences of 11 consecutive recessive bits on the CAN bus and is ready to leave Bus Off) has been detected*/
			/*
			 * clean BOFFREC bit to trigger bus off recovery
			 */
			FLEXCAN_BOFFREC_Config(instance, false);
#endif  /* end of CAN_BUSOFF_RECOVERY_MANUAL */

			/* wait for the FlexCAN has actually exited bus-off  */
  			while((base->ESR1&CAN_ESR1_FLTCONF_MASK)>=CAN_ESR1_FLTCONF(0x2));

			PINS_DRV_SetPins(RGB_LED_GPIO,(1<<RGB_GREEN_LED_PIN)); /*turn OFF green RGB LED*/

			BusOff_Flag = false; /* clean the flag */

#ifdef CAN_BUSOFF_RECOVERY_MANUAL
			/* disable automatic bus-off recovery again */
			FLEXCAN_BOFFREC_Config(instance, true);
#endif  /* end of CAN_BUSOFF_RECOVERY_MANUAL */

		}

	}
}
/*************************************************************
 * API to configure the FlexCAN bus off recovery method
 *
 * enable = true:  disable automatic recovering from Bus Off state
 * enable = false: enable automatic recovering from Bus Off state
 *                 it is also a MUST for user recovery
 *
 *************************************************************/
void FLEXCAN_BOFFREC_Config(uint32_t instance, bool enable)
{
	DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

	CAN_Type * base = g_flexcanBasePtr[instance];
	FLEXCAN_EnterFreezeMode(base);


	if(true==enable)
	{
		/* set BOFFREC bit to disable automatic bus-off recovery */
		base->CTRL1 |= CAN_CTRL1_BOFFREC(enable);

		/* enable buss off done interrupt */
		base->CTRL2 |= CAN_CTRL2_BOFFDONEMSK(enable);
	}
	else
	{
		/* clean BOFFREC bit to enable automatic bus-off recovery */
		base->CTRL1 &= (uint32_t)(~CAN_CTRL1_BOFFREC_MASK);
		
		/* clean the bus-off done interrupt flag */
  		base->ESR1 = CAN_ESR1_BOFFDONEINT_MASK;
	}

	FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CAN_MB_CS_Update
 * Description   : This function update code and status configuration of
 *                 the specified FlexCAN module TX MB
 *  instance: CAN_PAL instance;
 *  buffIdx : TX MB index
 *  cs      : code and status input
 *
 * Implements    : CAN_MB_CS_Update_Activity
 *END**************************************************************************/
void CAN_MB_CS_Update(const can_instance_t * const instance, uint32_t buffIdx, uint32_t cs)
{
	/* get the CAN-FD enable configuration */
	CAN_TX_MB_Config.enableFD = (cs & CAN_MB_EDL_MASK)? true: false;

	/* get the bitrate switch enable configuration */
	CAN_TX_MB_Config.enableBRS = (cs & CAN_MB_BRS_MASK)? true: false;

	/* get the ID type configuration */
	CAN_TX_MB_Config.idType = (cs & CAN_CS_IDE_MASK)? CAN_MSG_ID_EXT: CAN_MSG_ID_STD;

	/* get the remote/data frame configuration */
	CAN_TX_MB_Config.isRemote = (cs & CAN_CS_RTR_MASK)?true: false;

	/* call CAN_PAL TX buffer configure API to update the CAN_PAL configuration */
	CAN_ConfigTxBuff(instance, buffIdx, &CAN_TX_MB_Config);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CAN_User_Init
 * Description   : This function complete the CAN initialization
 *
 * Notes         : Please add the CAN initialization codes per requirements
 *
 * Implements    : CAN_User_Init_Activity
 *END**************************************************************************/
void CAN_User_Init(void)
{
	uint32_t i = 0;
	
	/* assign the RxFIFIO filter acceptance ID table to rx_fif0 extension of can_pal config structure */
    can_pal1_rx_fifo_ext0.idFilterTable = (flexcan_id_table_t *)RxFIFO_FilterAcceptanceID_Table;
	
	/* initialize the CAN module */
	CAN_Init(&can_pal1_instance, &can_pal1_Config0);
	
	/*configure the RxFIFO ID filter table mask*/
   for(i=0; i<RxFIFO_USED_MB; i++)
   {
	   CAN_SetRxFilter(&can_pal1_instance,CAN_MSG_ID_STD,i,RxFIFO_ACK_ID_MASK);
   }

	/* install the TX/RX call back */
	CAN_InstallEventCallback(&can_pal1_instance,CAN_TX_RX_Callback, NULL);

#ifdef CAN_BUSOFF_RECOVERY_MANUAL
	/* disable bus off automatic recovery */
	FLEXCAN_BOFFREC_Config(can_pal1_instance.instIdx, true);
#endif

	/* install the FlexCAN error ISR callback, which will also enable error, TX warning, RX warning and bus-off interrupt */
	FLEXCAN_DRV_InstallErrorCallback(can_pal1_instance.instIdx, CAN_Error_Callback,NULL);

   /* configure the transmit MB */
   CAN_ConfigTxBuff(&can_pal1_instance,TX_MB, &CAN_TX_MB_Config);

   /* configure the receive MB1 with ACK ID for ID filter */
   CAN_ConfigRxBuff(&can_pal1_instance,RX_MB1, &CAN_RX_MB1_Config, RX_ACK_ID1);
   /* configure the RX MB1 ID filter, use individual with 4 LSB ID masked */
   CAN_SetRxFilter(&can_pal1_instance,CAN_MSG_ID_STD,(RX_MB1+RxFIFO_USED_MB),RX_ACK_ID1_MASK);
   
   /* configure the receive MB2 with ACK ID for ID filter */
   CAN_ConfigRxBuff(&can_pal1_instance,RX_MB2, &CAN_RX_MB2_Config, RX_ACK_ID2);
   /* configure the RX MB2 ID filter, use individual with ID8~ID9 ID masked */
   CAN_SetRxFilter(&can_pal1_instance,CAN_MSG_ID_STD,(RX_MB2+RxFIFO_USED_MB),RX_ACK_ID2_MASK);
}
