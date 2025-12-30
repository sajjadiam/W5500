#ifndef NET_DHCP_H
#define NET_DHCP_H
#include <stdint.h>


#define DHCP_BUF_SIZE					600
#define DHCP_OP								0		// 1 byte
#define DHCP_HTYPE						1		// 1 byte
#define DHCP_HLEN							2		// 1 byte
#define DHCP_HOPS							3		// 1 byte 
#define DHCP_XID							4		// 4 byte 
#define DHCP_SECS							8		// 2 byte 
#define DHCP_FLAGS						10	// 2 byte 
#define DHCP_CIADDR						12	// 4 byte Client IP
#define DHCP_YIADDR						16	// 4 byte Your IP Address prefer by server
#define DHCP_SIADDR						20	// 4 byte IP سرور DHCP
#define DHCP_GIADDR						24	// 4 byte Gateway Relay
#define DHCP_CHADDR						28	// 6 byte my mac
#define DHCP_PADDING_CHADDR		34	// until 43
#define DHCP_SNAME						44	// until 107
#define DHCP_FILE							108 // until 235
#define DHCP_MAGIC_COOOKIE		236	// 4 byte
#define DHCP_OPTIONS					240	//

#ifndef DHCP_SOCK
#define DHCP_SOCK							7
#endif // DHCP_SOCK
#define BROADCAST_DPORT				67
#define BROADCAST_PORT				68

//option 53 value meanning
#define OPT53_DISCOVER				0x01
#define OPT53_OFFER						0x02
#define OPT53_REQUEST					0x03
#define OPT53_DECLINE					0x04
#define OPT53_ACK							0x05
#define OPT53_NACK						0x06
#define OPT53_RELEASE					0x07
#define OPT53_INFORM					0x08

typedef enum { 
	DHCP_STATE_INIT 			= 0, 
	DHCP_STATE_DISCOVER		, 
	DHCP_STATE_WAIT_OFFER	, 
	DHCP_STATE_REQUEST		, 
	DHCP_STATE_WAIT_ACK		, 
	DHCP_STATE_BOUND			, 
	DHCP_STATE_RETRY			, 
	DHCP_STATE_FAIL				,
	DHCP_STATE_IDLE				,
	DHCP_STATE_END
}dhcp_state_t;
typedef enum{
	dhcp_idle_err					= 0,
	dhcp_offer_timeout_err,
	dhcp_ack_timeout_err,
	dhcp_nack_err,
}dhcp_error_t;
typedef enum{
	DHCP_Dynamic		= 0,
	DHCP_Reservation,
}dhcp_mode_t;
extern volatile dhcp_state_t dhcp_state;
typedef struct {
	uint32_t 			xid;              // Transaction ID
	uint8_t  			server_ip[4];     // DHCP Server Identifier
	uint8_t  			offered_ip[4];    // yiaddr از OFFER
	uint8_t  			subnet[4];
	uint8_t  			gateway[4];
	uint8_t  			dns[4];
	uint32_t 			lease_time;
	uint32_t 			renewal_Time;
	uint32_t 			rebinding_Time;
	uint32_t 			bound_tick;
	uint32_t 			last_tick;
	uint8_t  			retry_count;
	uint16_t 			bufLen;
	dhcp_state_t	state;
	dhcp_mode_t 	mode;
	dhcp_error_t 	error;
}dhcp_ctx_t;

typedef void (*DHCP_Func_t)(void);
void DHCP_INIT			(void); 
void DHCP_DISCOVER	(void); 
void DHCP_WAIT_OFFER(void); 
void DHCP_REQUEST		(void); 
void DHCP_WAIT_ACK	(void); 
void DHCP_BOUND			(void); 
void DHCP_RETRY			(void);
void DHCP_FAIL			(void);
void DHCP_IDLE			(void);
//-----------------------------------------------------
void dhcp_stateMachine(void);
#endif // NET_DHCP_H