#include "w5500_core.h"
#include <string.h>
// این آرایه در Flash ذخیره می‌شود و RAM اشغال نمی‌کند
// محاسبه مقادیر در زمان کامپایل انجام می‌شود
static const uint16_t w5_sock_masks[8] = {
    (S0_BUF_SIZE * 1024) - 1,
    (S1_BUF_SIZE * 1024) - 1,
    (S2_BUF_SIZE * 1024) - 1,
    (S3_BUF_SIZE * 1024) - 1,
    (S4_BUF_SIZE * 1024) - 1,
    (S5_BUF_SIZE * 1024) - 1,
    (S6_BUF_SIZE * 1024) - 1,
    (S7_BUF_SIZE * 1024) - 1
};
// تابع کمکی برای گرفتن سایز (اختیاری، برای تمیزی)
// چون MASK = SIZE - 1 است، سایز واقعی = MASK + 1
static inline uint16_t GetSocketSize(uint8_t sn) {
    return w5_sock_masks[sn] + 1;
}

static inline uint16_t GetSocketMask(uint8_t sn) {
    return w5_sock_masks[sn];
}
static inline uint16_t BSB_Sn_REG(uint8_t sn) {
    return sn * 4 + 1;
}
static inline uint16_t BSB_Sn_TXbuf(uint8_t sn) {
    return sn * 4 + 2;
}
static inline uint16_t BSB_Sn_RXbuf(uint8_t sn) {
	return sn * 4 + 3;
}
static inline uint16_t BSB_Sn_RX_RSR(w5500_bsb_t socket) {
	uint8_t rsr_h = W5500_ReadByte(W5500_SRB_Sn_RX_RSR0, socket);
	uint8_t rsr_l = W5500_ReadByte(W5500_SRB_Sn_RX_RSR1, socket);
  return (rsr_h << 8) | rsr_l;
}
static inline uint16_t BSB_Sn_RX_RD(w5500_bsb_t socket) {
	uint8_t ptr_h = W5500_ReadByte(W5500_SRB_Sn_RX_RD0, socket);
	uint8_t ptr_l = W5500_ReadByte(W5500_SRB_Sn_RX_RD1, socket);
  return (ptr_h << 8) | ptr_l;
}
//aux
void W5500_MemInit(void) {
	// نوشتن مقادیر در سخت‌افزار W5500
	// اینجا متاسفانه نمی‌توانیم لوپ بزنیم چون مقادیر ماکرو هستند و نام متفاوت دارند
	// اما می‌توانیم یک آرایه موقت بسازیم
	const uint8_t sizes[8] = {
		S0_BUF_SIZE, S1_BUF_SIZE, S2_BUF_SIZE, S3_BUF_SIZE,
		S4_BUF_SIZE, S5_BUF_SIZE, S6_BUF_SIZE, S7_BUF_SIZE
	};
	
	for(int i=0; i<8; i++) {
		W5500_WriteByte(W5500_SRB_Sn_TXBUF_SIZE, BSB_Sn_REG(i), sizes[i]);
		W5500_WriteByte(W5500_SRB_Sn_RXBUF_SIZE, BSB_Sn_REG(i), sizes[i]);
	}
}
static void W5500_SoftwareReset(void){
	// 1. Reset W5500 (Software Reset)
	W5500_WriteByte(W5500_CRB_MR,W5500_BSB_Common,W5500_MR_RST);
	// Wait for reset to clear (simple delay usually needed)
	for(volatile int i=0; i<10000; i++);
}
// main
void W5500_Init_static_IP(uint8_t* mac, uint8_t* ip, uint8_t* sn, uint8_t* gw){
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
uint8_t W5500_SocketInit(uint8_t sn, uint16_t port,w5500_sn_proto_t protocol /*,w5500_sock_bufsize_t bufSize*/){
	if(sn > 7){
		return 0;
	}
	if((protocol != W5500_SN_MR_P_TCP) && (protocol != W5500_SN_MR_P_UDP) && (protocol != W5500_SN_MR_P_MACRAW)){
		return 0;
	}
	// select block
	const w5500_bsb_t socket = BSB_Sn_REG(sn);
	// close socket 
	W5500_WriteByte(W5500_SRB_Sn_CR,socket,W5500_SN_CR_CLOSE);
	// select protocol mode
	W5500_WriteByte(W5500_SRB_Sn_MR,socket,protocol);
	// select port 
	W5500_WriteByte(W5500_SRB_Sn_PORT0,socket,((port >> 8) & 0xFF));
	W5500_WriteByte(W5500_SRB_Sn_PORT1,socket,(port & 0xFF));
	// open socket
	W5500_WriteByte(W5500_SRB_Sn_CR,socket,W5500_SN_CR_OPEN);
	uint8_t timeout = 0;
	while(timeout < 200){
		if(W5500_ReadByte(W5500_SRB_Sn_SR,socket) == W5500_SN_SR_INIT){
			return 1;
		}
		__NOP();
		timeout++;
	}
	return 0;
}
uint8_t W5500_SocketListen(uint8_t sn){
	if(sn > 7){
		return 0;
	}
	const w5500_bsb_t socket = BSB_Sn_REG(sn);
	W5500_WriteByte(W5500_SRB_Sn_CR,socket,W5500_SN_CR_LISTEN);
	if(W5500_ReadByte(W5500_SRB_Sn_SR,socket) != W5500_SN_SR_LISTEN){
		return 0;
	}
	return 1;
}

int8_t W5500_Send(uint8_t sn, uint8_t *buf, uint16_t len){
	if(len == 0){
		return 0;
	}
	if(sn > 7){
		return -1;
	}
	// 1. محاسبه سوکت و بلاک TX
  // فرض: هر سوکت 2KB حافظه دارد -> Mask = 0x07FF (2047)
	const w5500_bsb_t socket = BSB_Sn_REG(sn);
	const uint16_t SMASK = GetSocketMask(sn);
  const uint16_t SSIZE = GetSocketSize(sn);
	
	// 2. صبر برای فضای خالی (با تایم‌اوت)
	uint32_t startTick = HAL_GetTick(); 
	uint16_t freeSize;
	do{
		// 1. خواندن فضای خالی
		uint8_t fsr_h = W5500_ReadByte(W5500_SRB_Sn_TX_FSR0, socket);
		uint8_t fsr_l = W5500_ReadByte(W5500_SRB_Sn_TX_FSR1, socket);
		freeSize = (fsr_h << 8) | fsr_l;
		// 2. چک کردن وضعیت سوکت (نکته طلایی صنعتی)
		// اگر ارتباط قطع شده باشد، منتظر ماندن برای فضای خالی بی‌فایده است
		uint8_t status = W5500_ReadByte(W5500_SRB_Sn_SR, socket);
		if ((status != W5500_SN_SR_ESTABLISHED) && (status != W5500_SN_SR_CLOSE_WAIT)) {
				return -1; // ارتباط قطع شده، ارسال کنسل است
		}
		// 3. چک کردن تایم‌اوت (مثلا 1000 میلی‌ثانیه)
		if ((HAL_GetTick() - startTick) > 1000) {
			return -2; // تایم‌اوت: شبکه شلوغ است یا بافر خالی نمی‌شود
		}
	} while (freeSize < len); //اين شرط احساس ميکنم شديدا بلاک کننده اس 
	// 3. خواندن پوینتر فعلی نوشتن
	uint8_t ptr_h = W5500_ReadByte(W5500_SRB_Sn_TX_WR0, socket);
	uint8_t ptr_l = W5500_ReadByte(W5500_SRB_Sn_TX_WR1, socket);
	uint16_t ptr = (ptr_h << 8) | ptr_l;
	// 4. محاسبه آفست فیزیکی در بافر (محلی که باید بنویسیم)
	uint16_t offset = ptr & SMASK; // برای بافر 2KB (پیش‌فرض)
	// 5. بررسی نیاز به شکستن دیتا (Wrap Around Check)
	if ( (offset + len) > SSIZE ) {
		// حالت خاص: دیتا از انتهای بافر بیرون می‌زند
		uint16_t size1 = SSIZE - offset; // مقدار دیتایی که تا ته بافر جا می‌شود
		uint16_t size2 = len - size1;    // مقدار باقیمانده که باید برود اول بافر
		
		// تیکه اول: از آفست تا ته بافر
		W5500_WriteBuf(offset, BSB_Sn_TXbuf(sn), buf, size1);
		
		// تیکه دوم: از اول بافر (آفست 0)
		W5500_WriteBuf(0, BSB_Sn_TXbuf(sn), buf + size1, size2);
			
	} else {
		// حالت عادی: دیتا یکجا جا می‌شود
		W5500_WriteBuf(offset, BSB_Sn_TXbuf(sn), buf, len);
	}
	// 6. آپدیت پوینتر نوشتن (به اندازه کل len جلو می‌رود)
	ptr += len;
	W5500_WriteByte(W5500_SRB_Sn_TX_WR0, socket, (ptr >> 8) & 0xFF);
	W5500_WriteByte(W5500_SRB_Sn_TX_WR1, socket, ptr & 0xFF);
	// 7. فرمان ارسال
  W5500_WriteByte(W5500_SRB_Sn_CR, socket, W5500_SN_CR_SEND);
	// 8. صبر برای اتمام ارسال
	startTick = HAL_GetTick();
	while (W5500_ReadByte(W5500_SRB_Sn_CR, socket)) {
		if ((HAL_GetTick() - startTick) > 100) break; // نباید زیاد طول بکشد
	}
	
	return 0; // موفقیت
}
uint16_t W5500_Recv(uint8_t sn, uint8_t *buf, uint16_t max_len) {
	if(sn > 7){
		return 0; // خطا 
	}  
	// 1. محاسبه آدرس بلاک‌ رجیستر
	const w5500_bsb_t socket_reg = BSB_Sn_REG(sn);
	const w5500_bsb_t socket_rx = BSB_Sn_RXbuf(sn);
	//دریافت تنظیمات سایز و ماسک (از توابع کمکی که ساختیم)
	const uint16_t SMASK = GetSocketMask(sn);
  const uint16_t SSIZE = GetSocketSize(sn);
	
	// 2. چقدر دیتا آمده است؟ (Received Size Register)
	uint16_t dataSize = BSB_Sn_RX_RSR(socket_reg);
	if(dataSize == 0){
		return 0;
	}
	uint16_t readLen = dataSize;
	if (readLen > max_len) {
		readLen = max_len;
	}
	// 4. خواندن پوینتر خواندن (Read Pointer)
	uint16_t ptr = BSB_Sn_RX_RD(socket_reg);
	// 5. محاسبه آفست فیزیکی در بافر حلقوی
	uint16_t offset = ptr & SMASK;
	// 6. خواندن دیتا با در نظر گرفتن Wrap Around (دو تکه شدن)
	if ( (offset + readLen) > SSIZE ) {
		// حالت خاص: دیتا از انتهای بافر رد شده و به اول برگشته
		uint16_t size1 = SSIZE - offset;    // تکه اول: از آفست تا ته بافر
		uint16_t size2 = readLen - size1;   // تکه دوم: از اول بافر (آدرس 0)

		// خواندن بخش اول
		W5500_ReadBuf(offset, socket_rx, buf, size1);
		
		// خواندن بخش دوم (نوشتن در ادامه بافر buf)
		W5500_ReadBuf(0, socket_rx, buf + size1, size2);
	} 
	else{
		// حالت عادی: دیتا یک تکه است و جا می‌شود
		W5500_ReadBuf(offset, socket_rx, buf, readLen);
	}
	// 7. آپدیت پوینتر خواندن (RD) به اندازه دیتایی که واقعا خواندیم
	ptr += readLen;
	W5500_WriteByte(W5500_SRB_Sn_RX_RD0, socket_reg, (ptr >> 8) & 0xFF);
	W5500_WriteByte(W5500_SRB_Sn_RX_RD1, socket_reg, ptr & 0xFF);
	// 8. صدور فرمان RECV (یعنی پردازش این بخش تمام شد)
	W5500_WriteByte(W5500_SRB_Sn_CR, socket_reg, W5500_SN_CR_RECV);
	// صبر کوتاه برای اعمال دستور (اختیاری ولی توصیه شده)
	while(W5500_ReadByte(W5500_SRB_Sn_CR, socket_reg));
	
	return readLen; // مقدار دیتایی که خواندیم را برمی‌گردانیم
}