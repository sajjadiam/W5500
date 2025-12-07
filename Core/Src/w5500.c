#include "w5500.h"
#include "spi.h"
typedef void (*W5500_InitCallback_t)(void);

static volatile W5500_InitState w5_init_state = W5_STATE_IDLE;
static W5500_NetConfig w5_cfg;   // یک کپی از تنظیمات
static uint8_t w5_last_error;    // کد خطا اگر خواستی
static uint32_t w5_reset_timestamp = 0;
static uint8_t  w5_reset_hw_phase;
static uint8_t  w5_reset_sw_phase;
//static AddressCaster	adrs;
//
uint8_t W5500_IsReady(void){
	return (w5_init_state == W5_STATE_DONE) ? 1U : 0U;
}

W5500_InitState W5500_GetInitState(void){
	return w5_init_state;
}

uint8_t W5500_GetLastError(void){
	return w5_last_error;
}
//hardware reset -> can use in all states
static bool W5500_HardReset(void){
	switch(w5_reset_hw_phase){
		case 0:{
			// شروع ریست سخت‌افزاری: پین را LOW کن
			HAL_GPIO_WritePin(RST_PORT, RST_PIN,GPIO_PIN_RESET);
			w5_reset_timestamp = HAL_GetTick();
			w5_reset_hw_phase = 1;
			break;
		}
    case 1:{
			// حداقل 2ms پین در حالت LOW
			if (HAL_GetTick() - w5_reset_timestamp >= 2) {
				HAL_GPIO_WritePin(RST_PORT, RST_PIN,GPIO_PIN_SET);
				w5_reset_timestamp = HAL_GetTick();
				w5_reset_hw_phase = 2;
			}
			break;
		}
    case 2:{
			// چند ms صبر کن تا چیپ بالا بیاد (مثلاً 10ms)
			if (HAL_GetTick() - w5_reset_timestamp >= 10) {
				w5_reset_hw_phase = 0;
				return true;
			}
			break;
		}
    default:{
			w5_reset_hw_phase = 0;
			break;
		}
	}
	return false;
}
//software reset -> can use in all states
static bool W5500_SoftReset(void){
	switch (w5_reset_sw_phase) {
    case 0: {
			uint8_t tx[4];
			uint8_t rx[4];
			
			uint16_t addr = CRB_MR;
			tx[0] = (uint8_t)(addr >> 8);    // high
			tx[1] = (uint8_t)(addr & 0xFF);  // low
			tx[2] = ControlByte(BSB_Common, OM_VDM, RW_WRITE);
			tx[3] = M_RST;  // بیت 7 = 1 → نرم‌ریست

			W5500_Select();
			HAL_StatusTypeDef st = HAL_SPI_TransmitReceive(&hspi2, tx, rx, 4, 1000);
			W5500_Unselect();

			if (st != HAL_OK) {
				w5_last_error = 1;   // مثلا کد خطا برای نرم‌ریست
				w5_reset_sw_phase = 0;
				return false;
			}

			w5_reset_timestamp = HAL_GetTick();
			w5_reset_sw_phase = 1;
			break;
    }

    case 1:{
        // کمی صبر کن تا ریست داخلی انجام شود (مثلاً 2ms)
			if (HAL_GetTick() - w5_reset_timestamp >= 2) {
					w5_reset_sw_phase = 0;
					return true;
			}
			break;
		}
    default:{
			w5_reset_sw_phase = 0;
			break;
		}
	}
    return false;
}
//init starts
void W5500_InitStart(const W5500_NetConfig *cfg){
	w5_cfg = *cfg;  // کپی تنظیمات
	w5_last_error = 0;
	HAL_GPIO_WritePin(RST_PORT, RST_PIN,GPIO_PIN_SET);
	W5500_Unselect();
	w5_init_state = W5_STATE_RESET_HW; // تنظيم مرحله
	w5_reset_hw_phase = 0;   // شروع از فاز صفر
}
//init functions
static void W5_STATE_IDLE_func 					(void){
	// هیچ کاری نمی‌کنیم. تا وقتی W5500_InitStart صدا نخوره تو همین حالت می‌مونه.
}
static void W5_STATE_RESET_HW_func    	(void){
	if(W5500_HardReset()){
		w5_init_state = W5_STATE_RESET_SW;
		w5_reset_sw_phase = 0;
	}
}
static void W5_STATE_RESET_SW_func    	(void){
	if(W5500_SoftReset()){
		w5_init_state = W5_STATE_READ_VERSION;
	}
}
static void W5_STATE_READ_VERSION_func	(void){
	uint8_t tx[4];
	uint8_t rx[4];

	uint16_t addr = CRB_VERSIONR;  					// 0x0039

	tx[0] = (uint8_t)(addr >> 8);          	// high
	tx[1] = (uint8_t)(addr & 0xFF);        	// low
	tx[2] = ControlByte(BSB_Common, OM_VDM, RW_READ);
	tx[3] = 0x00;                          	// dummy

	W5500_Select();
	HAL_StatusTypeDef st = HAL_SPI_TransmitReceive(&hspi2, tx, rx, 4, 1000);
	W5500_Unselect();

	if (st != HAL_OK) {
		w5_last_error = 2;                 // مثلا error code برای read version
		w5_init_state = W5_STATE_ERROR;
		return;
	}

	uint8_t ver = rx[3];                   // دیتای VERSIONR

	// چک حداقلی: 0x00 یا 0xFF نباشه (احتمال نویز / عدم ارتباط)
	if (ver != W5500_VERSION) {
			w5_last_error = 3;                 // نسخه نامعتبر
			w5_init_state = W5_STATE_ERROR;
			return;
	}

	// می‌تونی اینجا ver رو برای دیباگ نگه داری اگر خواستی
	// مثلاً یه متغیر global:
	// w5_version = ver;

	w5_init_state = W5_STATE_CONFIG_PHY;
}
static void W5_STATE_CONFIG_PHY_func  	(void){
	uint8_t tx[4];
	uint8_t rx[4];
	uint16_t addr = CRB_PHYCFGR;
	
	tx[0] = (uint8_t)(addr >> 8);          	// high
	tx[1] = (uint8_t)(addr & 0xFF);        	// low
	tx[2] = ControlByte(BSB_Common, OM_VDM, RW_WRITE);
	//tx[3] = PhyByte(x,y,z);
}
static void W5_STATE_CONFIG_NET_func  	(void){
	
}
static void W5_STATE_DONE_func        	(void){
	
}
static void W5_STATE_ERROR_func        	(void){
	
}
//init table
static const W5500_InitCallback_t W5500_Init_machine[W5_STATE_END] = {
	[W5_STATE_IDLE 		    ]	= W5_STATE_IDLE_func,
	[W5_STATE_RESET_HW	  ]	= W5_STATE_RESET_HW_func,
	[W5_STATE_RESET_SW	  ]	= W5_STATE_RESET_SW_func,
	[W5_STATE_READ_VERSION]	= W5_STATE_READ_VERSION_func,
	[W5_STATE_CONFIG_PHY	]	= W5_STATE_CONFIG_PHY_func,
	[W5_STATE_CONFIG_NET	]	= W5_STATE_CONFIG_NET_func,
	[W5_STATE_DONE	      ]	= W5_STATE_DONE_func,
	[W5_STATE_ERROR	      ]	= W5_STATE_ERROR_func,
};
//
void W5500_InitProcess(void){
	if (w5_init_state < W5_STATE_END) {
		W5500_Init_machine[w5_init_state]();
	}
}
