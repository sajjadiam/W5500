#include "net_dhcp.h"

#include <stdlib.h>
#include <string.h>
#include "net_if.h"
#include "W5500_core.h"


static dhcp_ctx_t dhcp_ctx;
DHCP_Func_t dhcp_sm[DHCP_STATE_END] = {
	[DHCP_STATE_INIT 			] = DHCP_INIT				,			
	[DHCP_STATE_DISCOVER	]	= DHCP_DISCOVER		,
	[DHCP_STATE_WAIT_OFFER]	= DHCP_WAIT_OFFER	,
	[DHCP_STATE_REQUEST		] = DHCP_REQUEST		,		
	[DHCP_STATE_WAIT_ACK	]	= DHCP_WAIT_ACK		,
	[DHCP_STATE_BOUND			] = DHCP_BOUND			,			
	[DHCP_STATE_RETRY			] = DHCP_RETRY			,			
	[DHCP_STATE_FAIL			]	= DHCP_FAIL				,
	[DHCP_STATE_IDLE			]	= DHCP_IDLE
};
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
	W5500_SocketInit(DHCP_SOCK/*socket*/, BROADCAST_PORT/*port*/,W5500_SN_MR_P_UDP/*protocol*/);
	// 3. clear DHCP context
	srand(SysTick->VAL ^ UID_WORD0 ^ UID_WORD1 ^ UID_WORD2);				// turn on random cereator 
	dhcp_clear_context();
	// 4. go to DISCOVER
	dhcp_ctx.state = DHCP_STATE_DISCOVER;
	
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
	discover[DHCP_FLAGS + 1]					= 0x00;
	
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
	/* Option 57: Maximum DHCP Message Size */
	discover[opt++] = 57;
	discover[opt++] = 2;
	discover[opt++] = (DHCP_BUF_SIZE >> 8) & 0xFF;
	discover[opt++] = DHCP_BUF_SIZE & 0xFF;
	/* End */
	discover[opt++] = 255;
	dhcp_ctx.bufLen = opt;
}
void DHCP_DISCOVER(void){
	// 1. build DHCP DISCOVER packet
	uint8_t discover[DHCP_BUF_SIZE] = {0}; // اندازه کافی برای BOOTP + Cookie + Options
	init_dhcp_discover_buf(discover);
	// 2. send broadcast to 255.255.255.255:67
	uint8_t broadcast_ip[4] = { 255 ,255 ,255 ,255};
	W5500_Set_UDP_Destination(DHCP_SOCK,broadcast_ip,BROADCAST_DPORT);
	W5500_Send_UDP(DHCP_SOCK,discover,dhcp_ctx.bufLen);
	// 3. start timeout timer
	dhcp_ctx.last_tick = HAL_GetTick();
	// 4. go to WAIT_OFFER
	dhcp_ctx.state = DHCP_STATE_WAIT_OFFER;
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
	uint8_t dhcp_buf[DHCP_BUF_SIZE] = {0};
	int len;
	// 1. check timeout
	if (HAL_GetTick() - dhcp_ctx.last_tick > 3000) {
		dhcp_ctx.state = DHCP_STATE_RETRY;
		dhcp_ctx.error = dhcp_offer_timeout_err;
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
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,6);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.dns,opt_val,4);
	}
	// 9. go to REQUEST
	dhcp_ctx.state = DHCP_STATE_REQUEST;
}
static void init_dhcp_request_buf(uint8_t* request){
	memset(request, 0, DHCP_BUF_SIZE);
	
	const uint8_t* mac = netif_get_mac();
	
	request[DHCP_OP] 									= 0x01; 												// BOOTREQUEST
	
	request[DHCP_HTYPE] 							= 0x01; 												// Ethernet
	
	request[DHCP_HLEN] 								= 0x06; 												// MAC LENGTH
	
	request[DHCP_HOPS] 								= 0x00;													// relay count
	
	request[DHCP_XID] 								= (dhcp_ctx.xid >> 24) & 0xFF; 	// upper byte of random number
	request[DHCP_XID + 1] 						= (dhcp_ctx.xid >> 16) & 0xFF; 	//
	request[DHCP_XID + 2] 						= (dhcp_ctx.xid >> 8)  & 0xFF; 	//
	request[DHCP_XID + 3] 						= dhcp_ctx.xid 				 & 0xFF; 	// lower byte of random number
	
	request[DHCP_FLAGS]								= 0x80;													// Broadcast flag
	request[DHCP_FLAGS + 1]						= 0x00;
	
	memcpy(&request[DHCP_CHADDR], mac, 6);
	
	request[DHCP_MAGIC_COOOKIE] 			= 0x63;
	request[DHCP_MAGIC_COOOKIE + 1] 	= 0x82;
	request[DHCP_MAGIC_COOOKIE + 2] 	= 0x53;
	request[DHCP_MAGIC_COOOKIE + 3] 	= 0x63;
	
	uint16_t opt = DHCP_OPTIONS;
	
	/* Option 54: server Identifier */
	request[opt++] = 54;
	request[opt++] = 4;
	request[opt++] = dhcp_ctx.server_ip[0];
	request[opt++] = dhcp_ctx.server_ip[1];
	request[opt++] = dhcp_ctx.server_ip[2];
	request[opt++] = dhcp_ctx.server_ip[3];
	/* Option 53: DHCP Message Type = DHCPREQUEST(3) */
	request[opt++] = 53;
	request[opt++] = 1;
	request[opt++] = 3;
	/* Option 50: Requested IP Address */
	request[opt++] = 50;
	request[opt++] = 4;
	request[opt++] = dhcp_ctx.offered_ip[0];
	request[opt++] = dhcp_ctx.offered_ip[1];
	request[opt++] = dhcp_ctx.offered_ip[2];
	request[opt++] = dhcp_ctx.offered_ip[3];
	/* Option 61: Client Identifier */
	request[opt++] = 61;
	request[opt++] = 7;
	request[opt++] = 1;   // Ethernet
	memcpy(&request[opt], mac, 6);
	opt += 6;
	/* Option 55: Parameter Request List */
	request[opt++] = 55;
	request[opt++] = 6;
	request[opt++] = 1;   // Subnet Mask
	request[opt++] = 3;   // Router
	request[opt++] = 6;   // DNS
	request[opt++] = 51;  // Lease time
	request[opt++] = 58;  // T1
	request[opt++] = 59;  // T2
	/* Option 57: Maximum DHCP Message Size */
	request[opt++] = 57;
	request[opt++] = 2;
	request[opt++] = (DHCP_BUF_SIZE >> 8) & 0xFF;
	request[opt++] = DHCP_BUF_SIZE & 0xFF;
	/* End */
	request[opt++] = 255;
	dhcp_ctx.bufLen = opt;
}
void DHCP_REQUEST(void){
	uint8_t request[DHCP_BUF_SIZE] = {0}; // اندازه کافی برای BOOTP + Cookie + Options
	// - build DHCP REQUEST
	init_dhcp_request_buf(request);
	// - send broadcast or unicast
	W5500_SocketInit(DHCP_SOCK/*socket*/, BROADCAST_PORT/*port*/,W5500_SN_MR_P_UDP/*protocol*/);
	uint8_t server_ip[4] = { 255 ,255 ,255 ,255};
	W5500_Set_UDP_Destination(DHCP_SOCK,server_ip,BROADCAST_DPORT);
	W5500_Send_UDP(DHCP_SOCK,request,dhcp_ctx.bufLen);
	// - start timeout
	dhcp_ctx.last_tick = HAL_GetTick();
	// - go to WAIT_ACK
	dhcp_ctx.state = DHCP_STATE_WAIT_ACK;
}
void DHCP_WAIT_ACK(void){
	// - wait for UDP packet
	uint8_t dhcp_buf[DHCP_BUF_SIZE] = {0};
	int len;
	// 1. check timeout
	if (HAL_GetTick() - dhcp_ctx.last_tick > 3000) {
		dhcp_ctx.state = DHCP_STATE_RETRY;
		dhcp_ctx.error = dhcp_ack_timeout_err;
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
	uint32_t rx_xid = ((uint32_t)dhcp_buf[DHCP_XID] << 24) | ((uint32_t)dhcp_buf[DHCP_XID + 1] << 16) | 
										((uint32_t)dhcp_buf[DHCP_XID + 2] << 8) | ((uint32_t)dhcp_buf[DHCP_XID + 3]);
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
	// - if message type == ACK:(check option 53)
	uint8_t* opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,53);
	if((opt_val == NULL)){
		return;
	}
	if(*opt_val == OPT53_NACK){
		dhcp_ctx.state = DHCP_STATE_RETRY;
		dhcp_ctx.error = dhcp_nack_err;
		return;
	}
	if(*opt_val != OPT53_ACK){
		return;
	}
	//     - extract subnet, gateway, dns
	// 	- save YIADDR as offered_ip.
	memcpy(dhcp_ctx.offered_ip,&dhcp_buf[DHCP_YIADDR],4);
	//  - save Option 54 as server_ip
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,54);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.server_ip,opt_val,4);
	}
	else{
		return;
	}
	//	- save Option 1 as Subnet mask
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,1);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.subnet,opt_val,4);
	}
	else{
		return;
	}
	//	- save Option 3 as Gateway
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,3);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.gateway,opt_val,4);
	}
	else{
		return;
	}
	//	- save Option 6 as DNS
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,6);
	if(opt_val != NULL){
		memcpy(dhcp_ctx.dns,opt_val,4);
	}
	//  - save option 51 as lease time 
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,51);
	if(opt_val != NULL){
		dhcp_ctx.lease_time = ((uint32_t)*opt_val << 24) | ((uint32_t)*(opt_val + 1) << 16) | 
													((uint32_t)*(opt_val + 2) << 8) | (uint32_t)*(opt_val + 3);
	}
	//  - save option 58 as Renewal Time (T1) 
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,58);
	if(opt_val != NULL){
		dhcp_ctx.renewal_Time = ((uint32_t)*opt_val << 24) | ((uint32_t)*(opt_val + 1) << 16) | 
														((uint32_t)*(opt_val + 2) << 8) | (uint32_t)*(opt_val + 3);
	}
	//  - save option 59 as Rebinding Time (T2) 
	opt_val = get_dhcp_option(dhcp_buf,DHCP_BUF_SIZE,59);
	if(opt_val != NULL){
		dhcp_ctx.rebinding_Time = ((uint32_t)*opt_val << 24) | ((uint32_t)*(opt_val + 1) << 16) | 
															((uint32_t)*(opt_val + 2) << 8) | (uint32_t)*(opt_val + 3);
	}
	// 9. go to BOUND
	dhcp_ctx.state = DHCP_STATE_BOUND;
}
void DHCP_BOUND(void){
	dhcp_ctx.bound_tick = HAL_GetTick();
	// - network is usable
	W5500_Init_static_IP((uint8_t*)netif_get_mac(),dhcp_ctx.offered_ip,dhcp_ctx.subnet,dhcp_ctx.gateway);
	netif_set_dns1(dhcp_ctx.dns);
	// - start lease timer (optional)
	
	// - normal operation
	dhcp_ctx.state = DHCP_STATE_IDLE;
}
void DHCP_RETRY(void){
	if(dhcp_ctx.retry_count >= 10){
		dhcp_ctx.state = DHCP_STATE_FAIL;
		return;
	}
	switch(dhcp_ctx.error){
		case dhcp_idle_err:{
			
			break;
		}
		case dhcp_offer_timeout_err:{
			
			break;
		}
		case dhcp_ack_timeout_err:{
			
			break;
		}
		case dhcp_nack_err:{
			
			break;
		}
		default:{
			
			break;
		}
	}
}
void DHCP_FAIL			(void){
	
}
void DHCP_IDLE			(void){
	if(dhcp_ctx.bound_tick - HAL_GetTick() > 1){
		
	}
}
void dhcp_stateMachine(void){
	dhcp_sm[dhcp_ctx.state]();
}