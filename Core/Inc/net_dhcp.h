#ifndef NET_DHCP_H
#define NET_DHCP_H
#include <stdint.h>


#define DHCP_BUF_SIZE						300
#define OFFSET_OP								0		// 1 byte
#define OFFSET_HTYPE						1		// 1 byte
#define OFFSET_HLEN							2		// 1 byte
#define OFFSET_HOPS							3		// 1 byte 
#define OFFSET_XID							4		// 4 byte 
#define OFFSET_SECS							8		// 2 byte 
#define OFFSET_FLAGS						10	// 2 byte 
#define OFFSET_CIADDR						12	// 4 byte Client IP
#define OFFSET_YIADDR						16	// 4 byte Your IP Address prefer by server
#define OFFSET_SIADDR						20	// 4 byte IP سرور DHCP
#define OFFSET_GIADDR						24	// 4 byte Gateway Relay
#define OFFSET_CHADDR						28	// 6 byte my mac
#define OFFSET_PADDING_CHADDR		34	// until 43
#define OFFSET_SNAME						44	// until 107
#define OFFSET_FILE							108 // until 235
#define OFFSET_MAGIC_COOOKIE		236	// 4 byte

typedef enum {
	DHCP_STATE_INIT = 0,
	DHCP_STATE_DISCOVER,
	DHCP_STATE_WAIT_OFFER,
	DHCP_STATE_REQUEST,
	DHCP_STATE_WAIT_ACK,
	DHCP_STATE_BOUND,
	DHCP_STATE_ERROR
}dhcp_state_t;
extern volatile dhcp_state_t dhcp_state;
typedef struct {
	uint32_t xid;              // Transaction ID
	uint8_t  server_ip[4];     // DHCP Server Identifier
	uint8_t  offered_ip[4];    // yiaddr از OFFER
	uint8_t  subnet[4];
	uint8_t  gateway[4];
	uint32_t lease_time;

	uint8_t  retry_count;
	uint32_t last_tick;
}dhcp_ctx_t;

void DHCP_INIT(void);
void DHCP_DISCOVER(void);
void DHCP_WAIT_OFFER(void);
void DHCP_REQUEST(void);
void DHCP_WAIT_ACK(void);
void DHCP_BOUND(void);
void DHCP_ERROR(void);

#endif // NET_DHCP_H