#include "w5500.h"
#include "spi.h"
void 							W5500_Handle_init							(W5500_Handler* hW5){
	hW5->hspi     			= W5500_UNIT_SPI;        // یا هر SPI دیگه
	hW5->Gpio.cs_port  	= CS_PORT;
	hW5->Gpio.cs_pin   	= CS_PIN;
	hW5->Gpio.rst_port 	= RST_PORT;
	hW5->Gpio.rst_pin  	= RST_PIN;
	hW5->Gpio.int_port 	= INT_PORT;
	hW5->Gpio.int_pin  	= INT_PIN;
}
void 							W5500_HardReset								(W5500_Handler* hW5){
	HAL_GPIO_WritePin(hW5->Gpio.rst_port, hW5->Gpio.rst_pin, GPIO_PIN_RESET);
	HAL_Delay(2);  // طبق دیتاشیت حداقل 2ms
	HAL_GPIO_WritePin(hW5->Gpio.rst_port, hW5->Gpio.rst_pin, GPIO_PIN_SET);
	HAL_Delay(10); // چند ms صبر تا چیپ کامل بالا بیاد
}
HAL_StatusTypeDef W5500_WriteReg								(W5500_Handler* hW5 ,const W5500_RegOp* op){
	if (op->len == 0 || op->len > W5_MAX_REG_DATA){
		return HAL_ERROR;	
	}	
	uint8_t tx[W5_FRAME_SIZE];
	tx[0] = (uint8_t)(op->addr >> 8);
	tx[1] = (uint8_t)(op->addr & 0xFF);
	tx[2] = W5500_ControlByte(op->block,OM_VDM,RW_WRITE);
	for (uint16_t i = 0; i < op->len; i++){
		tx[3 + i] = op->data[i];	
	}	
	W5500_Select(hW5);	
	HAL_StatusTypeDef st = HAL_SPI_Transmit(hW5->hspi, tx,W5_HDR_SIZE + op->len, 1000);
	W5500_Unselect(hW5);	
	return st;	
}	
HAL_StatusTypeDef W5500_ReadReg									(W5500_Handler* hW5 ,const W5500_RegOp* op){
	if (op->len == 0 || op->len > W5_MAX_REG_DATA){
		return HAL_ERROR;	
	}	
	uint8_t hdr[W5_HDR_SIZE];
	hdr[0] = (uint8_t)(op->addr >> 8);
	hdr[1] = (uint8_t)(op->addr & 0xFF);
	hdr[2] = W5500_ControlByte(op->block,OM_VDM,RW_READ);
	W5500_Select(hW5);	
	HAL_StatusTypeDef st = HAL_SPI_Transmit(hW5->hspi,hdr,sizeof(hdr),1000);
	if (st == HAL_OK){	
		st = HAL_SPI_Receive(hW5->hspi, op->data, op->len, 1000);
  }	
	W5500_Unselect(hW5);	
	return st;
}	
HAL_StatusTypeDef W5500_SoftReset								(W5500_Handler* hW5){
	uint8_t val 					= M_RST;
	W5500_RegOp 			op;
	op.addr  							= CRB_MR;
	op.block 							= BSB_Common;
	op.data  							= &val;
	op.len   							= CR_DATA_LEN_MR;   // که ۱ است
	HAL_StatusTypeDef st 	= W5500_WriteReg(hW5,&op);
	if(st != HAL_OK){
		return st;
	}
	HAL_Delay(2);  // کمی صبر کنیم
	return HAL_OK;
}
uint8_t W5500_Version														(W5500_Handler* hW5){
	uint8_t ver = 0;
	W5500_RegOp op;
	op.addr  = CRB_VERSIONR;
	op.block = BSB_Common;
	op.data  = &ver;
	op.len   = CR_DATA_LEN_VERSIONR;

	if (W5500_ReadReg(hW5, &op) == HAL_OK){
		return op.data[0];
	}
	return 0;
}
static HAL_StatusTypeDef W5500_WriteCommonRegBlock(W5500_Handler* hW5 ,uint16_t addr ,const uint8_t* data ,uint16_t len){
	W5500_RegOp op;
	op.addr  = addr;
	op.block = BSB_Common;
	op.data  = (uint8_t*)data;   // cast به خاطر const
	op.len   = len;
	return W5500_WriteReg(hW5, &op);
}
HAL_StatusTypeDef W5500_WriteGAR								(W5500_Handler* hW5 ,const uint8_t* gar){
	return W5500_WriteCommonRegBlock(hW5, CRB_GAR0, gar, 4);
}
HAL_StatusTypeDef W5500_WriteSUBR								(W5500_Handler* hW5 ,const uint8_t* subr){
	return W5500_WriteCommonRegBlock(hW5, CRB_SUBR0, subr, 4);
}
HAL_StatusTypeDef W5500_WriteSHAR								(W5500_Handler* hW5 ,const uint8_t* shar){
	return W5500_WriteCommonRegBlock(hW5, CRB_SHAR0, shar, 6);
}
HAL_StatusTypeDef W5500_WriteSIPR								(W5500_Handler* hW5 ,const uint8_t* sipr){
	return W5500_WriteCommonRegBlock(hW5, CRB_SIPR0, sipr, 4);
}
HAL_StatusTypeDef W5500_NetConfigure						(W5500_Handler* hW5 ,const W5500_NetConfig* cfg){
	if (hW5 == NULL || cfg == NULL) {
		return HAL_ERROR;
	}
	// 1) Gateway
	HAL_StatusTypeDef status = W5500_WriteGAR(hW5, cfg->gateway);
	if (status != HAL_OK) {
		return status;
	}
	// 2) Subnet Mask
	status = W5500_WriteSUBR(hW5, cfg->subnet);
	if (status != HAL_OK) {
		return status;
	}
	// 3) MAC Address
	status = W5500_WriteSHAR(hW5, cfg->mac);
	if (status != HAL_OK) {
		return status;
	}
	// 4) Source IP Address
	status = W5500_WriteSIPR(hW5, cfg->ip);
	if (status != HAL_OK) {
		return status;
	}
	// اگر خواستی: اینجا می‌توانی config را در handler هم ذخیره کنی
	// hW5->net = *config;   // فقط اگر همچین فیلدی تعریف کرده‌ای
	return HAL_OK;
}
static HAL_StatusTypeDef W5500_ReadCommonRegBlock(W5500_Handler *hW5 ,uint16_t addr ,uint8_t *data ,uint16_t len){
	W5500_RegOp op;
	op.addr  = addr;
	op.block = BSB_Common;
	op.data  = data;
	op.len   = len;
	return W5500_ReadReg(hW5, &op);
}
HAL_StatusTypeDef W5500_WriteRTR								(W5500_Handler* hW5 ,uint16_t rtr){
	
}
HAL_StatusTypeDef W5500_WriteRCR								(W5500_Handler* hW5 ,uint8_t  rcr){
	
}
HAL_StatusTypeDef W5500_ClearIR									(W5500_Handler* hW5){
	
}
HAL_StatusTypeDef W5500_WriteIMR								(W5500_Handler* hW5 ,uint8_t imr){
	
}
HAL_StatusTypeDef W5500_WriteSIMR								(W5500_Handler* hW5 ,uint8_t simr){
	
}
HAL_StatusTypeDef W5500_ReadPHYCFGR							(W5500_Handler* hW5 ,uint8_t* phycfgr){
	if (hW5 == NULL || phycfgr == NULL) {
		return HAL_ERROR;
	}
	return W5500_ReadCommonRegBlock(hW5 ,CRB_PHYCFGR ,phycfgr ,CR_DATA_LEN_PHYCFGR);
}
HAL_StatusTypeDef W5500_WritePHYCFGR						(W5500_Handler* hW5 ,uint8_t phycfgr){
	if (hW5 == NULL) {
		return HAL_ERROR;
	}
	return W5500_WriteCommonRegBlock(hW5 ,CRB_PHYCFGR ,&phycfgr ,CR_DATA_LEN_PHYCFGR);
}
bool 							W5500_IsLinkUp								(W5500_Handler* hW5){
	uint8_t reg = 0;
	if (W5500_ReadPHYCFGR(hW5, &reg) != HAL_OK) {
		return false;   // اگر نشد بخونیم، محافظه‌کارانه می‌گیم لینک نیست
	}
	// بیت PHYCFG_LNK = 0x01 -> 1 یعنی لینک up ، صفر یعنی down
	return ( (reg & PHYCFG_LNK) != 0 );
}
HAL_StatusTypeDef W5500_WaitForLink							(W5500_Handler* hW5 ,uint32_t timeout_ms){
	if (hW5 == NULL) {
		return HAL_ERROR;
	}
	uint32_t start = HAL_GetTick();
	while ((HAL_GetTick() - start) < timeout_ms){
		if (W5500_IsLinkUp(hW5)){
			return HAL_OK;   // لینک وصل شد
		}
		HAL_Delay(100);      // هر 100ms یک‌بار چک کن
	}
	// اگر تا timeout بالا نیامد:
	return HAL_TIMEOUT;
}
void 							W5500_SetNetConfigInHandler		(W5500_Handler* hW5 ,const W5500_NetConfig* cfg){
	
}
void 							W5500_GetNetConfigFromHandler	(W5500_Handler* hW5 ,W5500_NetConfig* cfg_out){
	
}
bool 							W5500_ValidateNetConfig				(const W5500_NetConfig* cfg){
	
}
HAL_StatusTypeDef W5500_InitStage1							(W5500_Handler* hW5 ,const W5500_NetConfig* cfg){
	//handle init
	//W5500_HardReset
	//W5500_SoftReset
	//read W5500_Version
	//check W5500_Version
	//set config
	//write config
	//set retrys
	//set interuppt mask
	//set phy
	//write phy 
	//check link up with time out
	//
}