#include "W5500.h"
#include "spi.h"

static uint8_t W5500_SPI_ReadWrite(uint8_t byte) {
    uint8_t rx_data = 0;
    // در ارسال، همزمان دریافت هم انجام می‌شود تا پرچم‌ها پاک شوند
    HAL_SPI_TransmitReceive(W5500_SPI_HANDLE, &byte, &rx_data, 1, 100);
    return rx_data;
}
static void W5500_SendAddr(uint16_t addr) {
    W5500_SPI_ReadWrite((addr & 0xFF00) >> 8);
    W5500_SPI_ReadWrite(addr & 0x00FF);
}
void W5500_WriteByte(uint8_t addr, w5500_bsb_t block, uint8_t data){
	uint8_t control = W5500_CTRL(block, W5500_RW_WRITE, W5500_OM_VDM); // Write + VDM
	
	W5500_Select();
	
	W5500_SendAddr(addr);
	W5500_SPI_ReadWrite(control);
	W5500_SPI_ReadWrite(data);
	
	W5500_Deselect();
}
uint8_t W5500_ReadByte(uint8_t addr, uint8_t block){
	uint8_t data;
	uint8_t control = W5500_CTRL(block, W5500_RW_READ, W5500_OM_VDM); // Read + VDM
	
	W5500_Select();
	
	W5500_SendAddr(addr);
	W5500_SPI_ReadWrite(control);
	data = W5500_SPI_ReadWrite(0x00);
	
	W5500_Deselect();
	return data;
}

void W5500_WriteReg(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len) {
    if(len == 0) return;

    uint8_t control = W5500_CTRL(block, W5500_RW_WRITE, W5500_OM_VDM);
    
    W5500_Select();
    W5500_SendAddr(addr);
    W5500_SPI_ReadWrite(control); // <--- این خط جا افتاده بود
    
    for(uint16_t i=0; i < len; i++) {
        W5500_SPI_ReadWrite(buf[i]);
    }
    
    W5500_Deselect();
}

void W5500_ReadReg(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len) {
    if(len == 0) return;

    uint8_t control = W5500_CTRL(block, W5500_RW_READ, W5500_OM_VDM);
    
    W5500_Select();
    W5500_SendAddr(addr);
    W5500_SPI_ReadWrite(control);
    
    for(uint16_t i=0; i < len; i++) {
        // خواندن با ارسال Dummy Byte
        buf[i] = W5500_SPI_ReadWrite(0x00);
    }
    
    W5500_Deselect(); // <--- این خط جا افتاده بود
}

void W5500_WriteBuf(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len) {
    if(len == 0) return;

    uint8_t control = W5500_CTRL(block, W5500_RW_WRITE, W5500_OM_VDM);
    
    W5500_Select();
    
    W5500_SendAddr(addr);
    W5500_SPI_ReadWrite(control);
    
    // ارسال آرایه
    // نکته: برای سرعت بالاتر در انتقال‌های حجیم، اینجا میتوانید از HAL_SPI_Transmit استفاده کنید
    // چون نیازی به خواندن دیتای برگشتی نداریم.
    HAL_SPI_Transmit(W5500_SPI_HANDLE, buf, len, 1000); 
    
    W5500_Deselect();
}

void W5500_ReadBuf(uint16_t addr, uint8_t block, uint8_t* buf, uint16_t len) {
    if(len == 0) return;

    uint8_t control = W5500_CTRL(block, W5500_RW_READ, W5500_OM_VDM);
    
    W5500_Select();
    
    W5500_SendAddr(addr);
    W5500_SPI_ReadWrite(control);
    
    // دریافت آرایه
    HAL_SPI_Receive(W5500_SPI_HANDLE, buf, len, 1000);
    
    W5500_Deselect();
}

void W5500_HardwareReset(void) {
    // 1. پایین کشیدن پایه ریست (Active Low)
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(1); // حداقل 500 میکروثانیه لازم است
    
    // 2. بالا بردن پایه (فعال شدن چیپ)
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
    
    // 3. صبر برای پایدار شدن PLL داخلی (بسیار مهم)
    HAL_Delay(100); // حدود 100 میلی‌ثانیه صبر کنید تا چیپ کاملا بیدار شود
}