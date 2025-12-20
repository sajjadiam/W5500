#ifndef W5500_CORE_H
#define W5500_CORE_H
#include <stdint.h>
void W5500_Init(uint8_t* mac, uint8_t* ip, uint8_t* sn, uint8_t* gw); //(تنظیمات اولیه شبکه)
uint8_t W5500_SocketOpen(uint8_t sn, uint8_t protocol, uint16_t port); //(باز کردن سوکت)
uint8_t W5500_SocketConnect(uint8_t sn, uint8_t* addr, uint16_t port); //(برای حالت کلاینت)
void W5500_SocketSend(uint8_t sn, uint8_t* buf, uint16_t len);
uint16_t W5500_SocketRecv(uint8_t sn, uint8_t* buf, uint16_t len);
uint8_t W5500_GetSocketStatus(uint8_t sn);
#endif // W5500_CORE_H