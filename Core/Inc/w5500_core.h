#ifndef W5500_CORE_H
#define W5500_CORE_H
#include <stdint.h>
#include "W5500.h"

#define S0_BUF_SIZE     W5_KB_4
#define S1_BUF_SIZE     W5_KB_4
#define S2_BUF_SIZE     W5_KB_2
#define S3_BUF_SIZE     W5_KB_2
#define S4_BUF_SIZE     W5_KB_1
#define S5_BUF_SIZE     W5_KB_1
#define S6_BUF_SIZE     W5_KB_1
#define S7_BUF_SIZE     W5_KB_1  // مجموع الان 17 است -> باید ارور بدهد

#define TOTAL_BUF_SIZE  (S0_BUF_SIZE + S1_BUF_SIZE + S2_BUF_SIZE + S3_BUF_SIZE + \
                         S4_BUF_SIZE + S5_BUF_SIZE + S6_BUF_SIZE + S7_BUF_SIZE)
												 
#if (TOTAL_BUF_SIZE > W5_KB_16)
	#error "W5500 Error: Total Socket Memory exceeds 16KB! Please reduce buffer sizes."
#endif






typedef enum{ // W5500 spi state
	W5500_IDLE				= 0,
	W5500_TX_RX_BUSY	= 1,
  W5500_DATA_READY	= 2,
}w5500_State_t;



void W5500_Init(uint8_t* mac, uint8_t* ip, uint8_t* sn, uint8_t* gw); //(تنظیمات اولیه شبکه)


uint8_t W5500_SocketInit(uint8_t sn, uint16_t port,w5500_sn_proto_t protocol /*,w5500_sock_bufsize_t bufSize*/); // باز کردن سوکت در حالت TCP
uint8_t W5500_SocketListen(uint8_t sn); // بردن سوکت به حالت Listen (برای سرور)

void W5500_Send(uint8_t sn, uint8_t *buf, uint16_t len); 
uint16_t W5500_Recv(uint8_t sn, uint8_t *buf, uint16_t max_len);


//aux

#endif // W5500_CORE_H