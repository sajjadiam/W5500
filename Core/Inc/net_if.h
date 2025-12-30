#ifndef NET_IF_H
#define NET_IF_H
#include <stm32f1xx.h>

// Pointers to the three 32-bit registers
#define UID_WORD0 							(*(volatile uint32_t *) (UID_BASE))
#define UID_WORD1 							(*(volatile uint32_t *) (UID_BASE + 0x04))
#define UID_WORD2 							(*(volatile uint32_t *) (UID_BASE + 0x08))


void netif_init_mac(void);
const uint8_t* netif_get_mac(void);
void netif_set_dns1(uint8_t* dns);
void netif_set_dns2(uint8_t* dns);
uint8_t* netif_get_dns1(void);
uint8_t* netif_get_dns2(void);
#endif //NET_IF_H