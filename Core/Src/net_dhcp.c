#include "net_dhcp.h"

#include <stdlib.h>
#include <string.h>
#include "net_if.h"
#include "W5500_core.h"

static uint16_t buf_len = 0;
static uint8_t dhcp_buf[DHCP_BUF_SIZE]; // اندازه کافی برای BOOTP + Cookie + Options
static uint8_t offered_ip[4];
static uint8_t server_ip[4];
volatile dhcp_state_t dhcp_state = DHCP_STATE_INIT;
static dhcp_ctx_t dhcp_ctx;

static uint32_t generate_xid(void){
    uint32_t rand1 = (uint32_t)rand();
    uint32_t rand2 = (uint32_t)rand();
    
    // Combine two 15-bit or 16-bit rand() results into one 32-bit number
    // The exact method depends on your compiler's RAND_MAX value.
    // If RAND_MAX is 32767 (0x7FFF), you need more calls to fill 32 bits.
    // A common, if "hacky", approach:
    uint32_t combined = ((rand1 & 0xFFFF) << 16) | (rand2 & 0xFFFF);
    
    return combined;
}
static void dhcp_clear_context(void){
	memset(&dhcp_ctx, 0, sizeof(dhcp_ctx));

	// xid باید جدید باشه
	dhcp_ctx.xid = generate_xid();

	dhcp_ctx.retry_count = 0;
}
void DHCP_INIT(void){
	// - init MAC
	netif_init_mac();
	// - init socket UDP (port 68)
	W5500_SocketInit(0/*socket*/, 68/*port*/,W5500_SN_MR_P_UDP/*protocol*/);
	// - clear DHCP context
	dhcp_clear_context();
	// - go to DISCOVER
	dhcp_state = DHCP_STATE_DISCOVER;
}
void DHCP_DISCOVER(void){
	// - build DHCP DISCOVER packet
	// - send broadcast to 255.255.255.255:67
	// - start timeout timer
	// - go to WAIT_OFFER
}
void DHCP_WAIT_OFFER(void){
	// - wait for UDP packet
	// - check BOOTREPLY
	// - check XID
	// - check CHADDR
	// - parse options
	// - if message type == OFFER:
	//     - save YIADDR as offered_ip
	//     - save Option 54 as server_ip
	//     - go to REQUEST
	// - if timeout -> retry DISCOVER
}
void DHCP_REQUEST(void){
	// - build DHCP REQUEST
	// - Option 50 = offered_ip
	// - Option 54 = server_ip
	// - send broadcast or unicast
	// - start timeout
	// - go to WAIT_ACK
}
void DHCP_WAIT_ACK(void){
	// - wait for UDP packet
	// - check BOOTREPLY
	// - check XID
	// - parse options
	// - if message type == ACK:
	//     - extract subnet, gateway, dns
	//     - configure network stack
	//     - go to BOUND
	// - if NAK:
	//     - go to DISCOVER
}
void DHCP_BOUND(void){
	// - network is usable
	// - start lease timer (optional)
	// - normal operation
}
void DHCP_ERROR(void){
	
}


static void init_dhcp_discover_buf(uint8_t* discover){
	memset(discover, 0, DHCP_BUF_SIZE);
	
	const uint8_t* mac = netif_get_mac();
	
	discover[OFFSET_OP] 								= 0x01; 										// BOOTREQUEST
	
	discover[OFFSET_HTYPE] 							= 0x01; 										// Ethernet
	
	discover[OFFSET_HLEN] 							= 0x06; 										// MAC LENGTH
	
	srand(SysTick->VAL ^ UID_WORD0 ^ UID_WORD1 ^ UID_WORD2);				// turn on random cereator 
	discover[OFFSET_XID] 								= (dhcp_ctx.xid >> 24) & 0xFF; // upper byte of random number
	discover[OFFSET_XID + 1] 						= (dhcp_ctx.xid >> 16) & 0xFF; //
	discover[OFFSET_XID + 2] 						= (dhcp_ctx.xid >> 8)  & 0xFF; //
	discover[OFFSET_XID + 3] 						= dhcp_ctx.xid 				 & 0xFF; // lower byte of random number
	
	discover[OFFSET_FLAGS]							= 0x80;											// Broadcast flag
	
	memcpy(&discover[OFFSET_CHADDR], mac, 6);
	
	discover[OFFSET_MAGIC_COOOKIE] 			= 0x63;
	discover[OFFSET_MAGIC_COOOKIE + 1] 	= 0x82;
	discover[OFFSET_MAGIC_COOOKIE + 2] 	= 0x53;
	discover[OFFSET_MAGIC_COOOKIE + 3] 	= 0x63;
	
	uint16_t opt = OFFSET_MAGIC_COOOKIE + 4;

	/* Option 53: DHCP Message Type = Discover */
	discover[opt++] = 53;
	discover[opt++] = 1;
	discover[opt++] = 1;

	/* Option 55: Parameter Request List */
	discover[opt++] = 55;
	discover[opt++] = 3;
	discover[opt++] = 1;   // Subnet Mask
	discover[opt++] = 3;   // Router
	discover[opt++] = 6;   // DNS

	/* Option 61: Client Identifier */
	discover[opt++] = 61;
	discover[opt++] = 7;
	discover[opt++] = 1;   // Ethernet
	memcpy(&discover[opt], mac, 6);
	opt += 6;

	/* End */
	discover[opt++] = 255;
	buf_len = opt;
}
static void dhcp_offer_check(uint8_t* offer){
	if(offer[OFFSET_OP] != 2){
		return;
	}
	
	const uint8_t* mac = netif_get_mac();
	memcmp(&offer[OFFSET_CHADDR],mac,6);
}