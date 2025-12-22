#include "w5500_core.h"
#include <string.h>
//aux
static void W5500_SoftwareReset(void){
	// 1. Reset W5500 (Software Reset)
	W5500_WriteByte(W5500_CRB_MR,W5500_BSB_Common,W5500_MR_RST);
	// Wait for reset to clear (simple delay usually needed)
	for(volatile int i=0; i<10000; i++);
}




















// main
void W5500_Init(uint8_t* mac, uint8_t* ip, uint8_t* sn, uint8_t* gw){
	// 1. Reset W5500 (Hardware Reset)
	W5500_HardwareReset();
	// 2. Reset W5500 (Software Reset)
	W5500_SoftwareReset();
	// 3. Set Gateway
	W5500_WriteReg(W5500_CRB_GAR0,W5500_BSB_Common,gw,CR_DATA_LEN_GAR);
	// 4. Set Subnet Mask
	W5500_WriteReg(W5500_CRB_SUBR0,W5500_BSB_Common,sn,CR_DATA_LEN_SUBR);
	// 5. Set MAC Address
	W5500_WriteReg(W5500_CRB_SHAR0,W5500_BSB_Common,mac,CR_DATA_LEN_SHAR);
	// 6. Set IP Address
	W5500_WriteReg(W5500_CRB_SIPR0,W5500_BSB_Common,ip,CR_DATA_LEN_SIPR);
	// 6. Set Socket Memory Size (2KB for Rx/Tx on Socket 0-3 default)
  // No action needed if defaults are okay, otherwise write to 0x001E/0x001F
}
uint8_t W5500_SocketInit(uint8_t sn, uint16_t port,w5500_sn_proto_t protocol){
	if(sn > 7){
		return 0;
	}
	if((protocol != W5500_SN_MR_P_TCP) && (protocol != W5500_SN_MR_P_UDP) && (protocol != W5500_SN_MR_P_MACRAW)){
		return 0;
	}
	// select block
	const w5500_bsb_t socket = 4 * sn + 1;
	// close socket 
	W5500_WriteByte(W5500_SRB_Sn_CR,socket,W5500_SN_CR_CLOSE);
	// select protocol mode
	W5500_WriteByte(W5500_SRB_Sn_MR,socket,protocol);
	// select port 
	W5500_WriteByte(W5500_SRB_Sn_PORT0,socket,((port >> 8) & 0xFF));
	W5500_WriteByte(W5500_SRB_Sn_PORT1,socket,(port & 0xFF));
	// open socket
	W5500_WriteByte(W5500_SRB_Sn_CR,socket,W5500_SN_CR_OPEN);
	if(W5500_ReadByte(W5500_SRB_Sn_SR,socket) != W5500_SN_SR_INIT){
		return 0;
	}
	return 1;
}
uint8_t W5500_SocketListen(uint8_t sn){
	if(sn > 7){
		return 0;
	}
	const w5500_bsb_t socket = 4 * sn + 1;
	W5500_WriteByte(W5500_SRB_Sn_CR,socket,W5500_SN_CR_LISTEN);
	if(W5500_ReadByte(W5500_SRB_Sn_SR,socket) != W5500_SN_SR_LISTEN){
		return 0;
	}
	return 1;
}

