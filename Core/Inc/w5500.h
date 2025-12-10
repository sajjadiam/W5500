#ifndef W5500_H
#define W5500_H

#include <stdint.h>
#include "w5500_config.h"
#include "main.h"
#include <stdbool.h>

#define CS_PORT						SPI2_CS_GPIO_Port
#define CS_PIN						SPI2_CS_Pin
#define RST_PORT					Reset_w5500_GPIO_Port
#define	RST_PIN						Reset_w5500_Pin
#define	INT_PORT					int_w5500_GPIO_Port
#define	INT_PIN						int_w5500_Pin
#define W5500_UNIT_SPI		&hspi2
#define W5_HDR_SIZE       3      // 2 بایت آدرس + 1 بایت کنترل
#define REG_BUF_SIZE(len)	(uint16_t)(W5_HDR_SIZE + (len))
#define W5_MAX_REG_DATA   8 
#define W5_FRAME_SIZE     (W5_HDR_SIZE + W5_MAX_REG_DATA)
//function pointer typedefs


//net config struct and init start
typedef enum{
	W5500_IpMode_Static = 0,
	W5500_IpMode_DHCP   = 1,
}W5500_IpMode;
typedef struct { // اين استراکچر براي تنظيمات شبکه هستش مثل مک و آي پي و گيت وي و ساب نت و مشخص کردن پورت ارتباطي
	uint8_t 						mac[6];      	// SHAR
	uint8_t 						ip[4];       	// SIPR
	uint8_t 						gateway[4];  	// GAR
	uint8_t 						subnet[4];   	// SUBR
	uint16_t 						src_port;   	// پورت پیش‌فرض مثلاً 8006 (برای socket 0 بعداً)
	W5500_IpMode				ip_mode;
}W5500_NetConfig;

typedef struct { // تنظيمات پورت وپين ها
	GPIO_TypeDef *cs_port;
	uint16_t      cs_pin;
	GPIO_TypeDef *rst_port;
	uint16_t      rst_pin;
	GPIO_TypeDef *int_port;  // اگه فعلاً استفاده نمی‌کنی می‌تونی NULL بزاری
	uint16_t      int_pin;
}W5500_GpioConfig;

typedef struct { // بلاک ديتاي رجيستري
	uint16_t      	addr; 	// آدرس رجیستر
	BSB           	block; 	// بلاک انتخابی
	uint8_t       	*data; 	// اشاره‌گر به داده
	uint16_t      	len;   	// متغيير
}W5500_RegOp;

typedef struct { // بلاک هندلر اصلي
	SPI_HandleTypeDef *hspi;
	W5500_NetConfig		net;
	W5500_GpioConfig 	Gpio;
}W5500_Handler;
//----------------------------------------------
void 							W5500_Handle_init							(W5500_Handler *hW5);
HAL_StatusTypeDef W5500_WriteReg								(W5500_Handler* hW5 ,const W5500_RegOp *op);
HAL_StatusTypeDef W5500_ReadReg									(W5500_Handler* hW5 ,const W5500_RegOp *op);
void 							W5500_HardReset								(W5500_Handler *hW5);
HAL_StatusTypeDef W5500_SoftReset								(W5500_Handler* hW5);
HAL_StatusTypeDef W5500_Version									(W5500_Handler* hW5);
HAL_StatusTypeDef W5500_WriteGAR								(W5500_Handler *hW5 ,const uint8_t* gar);
HAL_StatusTypeDef W5500_WriteSUBR								(W5500_Handler *hW5 ,const uint8_t* subr);
HAL_StatusTypeDef W5500_WriteSHAR								(W5500_Handler *hW5 ,const uint8_t* shar);
HAL_StatusTypeDef W5500_WriteSIPR								(W5500_Handler *hW5 ,const uint8_t* sipr);
HAL_StatusTypeDef W5500_NetConfigure						(W5500_Handler* hW5 ,const W5500_NetConfig *cfg);
HAL_StatusTypeDef W5500_WriteRTR								(W5500_Handler *hW5 ,uint16_t rtr);
HAL_StatusTypeDef W5500_WriteRCR								(W5500_Handler *hW5 ,uint8_t  rcr);
HAL_StatusTypeDef W5500_ClearIR									(W5500_Handler *hW5);
HAL_StatusTypeDef W5500_WriteIMR								(W5500_Handler *hW5 ,uint8_t imr);
HAL_StatusTypeDef W5500_WriteSIMR								(W5500_Handler *hW5 ,uint8_t simr);
HAL_StatusTypeDef W5500_ReadPHYCFGR							(W5500_Handler *hW5 ,uint8_t *phycfgr);
HAL_StatusTypeDef W5500_WritePHYCFGR						(W5500_Handler *hW5 ,uint8_t phycfgr);
bool 							W5500_IsLinkUp								(W5500_Handler *hW5);
HAL_StatusTypeDef W5500_WaitForLink							(W5500_Handler *hW5 ,uint32_t timeout_ms);
void 							W5500_SetNetConfigInHandler		(W5500_Handler *hW5 ,const W5500_NetConfig *cfg);
void 							W5500_GetNetConfigFromHandler	(W5500_Handler *hW5 ,W5500_NetConfig *cfg_out);
bool 							W5500_ValidateNetConfig				(const W5500_NetConfig *cfg);
HAL_StatusTypeDef W5500_InitStage1							(W5500_Handler *hW5 ,const W5500_NetConfig *cfg);
// inline function
//ControlByte write
static inline uint8_t W5500_ControlByte(BSB bsb ,OM om ,RWB rwb){
	return (((uint8_t)bsb << 3) | ((uint8_t)rwb << 2) | ((uint8_t)om & 0x03));
}
//PhyByte write
/*static inline uint8_t PhyByte(PHYCFG rst ,PHYCFG opmd ,OPMDC opmdc){
	return (((uint8_t)rst) | ((uint8_t)opmd) | (((uint8_t)opmdc & 0x07) << 3));
}*/
//W5500_Select
static inline void W5500_Select(W5500_Handler* hW5){
	HAL_GPIO_WritePin(hW5->Gpio.cs_port, hW5->Gpio.cs_pin, GPIO_PIN_RESET);
}
//W5500_Unselect
static inline void W5500_Unselect(W5500_Handler* hW5){
	HAL_GPIO_WritePin(hW5->Gpio.cs_port, hW5->Gpio.cs_pin, GPIO_PIN_SET);
}
//
static inline BSB W5500_SockRegBlock(uint8_t sn){
	return (BSB)((uint8_t)BSB_0_Register + (sn * 4));
}
// محاسبه Block برای TX Buffer سوکت n
static inline BSB W5500_SockTxBlock(uint8_t sn){
	return (BSB)((uint8_t)BSB_0_TX_Buffer + (sn * 4));
}
#endif // W5500_H