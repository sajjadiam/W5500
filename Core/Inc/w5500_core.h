#ifndef W5500_CORE_H
#define W5500_CORE_H
#include <stdint.h>
typedef enum{ // W5500 spi state
	W5500_IDLE				= 0,
	W5500_TX_RX_BUSY	= 1,
  W5500_DATA_READY	= 2,
}w5500_State_t;
typedef struct { // W5500 net config
	uint8_t 						mac[6];      	// SHAR
	uint8_t 						ip[4];       	// SIPR
	uint8_t 						gateway[4];  	// GAR
	uint8_t 						subnet[4];   	// SUBR
}W5500_NetConfig_t;

// --- ICMP Packet Structure ---
typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
    uint8_t  data[32]; // Payload example
} ICMP_Packet;

void W5500_Init(uint8_t* mac, uint8_t* ip, uint8_t* sn, uint8_t* gw); //(تنظیمات اولیه شبکه)
uint8_t W5500_SocketOpen(uint8_t sn, uint8_t protocol, uint16_t port); //(باز کردن سوکت)
uint8_t W5500_SocketConnect(uint8_t sn, uint8_t* addr, uint16_t port); //(برای حالت کلاینت)
void W5500_SocketSend(uint8_t sn, uint8_t* buf, uint16_t len);
uint16_t W5500_SocketRecv(uint8_t sn, uint8_t* buf, uint16_t len);
uint8_t W5500_GetSocketStatus(uint8_t sn);
//aux

#endif // W5500_CORE_H