void W5500_Send(uint8_t sn, uint8_t *buf, uint16_t len){
	if(sn > 7){
		return;
	}
	const w5500_bsb_t socket = 4 * sn + 1;
	//
	uint16_t ptr, freeSize;
	// 1. صبر کن تا جا خالی پیدا بشه
	uint32_t timeout = 0;
	do{
		uint8_t fsr_h = W5500_ReadByte(W5500_SRB_Sn_TX_FSR0, socket);
		uint8_t fsr_l = W5500_ReadByte(W5500_SRB_Sn_TX_FSR1, socket);
		freeSize = (fsr_h << 8) | fsr_l;
		
		timeout++;
    if(timeout > 10000) { // یک عدد تجربی (حدود چند میلی‌ثانیه)
			return; // خروج اضطراری برای جلوگیری از هنگ کردن
    }
	} while (freeSize < len); //اين شرط احساس ميکنم شديدا بلاک کننده اس 
	// 2. خواندن پوینتر فعلی نوشتن (TX Write Pointer)
	uint8_t ptr_h = W5500_ReadByte(W5500_SRB_Sn_TX_WR0, socket);
	uint8_t ptr_l = W5500_ReadByte(W5500_SRB_Sn_TX_WR1, socket);
	ptr = (ptr_h << 8) | ptr_l;
	// 3. محاسبه آدرس فیزیکی و نوشتن دیتا
	// نکته مهم: اگر دیتا از انتهای بافر رد شود و به اول برگردد (Wrap around)
	// باید دو تیکه بنویسیم. اما تابع WriteBuf ما هوشمند نیست.
	// فعلا برای سادگی فرض میکنیم Wrap Around توسط چیپ هندل نمیشه و ما باید آدرس درست بدیم.
	// اما W5500 آدرس خطی میخواد؟ بله، فقط Mask لازم است.
	uint16_t offset = ptr & 0x07FF; // برای بافر 2KB (پیش‌فرض)
	// در اینجا یک نکته فنی وجود دارد:
	// W5500 اجازه میده شما آدرس رو بدی، خودش Mask میکنه؟ خیر.
	// شما باید آفست رو حساب کنید.
	// راه ساده برای تست امروز (بدون هندل کردن پیچیدگی انتهای بافر):
	W5500_WriteBuf(offset, (socket + 1) , buf, len);
	// 4. آپدیت کردن پوینتر نوشتن (بسیار مهم!)
	ptr += len;
	W5500_WriteByte(W5500_SRB_Sn_TX_WR0, socket, (ptr >> 8) & 0xFF);
	W5500_WriteByte(W5500_SRB_Sn_TX_WR1, socket, ptr & 0xFF);
	// 5. فرمان ارسال
  W5500_WriteByte(W5500_SRB_Sn_CR, socket, W5500_SN_CR_SEND);
	// 6. صبر برای اتمام ارسال
	while(W5500_ReadByte(W5500_SRB_Sn_CR, socket));
}

uint16_t W5500_Recv(uint8_t sn, uint8_t *buf, uint16_t max_len) {
	if(sn > 7){
		return 0; // خطا 
	}
	const w5500_bsb_t socket = 4 * sn + 1;
	uint16_t ptr, dataSize;
	
	// 1. چقدر دیتا اومده؟
	uint8_t rsr_h = W5500_ReadByte(W5500_SRB_Sn_RX_RSR0, socket);
	uint8_t rsr_l = W5500_ReadByte(W5500_SRB_Sn_RX_RSR1, socket);
	dataSize = (rsr_h << 8) | rsr_l;
	
	if(dataSize == 0){
		return 0;
	}
	if(dataSize > max_len){
		dataSize = max_len; // جلوگیری از سرریز بافر شما
	}
	
	// 2. خواندن پوینتر خواندن (RX Read Pointer)
	uint8_t ptr_h = W5500_ReadByte(W5500_SRB_Sn_RX_RD0, socket);
	uint8_t ptr_l = W5500_ReadByte(W5500_SRB_Sn_RX_RD1, socket);
	ptr = (ptr_h << 8) | ptr_l;
	
	// 3. خواندن دیتا
	uint16_t offset = ptr & 0x07FF; // Mask 2KB
	W5500_ReadBuf(offset, (socket + 2), buf, dataSize);
	
	// 4. آپدیت پوینتر و دستور RECV (که یعنی من خوندم، آزاد کن)
	ptr += dataSize;
	W5500_WriteByte(W5500_SRB_Sn_RX_RD0, socket, (ptr >> 8) & 0xFF);
	W5500_WriteByte(W5500_SRB_Sn_RX_RD1, socket, ptr & 0xFF);
	
	W5500_WriteByte(W5500_SRB_Sn_CR, socket, W5500_SN_CR_RECV);
	return dataSize;
}