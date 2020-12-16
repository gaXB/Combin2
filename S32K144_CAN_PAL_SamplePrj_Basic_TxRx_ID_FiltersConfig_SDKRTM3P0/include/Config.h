/*
 * Config.h
 *
 *  Created on: Oct 22, 2020
 *      Author: nxa07657
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "Cpu.h"

/**************************************************************************
 * RGB LED configuration for S32K144EVB-Q100
 **************************************************************************/
#define RGB_LED_GPIO        PTD    /*GPIO used for the on-board RGB led control*/
#define RGB_BLUE_LED_PIN    0u     /*PTD0  connected to the blue RGB LED*/
#define RGB_RED_LED_PIN     15u    /*PTD15 connected to the blue RGB LED*/
#define RGB_GREEN_LED_PIN   16u    /*PTD16 connected to the blue RGB LED*/

/*
 * define the MB index for CAN message TX and RX
 * the MB index is the logic MB index instead of classical MB
 *
 * If RxFIFO is enabled: the MB0 will be used by RxFIFO, and it's Buff configuration and ID filter settings
 *  are configured in init() API, user should not call CAN_ConfigRxBuff() and CAN_SetRxFilter() with MB0
 *  the MB0 cannot be used as TX as well.
 *
 *  If RxFIFO is disabled(default setting): all MB are individual to be used as TX or RX
 *
 */
/* For this demo, RxFIFO is enabled, so MB0 is reserved and cannot be assigned here
 * Note: for CAN_PAL here with RxFIFO enabled, the MB is the virtual MB, not actual MB
 */
#define RX_MB1   1         /* define the RX MB1 */
#define RX_MB2   2         /* define the RX MB2 */
#define TX_MB    3         /* define the TX MB  */
/*
 * define the MB index for CAN message TX and RX
 * the MB index is the logic MB index instead of classical MB
 *
 * If RxFIFO is enabled: the MB0 will be used by RxFIFO, and it's Buff configuration and ID filter settings
 *  are configured in init() API, user should not call CAN_ConfigRxBuff() and CAN_SetRxFilter() with MB0
 *  the MB0 cannot be used as TX as well.
 *
 *  If RxFIFO is disabled(default setting): all MB are individual to be used as TX or RX
 *
 */
/* For this demo, RxFIFO is enabled, so MB0 is reserved and cannot be assigned here
 * Note: for CAN_PAL here with RxFIFO enabled, the MB is the virtual MB, not actual MB
 */
#define RX_MB1   1         /* define the RX MB1 */
#define RX_MB2   2         /* define the RX MB2 */
#define TX_MB    3         /* define the TX MB  */

/* define the RxFIFO used standard MB number for RX ID filter configuration */
#define RxFIFO_USED_MB (7 +(can_pal1_rx_fifo_ext0.numIdFilters<<1))

/************************************************************************
* configure the following RX_ACK_ID1/2 and RX_ACK_ID1/2_MASK for the RX MB 
* ID filter:
* Below default configuration to 
*      receive CAN messages with ID = 0x120~0x12F(16 ID in total) with RX MB1
*      receive CAN messages with ID = 0x056,0x156, 0x256 and 0x356(4 ID in total) with RX MB2
*************************************************************************/
#define RX_ACK_ID1          0x123 /*define RX MB1 acceptance message ID*/
#define RX_ACK_ID2          0x256 /*define RX MB2 acceptance message ID*/

/**************************************************************************
 *  define the IDE-bit mask,
 * IDE_MASK_SET: match to receive only standard or extend ID message frames
 * IDE_MASK_CLR: mask and ignore IDE-bit, so both standard and extend ID message
 *               frames will be received
 **************************************************************************/
#define IDE_MASK_SET  (1<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT)
#define IDE_MASK_CLR  (0<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT)

