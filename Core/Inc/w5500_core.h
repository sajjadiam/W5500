#ifndef W5500_CORE_H
#define W5500_CORE_H
#include <stdint.h>
#include "W5500.h"

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


void W5500_Init(uint8_t* mac, uint8_t* ip, uint8_t* sn, uint8_t* gw); //(تنظیمات اولیه شبکه)


uint8_t W5500_SocketInit(uint8_t sn, uint16_t port,w5500_sn_proto_t protocol); // باز کردن سوکت در حالت TCP
uint8_t W5500_SocketListen(uint8_t sn); // بردن سوکت به حالت Listen (برای سرور)

void W5500_Send(uint8_t sn, uint8_t *buf, uint16_t len); 
uint16_t W5500_Recv(uint8_t sn, uint8_t *buf, uint16_t max_len);


//aux

#endif // W5500_CORE_H