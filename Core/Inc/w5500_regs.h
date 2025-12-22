#ifndef W5500_REGS_H
#define W5500_REGS_H

#include <stdint.h>

#define SR_DATA_LEN_SX_TXBUF_SIZE 1
#define W5500_VERSION											0x04
//Common Register Block Data Length
#define CR_DATA_LEN_MR										1
#define CR_DATA_LEN_GAR										4
#define CR_DATA_LEN_SUBR									4
#define CR_DATA_LEN_SHAR									6
#define CR_DATA_LEN_SIPR									4
#define CR_DATA_LEN_INTLEVEL							2
#define CR_DATA_LEN_IR										1
#define CR_DATA_LEN_IMR										1
#define CR_DATA_LEN_SIR										1
#define CR_DATA_LEN_SIMR									1
#define CR_DATA_LEN_RTR										2
#define CR_DATA_LEN_RCR										1
#define CR_DATA_LEN_PTIMER								1
#define CR_DATA_LEN_PMAGIC								1
#define CR_DATA_LEN_PHAR									6
#define CR_DATA_LEN_PSID									2
#define CR_DATA_LEN_PMRU									2
#define CR_DATA_LEN_UIPR									4
#define CR_DATA_LEN_UPORTR								2
#define CR_DATA_LEN_PHYCFGR								1
#define CR_DATA_LEN_VERSIONR							1
//Socket Register Block Data Length
#define SR_DATA_LEN_SX_MR									1
#define SR_DATA_LEN_SX_CR									1
#define SR_DATA_LEN_SX_IR									1
#define SR_DATA_LEN_SX_SR									1
#define SR_DATA_LEN_SX_PORT								2
#define SR_DATA_LEN_SX_DHAR								6
#define SR_DATA_LEN_SX_DIPR								4
#define SR_DATA_LEN_SX_DPORT							2
#define SR_DATA_LEN_SX_MSSR								2
#define SR_DATA_LEN_SX_TOS								1
#define SR_DATA_LEN_SX_TTL								1
#define SR_DATA_LEN_SX_RXBUF_SIZE					1
#define SR_DATA_LEN_SX_TX_FSR							2
#define SR_DATA_LEN_SX_TX_RD							2
#define SR_DATA_LEN_SX_TX_WR							2
#define SR_DATA_LEN_SX_RX_RSR							2
#define SR_DATA_LEN_SX_RX_RD							2
#define SR_DATA_LEN_SX_RX_WR							2
#define SR_DATA_LEN_SX_IMR								1
#define SR_DATA_LEN_SX_FRAG								2
#define SR_DATA_LEN_SX_KPALVTR						1
//mask defines 
#define W5500_PHYCFGR_OPMDC_MASK      		(0x38u)   /* bits 5..3 */
#define W5500_PHYCFGR_OPMD_MASK       		(0x40u)   /* bit 6     */
#define W5500_PHYCFGR_RST_MASK        		(0x80u)   /* bit 7     */
//-------------------------
typedef enum{	//Block Select Bits
	W5500_BSB_Common 				= 0x00,
	W5500_BSB_0_Register		= 0x01,
	W5500_BSB_0_TX_Buffer		= 0x02,
	W5500_BSB_0_RX_Buffer		= 0x03,
	// 0x04 reserved
	W5500_BSB_1_Register		= 0x05,
	W5500_BSB_1_TX_Buffer		= 0x06,
	W5500_BSB_1_RX_Buffer		= 0x07,
	// 0x08 reserved
	W5500_BSB_2_Register		= 0x09,
	W5500_BSB_2_TX_Buffer		= 0x0A,
	W5500_BSB_2_RX_Buffer		= 0x0B,
	// 0x0C reserved
	W5500_BSB_3_Register		= 0x0D,
	W5500_BSB_3_TX_Buffer		= 0x0E,
	W5500_BSB_3_RX_Buffer		= 0x0F,
	// 0x10 reserved
	W5500_BSB_4_Register		= 0x11,
	W5500_BSB_4_TX_Buffer		= 0x12,
	W5500_BSB_4_RX_Buffer		= 0x13,
	// 0x14 reserved
	W5500_BSB_5_Register		= 0x15,
	W5500_BSB_5_TX_Buffer		= 0x16,
	W5500_BSB_5_RX_Buffer		= 0x17,
	// 0x18 reserved
	W5500_BSB_6_Register		= 0x19,
	W5500_BSB_6_TX_Buffer		= 0x1A,
	W5500_BSB_6_RX_Buffer		= 0x1B,
	// 0x1C reserved
	W5500_BSB_7_Register		= 0x1D,
	W5500_BSB_7_TX_Buffer		= 0x1E,
	W5500_BSB_7_RX_Buffer		= 0x1F,
}w5500_bsb_t;
typedef enum{	//SPI Operation Mode Bits
	W5500_OM_VDM		= 0x00,       /* variable -> 0 : n byte	*/
	W5500_OM_FDM1		= 0x01,       /* fixed-> 1: 1 byte		 	*/
	W5500_OM_FDM2		= 0x02,       /* fixed-> 2: 2 byte		 	*/
	W5500_OM_FDM4		= 0x03,       /* fixed-> 3: 4 byte		 	*/
}w5500_om_t;
typedef enum{	//Read/Write Access Mode Bit
	W5500_RW_READ		= 0x00,
	W5500_RW_WRITE	= 0x01,
}w5500_rwb_t;
typedef enum{	//Common Register Block
	// Mode
	W5500_CRB_MR					= 0x0000,	
	// Gateway Address
	W5500_CRB_GAR0				= 0x0001,
	W5500_CRB_GAR1				= 0x0002,
	W5500_CRB_GAR2				= 0x0003,
	W5500_CRB_GAR3				= 0x0004,
	// Subnet Mask Address
	W5500_CRB_SUBR0				= 0x0005,
	W5500_CRB_SUBR1				= 0x0006,
	W5500_CRB_SUBR2				= 0x0007,
	W5500_CRB_SUBR3				= 0x0008,
	// Source Hardware Address (MAC ADDRESS)
	W5500_CRB_SHAR0				= 0x0009,
	W5500_CRB_SHAR1				= 0x000A,
	W5500_CRB_SHAR2				= 0x000B,
	W5500_CRB_SHAR3				= 0x000C,
	W5500_CRB_SHAR4				= 0x000D,
	W5500_CRB_SHAR5				= 0x000E,
	// Source IP Address
	W5500_CRB_SIPR0				= 0x000F,
	W5500_CRB_SIPR1				= 0x0010,
	W5500_CRB_SIPR2				= 0x0011,
	W5500_CRB_SIPR3				= 0x0012,
	// Interrupt Low Level Timer
	W5500_CRB_INTLEVEL0		= 0x0013,
	W5500_CRB_INTLEVEL1		= 0x0014,
	// Interrupt
	W5500_CRB_IR					= 0x0015,
	// Interrupt Mask
	W5500_CRB_IMR					= 0x0016,
	// Socket Interrupt 
	W5500_CRB_SIR					= 0x0017,
	// Socket Interrupt Mask
	W5500_CRB_SIMR				= 0x0018,
	// Retry Time
	W5500_CRB_RTR0				= 0x0019,
	W5500_CRB_RTR1				= 0x001A,
	// Retry Count
	W5500_CRB_RCR					= 0x001B,
	// PPP LCP Request Timer
	W5500_CRB_PTIMER			= 0x001C,
	// PPP LCP Magic number
	W5500_CRB_PMAGIC			= 0x001D,
	// PPP Destination MAC Address
	W5500_CRB_PHAR0				= 0x001E,
	W5500_CRB_PHAR1				= 0x001F,
	W5500_CRB_PHAR2				= 0x0020,
	W5500_CRB_PHAR3				= 0x0021,
	W5500_CRB_PHAR4				= 0x0022,
	W5500_CRB_PHAR5				= 0x0023,
	// PPP Session Identification
	W5500_CRB_PSID0				= 0x0024,
	W5500_CRB_PSID1				= 0x0025,
	// PPP Maximum Segment Size
	W5500_CRB_PMRU0				= 0x0026,
	W5500_CRB_PMRU1				= 0x0027,
	// Unreachable IP address
	W5500_CRB_UIPR0				= 0x0028,
	W5500_CRB_UIPR1				= 0x0029,
	W5500_CRB_UIPR2				= 0x002A,
	W5500_CRB_UIPR3				= 0x002B,
	// Unreachable Port
	W5500_CRB_UPORTR0			= 0x002C,
	W5500_CRB_UPORTR1			= 0x002D,
	// PHY Configuration
	W5500_CRB_PHYCFGR			= 0x002E,
	// Reserved				0x002F~0x0038
	// Chip version
	W5500_CRB_VERSIONR		= 0x0039,
	// Reserved				0x003A~0xFFFF
}w5500_cr_addr_t;
typedef enum{	//Socket Register Block
	// Socket n Mode
	W5500_SRB_Sn_MR				= 0x0000,
	// Socket n Command
	W5500_SRB_Sn_CR				= 0x0001,
	// Socket n Interrupt
	W5500_SRB_Sn_IR				= 0x0002,
	// Socket n Status
	W5500_SRB_Sn_SR				= 0x0003,
	// Socket n Source Port
	W5500_SRB_Sn_PORT0				= 0x0004,
	W5500_SRB_Sn_PORT1				= 0x0005,
	// Socket n Destination Hardware Address
	W5500_SRB_Sn_DHAR0				= 0x0006,
	W5500_SRB_Sn_DHAR1				= 0x0007,
	W5500_SRB_Sn_DHAR2				= 0x0008,
	W5500_SRB_Sn_DHAR3				= 0x0009,
	W5500_SRB_Sn_DHAR4				= 0x000A,
	W5500_SRB_Sn_DHAR5				= 0x000B,
	// Socket n Destination IP Address
	W5500_SRB_Sn_DIPR0				= 0x000C,
	W5500_SRB_Sn_DIPR1				= 0x000D,
	W5500_SRB_Sn_DIPR2				= 0x000E,
	W5500_SRB_Sn_DIPR3				= 0x000F,
	// Socket n Destination Port
	W5500_SRB_Sn_DPORT0			= 0x0010,
	W5500_SRB_Sn_DPORT1			= 0x0011,
	// Socket n Maximum Segment Size
	W5500_SRB_Sn_MSSR0				= 0x0012,
	W5500_SRB_Sn_MSSR1				= 0x0013,
	// Reserved						0x0014,
	// Socket n IP TOS 
	W5500_SRB_Sn_TOS				= 0x0015,
	// Socket n IP TTL 
	W5500_SRB_Sn_TTL				= 0x0016,
	// Reserved						0x0017~0x001D
	// Socket n Receive Buffer Size
	W5500_SRB_Sn_RXBUF_SIZE		= 0x001E,
	//Socket n Transmit Buffer Size
	W5500_SRB_Sn_TXBUF_SIZE		= 0x001F,
	// Socket n TX Free Size
	W5500_SRB_Sn_TX_FSR0			= 0x0020,
	W5500_SRB_Sn_TX_FSR1			= 0x0021,
	// Socket n TX Read Pointer
	W5500_SRB_Sn_TX_RD0			= 0x0022,
	W5500_SRB_Sn_TX_RD1			= 0x0023,
	// Socket n TX Write Pointer
	W5500_SRB_Sn_TX_WR0			= 0x0024,
	W5500_SRB_Sn_TX_WR1			= 0x0025,
	// Socket n RX Received Size
	W5500_SRB_Sn_RX_RSR0			= 0x0026,
	W5500_SRB_Sn_RX_RSR1			= 0x0027,
	// Socket n RX Read Pointer
	W5500_SRB_Sn_RX_RD0			= 0x0028,
	W5500_SRB_Sn_RX_RD1			= 0x0029,
	// Socket n RX Write Pointer
	W5500_SRB_Sn_RX_WR0			= 0x002A,
	W5500_SRB_Sn_RX_WR1			= 0x002B,
	// Socket n Interrupt Mask
	W5500_SRB_Sn_IMR				= 0x002C,
	// Socket n Fragment Offset in IP header
	W5500_SRB_Sn_FRAG0				= 0x002D,
	W5500_SRB_Sn_FRAG1				= 0x002E,
	//Keep alive timer 
	W5500_SRB_Sn_KPALVTR			= 0x002F,
	//Reserved						0x0030~0xFFFF
}w5500_sn_addr_t;
typedef enum{ //Mode Register
	// 0x01 -> Reserved
	// Force ARP 
	/* 0 : Disable Force ARP mode  ,  1 : Enable Force ARP mode */
	/* In Force ARP mode, It forces on sending ARP Request whenever data is sent. */
	W5500_MR_FARP		= 0x02,
	// 0x04 -> Reserved
	// PPPoE Mode 
	/* 0 : Disable PPPoE mode  ,  1 : Enable PPPoE mode */
	/* If you use ADSL, this bit should be ‘1’. */
	W5500_MR_PPPoE		= 0x08,
	// Ping Block Mode 
	/* 0 : Disable Ping block  ,  1 : Enable Ping block */
	/* If the bit is ‘1’, it blocks the response to a ping request. */
	W5500_MR_PING_BLOCK = 0x10,
	// Wake on LAN
	/* 0 : Disable WOL mode  ,  1 : Enable WOL mode */
	/* If WOL mode is enabled and the received magic packet over UDP has
		 been normally processed, the Interrupt PIN (INTn) asserts to low. When
		 using WOL mode, the UDP Socket should be opened with any source port
		 number. (Refer to Socket n Mode Register (Sn_MR) for opening Socket.)
		 Notice: The magic packet over UDP supported by W5500 consists of 6
		 bytes synchronization stream (‘0xFFFFFFFFFFFF’) and 16 times Target
		 MAC address stream in UDP payload. The options such like password are
		 ignored. You can use any UDP source port number for WOL mode. */
	W5500_MR_WOL		= 0x20,
	// 0x40 -> Reserved
	// Reset
	/* If this bit is ‘1’, All internal registers will be initialized. It will be
		 automatically cleared as ‘0’ after S/W reset. */
	W5500_MR_RST		= 0x80,
}w5500_mr_bits_t; 
typedef enum{	//Interrupt Register
	// 0x01 & 0x02 & 0x04 & 0x08 -> Reserved
	// Magic Packet
	/* When WOL mode is enabled and receives the magic packet over UDP, this bit is set. */
	W5500_IR_MP				= 0x10,
	// PPPoE Connection Close
	/* When PPPoE is disconnected during PPPoE mode, this bit is set. */
	W5500_IR_PPPoE		= 0x20,
	//Destination unreachable
	/* When receiving the ICMP (Destination port unreachable) packet, this
		 bit is set as ‘1’.
		 When this bit is ‘1’, Destination Information such as IP address and
		 Port number may be checked with the corresponding UIPR &
		 UPORTR. */
	W5500_IR_UNREACH	= 0x40,
	// IP Conflict
	/* Bit is set as ‘1’ when own source IP address is same with the sender
		 IP address in the received ARP request. */
	W5500_IR_CONFLICT = 0x80,
}w5500_ir_bits_t; 
typedef enum{	//Interrupt Mask Register
	// 0x01 & 0x02 & 0x04 & 0x08 -> Reserved
	// Magic Packet Interrupt Mask
	/* 0: Disable Magic Packet Interrupt  ,  1: Enable Magic Packet Interrupt */
	W5500_IMR_IR4	= 0x10,
	// PPPoE Close Interrupt Mask
	/* 0: Disable PPPoE Close Interrupt  ,  1: Enable PPPoE Close Interrupt */
	W5500_IMR_IR5	= 0x20,
	// Destination unreachable Interrupt Mask
	/* 0: Disable Destination unreachable Interrupt
		 1: Enable Destination unreachable Interrupt */
	W5500_IMR_IR6	= 0x40,
	// IP Conflict Interrupt Mask
	/* 0: Disable IP Conflict Interrupt  ,  1: Enable IP Conflict Interrupt */
	W5500_IMR_IR7	= 0x80,
}w5500_imr_bits_t;
typedef enum{	//Socket Interrupt Register
	/* When the interrupt of Socket n occurs, the n-th bit of SIR becomes ‘1’. */
	W5500_SIR_S0		= 0x01,
	W5500_SIR_S1		= 0x02,
	W5500_SIR_S2		= 0x04,
	W5500_SIR_S3		= 0x08,
	W5500_SIR_S4		= 0x10,
	W5500_SIR_S5		= 0x20,
	W5500_SIR_S6		= 0x40,
	W5500_SIR_S7		= 0x80,
}w5500_sir_bits_t;
typedef enum{	//Socket Interrupt Mask Register
	/* When the interrupt of Socket n occurs, the n-th bit of SIR becomes ‘1’. */
	W5500_SIMR_S0		= 0x01,
	W5500_SIMR_S1		= 0x02,
	W5500_SIMR_S2		= 0x04,
	W5500_SIMR_S3		= 0x08,
	W5500_SIMR_S4		= 0x10,
	W5500_SIMR_S5		= 0x20,
	W5500_SIMR_S6		= 0x40,
	W5500_SIMR_S7		= 0x80,
}w5500_simr_bits_t;
typedef enum{	//Operation Mode Configuration Bit
	// Operation Mode Configuration Bit[R/W] 
	/* These bits select the operation mode of PHY such as following table. */
	W5500_PHY_OPMDC_10BT_HD_Dis		= (0x00 << 3),
	W5500_PHY_OPMDC_10BT_FD_Dis		= (0x01 << 3),
	W5500_PHY_OPMDC_100BT_HD_Dis	= (0x02 << 3),
	W5500_PHY_OPMDC_100BT_FD_Dis	= (0x03 << 3),
	W5500_PHY_OPMDC_100BT_HD_En		= (0x04 << 3),
	W5500_PHY_OPMDC_NotUsed			= (0x05 << 3),
	W5500_PHY_OPMDC_PowerDownmode	= (0x06 << 3),
	W5500_PHY_OPMDC_All_En			= (0x07 << 3),
}w5500_phy_opmdc_t;
typedef enum{	//W5500 PHY Configuration Register
	// Link Status [Read Only]
	/* 1: Link up  ,  0: Link down */
	W5500_PHYCFGR_LNK		= 0x01,
	// Speed Status [Read Only]
	/* 1: 100Mpbs based  ,  0: 10Mpbs based */
	W5500_PHYCFGR_SPD		= 0x02,
	// Duplex Status [Read Only]
	/* 1: Full duplex  ,  0: Half duplex */
	W5500_PHYCFGR_DPX		= 0x04,
	//PHYCFG_OPMDX 
	// Configure PHY Operation Mode
	/* 1: Configure with OPMDC[2:0] in PHYCFGR
		 0: Configure with the H/W PINs(PMODE[2:0])
		 This bit configures PHY operation mode with OPMDC[2:0] bits or
		 PMODE[2:0] PINs. When W5500 is reset by POR or RSTn PIN, PHY
		 operation mode is configured with PMODE[2:0] PINs by default. After
		 POR or RSTn reset, user can re-configure PHY operation mode with
		 OPMDC[2:0]. If user wants to re-configure with PMDC[2:0], it should
		 reset PHY by setting the RST bit to ‘0’ after the user configures this
		 bit as ‘1’ and OPMDC[2:0] . */
	W5500_PHYCFGR_OPMD		= 0x40,
	// Reset [R/W]
	/* When this bit is ‘0’, internal PHY is reset.
		 After PHY reset, it should be set as ‘1’. */
	W5500_PHYCFGR_RST		= 0x80,
}w5500_phycfgr_bits_t;
typedef enum{ //W5500_Sokects_protoc
	// 	Protocol -> This configures the protocol mode of Socket n.
	/*  if((SX_M_P0 | SX_M_P1 | SX_M_P2 | SX_M_P3) == 0) => Closed
			if((SX_M_P0 | SX_M_P1 | SX_M_P2 | SX_M_P3) == 1) => TCP
			if((SX_M_P0 | SX_M_P1 | SX_M_P2 | SX_M_P3) == 2) => UDP
			if((SX_M_P0 | SX_M_P1 | SX_M_P2 | SX_M_P3) == 4) => MACRAW */
	/*	MACRAW mode should be only used in Socket 0. */
	W5500_SN_MR_P_Closed			= 0x00,
	W5500_SN_MR_P_TCP					= 0x01,
	W5500_SN_MR_P_UDP					= 0x02,
	W5500_SN_MR_P_MACRAW			= 0x04,
}w5500_sn_proto_t;
typedef enum{	//Socket n Mode Register
	// 	UNICAST Blocking in UDP mode
	/* 	0 : disable Unicast Blocking  ,  1 : enable Unicast Blocking
			This bit blocks receiving the unicast packet during UDP mode(P[3:0] =
			‘0010’) and MULTI = ‘1’. */
	//	IPv6 packet Blocking in MACRAW mode
	/*	0 : disable IPv6 Blocking  ,  1 : enable IPv6 Blocking
			This bit is applied only during MACRAW mode (P[3:0] = ‘0100’).
			It blocks to receiving the IPv6 packet. */
	W5500_SN_MR_UCASTB_MIP6B	= 0x10,
	//	ND -> Use No Delayed ACK
	/*	0 : Disable No Delayed ACK option  ,  1 : Enable No Delayed ACK option
			This bit is applied only during TCP mode (P[3:0] = ‘0001’).
			When this bit is ‘1’, It sends the ACK packet without delay as soon as a
			Data packet is received from a peer. When this bit is ‘0’, It sends the ACK
			packet after waiting for the timeout time configured by RTR. */
	//	MC -> Multicast
	/*	0 : using IGMP version 2  ,  1 : using IGMP version 1
			This bit is applied only during UDP mode(P[3:0] = ‘0010’) and MULTI =‘1’.
			It configures the version for IGMP messages (Join/Leave/Report). */
	// 	MMB -> Multicast Blocking in MACRAW mode
	/*	0 : disable Multicast Blocking  ,  1 : enable Multicast Blocking
			This bit is applied only when MACRAW mode(P[3:0] = ‘0100’). It blocks to
			receive the packet with multicast MAC address. */
	W5500_SN_MR_ND_MC_MMB		= 0x20,
	//	Broadcast Blocking in MACRAW and UDP mode
	/*	0 : disable Broadcast Blocking  ,  1 : enable Broadcast Blocking
			This bit blocks to receive broadcasting packet during UDP mode(P[3:0] =
			‘0010’). In addition, This bit does when MACRAW mode(P[3:0] = ‘0100’) */
	W5500_SN_MR_BCASTB				= 0x40,
	//	MULTI -> Multicasting in UDP mode
	/*	0 : disable Multicasting  ,  1 : enable Multicasting
			This bit is applied only during UDP mode(P[3:0] = ‘0010’).
			To use multicasting, Sn_DIPR & Sn_DPORT should be respectively
			configured with the multicast group IP address & port number before
			Socket n is opened by OPEN command of Sn_CR */
	//	MFEN -> MAC Filter Enable in MACRAW mode
	/*	0 : disable MAC Filtering  ,  1 : enable MAC Filtering
			This bit is applied only during MACRAW mode(P[3:0] = ‘0100’).
			When set as ‘1’, W5500 can only receive broadcasting packet or packet
			sent to itself. When this bit is ‘0’, W5500 can receive all packets on
			Ethernet. If user wants to implement Hybrid TCP/IP stack, it is
			recommended that this bit is set as ‘1’ for reducing host overhead to
			process the all received packets. */
	W5500_SN_MR_MULTI_MFEN		= 0x80,
}w5500_sn_mr_opt_bits_t; // Sn_MR 
typedef enum{	//Socket n Interrupt Register
	//	Sn_IR(CON) Interrupt
	/*	This is issued one time when the connection with peer is successful and
			then Sn_SR is changed to SOCK_ESTABLISHED. */
	W5500_SN_IR_CON			= 0x01,
	//	Sn_IR(DISCON) Interrupt 
	/*	This is issued when FIN or FIN/ACK packet is received from a peer. */
	W5500_SN_IR_DISCON		= 0x02,
	//	Sn_IR(RECV) Interrupt
	/*	This is issued whenever data is received from a peer. */
	W5500_SN_IR_RECV			= 0x04,
	//	Sn_IR(TIMEOUT) Interrupt
	/*	This is issued when ARPTO or TCPTO occurs. */
	W5500_SN_IR_TIMEOUT	= 0x08,
	//	Sn_IR(SENDOK) Interrupt
	/*	This is issued when SEND command is completed. */
	W5500_SN_IR_SEND_OK	= 0x10,
	//	0x20~0x80 -> Reserved
}w5500_sn_ir_bits_t;	// Socket X Interrupt Register
typedef enum{	//Socket n Interrupt Mask Register
	// Sn_IR(CON) Interrupt Mask
	W5500_SN_IMR_CON			= 0x01,
	// Sn_IR(DISCON) Interrupt Mask
	W5500_SN_IMR_DISCON	= 0x02,
	// Sn_IR(RECV) Interrupt Mask
	W5500_SN_IMR_RECV		= 0x04,
	// Sn_IR(TIMEOUT) Interrupt Mask
	W5500_SN_IMR_TIMEOUT	= 0x08,
	// Sn_IR(SENDOK) Interrupt Mask
	W5500_SN_IMR_SENDOK	= 0x10,
	// 0x20~0x80 -> Reserved
}w5500_sn_imr_bits_t; // Socket n Interrupt Mask Register
typedef enum{	//Socket n Command Register
	/*	Socket n is initialized and opened according to the protocol
			selected in Sn_MR (P3:P0). The table below shows the value of
			Sn_SR corresponding to Sn_MR. */
	W5500_SN_CR_OPEN				= 0x01,
	/*	This is valid only in TCP mode (Sn_MR(P3:P0) = Sn_MR_TCP). In this
			mode, Socket n operates as a ‘TCP server’ and waits for
			connection-request (SYN packet) from any ‘TCP client’.
			The Sn_SR changes the state from SOCK_INIT to SOCKET_LISTEN.
			When a ‘TCP client’ connection request is successfully established,
			the Sn_SR changes from SOCK_LISTEN to SOCK_ESTABLISHED and the
			Sn_IR(0) becomes ‘1’. But when a ‘TCP client’ connection request is
			failed, Sn_IR(3) becomes ‘1’ and the status of Sn_SR changes to
			SOCK_CLOSED. */
	W5500_SN_CR_LISTEN			= 0x02,
	/*	This is valid only in TCP mode and operates when Socket n acts as
			‘TCP client’. To connect, a connect-request (SYN packet) is sent to
			‘TCP server’ configured by Sn_DIPR & Sn_DPORT(destination address
			& port). If the connect-request is successful, the Sn_SR is changed
			to SOCK_ESTABLISHED and the Sn_IR(0) becomes ‘1’.
			The connect-request fails in the following three cases.
			1. When a ARPTO occurs (Sn_IR(3)=‘1’) because the destination
			hardware address is not acquired through the ARP-process.
			2. When a SYN/ACK packet is not received and TCPTO (Sn_IR(3) =
			‘1 )
			3. When a RST packet is received instead of a SYN/ACK packet.
			In these cases, Sn_SR is changed to SOCK_CLOSED. */
	W5500_SN_CR_CONNECT		= 0x04,
	/*	Valid only in TCP mode.
			Regardless of ‘TCP server’ or ‘TCP client’, the DISCON command
			processes the disconnect-process (‘Active close’ or ‘Passive close’).
			Active close: it transmits disconnect-request(FIN packet) to the
			connected peer
			Passive close: When FIN packet is received from peer,
			a FIN packet is replied back to the peer.
			When the disconnect-process is successful (that is, FIN/ACK packet
			is received successfully), Sn_SR is changed to SOCK_CLOSED.
			Otherwise, TCPTO occurs (Sn_IR(3)=‘1)= and then Sn_SR is changed
			to SOCK_CLOSED.
			cf> If CLOSE is used instead of DISCON, only Sn_SR is changed to
			SOCK_CLOSED without disconnect-process.
			If a RST packet is received from a peer during communication,
			Sn_SR is unconditionally changed to SOCK_CLOSED. */
	W5500_SN_CR_DISCON			= 0x08,
	/*	Close Socket n. Sn_SR is changed to SOCK_CLOSED. */
	W5500_SN_CR_CLOSE			= 0x10,
	/*	SEND transmits all the data in the Socket n TX buffer. For more
			details, please refer to Socket n TX Free Size Register (Sn_TX_FSR),
			Socket n, TX Write Pointer Register(Sn_TX_WR), and Socket n TX
			Read Pointer Register(Sn_TX_RD). */
	W5500_SN_CR_SEND				= 0x20,
	/*	Valid only in UDP mode.
			The basic operation is same as SEND. Normally SEND transmits data
			after destination hardware address is acquired by the automatic
			ARP-process(Address Resolution Protocol). But SEND_MAC transmits
			data without the automatic ARP-process. In this case, the
			destination hardware address is acquired from Sn_DHAR configured
			by host, instead of APR-process. */
	W5500_SN_CR_SEND_MAC		= 0x21,
	/*	Valid only in TCP mode.
			It checks the connection status by sending 1byte keep-alive packet.
			If the peer cannot respond to the keep-alive packet during timeout
			time, the connection is terminated and the timeout interrupt will
			occur. */
	W5500_SN_CR_SEND_KEEP	= 0x22,
	/*	RECV completes the processing of the received data in Socket n RX
			Buffer by using a RX read pointer register (Sn_RX_RD).
			For more details, refer to Socket n RX Received Size Register
			(Sn_RX_RSR), Socket n RX Write Pointer Register (Sn_RX_WR), and
			Socket n RX Read Pointer Register (Sn_RX_RD). */
	W5500_SN_CR_RECV				= 0x40,
}w5500_sn_cr_cmd_t;	// Socket n Command Register
typedef enum{	// Socket n Status Register
	/* 	Sn_SR indicates the status of Socket n. The status of Socket n is changed by Sn_CR
			or some special control packet as SYN, FIN packet in TCP. */
	/*	This indicates that Socket n is released.
			When DICON, CLOSE command is ordered, or when a
			timeout occurs, it is changed to SOCK_CLOSED regardless of
			previous status. */
	W5500_SN_SR_CLOSED			= 0x00,
	/*	This indicates Socket n is opened with TCP mode.
			It is changed to SOCK_INIT when Sn_MR (P[3:0]) = ‘0001’ and
			OPEN command is ordered.
			After SOCK_INIT, user can use LISTEN /CONNECT command. */
	W5500_SN_SR_INIT				= 0x13,
	/*	This indicates Socket n is operating as ‘TCP server’ mode
			and waiting for connection-request (SYN packet) from a
			peer (‘TCP client’).It will change to SOCK_ESTALBLISHED when
			the connectionrequest is successfully accepted.
			Otherwise it will change to SOCK_CLOSED after TCPTO
			occurred (Sn_IR(TIMEOUT) = ‘1’). */
	W5500_SN_SR_LISTEN			= 0x14,
	/*	This indicates the status of the connection of Socket n.
			It changes to SOCK_ESTABLISHED when the ‘TCP SERVER’
			processed the SYN packet from the ‘TCP CLIENT’ during
			SOCK_LISTEN, or when the CONNECT command is successful.
			During SOCK_ESTABLISHED, DATA packet can be transferred
			using SEND or RECV command. */
	W5500_SN_SR_ESTABLISHED	= 0x17,
	/*	This indicates Socket n received the disconnect-request (FIN
			packet) from the connected peer. This is half-closing status,
			and data can be transferred. For full-closing, DISCON
			command is used. But For just-closing, CLOSE command is
			used. */
	W5500_SN_SR_CLOSE_WAIT	= 0x1C,
	/*	This indicates Socket n is opened in UDP
			mode(Sn_MR(P[3:0]) = ‘0010’).
			It changes to SOCK_UDP when Sn_MR(P[3:0]) = ‘0010’) and
			OPEN command is ordered.
			Unlike TCP mode, data can be transfered without the
			connection-process. */
	W5500_SN_SR_UDP					= 0x22,
	/*	This indicates Socket 0 is opened in MACRAW mode
			(S0_MR(P[3:0]) = ‘0100’)and is valid only in Socket 0.
			It changes to SOCK_MACRAW when S0_MR(P[3:0] = ‘0100’
			and OPEN command is ordered.
			Like UDP mode socket, MACRAW mode Socket 0 can transfer
			a MAC packet (Ethernet frame) without the connectionprocess. */
	W5500_SN_SR_MACRAW			= 0x42,
	/*	The following table shows a temporary status indicated during 
			changing the status of Socket n. */
	/*	This indicates Socket n sent the connect-request packet
			(SYN packet) to a peer.
			It is temporarily shown when Sn_SR is changed from
			SOCK_INIT to SOCK_ESTABLISHED by CONNECT command.
			If connect-accept(SYN/ACK packet) is received from the
			peer at SOCK_SYNSENT, it changes to SOCK_ESTABLISHED.
			Otherwise, it changes to SOCK_CLOSED after TCPTO
			(Sn_IR[TIMEOUT] = ‘1’) is occurred. */
	W5500_SN_SR_SYNSENT			= 0x15,
	/*	It indicates Socket n successfully received the connectrequest 
			packet (SYN packet) from a peer.
			If socket n sends the response (SYN/ACK packet) to the
			peer successfully, it changes to SOCK_ESTABLISHED. If not,
			it changes to SOCK_CLOSED after timeout occurs
			(Sn_IR[TIMEOUT] = ‘1’). */
	W5500_SN_SR_SYNRECV			= 0x16,
	/*	These indicate Socket n is closing.
			These are shown in disconnect-process such as active-close
			and passive-close.
			When Disconnect-process is successfully completed, or when
			timeout occurs, these change to SOCK_CLOSED. */
	W5500_SN_SR_FIN_WAIT		= 0x18,
	W5500_SN_SR_CLOSING			= 0x1A,
	W5500_SN_SR_TIME_WAIT		= 0x1B,
	/*	This indicates Socket n is waiting for the response (FIN/ACK
			packet) to the disconnect-request (FIN packet) by passiveclose.
			It changes to SOCK_CLOSED when Socket n received the
			response successfully, or when timeout occurs
			(Sn_IR[TIMEOUT] = ‘1’). */
	W5500_SN_SR_LAST_ACK		= 0x1D,
}w5500_sn_sr_status_t; // Socket n Status Register
typedef enum{
	W5500_SOCK_BUF_0KB  = 0,
	W5500_SOCK_BUF_1KB  = 1,
	W5500_SOCK_BUF_2KB  = 2,
	W5500_SOCK_BUF_4KB  = 4,
	W5500_SOCK_BUF_8KB  = 8,
    W5500_SOCK_BUF_16KB = 16,
} w5500_sock_bufsize_t;
/*control bit macro*/
#define W5500_CTRL(bsb, rwb, om)   	(((uint8_t)(bsb) << 3) | ((uint8_t)(rwb) << 2) | ((uint8_t)(om) & 0x03))
/*phy byte macro */
#define W5500_PHYCFGR_BUILD(opmd_en, opmdc_val_shifted) ((uint8_t)(W5500_PHYCFGR_RST_MASK) \
	| ((opmd_en) ? W5500_PHYCFGR_OPMD_MASK : 0u) \
	| ((uint8_t)(opmdc_val_shifted) & W5500_PHYCFGR_OPMDC_MASK))
/*phy rest macro*/
/* 2) Assert ریست PHY: بیت RST باید 0 شود */
#define W5500_PHYCFGR_RST_ASSERT(reg_val) ((uint8_t)((reg_val) & (uint8_t)~W5500_PHYCFGR_RST_MASK))

/* 3) Deassert ریست PHY: بیت RST باید 1 شود */
#define W5500_PHYCFGR_RST_DEASSERT(reg_val) ((uint8_t)((reg_val) | W5500_PHYCFGR_RST_MASK))

#endif // W5500_REGS_H