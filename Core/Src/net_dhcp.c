#include "net_dhcp.h"
#include <stdint.h>
#include <string.h>

static uint8_t dhcp_discover[DHCP_BUF_SIZE]; // اندازه کافی برای BOOTP + Cookie + Options

void init_dhcp_discover(uint8_t* discover){
	uint16_t len = DHCP_BUF_SIZE;
	while(len-- > 0){
		*discover++ = 0;
	}
}
