#include "w5500_core.h"
#include "W5500.h"
#include <string.h>
//aux
static void W5500_SoftwareReset(void){
	// 1. Reset W5500 (Software Reset)
	W5500_WriteByte(W5500_CRB_ADDR_MR,W5500_BSB_Common,W5500_MR_RST);
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
	W5500_WriteReg(W5500_CRB_ADDR_GAR0,W5500_BSB_Common,gw,CR_DATA_LEN_GAR);
	// 4. Set Subnet Mask
	W5500_WriteReg(W5500_CRB_ADDR_SUBR0,W5500_BSB_Common,sn,CR_DATA_LEN_SUBR);
	// 5. Set MAC Address
	W5500_WriteReg(W5500_CRB_ADDR_SHAR0,W5500_BSB_Common,mac,CR_DATA_LEN_SHAR);
	// 6. Set IP Address
	W5500_WriteReg(W5500_CRB_ADDR_SIPR0,W5500_BSB_Common,ip,CR_DATA_LEN_SIPR);
	// 6. Set Socket Memory Size (2KB for Rx/Tx on Socket 0-3 default)
  // No action needed if defaults are okay, otherwise write to 0x001E/0x001F
}