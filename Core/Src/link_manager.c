#include "link_manager.h"

w5500_phycfgr_bits_t link_status(void){
	return (W5500_ReadByte(W5500_CRB_PHYCFGR,W5500_BSB_Common) & W5500_PHYCFGR_LNK);
}
