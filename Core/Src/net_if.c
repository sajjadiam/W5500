#include <stdint.h>
#include "net_if.h"




static uint8_t device_mac[6];
static void set_mac(uint8_t* macArr){
	macArr[0] = 0x02;												// local mac
	macArr[1] = 0x01;												// uint adder example 0x01 for security uint
	macArr[2] = 0x01;												// number of Device
	macArr[3] = (uint8_t)(UID_WORD0 & 0xFF);// number of Device
	macArr[4] = (uint8_t)((UID_WORD1 >> 8) & 0xFF);												// number of Device
	macArr[5] = (uint8_t)(UID_WORD2 & 0xFF);													// number of Device
}

void netif_init_mac(void){
    set_mac(device_mac);
}

const uint8_t* netif_get_mac(void){
    return device_mac;
}