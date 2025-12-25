#include "net_dhcp.h"

#include <stdint.h>
#include <stdlib.h>

static void set_mac(uint8_t* macArr){
	macArr[0] = 0x02;												// local mac
	macArr[1] = 0x01;												// uint adder example 0x01 for security uint
	macArr[2] = 0x01;												// number of Device
	macArr[3] = (uint8_t)(UID_WORD0 & 0xFF);// number of Device
	macArr[4] = (uint8_t)((UID_WORD1 >> 8) & 0xFF);												// number of Device
	macArr[5] = (uint8_t)(UID_WORD2 & 0xFF);													// number of Device
}

static uint8_t dhcp_discover[DHCP_BUF_SIZE]; // اندازه کافی برای BOOTP + Cookie + Options

static void reset_dhcp_discover_buf(uint8_t* discover){
	uint16_t len = DHCP_BUF_SIZE;
	while(len-- > 0){
		*discover++ = 0;
	}
}
static uint32_t generate_32bit_random(void) {
    uint32_t rand1 = (uint32_t)rand();
    uint32_t rand2 = (uint32_t)rand();
    
    // Combine two 15-bit or 16-bit rand() results into one 32-bit number
    // The exact method depends on your compiler's RAND_MAX value.
    // If RAND_MAX is 32767 (0x7FFF), you need more calls to fill 32 bits.
    // A common, if "hacky", approach:
    uint32_t combined = ((rand1 & 0xFFFF) << 16) | (rand2 & 0xFFFF);
    
    return combined;
}
static void init_dhcp_discover_buf(uint8_t* discover){
	reset_dhcp_discover_buf(dhcp_discover);
	discover[OFFSET_OP] 								= 0x01; 		//BOOTREQUEST
	discover[OFFSET_HTYPE] 							= 0x01; 		//Ethernet
	discover[OFFSET_HLEN] 							= 0x06; 		//MAC LENGTH
	srand(SysTick->VAL);
	uint32_t randomNum 									= generate_32bit_random();
	discover[OFFSET_XID] 								= randomNum & 0xFF; 					//
	discover[OFFSET_XID + 1] 						= (randomNum >> 8) & 0xFF; 		//
	discover[OFFSET_XID + 2] 						= (randomNum >> 16) & 0xFF; 	//
	discover[OFFSET_FLAGS + 1]					= 0x80;
	set_mac(mac_addr);
	discover[OFFSET_CHADDR]							= mac_addr[0];
	discover[OFFSET_CHADDR + 1]					= mac_addr[1];
	discover[OFFSET_CHADDR + 2]					= mac_addr[2];
	discover[OFFSET_CHADDR + 3]					= mac_addr[3];
	discover[OFFSET_CHADDR + 4]					= mac_addr[4];
	discover[OFFSET_CHADDR + 5]					= mac_addr[5];
	discover[OFFSET_MAGIC_COOOKIE] 			= 0x63;
	discover[OFFSET_MAGIC_COOOKIE + 1] 	= 0x82;
	discover[OFFSET_MAGIC_COOOKIE + 2] 	= 0x53;
	discover[OFFSET_MAGIC_COOOKIE + 3] 	= 0x63;
	discover[OFFSET_OPTION53] 					= 0x35;
	discover[OFFSET_OPTION53 + 1] 			= 0x01;
	discover[OFFSET_OPTION53 + 2] 			= 0x01;
	discover[OFFSET_OPTION55] 					= 0x37;
	discover[OFFSET_OPTION55 + 1] 			= 0x03;
	discover[OFFSET_OPTION55 + 2] 			= 0x01;
	discover[OFFSET_OPTION55 + 3] 			= 0x03;
	discover[OFFSET_OPTION55 + 4] 			= 0x06;
	discover[OFFSET_OPTION61] 					= 0x3D;
	discover[OFFSET_OPTION61 + 1] 			= 0x07;
	discover[OFFSET_OPTION61 + 2] 			= 0x01;
	discover[OFFSET_OPTION61 + 3]				= mac_addr[0];
	discover[OFFSET_OPTION61 + 4]				= mac_addr[1];
	discover[OFFSET_OPTION61 + 5]				= mac_addr[2];
	discover[OFFSET_OPTION61 + 6]				= mac_addr[3];
	discover[OFFSET_OPTION61 + 7]				= mac_addr[4];
	discover[OFFSET_OPTION61 + 8]				= mac_addr[5];
	discover[OFFSET_OPTION255]					= 0xFF;
}