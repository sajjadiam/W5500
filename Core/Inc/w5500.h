#ifndef W5500_H
#define W5500_H

#include <stdint.h>
#include "w5500_config.h"
#include "main.h"
#include <stdbool.h>

#define CS_PORT		SPI2_CS_GPIO_Port
#define CS_PIN		SPI2_CS_Pin
#define RST_PORT	Reset_w5500_GPIO_Port
#define	RST_PIN		Reset_w5500_Pin
#define	INT_PORT	int_w5500_GPIO_Port
#define	INT_PIN		int_w5500_Pin
//net config struct and init start
typedef struct {
	uint8_t mac[6];      // SHAR
	uint8_t ip[4];       // SIPR
	uint8_t gateway[4];  // GAR
	uint8_t subnet[4];   // SUBR
	uint16_t src_port;   // پورت پیش‌فرض مثلاً 8006 (برای socket 0 بعداً)
} W5500_NetConfig;
void W5500_InitStart(const W5500_NetConfig *cfg);
//init states 
typedef enum{
	W5_STATE_IDLE = 0,        // هنوز شروع نشده
	W5_STATE_RESET_HW,        // پالس روی پایه RESET
	W5_STATE_RESET_SW,        // نوشتن بیت M_RST در MR
	W5_STATE_READ_VERSION,    // خوندن VERSIONR برای اطمینان
	W5_STATE_CONFIG_PHY,      // تنظیم PHYCFGR
	W5_STATE_CONFIG_NET,      // نوشتن GAR/SUBR/SHAR/SIPR
	W5_STATE_DONE,            // آماده‌ی کار
	W5_STATE_ERROR,           // خطا
	W5_STATE_END
} W5500_InitState;
//adress
/*
typedef union{
	uint16_t 	address1;
	uint8_t		address2[2];
}AddressCaster;
*/
//-----------------------------------------------------------------------
typedef void (*W5500_SpiCallback_t)(void *user_ctx);
HAL_StatusTypeDef W5500_SpiTransferAsync(uint8_t *tx,uint8_t *rx,uint16_t len,W5500_SpiCallback_t cb,void *user_ctx);
//----------------------------------------------
void W5500_InitProcess(void);          // از داخل حلقه‌ی اصلی صدا زده می‌شود
uint8_t W5500_IsReady(void);           // 1 = آماده, 0 = هنوز
W5500_InitState W5500_GetInitState(void);
uint8_t W5500_GetLastError(void);
// inline function
//ControlByte write
static inline uint8_t ControlByte(BSB bsb ,OM om ,RWB rwb){
	return (((uint8_t)bsb << 3) | ((uint8_t)rwb << 2) | ((uint8_t)om & 0x03));
}
//PhyByte write
static inline uint8_t PhyByte(PHYCFG rst ,PHYCFG opmd ,OPMDC opmdc){
	return (((uint8_t)rst) | ((uint8_t)opmd) | (((uint8_t)opmdc & 0x07) << 3));
}
//W5500_Select
static inline void W5500_Select(void){
    HAL_GPIO_WritePin(CS_PORT,CS_PIN, GPIO_PIN_RESET);  // CS پایین
}
//W5500_Unselect
static inline void W5500_Unselect(void){
	HAL_GPIO_WritePin(CS_PORT,CS_PIN, GPIO_PIN_SET);    // CS بالا
}
#endif // W5500_H