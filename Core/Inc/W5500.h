#ifndef W5500_H
#define W5500_H

#include <stdint.h>
#include "W5500_regs.h"

#define W5_HDR_SIZE       3      // 3Byte -> 2Byte Address + 1Byte Controll
#define REG_BUF_SIZE(len)	(uint16_t)(W5_HDR_SIZE + (len))
#define W5_MAX_REG_DATA   6 // 6Byte data only
#define MAX_DATA_SIZE			32 // Byte
#define W5_FRAME_SIZE     (W5_HDR_SIZE + MAX_DATA_SIZE)
#define W5_SOCKET_NUM			3


typedef enum{ // W5500 spi state
	W5500_IDLE				= 0,
	W5500_TX_RX_BUSY	= 1,
  W5500_DATA_READY	= 2,
}w5500_State_t;
typedef enum{ // W5500 IpMode 
	W5500_IpMode_Static = 0,
	W5500_IpMode_DHCP   = 1,
}W5500_IpMode;
typedef struct { // W5500 net config
	uint8_t 						mac[6];      	// SHAR
	uint8_t 						ip[4];       	// SIPR
	uint8_t 						gateway[4];  	// GAR
	uint8_t 						subnet[4];   	// SUBR
	uint16_t 						src_port;   	// پورت پیش‌فرض مثلاً 8006 (برای socket 0 بعداً)
	W5500_IpMode				ip_mode;
}W5500_NetConfig;




void W5500_WriteByte(uint8_t addr, uint8_t block, uint8_t data);
uint8_t W5500_ReadByte(uint8_t addr, uint8_t block);
void W5500_WriteBuf(uint8_t addr, uint8_t block, uint8_t* buf, uint16_t len);
void W5500_ReadBuf(uint8_t addr, uint8_t block, uint8_t* buf, uint16_t len);

#endif //W5500_H