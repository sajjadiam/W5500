#ifndef NET_DHCP_H
#define NET_DHCP_H
#include <stm32f1xx.h>


// Pointers to the three 32-bit registers
#define UID_WORD0 							(*(volatile uint32_t *) (UID_BASE))
#define UID_WORD1 							(*(volatile uint32_t *) (UID_BASE + 0x04))
#define UID_WORD2 							(*(volatile uint32_t *) (UID_BASE + 0x08))


#define DHCP_BUF_SIZE						300
#define OFFSET_OP								0		// 1 byte
#define OFFSET_HTYPE						1		// 1 byte
#define OFFSET_HLEN							2		// 1 byte
#define OFFSET_HOPS							3		// 1 byte 
#define OFFSET_XID							4		// 4 byte 
#define OFFSET_SECS							8		// 2 byte 
#define OFFSET_FLAGS						10	// 2 byte 
#define OFFSET_CIADDR						12	// 4 byte 
#define OFFSET_YIADDR						16	// 4 byte 
#define OFFSET_SIADDR						20	// 4 byte 
#define OFFSET_GIADDR						24	// 4 byte 
#define OFFSET_CHADDR						28	// 6 byte mac
#define OFFSET_PADDING_CHADDR		34	// until 43
#define OFFSET_SNAME						44	// until 107
#define OFFSET_FILE							108 // until 235
#define OFFSET_MAGIC_COOOKIE		236	// 4 byte
#define OFFSET_OPTION53					240	// 3 byte  Option 53 	– Message Type
#define OFFSET_OPTION55					243	// 5 byte  Option 55 	– Parameter Request List
#define OFFSET_OPTION61					248	// 9 byte  Option 61 	– Client Identifier
#define OFFSET_OPTION255				257	// 1 byte  Option 255	– End

extern unsigned char mac_addr[6];
void DHCP_Init(unsigned char socket_num); //(اختصاص یک سوکت خاص به DHCP)
void DHCP_Run(void); //(باید در حلقه اصلی برنامه مدام صدا زده شود تا تایم‌اوت‌ها را چک کند)
void DHCP_ParseMessage(void); //(تحلیل پکت دریافتی از سرور)
#endif // NET_DHCP_H