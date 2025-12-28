#include "net_dhcp.h"

#include <stdlib.h>
#include <string.h>
#include "net_if.h"
#include "W5500_core.h"

static uint8_t dhcp_buf[DHCP_BUF_SIZE]; // اندازه کافی برای BOOTP + Cookie + Options
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
	// 1. init MAC
	netif_init_mac();
	// 2. init socket UDP (port 68)
	W5500_SocketInit(0/*socket*/, 68/*port*/,W5500_SN_MR_P_UDP/*protocol*/);
	// 3. clear DHCP context
	srand(SysTick->VAL ^ UID_WORD0 ^ UID_WORD1 ^ UID_WORD2);				// turn on random cereator 
	dhcp_clear_context();
	// 4. go to DISCOVER
	dhcp_state = DHCP_STATE_DISCOVER;
}
static void init_dhcp_discover_buf(uint8_t* discover){
	memset(discover, 0, DHCP_BUF_SIZE);
	
	const uint8_t* mac = netif_get_mac();
	
	discover[DHCP_OP] 								= 0x01; 												// BOOTREQUEST
	
	discover[DHCP_HTYPE] 							= 0x01; 												// Ethernet
	
	discover[DHCP_HLEN] 							= 0x06; 												// MAC LENGTH
	
	discover[DHCP_HOPS] 							= 0x00;													// relay count
	
	discover[DHCP_XID] 								= (dhcp_ctx.xid >> 24) & 0xFF; 	// upper byte of random number
	discover[DHCP_XID + 1] 						= (dhcp_ctx.xid >> 16) & 0xFF; 	//
	discover[DHCP_XID + 2] 						= (dhcp_ctx.xid >> 8)  & 0xFF; 	//
	discover[DHCP_XID + 3] 						= dhcp_ctx.xid 				 & 0xFF; 	// lower byte of random number
	
	discover[DHCP_FLAGS]							= 0x80;													// Broadcast flag
	discover[DHCP_FLAGS + 1]							= 0x00;
	
	memcpy(&discover[DHCP_CHADDR], mac, 6);
	
	discover[DHCP_MAGIC_COOOKIE] 			= 0x63;
	discover[DHCP_MAGIC_COOOKIE + 1] 	= 0x82;
	discover[DHCP_MAGIC_COOOKIE + 2] 	= 0x53;
	discover[DHCP_MAGIC_COOOKIE + 3] 	= 0x63;
	
	uint16_t opt = DHCP_OPTIONS;

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
	dhcp_ctx.bufLen = opt;
}
void DHCP_DISCOVER(void){
	// 1. build DHCP DISCOVER packet
	init_dhcp_discover_buf(dhcp_buf);
	// 2. send broadcast to 255.255.255.255:67
	uint8_t server_ip[4] = { 255 ,255 ,255 ,255};
	W5500_Set_UDP_Destination(0,server_ip,67);
	// 3. start timeout timer
	dhcp_ctx.last_tick = HAL_GetTick();
	// 4. go to WAIT_OFFER
	dhcp_state = DHCP_STATE_WAIT_OFFER;
}
static uint8_t* get_dhcp_option(uint8_t* buf, uint16_t buf_len, uint8_t target_option){
	// ۱. رفتن به شروع آپشن‌ها (بعد از Magic Cookie)
	uint16_t curr = DHCP_OPTIONS;
	// ۲. حرکت در بافر تا رسیدن به انتهای پکت یا پیدا کردن آپشن
	while (curr < buf_len) {
		uint8_t opt_code = buf[curr];
		// اگر به کد ۲۵۵ رسیدیم، یعنی آپشن‌ها تمام شد
		if (opt_code == 0xFF){
			return NULL;
		}
		// اگر کد صفر بود (Padding)، فقط یک بایت جلو برو
		if (opt_code == 0x00) {
			curr++;
		}
		else{
			uint8_t opt_len = buf[curr + 1];
			// ۳. بررسی اینکه آیا این همان آپشن هدف ماست؟
			if (opt_code == target_option) {
				return &buf[curr + 2]; // آدرسِ بخش Value را برمی‌گردانیم
			}
			// ۴. پرش به آپشن بعدی
			curr += (2 + opt_len);
		}
	}
	return NULL; // اگر پیدا نشد
}
void DHCP_WAIT_OFFER(void){
	
	int len;
	// 1. check timeout
	if (HAL_GetTick() - dhcp_ctx.last_tick > 3000) {
		dhcp_state = DHCP_STATE_RETRY;
		return;
	}
	// 2. check if packet received
	len = W5500_Recv_UDP(0, dhcp_buf, DHCP_BUF_SIZE);
	if (len <= 0) {
		return; // هنوز چیزی نیومده
	}
	// 3. minimal length check
	if (len < DHCP_OPTIONS) {
		return;
	}
	// 4. OP must be BOOTREPLY
	if (dhcp_buf[DHCP_OP] != 0x02) {
		return;
	}
	// 5. check XID
	uint32_t rx_xid = (dhcp_buf[DHCP_XID] << 24) | (dhcp_buf[DHCP_XID + 1] << 16) | (dhcp_buf[DHCP_XID + 2] << 8) | (dhcp_buf[DHCP_XID + 3]);
	if (rx_xid != dhcp_ctx.xid) {
		return;
	}
	// 6. check CHADDR
	const uint8_t* mac = netif_get_mac();
	if (memcmp(&dhcp_buf[DHCP_CHADDR], mac, 6) != 0) {
		return;
	}
	// 7. check magic cookie
	if (dhcp_buf[DHCP_MAGIC_COOOKIE]     != 0x63 || dhcp_buf[DHCP_MAGIC_COOOKIE + 1] != 0x82 ||
			dhcp_buf[DHCP_MAGIC_COOOKIE + 2] != 0x53 || dhcp_buf[DHCP_MAGIC_COOOKIE + 3] != 0x63){
		return;
	}
	// 8. parse options
	// - if message type == OFFER: (check option 53)
	uint8_t* opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,53);
	
	if((opt_val == NULL) || (*opt_val != 0x02)){
		return;
	}
	// 	- save YIADDR as offered_ip.
	memcpy(dhcp_ctx.offered_ip,&dhcp_buf[DHCP_YIADDR],4);
	//  - save Option 54 as server_ip
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,54);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.server_ip,opt_val,4);
	}
	//	- save Option 1 as Subnet mask
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,1);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.subnet,opt_val,4);
	}
	//	- save Option 3 as Gateway
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,3);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.gateway,opt_val,4);
	}
	//	- save Option 6 as DNS
	
	//  - go to REQUEST
	// - if timeout -> retry DISCOVER

	// 11. go to REQUEST
	dhcp_state = DHCP_STATE_REQUEST;
}
static void init_dhcp_request_buf(uint8_t* buf){
	
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
void DHCP_RETRY(void){
	
}



