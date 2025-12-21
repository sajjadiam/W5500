#ifndef W5500_H
#define W5500_H

#include <stdint.h>

#include "main.h"
#include "W5500_regs.h"


#define W5500_SPI_HANDLE      &hspi2


#define W5_HDR_SIZE       3      // 3Byte -> 2Byte Address + 1Byte Controll
#define REG_BUF_SIZE(len)	(uint16_t)(W5_HDR_SIZE + (len))
#define W5_MAX_REG_DATA   6 // 6Byte data only
#define MAX_DATA_SIZE			32 // Byte
#define W5_FRAME_SIZE     (W5_HDR_SIZE + MAX_DATA_SIZE)
#define W5_SOCKET_NUM			3

// main func
void W5500_WriteByte(uint8_t addr, w5500_bsb_t block, uint8_t data);
void W5500_WriteReg(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len);
void W5500_WriteBuf(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len);
uint8_t W5500_ReadByte(uint8_t addr, uint8_t block);
void W5500_ReadReg(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len);
void W5500_ReadBuf(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len);
void W5500_HardwareReset(void);
//aux func
static inline void W5500_Select(void){
	HAL_GPIO_WritePin(W5500_CS_GPIO_Port,W5500_CS_Pin,GPIO_PIN_RESET);
}
static inline void W5500_Deselect(void){
	HAL_GPIO_WritePin(W5500_CS_GPIO_Port,W5500_CS_Pin,GPIO_PIN_SET);
}

#endif //W5500_H