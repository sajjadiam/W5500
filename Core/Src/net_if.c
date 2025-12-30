#include <stdint.h>
#include "net_if.h"
#include "string.h"

static uint8_t device_mac[6];
static uint8_t net_dns1[4];
static uint8_t net_dns2[4];
static uint8_t net_ip[4];
uint8_t has_ip = 0;
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
void netif_set_dns1(uint8_t* dns){
	memcpy(net_dns1 ,dns,4);
}
void netif_set_dns2(uint8_t* dns){
	memcpy(net_dns2 ,dns,4);
}
uint8_t* netif_get_dns1(void){
	return net_dns1;
}
uint8_t* netif_get_dns2(void){
	return net_dns2;
}
void netif_set_ip(uint8_t* ip){
	memcpy(net_ip ,ip,4);
	has_ip = 1;
}
uint8_t* netif_get_ip(void){
	return net_ip;
}