/**************************************************************************
 *  define the IDE-bit mask,
 * RTR_MASK_SET: match to receive only data or remote message frames
 * RTR_MASK_CLR: mask and ignore RTR-bit, so both data and remote message
 *               frames will be received
 **************************************************************************/
#define RTR_MASK_SET  (1<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT)
#define RTR_MASK_CLR  (0<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT)

#define RX_ACK_ID1_MASK     (0x7F0 | IDE_MASK_SET | RTR_MASK_SET) /* mask the lower 4LSB ID */
#define RX_ACK_ID2_MASK     (0x4FF | IDE_MASK_SET | RTR_MASK_SET) /* mask the ID8~ID9 */

#define CAN_RX_Buffer_MAX         2   /*define the RX buffer size*/
#define CAN_RxFIFO_RX_Buffer_MAX  2   /*define the RxFIFO RX buffer size*/

/*
 * define the RxFIFO ID filter table mask, for this demo, configure RxFIFO with Type-B filter:
 *  two 16-bit filer with standard ID
 */
#define RxFIFO_ACK_ID_MASK  0x7FF   /* all ID bits(11 bits for standard message frame) must match with RxFIFO ID table settings  */

/* enable this macro define to disable bus off automatically recovery feature, and user should add bus-off recovery process codes
 * comment this macro will use FlexCAN default configuration of bus off automatic recovery.
 */
//#define CAN_BUSOFF_RECOVERY_MANUAL  1

extern uint32_t CAN_RX1_BufferIndex;    /*buffer index for CAN RX MB1*/
extern uint32_t CAN_RX2_BufferIndex;    /*buffer index for CAN RX MB2*/
extern uint32_t CAN_RxDataBufferIndex; /*the latest CAN message data buffer index */

extern bool CAN_RX1_Completed_Flag; /* CAN RX MB1 flag used for sync between CAN TXRX ISR and application*/
extern bool CAN_RX2_Completed_Flag; /* CAN RX MB2 flag used for sync between CAN TXRX ISR and application*/

extern can_message_t CAN_RX1_Message_Buffer[CAN_RX_Buffer_MAX];/*circle buffer for CAN RX MB1*/
extern can_message_t CAN_RX2_Message_Buffer[CAN_RX_Buffer_MAX];/*circle buffer for CAN RX MB2*/

extern uint32_t CAN_RxFIFO_RX_BufferIndex ;    /*buffer index for CAN RxFIFO RX*/
extern uint32_t CAN_RxFIFO_RxDataBufferIndex; /*the latest RxFIFO CAN message data buffer index */
extern bool CAN_RxFIFO_RX_Completed_Flag; /*flag used for sync between CAN TXRX ISR and application*/
extern can_message_t CAN_RxFIFO_RX_Message_Buffer[CAN_RxFIFO_RX_Buffer_MAX];/*circle buffer for CAN RxFIFO RX*/

extern bool BusOff_Flag; /* the bus off flag */

/* CAN hardware MB(Message Buffer) configuration for TX MB */
extern can_buff_config_t CAN_TX_MB_Config;

/* CAN hardware MB(Message Buffer) configuration for RX MB1 */
extern can_buff_config_t CAN_RX_MB1_Config;
/* CAN hardware MB(Message Buffer) configuration for RX MB2 */
extern can_buff_config_t CAN_RX_MB2_Config;

/* FlexCAN RxFIFO ID filter table */
extern const flexcan_id_table_t RxFIFO_FilterAcceptanceID_Table[16];

/* CAN message transmit buffer*/
extern can_message_t CAN_TX_Buffer;

extern void CAN_User_Init(void);
extern void FLEXCAN_BOFFREC_Config(uint32_t instance, bool enable);
extern void CAN_TX_RX_Callback(uint32_t instance, can_event_t eventType, uint32_t objIdx, void *driverState);
extern void CAN_Error_Callback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState);
extern void CAN_MB_CS_Update(const can_instance_t * const instance, uint32_t buffIdx,uint32_t cs);

#endif /* CONFIG_H_ */
