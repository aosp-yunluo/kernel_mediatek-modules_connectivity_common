/*
* Copyright (C) 2011-2014 MediaTek Inc.
*
* This program is free software: you can redistribute it and/or modify it under the terms of the
* GNU General Public License version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file
*    \brief  Declaration of library functions
*
*    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG         "[WMT-IC]"
#define CFG_IC_SOC 1

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "osal_typedef.h"
#include "wmt_ic.h"
#include "wmt_core.h"
#include "wmt_lib.h"
#include "stp_core.h"
#include "mtk_wcn_consys_hw.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define DEFAULT_PATCH_FRAG_SIZE (1000)
#define WMT_PATCH_FRAG_1ST (0x1)
#define WMT_PATCH_FRAG_MID (0x2)
#define WMT_PATCH_FRAG_LAST (0x3)

#define CFG_CHECK_WMT_RESULT (1)
/* BT Port 2 Feature. this command does not need
 *  after coex command is downconfirmed by LC,
 */
#define CFG_WMT_BT_PORT2 (0)

#define CFG_SET_OPT_REG (0)
#define CFG_WMT_I2S_DBGUART_SUPPORT (0)
#define CFG_SET_OPT_REG_SWLA (0)
#define CFG_SET_OPT_REG_MCUCLK (0)
#define CFG_SET_OPT_REG_MCUIRQ (0)

#define CFG_SUBSYS_COEX_NEED 0

#define CFG_WMT_COREDUMP_ENABLE 0

#define CFG_WMT_MULTI_PATCH (1)

#define CFG_WMT_CRYSTAL_TIMING_SET (0)

#define CFG_WMT_SDIO_DRIVING_SET (0)

#define CFG_WMT_UART_HIF_USE (0)

#define CFG_WMT_WIFI_5G_SUPPORT (1)

#define CFG_WMT_PATCH_DL_OPTM (1)
#if CFG_WMT_LTE_COEX_HANDLING
#define CFG_WMT_FILTER_MODE_SETTING (1)
#else
#define CFG_WMT_FILTER_MODE_SETTING (0)
#endif
/* #define MTK_WCN_CMB_MERGE_INTERFACE_SUPPORT (0) */

#define CFG_WMT_POWER_ON_DLM  (1)
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

static UINT8 gFullPatchName[NAME_MAX + 1];
static const WMT_IC_INFO_S *gp_soc_info;
static WMT_PATCH gp_soc_patch_info;
static WMT_CO_CLOCK gCoClockEn = WMT_CO_CLOCK_DIS;
#if 0
static UINT8 WMT_WAKEUP_DIS_GATE_CMD[] = { 0x1, 0x3, 0x01, 0x00, 0x04 };
static UINT8 WMT_WAKEUP_DIS_GATE_EVT[] = { 0x2, 0x3, 0x02, 0x0, 0x0, 0x04 };

static UINT8 WMT_WAKEUP_EN_GATE_CMD[] = { 0x1, 0x3, 0x01, 0x00, 0x05 };
static UINT8 WMT_WAKEUP_EN_GATE_EVT[] = { 0x2, 0x3, 0x02, 0x0, 0x0, 0x05 };
#endif

#if CFG_WMT_UART_HIF_USE
static UINT8 WMT_QUERY_BAUD_CMD[] = { 0x01, 0x04, 0x01, 0x00, 0x02 };
static UINT8 WMT_QUERY_BAUD_EVT_115200[] = { 0x02, 0x04, 0x06, 0x00, 0x00, 0x02, 0x00, 0xC2, 0x01, 0x00 };
static UINT8 WMT_QUERY_BAUD_EVT_X[] = { 0x02, 0x04, 0x06, 0x00, 0x00, 0x02, 0xAA, 0xAA, 0xAA, 0xBB };
static UINT8 WMT_SET_BAUD_CMD_X[] = { 0x01, 0x04, 0x05, 0x00, 0x01, 0xAA, 0xAA, 0xAA, 0xBB };
static UINT8 WMT_SET_BAUD_EVT[] = { 0x02, 0x04, 0x02, 0x00, 0x00, 0x01 };
static UINT8 WMT_SET_WAKEUP_WAKE_CMD_RAW[] = { 0xFF };
static UINT8 WMT_SET_WAKEUP_WAKE_EVT[] = { 0x02, 0x03, 0x02, 0x00, 0x00, 0x03 };
#endif
static UINT8 WMT_QUERY_STP_CMD[] = { 0x01, 0x04, 0x01, 0x00, 0x04 };
static UINT8 WMT_QUERY_STP_EVT_DEFAULT[] = { 0x02, 0x04, 0x06, 0x00, 0x00, 0x04, 0x11, 0x00, 0x00, 0x00 };
static UINT8 WMT_QUERY_STP_EVT[] = { 0x02, 0x04, 0x06, 0x00, 0x00, 0x04, 0xDB, 0x0E, 0x68, 0x01 };
static UINT8 WMT_PATCH_CMD[] = { 0x01, 0x01, 0x00, 0x00, 0x00 };
static UINT8 WMT_PATCH_EVT[] = { 0x02, 0x01, 0x01, 0x00, 0x00 };
static UINT8 WMT_RESET_CMD[] = { 0x01, 0x07, 0x01, 0x00, 0x04 };
static UINT8 WMT_RESET_EVT[] = { 0x02, 0x07, 0x01, 0x00, 0x00 };

#if CFG_WMT_BT_PORT2
static UINT8 WMT_BTP2_CMD[] = { 0x01, 0x10, 0x03, 0x00, 0x01, 0x03, 0x01 };
static UINT8 WMT_BTP2_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };
#endif

/*soc patial patch address cmd & evt need firmware owner provide*/
#if CFG_WMT_MULTI_PATCH
static UINT8 WMT_PATCH_ADDRESS_CMD[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x3c, 0x02, 0x09, 0x02,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
};
static UINT8 WMT_PATCH_ADDRESS_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };

static UINT8 WMT_PATCH_P_ADDRESS_CMD[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0xc4, 0x04, 0x09, 0x02,
		0x00, 0x3f, 0x00, 0x01,
		0xff, 0xff, 0xff, 0xff
};
static UINT8 WMT_PATCH_P_ADDRESS_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };

static UINT8 WMT_PATCH_PDA_CFG_CMD[] = {
		0x01, 0x01, 0x11, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,	/*Target address*/
		0x00, 0x00, 0x00, 0x00,	/*Download size*/
		0x00, 0x00, 0x00, 0x00,	/*Scramble key*/
		0x2f, 0x02, 0x02, 0x00	/*Configuration*/
};
static UINT8 WMT_PATCH_PDA_CFG_EVT[] = { 0x02, 0x01, 0x01, 0x00, 0x00};

static UINT8 WMT_PATCH_ADDRESS_CMD_NEW[] = { 0x01, 0x01, 0x05, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};

static UINT8 WMT_PATCH_ADDRESS_EVT_NEW[] = { 0x02, 0x01, 0x01, 0x00, 0x00};
#endif

/*coex cmd/evt++*/
static UINT8 WMT_COEX_SETTING_CONFIG_CMD[] = { 0x01, 0x10, 0x02, 0x00, 0x01, 0x00 };
static UINT8 WMT_COEX_SETTING_CONFIG_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };

#if CFG_SUBSYS_COEX_NEED
static UINT8 WMT_BT_COEX_SETTING_CONFIG_CMD[] = { 0x01, 0x10, 0x0B,
	0x00, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA
};
static UINT8 WMT_BT_COEX_SETTING_CONFIG_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };

static UINT8 WMT_WIFI_COEX_SETTING_CONFIG_CMD[] = { 0x01, 0x10, 0x0C,
	0x00, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA
};
static UINT8 WMT_WIFI_COEX_SETTING_CONFIG_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };

static UINT8 WMT_PTA_COEX_SETTING_CONFIG_CMD[] = { 0x01, 0x10, 0x0A,
	0x00, 0x04,
	0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xEE, 0xFF, 0xFF, 0xFE
};
static UINT8 WMT_PTA_COEX_SETTING_CONFIG_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };

static UINT8 WMT_MISC_COEX_SETTING_CONFIG_CMD[] = { 0x01, 0x10, 0x09,
	0x00, 0x05,
	0xAA, 0xAA, 0xAA, 0xAA,
	0xBB, 0xBB, 0xBB, 0xBB
};
static UINT8 WMT_MISC_COEX_SETTING_CONFIG_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };
#endif

static UINT8 WMT_EPA_SETTING_CONFIG_CMD[] = { 0x01, 0x02, 0x02, 0x00, 0x0E, 0x00 };
static UINT8 WMT_EPA_SETTING_CONFIG_EVT[] = { 0x02, 0x02, 0x01, 0x00, 0x00 };

/*coex cmd/evt--*/
static UINT8 WMT_SET_STP_CMD[] = { 0x01, 0x04, 0x05, 0x00, 0x03, 0xDB, 0x0E, 0x68, 0x01 };
static UINT8 WMT_SET_STP_EVT[] = { 0x02, 0x04, 0x02, 0x00, 0x00, 0x03 };
static UINT8 WMT_STRAP_CONF_CMD_FM_COMM[] = { 0x01, 0x05, 0x02, 0x00, 0x02, 0x02 };
static UINT8 WMT_STRAP_CONF_EVT[] = { 0x02, 0x05, 0x02, 0x00, 0x00, 0x02 };

#if 0
static UINT8 WMT_SET_OSC32K_BYPASS_CMD[] = { 0x01, 0x0A, 0x01, 0x00, 0x05 };
static UINT8 WMT_SET_OSC32K_BYPASS_EVT[] = { 0x02, 0x0A, 0x01, 0x00, 0x00 };
#endif

#if 0
/* to enable dump feature */
static UINT8 WMT_CORE_DUMP_EN_CMD[] = { 0x01, 0x0F, 0x02, 0x00, 0x03, 0x01 };
static UINT8 WMT_CORE_DUMP_EN_EVT[] = { 0x02, 0x0F, 0x01, 0x00, 0x00 };

/* to get system stack dump when f/w assert */
static UINT8 WMT_CORE_DUMP_LEVEL_01_CMD[] = { 0x1, 0x0F, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static UINT8 WMT_CORE_DUMP_LEVEL_01_EVT[] = { 0x2, 0x0F, 0x01, 0x00, 0x00 };

/* to get task and system stack dump when f/w assert */
static UINT8 WMT_CORE_DUMP_LEVEL_02_CMD[] = { 0x1, 0x0F, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static UINT8 WMT_CORE_DUMP_LEVEL_02_EVT[] = { 0x2, 0x0F, 0x01, 0x00, 0x00 };

/* to get bt related memory dump when f/w assert */
static UINT8 WMT_CORE_DUMP_LEVEL_03_CMD[] = { 0x1, 0x0F, 0x07, 0x00, 0x03, 0x00, 0x00, 0x09, 0xF0, 0x00, 0x0A };
static UINT8 WMT_CORE_DUMP_LEVEL_03_EVT[] = { 0x2, 0x0F, 0x01, 0x00, 0x00 };
#endif
/* to get full dump when f/w assert */
static UINT8 WMT_CORE_DUMP_LEVEL_04_CMD[] = { 0x1, 0x0F, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static UINT8 WMT_CORE_DUMP_LEVEL_04_EVT[] = { 0x2, 0x0F, 0x01, 0x00, 0x00 };

static UINT8 WMT_CORE_CO_CLOCK_CMD[] = { 0x1, 0x0A, 0x02, 0x00, 0x08, 0x03 };
static UINT8 WMT_CORE_CO_CLOCK_EVT[] = { 0x2, 0x0A, 0x01, 0x00, 0x00 };

static UINT8 WMT_CORE_START_RF_CALIBRATION_CMD[] = { 0x1, 0x14, 0x1, 0x00, 0x01 };
static UINT8 WMT_CORE_START_RF_CALIBRATION_EVT[] = { 0x2, 0x14, 0x02, 0x00, 0x00, 0x01 };

#if (MTK_WCN_CMB_MERGE_INTERFACE_SUPPORT)
static UINT8 WMT_SET_I2S_SLAVE_REG_CMD[] = { 0x01, 0x08, 0x10, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
	    , 0x78, 0x00, 0x05, 0x80	/*addr:0x80050078 */
	    , 0x00, 0x00, 0x11, 0x01	/*value:0x11010000 */
	    , 0x00, 0x00, 0x77, 0x07	/*mask:0x07770000 */
};

static UINT8 WMT_SET_I2S_SLAVE_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
};

static UINT8 WMT_SET_DAI_TO_PAD_REG_CMD[] = { 0x01, 0x08, 0x10, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
	    , 0x74, 0x00, 0x05, 0x80	/*addr:0x80050074 */
	    , 0x44, 0x44, 0x00, 0x00	/*value:0x11010000 */
	    , 0x77, 0x77, 0x00, 0x00	/*mask:0x07770000 */
};

static UINT8 WMT_SET_DAI_TO_PAD_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
};

static UINT8 WMT_SET_DAI_REG_CMD[] = { 0x01, 0x08, 0x10, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
	    , 0xA0, 0x00, 0x05, 0x80	/*addr:0x80050074 */
	    , 0x04, 0x00, 0x00, 0x00	/*value:0x11010000 */
	    , 0x04, 0x00, 0x00, 0x00	/*mask:0x07770000 */
};

static UINT8 WMT_SET_DAI_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
};
#endif

#if !(CFG_IC_SOC)		/* For MT6628 no need to set ALLEINT registers, done in f/w */
/* enable all interrupt */
static UINT8 WMT_SET_ALLINT_REG_CMD[] = { 0x01, 0x08, 0x10, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
	    , 0x00, 0x03, 0x05, 0x80	/*addr:0x80050300 */
	    , 0x00, 0xC4, 0x00, 0x00	/*value:0x0000C400 */
	    , 0x00, 0xC4, 0x00, 0x00	/*mask:0x0000C400 */
};

static UINT8 WMT_SET_ALLINT_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
};

#endif

#if CFG_SET_OPT_REG_SWLA	/* enable swla: eesk(7) eecs(8) oscen(19) sck0(24) scs0(25)  */
static UINT8 WMT_SET_SWLA_REG_CMD[] = { 0x01, 0x08, 0x1C, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x02		/*2 registers */
	    , 0x10, 0x01, 0x05, 0x80	/*addr:0x80050110 */
	    , 0x10, 0x10, 0x01, 0x00	/*value:0x00011010 */
	    , 0xF0, 0xF0, 0x0F, 0x00	/*mask:0x000FF0F0 */
	    , 0x40, 0x01, 0x05, 0x80	/*addr:0x80050140 */
	    , 0x00, 0x10, 0x01, 0x00	/*value:0x00011000 */
	    , 0x00, 0xF0, 0x0F, 0x00	/*mask:0x000FF000 */
};

static UINT8 WMT_SET_SWLA_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x02		/*2 registers */
};
#endif

#if CFG_SET_OPT_REG_MCUCLK	/* enable mcu clk: antsel_4, eedi */
static UINT8 WMT_SET_MCUCLK_REG_CMD[] = { 0x01, 0x08, (4 + 12 * 4), 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/* type: reg */
	    , 0x00		/* rev */
	    , 0x04		/* 4 registers */
	    , 0x00, 0x04, 0x00, 0x80	/* addr:0x8000 0400 */
	    , 0x00, 0x14, 0x00, 0x00	/* value:0x0000 1400(osc, hclk), 0x0000 1501(PLL, en) */
	    , 0xFF, 0xFF, 0x00, 0x00	/* mask:0x0000 FFFF */
	    , 0x80, 0x01, 0x05, 0x80	/* addr:0x8005 0180 */
	    , 0x12, 0x13, 0x00, 0x00	/* value:0x0000 1312(osc, hclk), 0x0000 1a19(PLL, en) */
	    , 0xFF, 0xFF, 0x00, 0x00	/* mask:0x0000 FFFF */
	    , 0x00, 0x01, 0x05, 0x80	/* addr:0x8005 0100 */
	    , 0x00, 0x00, 0x02, 0x00	/* value:0x0002 0000 */
	    , 0x00, 0x00, 0x0F, 0x00	/* mask:0x000F 0000 */
	    , 0x10, 0x01, 0x05, 0x80	/* addr:0x8005 0110 */
	    , 0x02, 0x00, 0x00, 0x00	/* value:0x0000 0002 */
	    , 0x0F, 0x00, 0x00, 0x00	/* mask:0x0000 000F */
};

static UINT8 WMT_SET_MCUCLK_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/* S: 0 */
	    , 0x00		/* type: reg */
	    , 0x00		/* rev */
	    , 0x04		/* 4 registers */
};
#endif

#if CFG_WMT_I2S_DBGUART_SUPPORT	/* register write for debug uart */
static UINT8 WMT_SET_DBGUART_REG_CMD[] = { 0x01, 0x08, 0x1C, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x02		/*2 registers */
	    , 0x30, 0x01, 0x05, 0x80	/*addr:0x80050130 */
	    , 0x00, 0x00, 0x00, 0x00	/*value:0x00000000 */
	    , 0xF0, 0x0F, 0x00, 0x00	/*mask:0x00000FF0 */
	    , 0x40, 0x01, 0x05, 0x80	/*addr:0x80050140 */
	    , 0x00, 0x01, 0x00, 0x00	/*value:0x00000100 */
	    , 0x00, 0x01, 0x00, 0x00	/*mask:0x00000100 */
};

static UINT8 WMT_SET_DBGUART_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x02		/*2 registers */
};
#endif

#if CFG_SET_OPT_REG_MCUIRQ	/* enable mcu irq: antsel_4, wlan_act */
#if 1				/* Ray */
static UINT8 WMT_SET_MCUIRQ_REG_CMD[] = { 0x01, 0x08, (4 + 12 * 4), 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/* type: reg */
	    , 0x00		/* rev */
	    , 0x04		/* 4 registers */
	    , 0x00, 0x04, 0x00, 0x80	/* addr:0x8000_0400 */
	    , 0x03, 0x14, 0x00, 0x00	/* value:0x0000_1403 check confg debug flag 3 low word */
	    , 0xFF, 0xFF, 0x00, 0x00	/* mask:0x0000_FFFF */
	    /* cirq_int_n */
	    , 0x10, 0x01, 0x05, 0x80	/* addr:0x8005_0110 */
	    , 0x02, 0x00, 0x00, 0x00	/* value:0x0000_0002 set EEDI as cirq_int_n debug flag (monitor flag2) */
	    , 0x07, 0x00, 0x00, 0x00	/* mask:0x0000_0007 */
	    , 0x00, 0x01, 0x05, 0x80	/* addr:0x8005_0100 */
	    , 0x00, 0x00, 0x02, 0x00	/* value:0x0002_0000 (ANTSEL4=>monitor flag 0, ahb_x2_gt_ck debug flag) */
	    , 0x00, 0x00, 0x07, 0x00	/* mask:0x0007_0000 */
	    /* 1.    ARM irq_b, monitor flag 0 */
	    , 0x80, 0x01, 0x05, 0x80	/* addr:0x8005_0180 */
	    , 0x1F, 0x1E, 0x00, 0x00	/* value:0x0000_1E1F check mcusys debug flag */
	    , 0x7F, 0x7F, 0x00, 0x00	/* mask:0x0000_7F7F */
};

static UINT8 WMT_SET_MCUIRQ_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/* S: 0 */
	    , 0x00		/* type: reg */
	    , 0x00		/* rev */
	    , 0x04		/* 5 registers */
};
#elif 0				/* KC */
static UINT8 WMT_SET_MCUIRQ_REG_CMD[] = { 0x01, 0x08, (4 + 12 * 5), 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/* type: reg */
	    , 0x00		/* rev */
	    , 0x05		/* 5 registers */
	    , 0x00, 0x04, 0x00, 0x80	/* addr:0x8000_0400 */
	    , 0x00, 0x02, 0x00, 0x00	/* value:0x0000_0200 [15:8]=0x2 arm irq_b, 0xA irq_bus[5] bt_timcon_irq_b */
	    , 0x00, 0xFF, 0x00, 0x00	/* mask:0x0000_FF00 */
	    /* 1.    ARM irq_b, monitor flag 0 */
	    , 0x80, 0x01, 0x05, 0x80	/* addr:0x8005_0180 */
	    , 0x18, 0x00, 0x00, 0x00	/* value:0x0000_0018 [6:0]=001_1000 (monitor flag 0 select, MCUSYS, SEL:8) */
	    , 0x7F, 0x00, 0x00, 0x00	/* mask:0x0000_007F */
	    , 0x00, 0x01, 0x05, 0x80	/* addr:0x8005_0100 */
	    , 0x00, 0x00, 0x02, 0x00	/* value:0x0002_0000 (ANTSEL4=>monitor flag 0) */
	    , 0x00, 0x00, 0x07, 0x00	/* mask:0x0007_0000 */
	    /* 2.    irq_bus[5] bt_timcon_irq_b monitor flag 15 */
	    , 0xB0, 0x01, 0x05, 0x80	/* addr:0x8005_01B0 */
	    , 0x00, 0x00, 0x00, 0x16	/* value:0x1600_0000 [30:24]=001_0110 (monitor flag 15 select, MCUSYS, SEL:6) */
	    , 0x00, 0x00, 0x00, 0x7F	/* mask:0x7F00_0000 */
	    , 0x30, 0x01, 0x05, 0x80	/* addr:0x8005_0130 */
	    , 0x00, 0x20, 0x00, 0x00	/* value:0x0000_2000 (WLAN_ACT=>monitor flag 15) */
	    , 0x00, 0x70, 0x00, 0x00	/* mask:0x0000_7000 */
};

static UINT8 WMT_SET_MCUIRQ_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/* S: 0 */
	    , 0x00		/* type: reg */
	    , 0x00		/* rev */
	    , 0x05		/* 5 registers */
};
#endif
#endif

#if CFG_WMT_CRYSTAL_TIMING_SET
static UINT8 WMT_SET_CRYSTAL_TRIMING_CMD[] = { 0x01, 0x12, 0x02, 0x00, 0x01, 0x00 };
static UINT8 WMT_SET_CRYSTAL_TRIMING_EVT[] = { 0x02, 0x12, 0x02, 0x00, 0x01, 0x00 };

static UINT8 WMT_GET_CRYSTAL_TRIMING_CMD[] = { 0x01, 0x12, 0x02, 0x00, 0x00, 0x00 };
static UINT8 WMT_GET_CRYSTAL_TRIMING_EVT[] = { 0x02, 0x12, 0x02, 0x00, 0x00, 0x00 };
#endif

#ifdef CFG_WMT_READ_EFUSE_VCN33
static UINT8 WMT_GET_EFUSE_VCN33_CMD[] = { 0x01, 0x12, 0x02, 0x00, 0x04, 0x00 };
static UINT8 WMT_GET_EFUSE_VCN33_EVT[] = { 0x02, 0x12, 0x02, 0x00, 0x04, 0x00 };
#endif

/* set sdio driving */
#if CFG_WMT_SDIO_DRIVING_SET
static UINT8 WMT_SET_SDIO_DRV_REG_CMD[] = { 0x01, 0x08, 0x10, 0x00	/*length */
	    , 0x01		/* op: w */
	    , 0x01		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
	    , 0x50, 0x00, 0x05, 0x80	/*addr:0x80050050 */
	    , 0x44, 0x44, 0x04, 0x00	/*value:0x00044444 */
	    , 0x77, 0x77, 0x07, 0x00	/*mask:0x00077777 */
};

static UINT8 WMT_SET_SDIO_DRV_REG_EVT[] = { 0x02, 0x08, 0x04, 0x00	/*length */
	    , 0x00		/*S: 0 */
	    , 0x00		/*type: reg */
	    , 0x00		/*rev */
	    , 0x01		/*1 registers */
};
#endif

#if CFG_WMT_WIFI_5G_SUPPORT
static UINT8 WMT_GET_SOC_ADIE_CHIPID_CMD[] = { 0x01, 0x13, 0x04, 0x00, 0x02, 0x04, 0x24, 0x00 };
static UINT8 WMT_GET_SOC_ADIE_CHIPID_EVT[] = {
		0x02, 0x13, 0x09, 0x00, 0x00, 0x02, 0x04, 0x24,
		0x00, 0x00, 0x00, 0x00, 0x00
};
static UINT8 WMT_GET_SOC_6625_L_CMD[] = { 0x01, 0x13, 0x04, 0x00, 0x02, 0x04, 0x20, 0x01 };
static UINT8 WMT_GET_SOC_6625_L_EVT[] = {
		0x02, 0x13, 0x09, 0x00, 0x00, 0x02, 0x04, 0x20,
		0x01, 0x00, 0x00, 0x00, 0x00
};
#endif

#if CFG_WMT_PATCH_DL_OPTM
static UINT8 WMT_SET_MCU_CLK_EN_CMD[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x34, 0x03, 0x00, 0x80,
		0x00, 0x00, 0x01, 0x00,
		0xff, 0xff, 0xff, 0xff
};
static UINT8 WMT_SET_MCU_CLK_EN_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };

static UINT8 WMT_SET_MCU_CLK_138_CMD[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x0c, 0x01, 0x00, 0x80,
		0x59, 0x4d, 0x84, 0x00,
		0xff, 0xff, 0xff, 0xff
};
static UINT8 WMT_SET_MCU_CLK_138_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };

static UINT8 WMT_SET_MCU_CLK_26_CMD[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x0c, 0x01, 0x00, 0x80,
		0x00, 0x4d, 0x84, 0x00,
		0xff, 0xff, 0xff, 0xff
};
static UINT8 WMT_SET_MCU_CLK_26_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };

static UINT8 WMT_SET_MCU_CLK_DIS_CMD[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x34, 0x03, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
};
static UINT8 WMT_SET_MCU_CLK_DIS_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };

/*only for 6797,enable high clock frequency*/
/*CLK EN*/
static UINT8 WMT_SET_MCU_CLK_EN_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x10, 0x11, 0x02, 0x81, 0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x10
};
/*RATIO SET*/
static UINT8 WMT_SET_MCU_RATIO_SET_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x0c, 0x01, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00,
	0xc0, 0x00, 0x00, 0x00
};
/*DIV SET*/
static UINT8 WMT_SET_MCU_DIV_SET_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x18, 0x11, 0x02, 0x80, 0x07, 0x00, 0x00, 0x00,
	0x3f, 0x00, 0x00, 0x00
};
/*HCLK SET*/
static UINT8 WMT_SET_MCU_HCLK_SET_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x00, 0x11, 0x02, 0x81, 0x04, 0x00, 0x00, 0x00,
	0x07, 0x00, 0x00, 0x00
};

/*Change clock to 26MHz*/
/*HCLK DIS*/
static UINT8 WMT_SET_MCU_HCLK_DIS_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x00, 0x11, 0x02, 0x81, 0x00, 0x00, 0x00, 0x00,
	0x07, 0x00, 0x00, 0x00
};
/*RATIO DIS*/
static UINT8 WMT_SET_MCU_RATIO_DIS_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x0c, 0x01, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0x00, 0x00, 0x00
};
/*CLK DIS*/
static UINT8 WMT_SET_MCU_CLK_DIS_6797[] = {
	0x01, 0x08, 0x10, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x10, 0x11, 0x02, 0x81, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x10
};

static UINT8 WMT_SET_MCU_CLK_EVT_6797[] = {
	0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01
};

#endif


static UINT8 WMT_COEX_CONFIG_BT_CTRL_CMD[] = {
		0x01, 0x10, 0x04, 0x00, 0x1A, 0x00, 0x00, 0x00 };
static UINT8 WMT_COEX_CONFIG_ADDJUST_OPP_TIME_RATIO_CMD[] = {
		0x01, 0x10, 0x04, 0x00, 0x1B, 0x00, 0x00, 0x00 };
static UINT8 WMT_COEX_CONFIG_ADDJUST_BLE_SCAN_TIME_RATIO_CMD[] = {
		0x01, 0x10, 0x04, 0x00, 0x1C, 0x00, 0x00, 0x00 };

static UINT8 WMT_COEX_SPLIT_MODE_EVT[] = { 0x02, 0x10, 0x01, 0x00, 0x00 };

#if CFG_WMT_FILTER_MODE_SETTING
static UINT8 WMT_COEX_TDM_REQ_ANTSEL_NUM_CMD[] = { 0x01, 0x10, 0x02, 0x00, 0x14, 0x00 };
static UINT8 WMT_COEX_FILTER_SPEC_CMD_TEST[] = {
		0x01, 0x10, 0x45, 0x00, 0x11, 0x00, 0x00, 0x01,
		0x00, 0x11, 0x11, 0x16, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x63, 0x63, 0x63, 0x00, 0x39, 0x43, 0x63,
		0x63, 0x02, 0x02, 0x03, 0x00, 0x01, 0x01, 0x01,
		0x01, 0x0e, 0x0e, 0x0e, 0x00, 0x0a, 0x0c, 0x0e,
		0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static UINT8 WMT_COEX_LTE_FREQ_IDX_TABLE_CMD[] = {
		0x01, 0x10, 0x21, 0x00, 0x12, 0xfc, 0x08, 0x15,
		0x09, 0x2e, 0x09, 0x47, 0x09, 0xc4, 0x09, 0xd4,
		0x09, 0xe3, 0x09, 0x5a, 0x0a, 0x14, 0x09, 0x2d,
		0x09, 0x46, 0x09, 0x60, 0x09, 0xd3, 0x09, 0xe2,
		0x09, 0x59, 0x0a, 0x8B, 0x0a};
static UINT8 WMT_COEX_LTE_CHAN_UNSAFE_CMD[] = { 0x01, 0x10, 0x02, 0x00, 0x13, 0x00 };
static UINT8 WMT_COEX_IS_LTE_L_CMD[] = { 0x01, 0x10, 0x02, 0x00, 0x21, 0x01 };

#if 0
static UINT8 WMT_COEX_SPLIT_FILTER_CMD_TEST[] = {
		0x01, 0x10, 0x19, 0x00, 0x0F, 0x00, 0x00, 0x00,
		0x00, 0x6c, 0x09, 0x8a, 0x09, 0x8a, 0x09, 0x9e,
		0x09, 0x01, 0x07, 0x07, 0x0b, 0x07, 0x07, 0x00,
		0x32, 0x27, 0x4e, 0x27, 0x32
};

static UINT8 WMT_COEX_FILTER_SPEC_CMD_TEST[] = {
		0x01, 0x10, 0x45, 0x00, 0x11, 0x00, 0x00, 0x01,
		0x00, 0x07, 0x07, 0x07, 0x54, 0x54, 0x00, 0x00,
		0x00, 0x50, 0x50, 0x50, 0x54, 0x54, 0x39, 0x39,
		0x39, 0x02, 0x02, 0x02, 0x0e, 0x0e, 0x01, 0x01,
		0x01, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0a, 0x0a,
		0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};

static UINT8 WMT_COEX_LTE_FREQ_IDX_TABLE_CMD_TEST[] = {
		0x01, 0x10, 0x21, 0x00, 0x12, 0xfc, 0x08, 0x15,
		0x09, 0x2e, 0x09, 0x47, 0x09, 0xc4, 0x09, 0xdd,
		0x09, 0xf6, 0x09, 0x0f, 0xaf, 0x14, 0x09, 0x2d,
		0x09, 0x46, 0x09, 0x5f, 0x09, 0xdd, 0x09, 0xf5,
		0x09, 0x0d, 0x0a, 0x27, 0x0a
};
static UINT8 WMT_COEX_LTE_CHAN_UNSAFE_CMD_TEST[] = { 0x01, 0x10, 0x02, 0x00, 0x13, 0x00 };
static UINT8 WMT_COEX_EXT_COMPONENT_CMD_TEST[] = { 0x01, 0x10, 0x03, 0x00, 0x0d, 0x7f, 0x03 };
#endif

static UINT8 WMT_COEX_FILTER_SPEC_CMD_0[] = {
		0x01, 0x10, 0x45, 0x00, 0x11, 0x00, 0x00, 0x01,
		0x00, 0x16, 0x16, 0x16, 0x16, 0x00, 0x00, 0x00,
		0x00, 0x63, 0x63, 0x63, 0x63, 0x3c, 0x3c, 0x3c,
		0x3c, 0x04, 0x04, 0x04, 0x04, 0x01, 0x01, 0x01,
		0x01, 0x0e, 0x0e, 0x0e, 0x0e, 0x0b, 0x0b, 0x0b,
		0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};

static UINT8 WMT_COEX_LTE_FREQ_IDX_TABLE_CMD_0[] = {
		0x01, 0x10, 0x21, 0x00, 0x12, 0xfc, 0x08, 0x15,
		0x09, 0x2e, 0x09, 0x47, 0x09, 0xc4, 0x09, 0xdd,
		0x09, 0xf6, 0x09, 0x0f, 0x0a, 0x14, 0x09, 0x2d,
		0x09, 0x46, 0x09, 0x5f, 0x09, 0xdd, 0x09, 0xf5,
		0x09, 0x0d, 0x0a, 0x27, 0x0a
};
static UINT8 WMT_COEX_IS_LTE_PROJ_CMD[] = { 0x01, 0x10, 0x02, 0x00, 0x15, 0x01 };

static UINT8 WMT_COEX_FILTER_SPEC_CMD_6752[] = {
		0x01, 0x10, 0x45, 0x00, 0x11, 0x00, 0x00, 0x01,
		0x00, 0x11, 0x11, 0x16, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x63, 0x63, 0x63, 0x00, 0x39, 0x43, 0x63,
		0x63, 0x02, 0x02, 0x03, 0x00, 0x01, 0x01, 0x01,
		0x01, 0x0E, 0x0E, 0x0E, 0x00, 0x0A, 0x0C, 0x0E,
		0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
};

static UINT8 WMT_COEX_LTE_FREQ_IDX_TABLE_CMD_6752[] = {
		0x01, 0x10, 0x21, 0x00, 0x12, 0xFC, 0x08, 0x15,
		0x09, 0x2E, 0x09, 0x47, 0x09, 0xC4, 0x09, 0xD4,
		0x09, 0xE3, 0x09, 0x5A, 0x0A, 0x14, 0x09, 0x2D,
		0x09, 0x46, 0x09, 0x60, 0x09, 0xD3, 0x09, 0xE2,
		0x09, 0x59, 0x0A, 0x8B, 0x0A
};
#endif

static UINT8 WMT_BT_TSSI_FROM_WIFI_CONFIG_CMD[] = {
		0x01, 0x02, 0x04, 0x00, 0x0D, 0x01, 0x1E, 0x00
};

static UINT8 WMT_BT_TSSI_FROM_WIFI_EVENT[] = {
		0x02, 0x02, 0x01, 0x00, 0x00
};

#if CFG_WMT_POWER_ON_DLM
static UINT8 WMT_POWER_CTRL_DLM_CMD1[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x60, 0x00, 0x10, 0x80,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x0f, 0x00, 0x00
};

static UINT8 WMT_POWER_CTRL_DLM_CMD2[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x60, 0x00, 0x10, 0x80,
		0x00, 0x00, 0x00, 0x00,
		0xf0, 0x00, 0x00, 0x00
};

static UINT8 WMT_POWER_CTRL_DLM_CMD3[] = {
		0x01, 0x08, 0x10, 0x00,
		0x01, 0x01, 0x00, 0x01,
		0x60, 0x00, 0x10, 0x80,
		0x00, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00
};
static UINT8 WMT_POWER_CTRL_DLM_EVT[] = { 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01 };
#endif

#if (!CFG_IC_SOC)

/* stp sdio init scripts */
static struct init_script init_table_1_1[] = {
	/* table_1_1 is only applied to common SDIO interface */
	INIT_CMD(WMT_SET_ALLINT_REG_CMD, WMT_SET_ALLINT_REG_EVT, "enable all interrupt"),
	/* applied to MT6628 ? */
	INIT_CMD(WMT_WAKEUP_DIS_GATE_CMD, WMT_WAKEUP_DIS_GATE_EVT, "disable gating"),
};

#endif

static struct init_script init_table_1_2[] = {
	INIT_CMD(WMT_QUERY_STP_CMD, WMT_QUERY_STP_EVT_DEFAULT, "query stp default"),
};

#if CFG_WMT_UART_HIF_USE
static struct init_script init_table_2[] = {
	INIT_CMD(WMT_QUERY_BAUD_CMD, WMT_QUERY_BAUD_EVT_X, "query baud X"),
};
#endif

static struct init_script init_table_3[] = {
	INIT_CMD(WMT_RESET_CMD, WMT_RESET_EVT, "wmt reset"),
#if CFG_WMT_BT_PORT2
	INIT_CMD(WMT_BTP2_CMD, WMT_BTP2_EVT, "set bt port2"),
#endif
};

#if CFG_WMT_CRYSTAL_TIMING_SET
static struct init_script set_crystal_timing_script[] = {
	INIT_CMD(WMT_SET_CRYSTAL_TRIMING_CMD, WMT_SET_CRYSTAL_TRIMING_EVT, "set crystal trim value"),
};

static struct init_script get_crystal_timing_script[] = {
	INIT_CMD(WMT_GET_CRYSTAL_TRIMING_CMD, WMT_GET_CRYSTAL_TRIMING_EVT, "get crystal trim value"),
};
#endif
#ifdef CFG_WMT_READ_EFUSE_VCN33
static struct init_script get_efuse_vcn33_script[] = {
	INIT_CMD(WMT_GET_EFUSE_VCN33_CMD, WMT_GET_EFUSE_VCN33_EVT, "get efuse vcn33 value"),
};
#endif

static struct init_script init_table_4[] = {
	INIT_CMD(WMT_SET_STP_CMD, WMT_SET_STP_EVT, "set stp"),
};

static struct init_script init_table_5[] = {
	INIT_CMD(WMT_QUERY_STP_CMD, WMT_QUERY_STP_EVT, "query stp"),
};

static struct init_script init_table_5_1[] = {
	INIT_CMD(WMT_STRAP_CONF_CMD_FM_COMM, WMT_STRAP_CONF_EVT, "configure FM comm"),
};

static struct init_script init_table_6[] = {
	INIT_CMD(WMT_CORE_DUMP_LEVEL_04_CMD, WMT_CORE_DUMP_LEVEL_04_EVT, "setup core dump level"),
};

static struct init_script calibration_table[] = {
	INIT_CMD(WMT_CORE_START_RF_CALIBRATION_CMD, WMT_CORE_START_RF_CALIBRATION_EVT, "start RF calibration data"),
};

#if CFG_WMT_PATCH_DL_OPTM
static struct init_script set_mcuclk_table_1[] = {
	INIT_CMD(WMT_SET_MCU_CLK_EN_CMD, WMT_SET_MCU_CLK_EN_EVT, "enable set mcu clk"),
	INIT_CMD(WMT_SET_MCU_CLK_138_CMD, WMT_SET_MCU_CLK_138_EVT, "set mcu clk to 138.67MH"),
};

static struct init_script set_mcuclk_table_2[] = {
	INIT_CMD(WMT_SET_MCU_CLK_26_CMD, WMT_SET_MCU_CLK_26_EVT, "set mcu clk to 26MH"),
	INIT_CMD(WMT_SET_MCU_CLK_DIS_CMD, WMT_SET_MCU_CLK_DIS_EVT, "disable set mcu clk"),
};

static struct init_script set_mcuclk_table_3[] = {
	INIT_CMD(WMT_SET_MCU_CLK_EN_6797, WMT_SET_MCU_CLK_EVT_6797, "enable set mcu clk"),
	INIT_CMD(WMT_SET_MCU_RATIO_SET_6797, WMT_SET_MCU_CLK_EVT_6797, "mcu ratio set"),
	INIT_CMD(WMT_SET_MCU_DIV_SET_6797, WMT_SET_MCU_CLK_EVT_6797, "mcu div set"),
	INIT_CMD(WMT_SET_MCU_HCLK_SET_6797, WMT_SET_MCU_CLK_EVT_6797, "set mcu clk to hclk"),
};
static struct init_script set_mcuclk_table_4[] = {
	INIT_CMD(WMT_SET_MCU_HCLK_DIS_6797, WMT_SET_MCU_CLK_EVT_6797, "disable mcu hclk"),
	INIT_CMD(WMT_SET_MCU_RATIO_DIS_6797, WMT_SET_MCU_CLK_EVT_6797, "disable mcu ratio set"),
	INIT_CMD(WMT_SET_MCU_CLK_DIS_6797, WMT_SET_MCU_CLK_EVT_6797, "disable mcu clk set"),
};

#endif

static UINT8 WMT_COEX_EXT_COMPONENT_CMD[] = {0x01, 0x10, 0x03, 0x00, 0x0d, 0x00, 0x00};

static struct init_script set_wifi_ext_component_table[] = {
	INIT_CMD(WMT_COEX_EXT_COMPONENT_CMD, WMT_COEX_SPLIT_MODE_EVT, "wifi ext component"),
};

#if CFG_WMT_FILTER_MODE_SETTING
static struct init_script set_wifi_lte_coex_table_1[] = {
	INIT_CMD(WMT_COEX_FILTER_SPEC_CMD_6752, WMT_COEX_SPLIT_MODE_EVT, "wifi lte coex filter spec"),
	INIT_CMD(WMT_COEX_LTE_FREQ_IDX_TABLE_CMD_6752, WMT_COEX_SPLIT_MODE_EVT, "wifi lte freq idx"),
	INIT_CMD(WMT_COEX_IS_LTE_PROJ_CMD, WMT_COEX_SPLIT_MODE_EVT, "set LTE project"),
};

static struct init_script set_wifi_lte_coex_table_2[] = {
	INIT_CMD(WMT_COEX_FILTER_SPEC_CMD_TEST, WMT_COEX_SPLIT_MODE_EVT, "wifi lte coex filter"),
	INIT_CMD(WMT_COEX_LTE_FREQ_IDX_TABLE_CMD, WMT_COEX_SPLIT_MODE_EVT, "wifi lte freq id table"),
	INIT_CMD(WMT_COEX_LTE_CHAN_UNSAFE_CMD, WMT_COEX_SPLIT_MODE_EVT, "wifi lte unsafe channel"),
	INIT_CMD(WMT_COEX_IS_LTE_L_CMD, WMT_COEX_SPLIT_MODE_EVT, "wifi coex is L branch"),
};

static struct init_script set_wifi_lte_coex_table_0[] = {
#if 0
	INIT_CMD(WMT_COEX_SPLIT_FILTER_CMD_TEST, WMT_COEX_SPLIT_MODE_EVT, "wifi lte coex split filter"),
	INIT_CMD(WMT_COEX_FILTER_SPEC_CMD_TEST, WMT_COEX_SPLIT_MODE_EVT, "wifi lte coex filter spec"),
	INIT_CMD(WMT_COEX_LTE_FREQ_IDX_TABLE_CMD_TEST, WMT_COEX_SPLIT_MODE_EVT, "wifi lte freq idx"),
	INIT_CMD(WMT_COEX_LTE_CHAN_UNSAFE_CMD_TEST, WMT_COEX_SPLIT_MODE_EVT, "wifi lte channel unsafe"),
	INIT_CMD(WMT_COEX_EXT_COMPONENT_CMD_TEST, WMT_COEX_SPLIT_MODE_EVT, "wifi coex ext component"),
#endif
	INIT_CMD(WMT_COEX_FILTER_SPEC_CMD_0, WMT_COEX_SPLIT_MODE_EVT, "def wifi lte coex filter spec"),
	INIT_CMD(WMT_COEX_LTE_FREQ_IDX_TABLE_CMD_0, WMT_COEX_SPLIT_MODE_EVT, "def wifi lte freq idx"),
};

static struct init_script get_tdm_req_antsel_num_table[] = {
	INIT_CMD(WMT_COEX_TDM_REQ_ANTSEL_NUM_CMD, WMT_COEX_SPLIT_MODE_EVT, "get tdm req antsel num"),
};
#endif

static struct init_script bt_tssi_from_wifi_table[] = {
	INIT_CMD(WMT_BT_TSSI_FROM_WIFI_CONFIG_CMD, WMT_BT_TSSI_FROM_WIFI_EVENT, "get bt tssi value from wifi"),
};

static struct init_script coex_config_addjust_table[] = {
	INIT_CMD(WMT_COEX_CONFIG_BT_CTRL_CMD, WMT_COEX_SPLIT_MODE_EVT, "coex config bt ctrl"),
	INIT_CMD(WMT_COEX_CONFIG_ADDJUST_OPP_TIME_RATIO_CMD, WMT_COEX_SPLIT_MODE_EVT, "opp time ratio"),
	INIT_CMD(WMT_COEX_CONFIG_ADDJUST_BLE_SCAN_TIME_RATIO_CMD, WMT_COEX_SPLIT_MODE_EVT, "ble scan time ratio"),

};

#if CFG_SET_OPT_REG
static struct init_script set_registers[] = {
	/* INIT_CMD(WMT_SET_GPS_REG_CMD, WMT_SET_GPS_REG_EVT, "set wmt registers"), */
	/* INIT_CMD(WMT_SET_SDIODRV_REG_CMD, WMT_SET_SDIODRV_REG_EVT, "set SDIO driving registers") */
#if CFG_WMT_I2S_DBGUART_SUPPORT
	INIT_CMD(WMT_SET_DBGUART_REG_CMD, WMT_SET_DBGUART_REG_EVT, "set debug uart registers"),
#endif
#if CFG_SET_OPT_REG_SWLA
	INIT_CMD(WMT_SET_SWLA_REG_CMD, WMT_SET_SWLA_REG_EVT, "set swla registers"),
#endif
#if CFG_SET_OPT_REG_MCUCLK
	INIT_CMD(WMT_SET_MCUCLK_REG_CMD, WMT_SET_MCUCLK_REG_EVT, "set mcuclk dbg registers"),
#endif
#if CFG_SET_OPT_REG_MCUIRQ
	INIT_CMD(WMT_SET_MCUIRQ_REG_CMD, WMT_SET_MCUIRQ_REG_EVT, "set mcu irq dbg registers"),
#endif
};
#endif

static struct init_script coex_table[] = {
	INIT_CMD(WMT_COEX_SETTING_CONFIG_CMD, WMT_COEX_SETTING_CONFIG_EVT, "coex_wmt"),

#if CFG_SUBSYS_COEX_NEED
/* no need in MT6628 */
	INIT_CMD(WMT_BT_COEX_SETTING_CONFIG_CMD, WMT_BT_COEX_SETTING_CONFIG_EVT, "coex_bt"),
	INIT_CMD(WMT_WIFI_COEX_SETTING_CONFIG_CMD, WMT_WIFI_COEX_SETTING_CONFIG_EVT, "coex_wifi"),
	INIT_CMD(WMT_PTA_COEX_SETTING_CONFIG_CMD, WMT_PTA_COEX_SETTING_CONFIG_EVT, "coex_ext_pta"),
	INIT_CMD(WMT_MISC_COEX_SETTING_CONFIG_CMD, WMT_MISC_COEX_SETTING_CONFIG_EVT, "coex_misc"),
#endif
};

static struct init_script epa_table[] = {
	INIT_CMD(WMT_EPA_SETTING_CONFIG_CMD, WMT_EPA_SETTING_CONFIG_EVT, "coex_wmt_epa"),
};

static struct init_script osc_type_table[] = {
	INIT_CMD(WMT_CORE_CO_CLOCK_CMD, WMT_CORE_CO_CLOCK_EVT, "osc_type"),
};

#if (MTK_WCN_CMB_MERGE_INTERFACE_SUPPORT)
static struct init_script merge_pcm_table[] = {
	INIT_CMD(WMT_SET_I2S_SLAVE_REG_CMD, WMT_SET_I2S_SLAVE_REG_EVT, "I2S_Slave"),
	INIT_CMD(WMT_SET_DAI_TO_PAD_REG_CMD, WMT_SET_DAI_TO_PAD_REG_EVT, "DAI_PAD"),
	INIT_CMD(WMT_SET_DAI_REG_CMD, WMT_SET_DAI_REG_EVT, "DAI_EVT"),
};
#endif

#if CFG_WMT_SDIO_DRIVING_SET
static struct init_script sdio_driving_table[] = {
	INIT_CMD(WMT_SET_SDIO_DRV_REG_CMD, WMT_SET_SDIO_DRV_REG_EVT, "sdio_driving"),
};
#endif

#if CFG_WMT_POWER_ON_DLM
static struct init_script wmt_power_on_dlm_table[] = {
	INIT_CMD(WMT_POWER_CTRL_DLM_CMD1, WMT_POWER_CTRL_DLM_EVT, "power on dlm cmd1"),
	INIT_CMD(WMT_POWER_CTRL_DLM_CMD2, WMT_POWER_CTRL_DLM_EVT, "power on dlm cmd2"),
	INIT_CMD(WMT_POWER_CTRL_DLM_CMD3, WMT_POWER_CTRL_DLM_EVT, "power on dlm cmd3")
};
#endif

/* SOC Chip Version and Info Table */
static const WMT_IC_INFO_S mtk_wcn_soc_info_table[] = {
	{
	 .u4HwVer = 0x8A00,
	 .cChipName = WMT_IC_NAME_DEFAULT,
	 .cChipVersion = WMT_IC_VER_E1,
	 .cPatchNameExt = WMT_IC_PATCH_E1_EXT,
	 /* need to refine? */
	 .eWmtHwVer = WMTHWVER_E1,
	 .bWorkWithoutPatch = MTK_WCN_BOOL_FALSE,
	 .bPsmSupport = MTK_WCN_BOOL_TRUE,
	 },
	{
	 .u4HwVer = 0x8A01,
	 .cChipName = WMT_IC_NAME_DEFAULT,
	 .cChipVersion = WMT_IC_VER_E2,
	 .cPatchNameExt = WMT_IC_PATCH_E1_EXT,
	 .eWmtHwVer = WMTHWVER_E2,
	 .bWorkWithoutPatch = MTK_WCN_BOOL_FALSE,
	 .bPsmSupport = MTK_WCN_BOOL_TRUE,
	 },
	{
	 .u4HwVer = 0x8B00,
	 .cChipName = WMT_IC_NAME_DEFAULT,
	 .cChipVersion = WMT_IC_VER_E2,
	 .cPatchNameExt = WMT_IC_PATCH_E1_EXT,
	 .eWmtHwVer = WMTHWVER_E2,
	 .bWorkWithoutPatch = MTK_WCN_BOOL_FALSE,
	 .bPsmSupport = MTK_WCN_BOOL_TRUE,
	 },
	{
	 .u4HwVer = 0x8B01,
	 .cChipName = WMT_IC_NAME_DEFAULT,
	 .cChipVersion = WMT_IC_VER_E3,
	 .cPatchNameExt = WMT_IC_PATCH_E1_EXT,
	 .eWmtHwVer = WMTHWVER_E3,
	 .bWorkWithoutPatch = MTK_WCN_BOOL_FALSE,
	 .bPsmSupport = MTK_WCN_BOOL_TRUE,
	 },
	{
	 .u4HwVer = 0x8C00,
	 .cChipName = WMT_IC_NAME_DEFAULT,
	 .cChipVersion = WMT_IC_VER_E2,
	 .cPatchNameExt = WMT_IC_PATCH_E1_EXT,
	 .eWmtHwVer = WMTHWVER_E2,
	 .bWorkWithoutPatch = MTK_WCN_BOOL_FALSE,
	 .bPsmSupport = MTK_WCN_BOOL_TRUE,
	 }
};

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
static MTK_WCN_BOOL mtk_wcn_soc_trigger_assert(VOID);

static INT32 mtk_wcn_soc_sw_init(P_WMT_HIF_CONF pWmtHifConf);

static INT32 mtk_wcn_soc_sw_deinit(P_WMT_HIF_CONF pWmtHifConf);

static INT32 mtk_wcn_soc_pin_ctrl(WMT_IC_PIN_ID id, WMT_IC_PIN_STATE state, UINT32 flag);

static INT32 mtk_wcn_soc_aif_ctrl(WMT_IC_PIN_STATE state, UINT32 flag);

static INT32 mtk_wcn_soc_ver_check(VOID);

static const WMT_IC_INFO_S *mtk_wcn_soc_find_wmt_ic_info(const UINT32 hw_ver);

static INT32 wmt_stp_init_coex(VOID);

static INT32 wmt_stp_init_epa(VOID);

#if CFG_WMT_FILTER_MODE_SETTING
static INT32 wmt_stp_wifi_lte_coex(VOID);
#endif

#if CFG_WMT_MULTI_PATCH
static INT32 mtk_wcn_soc_patch_dwn(UINT32 index);
static INT32 mtk_wcn_soc_patch_info_prepare(VOID);
static UINT32 mtk_wcn_soc_get_patch_num(VOID);
static INT32 mtk_wcn_soc_normal_patch_dwn(PUINT8 pPatchBuf, UINT32 patchSize, PUINT8 addressByte);
static INT32 mtk_wcn_soc_pda_patch_dwn(PUINT8 pPatchBuf, UINT32 patchSize, PUINT8 addressByte);
#else
static INT32 mtk_wcn_soc_patch_dwn(VOID);
#endif

static INT32 mtk_wcn_soc_co_clock_ctrl(WMT_CO_CLOCK on);
static WMT_CO_CLOCK mtk_wcn_soc_co_clock_get(VOID);

#if CFG_WMT_CRYSTAL_TIMING_SET
static INT32 mtk_wcn_soc_crystal_triming_set(VOID);
#endif

static MTK_WCN_BOOL mtk_wcn_soc_quick_sleep_flag_get(VOID);

static MTK_WCN_BOOL mtk_wcn_soc_aee_dump_flag_get(VOID);

#if CFG_WMT_SDIO_DRIVING_SET
static INT32 mtk_wcn_soc_set_sdio_driving(void);
#endif
/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/* SOC Operation Function Table */
WMT_IC_OPS wmt_ic_ops_soc = {
	.icId = 0x0000,		/* soc may have mt6572/82/71/83,but they have the same sw init flow */
	.sw_init = mtk_wcn_soc_sw_init,
	.sw_deinit = mtk_wcn_soc_sw_deinit,
	.ic_pin_ctrl = mtk_wcn_soc_pin_ctrl,
	.ic_ver_check = mtk_wcn_soc_ver_check,
	.co_clock_ctrl = mtk_wcn_soc_co_clock_ctrl,
	.is_quick_sleep = mtk_wcn_soc_quick_sleep_flag_get,
	.is_aee_dump_support = mtk_wcn_soc_aee_dump_flag_get,
	.trigger_stp_assert = mtk_wcn_soc_trigger_assert,
	.deep_sleep_ctrl = NULL,
};

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

static INT32 mtk_wcn_soc_sw_init(P_WMT_HIF_CONF pWmtHifConf)
{
	INT32 iRet = -1;
	unsigned long ctrlPa1;
	unsigned long ctrlPa2;
	UINT32 hw_ver;
	WMT_CTRL_DATA ctrlData;
#ifdef CFG_WMT_READ_EFUSE_VCN33
	UINT32 efuse_d3_vcn33 = 2; /*default voltage is 3.5V*/
#endif
#if CFG_WMT_MULTI_PATCH
	UINT32 patch_num = 0;
	UINT32 patch_index = 0;
#endif
#if CFG_WMT_WIFI_5G_SUPPORT
	UINT32 dDieChipid = 0;
	UINT32 aDieChipid = 0;
	UINT8 evtbuf[20];
	UINT32 u4Res;
	UINT32 pmicChipid = 0;
#endif
	P_WMT_GEN_CONF pWmtGenConf = NULL;

	WMT_DBG_FUNC(" start\n");

	osal_assert(gp_soc_info != NULL);
	if ((gp_soc_info == NULL)
	    || (pWmtHifConf == NULL)
	    ) {
		WMT_ERR_FUNC("null pointers: gp_soc_info(0x%p), pWmtHifConf(0x%p)\n", gp_soc_info, pWmtHifConf);
		return -1;
	}

	hw_ver = gp_soc_info->u4HwVer;

	/* 4 <3.2> start init for BTIF */
	if (pWmtHifConf->hifType == WMT_HIF_BTIF) {
		/* 1. Query chip STP default options (TEST-ONLY) */
		/* WMT_DBG_FUNC("WMT-CORE: init_table_1_2 set chip baud:%d", pWmtHifConf->au4HifConf[0]); */
		iRet = wmt_core_init_script(init_table_1_2, osal_array_size(init_table_1_2));
		if (iRet) {
			WMT_ERR_FUNC("init_table_1_2 fail(%d)\n", iRet);
			osal_assert(0);
			return -2;
		}

		/* 2. Set chip STP options */
		iRet = wmt_core_init_script(init_table_4, osal_array_size(init_table_4));
		if (iRet) {
			WMT_ERR_FUNC("init_table_4 fail(%d)\n", iRet);
			return -3;
		}

		/* 3. Enable host full mode */
		ctrlPa1 = WMT_STP_CONF_MODE;
		ctrlPa2 = MTKSTP_BTIF_FULL_MODE;
		iRet = wmt_core_ctrl(WMT_CTRL_STP_CONF, &ctrlPa1, &ctrlPa2);
		ctrlPa1 = WMT_STP_CONF_EN;
		ctrlPa2 = 1;
		iRet += wmt_core_ctrl(WMT_CTRL_STP_CONF, &ctrlPa1, &ctrlPa2);
		if (iRet) {
			WMT_ERR_FUNC("enable host STP-BTIF-FULL mode fail(%d)\n", iRet);
			return -4;
		}
		WMT_DBG_FUNC("enable host STP-BTIF-FULL mode\n");
		/*4. wait for 10ms, enough for chip do mechanism switch.(at least 2ms is needed) */
		osal_sleep_ms(10);
		/* 5. Query chip STP options (TEST-ONLY) */
		iRet = wmt_core_init_script(init_table_5, osal_array_size(init_table_5));
		if (iRet) {
			WMT_ERR_FUNC("init_table_5 fail(%d)\n", iRet);
			return -5;
		}
	}
#if CFG_WMT_POWER_ON_DLM
	iRet = wmt_core_init_script(wmt_power_on_dlm_table, osal_array_size(wmt_power_on_dlm_table));
	if (iRet)
		WMT_ERR_FUNC("wmt_power_on_dlm_table fail(%d)\n", iRet);
	WMT_DBG_FUNC("wmt_power_on_dlm_table ok\n");
#endif

	/* turn on VCN28 for reading efuse */
	ctrlPa1 = EFUSE_PALDO;
	ctrlPa2 = PALDO_ON;
	iRet = wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);

	/* 6. download patch */
#if CFG_WMT_MULTI_PATCH
	/* 6.1 Let launcher to search patch info */
	/* 6.2 Read patch number */
	/* If patch number is 0, it's first time connys power on */
	patch_num = mtk_wcn_soc_get_patch_num();
	if (patch_num == 0) {
		iRet = mtk_wcn_soc_patch_info_prepare();
		if (iRet) {
			WMT_ERR_FUNC("patch info perpare fail(%d)\n", iRet);
			return -6;
		}
		patch_num = mtk_wcn_soc_get_patch_num();
	}
#if CFG_WMT_PATCH_DL_OPTM
	if (wmt_ic_ops_soc.icId == 0x0279 ||
		wmt_ic_ops_soc.icId == 0x0507 ||
		wmt_ic_ops_soc.icId == 0x0713 ||
		wmt_ic_ops_soc.icId == 0x0788 ||
		wmt_ic_ops_soc.icId == 0x6765 ||
		wmt_ic_ops_soc.icId == 0x0688) {
		iRet = wmt_core_init_script(set_mcuclk_table_3, osal_array_size(set_mcuclk_table_3));
		if (iRet)
			WMT_ERR_FUNC("set_mcuclk_table_3 fail(%d)\n", iRet);
	} else {
		iRet = wmt_core_init_script(set_mcuclk_table_1, osal_array_size(set_mcuclk_table_1));
		if (iRet)
			WMT_ERR_FUNC("set_mcuclk_table_1 fail(%d)\n", iRet);
	}
#endif
	/* 6.3 Multi-patch Patch download */
	for (patch_index = 0; patch_index < patch_num; patch_index++) {
		iRet = mtk_wcn_soc_patch_dwn(patch_index);
		if (iRet) {
			WMT_ERR_FUNC("patch dwn fail (%d),patch_index(%d)\n", iRet, patch_index);
			return -7;
		}
		iRet = wmt_core_init_script(init_table_3, osal_array_size(init_table_3));
		if (iRet) {
			WMT_ERR_FUNC("init_table_3 fail(%d)\n", iRet);
			return -8;
		}
	}

#if CFG_WMT_PATCH_DL_OPTM
	if (wmt_ic_ops_soc.icId == 0x0279 ||
		wmt_ic_ops_soc.icId == 0x0507 ||
		wmt_ic_ops_soc.icId == 0x0713 ||
		wmt_ic_ops_soc.icId == 0x0788 ||
		wmt_ic_ops_soc.icId == 0x6765 ||
		wmt_ic_ops_soc.icId == 0x0688) {
		iRet = wmt_core_init_script(set_mcuclk_table_4, osal_array_size(set_mcuclk_table_4));
		if (iRet)
			WMT_ERR_FUNC("set_mcuclk_table_4 fail(%d)\n", iRet);
	} else {
		iRet = wmt_core_init_script(set_mcuclk_table_2, osal_array_size(set_mcuclk_table_2));
		if (iRet)
			WMT_ERR_FUNC("set_mcuclk_table_2 fail(%d)\n", iRet);
	}
#endif

#else
	/* 6.3 Patch download */
	iRet = mtk_wcn_soc_patch_dwn();
	/* If patch download fail, we just ignore this error and let chip init process goes on */
	if (iRet)
		WMT_ERR_FUNC("patch dwn fail (%d), just omit\n", iRet);

	/* 6.4. WMT Reset command */
	iRet = wmt_core_init_script(init_table_3, osal_array_size(init_table_3));
	if (iRet) {
		WMT_ERR_FUNC("init_table_3 fail(%d)\n", iRet);
		return -8;
	}
#endif

#ifdef CFG_WMT_READ_EFUSE_VCN33
	/*get CrystalTiming value before set it */
	iRet = wmt_core_tx(get_efuse_vcn33_script[0].cmd, get_efuse_vcn33_script[0].cmdSz, &u4Res,
			MTK_WCN_BOOL_FALSE);
	if (iRet || (u4Res != get_efuse_vcn33_script[0].cmdSz)) {
		WMT_ERR_FUNC("WMT-CORE: write (%s) iRet(%d) cmd len err(%d, %d)\n",
				 get_efuse_vcn33_script[0].str, iRet, u4Res, get_efuse_vcn33_script[0].cmdSz);
	}
	/* EVENT BUF */
	osal_memset(get_efuse_vcn33_script[0].evt, 0, get_efuse_vcn33_script[0].evtSz);
	iRet = wmt_core_rx(get_efuse_vcn33_script[0].evt, get_efuse_vcn33_script[0].evtSz, &u4Res);
	if (iRet || (u4Res != get_efuse_vcn33_script[0].evtSz)) {
		WMT_ERR_FUNC("WMT-CORE: read (%s) iRet(%d) evt len err(rx:%d, exp:%d)\n",
				 get_efuse_vcn33_script[0].str, iRet, u4Res, get_efuse_vcn33_script[0].evtSz);
		mtk_wcn_stp_dbg_dump_package();
	}
	efuse_d3_vcn33 = WMT_GET_EFUSE_VCN33_EVT[5] & 0x03;
	WMT_INFO_FUNC("Read efuse to set PMIC voltage:(%d)\n", efuse_d3_vcn33);
	wmt_set_pmic_voltage(efuse_d3_vcn33);
#endif
	pWmtGenConf = wmt_get_gen_conf_pointer();
	if (wmt_ic_ops_soc.icId == 0x0279 ||
		wmt_ic_ops_soc.icId == 0x0507 ||
		wmt_ic_ops_soc.icId == 0x0713 ||
		wmt_ic_ops_soc.icId == 0x0788 ||
		wmt_ic_ops_soc.icId == 0x6765 ||
		wmt_ic_ops_soc.icId == 0x0688) {
		/* add WMT_COXE_CONFIG_EXT_COMPONENT_OPCODE command for 2G4 eLNA demand*/
		if (pWmtGenConf->coex_wmt_ext_component) {
			WMT_INFO_FUNC("coex_wmt_ext_component:0x%x\n", pWmtGenConf->coex_wmt_ext_component);
			set_wifi_ext_component_table[0].cmd[5] = pWmtGenConf->coex_wmt_ext_component;
		}
		iRet = wmt_core_init_script(set_wifi_ext_component_table,
				osal_array_size(set_wifi_ext_component_table));
		if (iRet)
			WMT_ERR_FUNC("wmt_core:set_wifi_ext_component_table %s(%d)\n",
					iRet ? "fail" : "ok", iRet);
	}

#if CFG_WMT_FILTER_MODE_SETTING
	if ((wmt_ic_ops_soc.icId == 0x6580) ||
		(wmt_ic_ops_soc.icId == 0x8163) ||
		(wmt_ic_ops_soc.icId == 0x6752) ||
		(wmt_ic_ops_soc.icId == 0x6582) ||
		(wmt_ic_ops_soc.icId == 0x6592) ||
		(wmt_ic_ops_soc.icId == 0x0279) ||
		(wmt_ic_ops_soc.icId == 0x0507) ||
		(wmt_ic_ops_soc.icId == 0x0326) ||
		(wmt_ic_ops_soc.icId == 0x0551) ||
		(wmt_ic_ops_soc.icId == 0x0321) ||
		(wmt_ic_ops_soc.icId == 0x0335) ||
		(wmt_ic_ops_soc.icId == 0x0688) ||
		(wmt_ic_ops_soc.icId == 0x0713) ||
		(wmt_ic_ops_soc.icId == 0x0788) ||
		(wmt_ic_ops_soc.icId == 0x0699) ||
		(wmt_ic_ops_soc.icId == 0x0337) ||
		(wmt_ic_ops_soc.icId == 0x6765) ||
		(wmt_ic_ops_soc.icId == 0x0633)) {
		wmt_stp_wifi_lte_coex();
		WMT_DBG_FUNC("wmt_stp_wifi_lte_coex done!\n");
	}
	if ((wmt_ic_ops_soc.icId == 0x6582) || (wmt_ic_ops_soc.icId == 0x6592)) {
		/*get gpio tdm req antsel number */
		ctrlPa1 = 0;
		ctrlPa2 = 0;
		wmt_core_ctrl(WMT_CTRL_GET_TDM_REQ_ANTSEL, &ctrlPa1, &ctrlPa2);
		WMT_INFO_FUNC("get GPIO TDM REQ ANTSEL number(%d)\n", ctrlPa1);
		/*set gpio tdm req antsel number to firmware */
		WMT_COEX_TDM_REQ_ANTSEL_NUM_CMD[5] = ctrlPa1;
		iRet = wmt_core_init_script(get_tdm_req_antsel_num_table,
		    osal_array_size(get_tdm_req_antsel_num_table));
		if (iRet)
			WMT_ERR_FUNC("get_tdm_req_antsel_num_table fail(%d)\n", iRet);
	}
#endif
	WMT_INFO_FUNC("bt_tssi_from_wifi=%d, bt_tssi_target=%d\n",
		      pWmtGenConf->bt_tssi_from_wifi, pWmtGenConf->bt_tssi_target);
	if (pWmtGenConf->bt_tssi_from_wifi) {
		if (wmt_ic_ops_soc.icId == 0x0713 ||
		    wmt_ic_ops_soc.icId == 0x6765 ||
		    wmt_ic_ops_soc.icId == 0x0788)
			WMT_BT_TSSI_FROM_WIFI_CONFIG_CMD[4] = 0x10;

		WMT_BT_TSSI_FROM_WIFI_CONFIG_CMD[5] = pWmtGenConf->bt_tssi_from_wifi;
		WMT_BT_TSSI_FROM_WIFI_CONFIG_CMD[6] = (pWmtGenConf->bt_tssi_target & 0x00FF) >> 0;
		WMT_BT_TSSI_FROM_WIFI_CONFIG_CMD[7] = (pWmtGenConf->bt_tssi_target & 0xFF00) >> 8;
		iRet = wmt_core_init_script(bt_tssi_from_wifi_table, osal_array_size(bt_tssi_from_wifi_table));
		if (iRet)
			WMT_ERR_FUNC("bt_tssi_from_wifi_table fail(%d)\n", iRet);
	}

	/* init epa before start RF calibration */
	iRet = wmt_stp_init_epa();

	if (iRet) {
		WMT_ERR_FUNC("init_epa fail(%d)\n", iRet);
		return -20;
	}

	/* 7. start RF calibration data */
	ctrlPa1 = BT_PALDO;
	ctrlPa2 = PALDO_ON;
	iRet = wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);
	ctrlPa1 = WIFI_PALDO;
	ctrlPa2 = PALDO_ON;
	iRet = wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);

	iRet = wmt_core_init_script(calibration_table, osal_array_size(calibration_table));
	if (iRet) {
		/* pwrap_read(0x0210,&ctrlPa1); */
		/* pwrap_read(0x0212,&ctrlPa2); */
		WMT_ERR_FUNC("power status: 210:(%d),212:(%d)!\n", ctrlPa1, ctrlPa2);
		WMT_ERR_FUNC("calibration_table fail(%d)\n", iRet);
		return -9;
	}

	ctrlPa1 = BT_PALDO;
	ctrlPa2 = PALDO_OFF;
	iRet = wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);
	ctrlPa1 = WIFI_PALDO;
	ctrlPa2 = PALDO_OFF;
	iRet = wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);

	/* turn off VCN28 after reading efuse */
	ctrlPa1 = EFUSE_PALDO;
	ctrlPa2 = PALDO_OFF;
	iRet = wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);

	iRet = wmt_stp_init_coex();
	if (iRet) {
		WMT_ERR_FUNC("init_coex fail(%d)\n", iRet);
		return -10;
	}
	WMT_DBG_FUNC("init_coex ok\n");

	if (wmt_ic_ops_soc.icId == 0x0788) {
		WMT_INFO_FUNC("coex_config_bt_ctrl:0x%x\n", pWmtGenConf->coex_config_bt_ctrl);
		coex_config_addjust_table[0].cmd[5] = pWmtGenConf->coex_config_bt_ctrl;
		WMT_INFO_FUNC("coex_config_bt_ctrl_mode:0x%x\n", pWmtGenConf->coex_config_bt_ctrl_mode);
		coex_config_addjust_table[0].cmd[6] = pWmtGenConf->coex_config_bt_ctrl_mode;
		WMT_INFO_FUNC("coex_config_bt_ctrl_rw:0x%x\n", pWmtGenConf->coex_config_bt_ctrl_rw);
		coex_config_addjust_table[0].cmd[7] = pWmtGenConf->coex_config_bt_ctrl_rw;

		WMT_INFO_FUNC("coex_config_addjust_opp_time_ratio:0x%x\n",
				pWmtGenConf->coex_config_addjust_opp_time_ratio);
		coex_config_addjust_table[1].cmd[5] = pWmtGenConf->coex_config_addjust_opp_time_ratio;
		WMT_INFO_FUNC("coex_config_addjust_opp_time_ratio_bt_slot:0x%x\n",
				pWmtGenConf->coex_config_addjust_opp_time_ratio_bt_slot);
		coex_config_addjust_table[1].cmd[6] =
			pWmtGenConf->coex_config_addjust_opp_time_ratio_bt_slot;
		WMT_INFO_FUNC("coex_config_addjust_opp_time_ratio_wifi_slot:0x%x\n",
				pWmtGenConf->coex_config_addjust_opp_time_ratio_wifi_slot);
		coex_config_addjust_table[1].cmd[7] =
			pWmtGenConf->coex_config_addjust_opp_time_ratio_wifi_slot;

		WMT_INFO_FUNC("coex_config_addjust_ble_scan_time_ratio:0x%x\n",
				pWmtGenConf->coex_config_addjust_ble_scan_time_ratio);
		coex_config_addjust_table[2].cmd[5] =
			pWmtGenConf->coex_config_addjust_ble_scan_time_ratio;
		WMT_INFO_FUNC("coex_config_addjust_ble_scan_time_ratio_bt_slot:0x%x\n",
				pWmtGenConf->coex_config_addjust_ble_scan_time_ratio_bt_slot);
		coex_config_addjust_table[2].cmd[6] =
			pWmtGenConf->coex_config_addjust_ble_scan_time_ratio_bt_slot;
		WMT_INFO_FUNC("coex_config_addjust_ble_scan_time_ratio_wifi_slot:0x%x\n",
				pWmtGenConf->coex_config_addjust_ble_scan_time_ratio_wifi_slot);
		coex_config_addjust_table[2].cmd[7] =
			pWmtGenConf->coex_config_addjust_ble_scan_time_ratio_wifi_slot;

		iRet = wmt_core_init_script(coex_config_addjust_table,
				osal_array_size(coex_config_addjust_table));
		if (iRet)
			WMT_ERR_FUNC("wmt_core:coex_config_addjust_table %s(%d)\n",
					iRet ? "fail" : "ok", iRet);
	}

#if CFG_WMT_CRYSTAL_TIMING_SET
	mtk_wcn_soc_crystal_triming_set();
#endif

#if CFG_WMT_SDIO_DRIVING_SET
	mtk_wcn_soc_set_sdio_driving();
#endif

	if (mtk_wcn_soc_co_clock_get() == WMT_CO_CLOCK_EN) {
		WMT_INFO_FUNC("co-clock enabled.\n");

		iRet = wmt_core_init_script(osc_type_table, osal_array_size(osc_type_table));
		if (iRet) {
			WMT_ERR_FUNC("osc_type_table fail(%d), goes on\n", iRet);
			return -11;
		}
	} else {
		WMT_WARN_FUNC("co-clock disabled.\n");
	}
#if (MTK_WCN_CMB_MERGE_INTERFACE_SUPPORT)
	iRet = wmt_core_init_script(merge_pcm_table, osal_array_size(merge_pcm_table));
	if (iRet) {
		WMT_ERR_FUNC("merge_pcm_table fail(%d), goes on\n", iRet);
		return -12;
	}
#endif

	/* 15. Set FM strap */
	WMT_STRAP_CONF_CMD_FM_COMM[5] = (UINT8) pWmtHifConf->au4StrapConf[0];
	WMT_STRAP_CONF_EVT[5] = (UINT8) pWmtHifConf->au4StrapConf[0];
	iRet = wmt_core_init_script(init_table_5_1, osal_array_size(init_table_5_1));
	if (iRet) {
		WMT_ERR_FUNC("init_table_5_1 fm mode(%d) fail(%d)\n", pWmtHifConf->au4StrapConf[0], iRet);
		return -13;
	}
	WMT_DBG_FUNC("set fm mode (%d) ok\n", pWmtHifConf->au4StrapConf[0]);

#if CFG_SET_OPT_REG		/*set registers */
	iRet = wmt_core_init_script(set_registers, osal_array_size(set_registers));
	if (iRet) {
		WMT_ERR_FUNC("set_registers fail(%d)", iRet);
		return -14;
	}
#endif

#if CFG_WMT_COREDUMP_ENABLE
	/*Open Core Dump Function @QC begin */
	mtk_wcn_stp_coredump_flag_ctrl(1);
#endif
	if (mtk_wcn_stp_coredump_flag_get() != 0) {
		iRet = wmt_core_init_script(init_table_6, osal_array_size(init_table_6));
		if (iRet) {
			WMT_ERR_FUNC("init_table_6 core dump setting fail(%d)\n", iRet);
			return -15;
		}
		WMT_DBG_FUNC("enable soc_consys firmware coredump\n");
	} else {
		WMT_DBG_FUNC("disable soc_consys firmware coredump\n");
	}

#if CFG_WMT_WIFI_5G_SUPPORT
	dDieChipid = wmt_ic_ops_soc.icId;
	WMT_DBG_FUNC("current SOC chipid is 0x%x\n", dDieChipid);
	if (dDieChipid == 0X6592) {
		/* read A die chipid by wmt cmd */
		iRet =
		    wmt_core_tx((PUINT8) &WMT_GET_SOC_ADIE_CHIPID_CMD[0], sizeof(WMT_GET_SOC_ADIE_CHIPID_CMD), &u4Res,
				MTK_WCN_BOOL_FALSE);
		if (iRet || (u4Res != sizeof(WMT_GET_SOC_ADIE_CHIPID_CMD))) {
			WMT_ERR_FUNC("wmt_core:read A die chipid CMD fail(%d),size(%d)\n", iRet, u4Res);
			return -16;
		}
		osal_memset(evtbuf, 0, sizeof(evtbuf));
		iRet = wmt_core_rx(evtbuf, sizeof(WMT_GET_SOC_ADIE_CHIPID_EVT), &u4Res);
		if (iRet || (u4Res != sizeof(WMT_GET_SOC_ADIE_CHIPID_EVT))) {
			WMT_ERR_FUNC("wmt_core:read A die chipid EVT fail(%d),size(%d)\n", iRet, u4Res);
			mtk_wcn_stp_dbg_dump_package();
			return -17;
		}

		osal_memcpy(&aDieChipid, &evtbuf[u4Res - 2], 2);
		WMT_INFO_FUNC("get SOC A die chipid(0x%x)\n", aDieChipid);

		if (aDieChipid == 0x6625) {
			iRet =
			    wmt_core_tx((PUINT8) &WMT_GET_SOC_6625_L_CMD[0], sizeof(WMT_GET_SOC_6625_L_CMD), &u4Res,
					MTK_WCN_BOOL_FALSE);
			if (iRet || (u4Res != sizeof(WMT_GET_SOC_6625_L_CMD)))
				WMT_ERR_FUNC("wmt_core:read A die efuse CMD fail(%d),size(%d)\n", iRet, u4Res);
			osal_memset(evtbuf, 0, sizeof(evtbuf));
			iRet = wmt_core_rx(evtbuf, sizeof(WMT_GET_SOC_6625_L_EVT), &u4Res);
			if (iRet || (u4Res != sizeof(WMT_GET_SOC_6625_L_EVT))) {
				WMT_ERR_FUNC("wmt_core:read A die efuse EVT fail(%d),size(%d)\n", iRet, u4Res);
				mtk_wcn_stp_dbg_dump_package();
			}
			WMT_INFO_FUNC("read SOC Adie Efuse(0x120) value:0x%2x,0x%2x,0x%2x,0x%2x -> %s\n",
				      evtbuf[u4Res - 4], evtbuf[u4Res - 3], evtbuf[u4Res - 2], evtbuf[u4Res - 1],
				      evtbuf[u4Res - 2] == 0x31 ? "MT6625L" : "MT6625");
		}
		/* get PMIC chipid */

		ctrlData.ctrlId = WMT_CTRL_SOC_PALDO_CTRL;
		ctrlData.au4CtrlData[0] = PMIC_CHIPID_PALDO;
		ctrlData.au4CtrlData[1] = 0;
		iRet = wmt_ctrl(&ctrlData);
		if (iRet < 0) {
			WMT_ERR_FUNC("wmt_core: read PMIC chipid fail(%d)\n", iRet);
			return -18;
		}
		pmicChipid = ctrlData.au4CtrlData[2];
		WMT_INFO_FUNC("current PMIC chipid(0x%x)\n", pmicChipid);

		/* MT6625 & MT6322, write 1 to 0x0414[12] */
		/* MT6625 & MT6323, assert */
		/* MT6627 & (MT6322 or MT6323),write 0 to 0x0414[12] */

		switch (aDieChipid) {
		case 0x6625:
			if (pmicChipid == 0x6322 || pmicChipid == 0x6356) {
				WMT_INFO_FUNC("wmt-core:enable wifi 5G support\n");
				ctrlPa1 = WIFI_5G_PALDO;
				ctrlPa2 = PALDO_ON;
				wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);
			} else if (pmicChipid == 0x6323) {
				osal_assert(0);
			} else {
				WMT_WARN_FUNC("wmt-core: unknown PMIC chipid\n");
			}
			break;
		case 0x6627:
			if ((pmicChipid == 0x6322) || (pmicChipid == 0x6323)) {
				WMT_INFO_FUNC("wmt-core: disable wifi 5G support\n");
				ctrlPa1 = WIFI_5G_PALDO;
				ctrlPa2 = PALDO_OFF;
				wmt_core_ctrl(WMT_CTRL_SOC_PALDO_CTRL, &ctrlPa1, &ctrlPa2);
			} else {
				WMT_WARN_FUNC("wmt-core: unknown PMIC chipid\n");
			}
			break;
		default:
			WMT_WARN_FUNC("wmt-core: unknown A die chipid(0x%x)\n", aDieChipid);
			break;
		}
	}
#endif

#if 1
	ctrlData.ctrlId = WMT_CTRL_SET_STP_DBG_INFO;
	ctrlData.au4CtrlData[0] = wmt_ic_ops_soc.icId;
	ctrlData.au4CtrlData[1] = (SIZE_T) gp_soc_info->cChipVersion;
	ctrlData.au4CtrlData[2] = (SIZE_T) &gp_soc_patch_info;
	iRet = wmt_ctrl(&ctrlData);
	if (iRet) {
		WMT_ERR_FUNC("set dump info fail(%d)\n", iRet);
		return -19;
	}
#endif

#if CFG_WMT_PS_SUPPORT
	osal_assert(gp_soc_info != NULL);
	if (gp_soc_info != NULL) {
		if (gp_soc_info->bPsmSupport != MTK_WCN_BOOL_FALSE)
			wmt_lib_ps_enable();
		else
			wmt_lib_ps_disable();
	}
#endif

	return 0;
}

static INT32 mtk_wcn_soc_sw_deinit(P_WMT_HIF_CONF pWmtHifConf)
{
	WMT_DBG_FUNC(" start\n");

#if CFG_WMT_PS_SUPPORT
	osal_assert(gp_soc_info != NULL);
	if ((gp_soc_info != NULL)
	    && (gp_soc_info->bPsmSupport != MTK_WCN_BOOL_FALSE)) {
		wmt_lib_ps_disable();
	}
#endif

	gp_soc_info = NULL;

	return 0;
}

static INT32 mtk_wcn_soc_aif_ctrl(WMT_IC_PIN_STATE state, UINT32 flag)
{
	INT32 ret = -1;
	UINT32 val;

	if ((flag & WMT_LIB_AIF_FLAG_MASK) == WMT_LIB_AIF_FLAG_SHARE) {
		WMT_INFO_FUNC("PCM & I2S PIN SHARE\n");
#if 0
		switch (state) {
		case WMT_IC_AIF_0:
			/* BT_PCM_OFF & FM line in/out */
			val = 0x00000770;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000000;
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);
			break;

		case WMT_IC_AIF_1:
			/* BT_PCM_ON & FM line in/out */
			val = 0x00000700;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000000;
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);
			break;

		case WMT_IC_AIF_2:
			/* BT_PCM_OFF & FM I2S */
			val = 0x00000710;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000800;	/* 800:3-wire, 000: 4-wire */
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);
			break;
		default:
			WMT_ERR_FUNC("unsupported state (%d)\n", state);
			ret = -1;
			break;
		}
#else
		WMT_WARN_FUNC("TBD!!");
		ret = 0;
#endif
	} else {
		/*PCM & I2S separate */
		WMT_INFO_FUNC("PCM & I2S PIN SEPARATE\n");
#if 0
		switch (state) {
		case WMT_IC_AIF_0:
			/* BT_PCM_OFF & FM line in/out */
			val = 0x00000770;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000000;
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);
			break;

		case WMT_IC_AIF_1:
			/* BT_PCM_ON & FM line in/out */
			val = 0x00000700;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000000;
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);
			break;

		case WMT_IC_AIF_2:
			/* BT_PCM_OFF & FM I2S */
			val = 0x00000070;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000800;	/* 800:3-wire, 000: 4-wire */
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);

			break;
		case WMT_IC_AIF_3:
			val = 0x00000000;
			ret = wmt_core_reg_rw_raw(1, 0x80050140, &val, 0x00000FF0);
			val = 0x00000800;	/* 800:3-wire, 000: 4-wire */
			ret += wmt_core_reg_rw_raw(1, 0x80050150, &val, 0x00000800);

			break;
		default:
			WMT_ERR_FUNC("unsupported state (%d)\n", state);
			ret = -1;
			break;
		}
#else
		switch (state) {
		case WMT_IC_AIF_0:
			/* BT_PCM_OFF & FM line in/out */
			ret = 0;
			break;
		case WMT_IC_AIF_1:
			/* BT_PCM_ON & FM line in/out */
			ret = 0;
			break;

		case WMT_IC_AIF_2:
			/* BT_PCM_OFF & FM I2S */
			val = 0x01110000;
			ret = wmt_core_reg_rw_raw(1, 0x80050078, &val, 0x0FFF0000);

			break;
		case WMT_IC_AIF_3:
			ret = 0;
			break;

		default:
			WMT_ERR_FUNC("unsupported state (%d)\n", state);
			ret = -1;
			break;
		}
#endif
	}

	if (!ret)
		WMT_WARN_FUNC("new state(%d) fail(%d)\n", state, ret);
	WMT_INFO_FUNC("new state(%d) ok\n", state);

	return ret;
}

static INT32 mtk_wcn_soc_gps_sync_ctrl(WMT_IC_PIN_STATE state, UINT32 flag)
{
	INT32 iRet = -1;
	UINT32 uVal = 0;

	/* gen3(6631) CONSYS can not access reg:0x80050078 and no need to do GPS SYNC
	 * may cause bus hang
	 */
	if (wmt_ic_ops_soc.icId != 0x0279 &&
		wmt_ic_ops_soc.icId != 0x0507 &&
		wmt_ic_ops_soc.icId != 0x0713 &&
		wmt_ic_ops_soc.icId != 0x0788 &&
		wmt_ic_ops_soc.icId != 0x6765 &&
		wmt_ic_ops_soc.icId != 0x0688) {
		if (state == WMT_IC_PIN_MUX)
			uVal = 0x1 << 28;
		else
			uVal = 0x5 << 28;
		iRet = wmt_core_reg_rw_raw(1, 0x80050078, &uVal, 0x7 << 28);
		if (iRet)
			WMT_ERR_FUNC("gps_sync pin ctrl failed, iRet(%d)\n", iRet);
	} else
		WMT_INFO_FUNC("This chip no need to sync GPS and MODEM!\n");

	/* anyway, we return 0 */
	return 0;
}

static INT32 mtk_wcn_soc_pin_ctrl(WMT_IC_PIN_ID id, WMT_IC_PIN_STATE state, UINT32 flag)
{
	INT32 ret;

	WMT_DBG_FUNC("ic pin id:%d, state:%d, flag:0x%x\n", id, state, flag);

	ret = -1;
	switch (id) {
	case WMT_IC_PIN_AUDIO:
		ret = mtk_wcn_soc_aif_ctrl(state, flag);
		break;

	case WMT_IC_PIN_EEDI:
		WMT_WARN_FUNC("TBD!!");
		/* We just return 0 here, prevent from WMT-FUNC do other register read/write */
		ret = 0;
		break;

	case WMT_IC_PIN_EEDO:
		WMT_WARN_FUNC("TBD!!");
		/* We just return 0 here, prevent from WMT-FUNC do other register read/write */
		ret = 0;
		break;
	case WMT_IC_PIN_GSYNC:
		ret = mtk_wcn_soc_gps_sync_ctrl(state, flag);
		break;
	default:
		break;
	}
	WMT_INFO_FUNC("ret = (%d)\n", ret);

	return ret;
}

INT32 mtk_wcn_soc_co_clock_ctrl(WMT_CO_CLOCK on)
{
	INT32 iRet = 0;

	if ((on >= WMT_CO_CLOCK_DIS) && (on < WMT_CO_CLOCK_MAX)) {
		gCoClockEn = on;
	} else {
		WMT_DBG_FUNC("0x%x: error parameter:%d\n", wmt_ic_ops_soc.icId, on);
		iRet = -1;
	}
	WMT_DBG_FUNC("0x%x: Co-clock %s\n", wmt_ic_ops_soc.icId,
		     (gCoClockEn == WMT_CO_CLOCK_DIS) ? "disabled" : "enabled");

	return iRet;
}

static MTK_WCN_BOOL mtk_wcn_soc_quick_sleep_flag_get(VOID)
{
	return MTK_WCN_BOOL_TRUE;
}

static MTK_WCN_BOOL mtk_wcn_soc_aee_dump_flag_get(VOID)
{
	return MTK_WCN_BOOL_FALSE;
}
static MTK_WCN_BOOL mtk_wcn_soc_trigger_assert(VOID)
{
	INT32 ret = 0;
	UINT32 u4Res;
	UINT32 tstCmdSz = 0;
	UINT32 tstEvtSz = 0;
	UINT8 tstCmd[64];
	UINT8 tstEvt[64];
	UINT8 WMT_ASSERT_CMD[] = { 0x01, 0x02, 0x01, 0x00, 0x08 };
	UINT8 WMT_ASSERT_EVT[] = { 0x02, 0x02, 0x00, 0x00, 0x00 };

	WMT_INFO_FUNC("Send Assert command !\n");
	tstCmdSz = osal_sizeof(WMT_ASSERT_CMD);
	tstEvtSz = osal_sizeof(WMT_ASSERT_EVT);
	osal_memcpy(tstCmd, WMT_ASSERT_CMD, tstCmdSz);
	osal_memcpy(tstEvt, WMT_ASSERT_EVT, tstEvtSz);

	ret = wmt_core_tx((PUINT8) tstCmd, tstCmdSz, &u4Res, MTK_WCN_BOOL_FALSE);
	if (ret || (u4Res != tstCmdSz)) {
		WMT_ERR_FUNC("WMT-CORE: wmt_cmd_test iRet(%d) cmd len err(%d, %d)\n", ret, u4Res,
			     tstCmdSz);
		ret = -1;
	}
	return (ret == 0);
}

WMT_CO_CLOCK mtk_wcn_soc_co_clock_get(VOID)
{
	return gCoClockEn;
}

static INT32 mtk_wcn_soc_ver_check(VOID)
{
	UINT32 hw_ver;
	UINT32 fw_ver;
	INT32 iret;
	const WMT_IC_INFO_S *p_info;
	unsigned long ctrlPa1;
	unsigned long ctrlPa2;

	/* 1. identify chip versions: HVR(HW_VER) and FVR(FW_VER) */
	WMT_LOUD_FUNC("0x%x: before read hw_ver (hw version)\n", wmt_ic_ops_soc.icId);
	iret = wmt_core_reg_rw_raw(0, GEN_HVR, &hw_ver, GEN_VER_MASK);
	if (iret) {
		WMT_ERR_FUNC("0x%x: read hw_ver fail:%d\n", wmt_ic_ops_soc.icId, iret);
		return -2;
	}
	WMT_DBG_FUNC("0x%x: read hw_ver (hw version) (0x%x)\n", wmt_ic_ops_soc.icId, hw_ver);

	WMT_LOUD_FUNC("0x%x: before fw_ver (rom version)\n", wmt_ic_ops_soc.icId);
	wmt_core_reg_rw_raw(0, GEN_FVR, &fw_ver, GEN_VER_MASK);
	if (iret) {
		WMT_ERR_FUNC("0x%x: read fw_ver fail:%d\n", wmt_ic_ops_soc.icId, iret);
		return -2;
	}
	WMT_DBG_FUNC("0x%x: read fw_ver (rom version) (0x%x)\n", wmt_ic_ops_soc.icId, fw_ver);

	p_info = mtk_wcn_soc_find_wmt_ic_info(hw_ver);
	if (p_info == NULL) {
		WMT_ERR_FUNC("0x%x: hw_ver(0x%x) find wmt ic info fail\n", wmt_ic_ops_soc.icId);
		return -3;
	}
	WMT_WARN_FUNC("0x%x: ic info: %s.%s (0x%x/0x%x, WMTHWVER:%d, patch_ext:%s)\n",
		      wmt_ic_ops_soc.icId, p_info->cChipName, p_info->cChipVersion,
		      hw_ver, fw_ver, p_info->eWmtHwVer, p_info->cPatchNameExt);

	/* hw id & version */
	ctrlPa1 = (wmt_ic_ops_soc.icId << 16) | (hw_ver & 0x0000FFFF);
	/* translated hw version & fw rom version */
	ctrlPa2 = ((UINT32) (p_info->eWmtHwVer) << 16) | (fw_ver & 0x0000FFFF);

	iret = wmt_core_ctrl(WMT_CTRL_HWIDVER_SET, &ctrlPa1, &ctrlPa2);
	if (iret)
		WMT_WARN_FUNC("0x%x: WMT_CTRL_HWIDVER_SET fail(%d)\n", wmt_ic_ops_soc.icId, iret);

	gp_soc_info = p_info;
	return 0;
}

static const WMT_IC_INFO_S *mtk_wcn_soc_find_wmt_ic_info(const UINT32 hw_ver)
{
	/* match chipversion with u4HwVer item in mtk_wcn_soc_info_table */
	const UINT32 size = osal_array_size(mtk_wcn_soc_info_table);
	INT32 index = 0;

	/* George: reverse the search order to favor newer version products
	* TODO:[FixMe][GeorgeKuo] Remove full match once API wmt_lib_get_hwver()
	* is changed correctly in the future!!
	* Leave full match here is a workaround for GPS to distinguish E3/E4 ICs.
	*/
	index = size - 1;
	/* full match */
	while ((index >= 0) && (hw_ver != mtk_wcn_soc_info_table[index].u4HwVer))
		--index;
	if (index >= 0) {
		WMT_DBG_FUNC("found ic info(0x%x) by full match! index:%d\n", hw_ver, index);
		return &mtk_wcn_soc_info_table[index];
	}

	WMT_WARN_FUNC("find no ic info for (0x%x) by full match!try major num match!\n", hw_ver);

	/* George: The ONLY CORRECT method to find supported hw table. Match MAJOR
	 * NUM only can help us support future minor hw ECO, or fab switch, etc.
	 * FULL matching eliminate such flexibility and software package have to be
	 * updated EACH TIME even when minor hw ECO or fab switch!!!
	 */
	/* George: reverse the search order to favor newer version products */
	index = size - 1;
	/* major num match */
	while ((index >= 0) &&
		(MAJORNUM(hw_ver) != MAJORNUM(mtk_wcn_soc_info_table[index].u4HwVer))) {
		--index;
	}
	if (index >= 0) {
		WMT_DBG_FUNC("0x%x: found ic info for hw_ver(0x%x) by major num! index:%d\n",
			wmt_ic_ops_soc.icId, hw_ver, index);
		return &mtk_wcn_soc_info_table[index];
	}

	WMT_ERR_FUNC("0x%x: find no ic info for hw_ver(0x%x) by full match nor major num match!\n",
			wmt_ic_ops_soc.icId, hw_ver);
	WMT_ERR_FUNC("Set default chip version: E1!\n");
	return &mtk_wcn_soc_info_table[0];
}

#if CFG_WMT_FILTER_MODE_SETTING
static INT32 wmt_stp_wifi_lte_coex(VOID)
{
	INT32 iRet;
	unsigned long addr;
	WMT_GEN_CONF *pWmtGenConf;

	/*Get wmt config */
	iRet = wmt_core_ctrl(WMT_CTRL_GET_WMT_CONF, &addr, 0);
	if (iRet) {
		WMT_ERR_FUNC("ctrl GET_WMT_CONF fail(%d)\n", iRet);
		return -2;
	}
	WMT_DBG_FUNC("ctrl GET_WMT_CONF ok(0x%08lx)\n", addr);

	pWmtGenConf = (P_WMT_GEN_CONF) addr;

	/*Check if WMT.cfg exists */
	if (pWmtGenConf->cfgExist == 0) {
		WMT_INFO_FUNC("cfgExist == 0, skip config chip\n");
		/*if WMT.cfg not existed, still return success and adopt the default value */
		return 0;
	}

	osal_sleep_ms(5);

	if (pWmtGenConf->coex_wmt_filter_mode == 0) {
		if ((wmt_ic_ops_soc.icId == 0x6752) ||
		    (wmt_ic_ops_soc.icId == 0x6580) ||
		    (wmt_ic_ops_soc.icId == 0x8163) ||
		    (wmt_ic_ops_soc.icId == 0x0326) ||
		    (wmt_ic_ops_soc.icId == 0x0551) ||
		    (wmt_ic_ops_soc.icId == 0x0699) ||
		    (wmt_ic_ops_soc.icId == 0x0321) ||
		    (wmt_ic_ops_soc.icId == 0x0335) ||
		    (wmt_ic_ops_soc.icId == 0x0337) ||
		    (wmt_ic_ops_soc.icId == 0x0633)) {
			iRet =
			    wmt_core_init_script(set_wifi_lte_coex_table_1, osal_array_size(set_wifi_lte_coex_table_1));
			WMT_DBG_FUNC("wmt_core:set_wifi_lte_coex_table_1 %s(%d)\n", iRet ? "fail" : "ok", iRet);
		} else if (wmt_ic_ops_soc.icId == 0x0279 ||
				wmt_ic_ops_soc.icId == 0x0507 ||
				wmt_ic_ops_soc.icId == 0x0713 ||
				wmt_ic_ops_soc.icId == 0x0788 ||
				wmt_ic_ops_soc.icId == 0x6765 ||
				wmt_ic_ops_soc.icId == 0x0688) {
			/* add WMT_COXE_CONFIG_EXT_COMPONENT_OPCODE command for 2G4 eLNA demand*/
			if (pWmtGenConf->coex_wmt_ext_component) {
				WMT_INFO_FUNC("coex_wmt_ext_component:0x%x\n", pWmtGenConf->coex_wmt_ext_component);
				set_wifi_lte_coex_table_2[0].cmd[5] = pWmtGenConf->coex_wmt_ext_component;
			}
			iRet =
			    wmt_core_init_script(set_wifi_lte_coex_table_2, osal_array_size(set_wifi_lte_coex_table_2));
			WMT_DBG_FUNC("wmt_core:set_wifi_lte_coex_table_2 %s(%d)\n", iRet ? "fail" : "ok", iRet);
		} else {
			iRet =
			    wmt_core_init_script(set_wifi_lte_coex_table_0, osal_array_size(set_wifi_lte_coex_table_0));
			WMT_DBG_FUNC("wmt_core:set_wifi_lte_coex_table_0 %s(%d)\n", iRet ? "fail" : "ok", iRet);
		}
	}

	return iRet;
}
#endif

static INT32 wmt_stp_init_coex(VOID)
{
	INT32 iRet;
	unsigned long addr;
	WMT_GEN_CONF *pWmtGenConf;

#define COEX_WMT  0

#if CFG_SUBSYS_COEX_NEED
	/* no need for MT6628 */
#define COEX_BT   1
#define COEX_WIFI 2
#define COEX_PTA  3
#define COEX_MISC 4
#endif
	/*Get wmt config */
	iRet = wmt_core_ctrl(WMT_CTRL_GET_WMT_CONF, &addr, 0);
	if (iRet) {
		WMT_ERR_FUNC("ctrl GET_WMT_CONF fail(%d)\n", iRet);
		return -2;
	}
	WMT_INFO_FUNC("ctrl GET_WMT_CONF ok(0x%08lx)\n", addr);

	pWmtGenConf = (P_WMT_GEN_CONF) addr;

	/*Check if WMT.cfg exists */
	if (pWmtGenConf->cfgExist == 0) {
		WMT_INFO_FUNC("cfgExist == 0, skip config chip\n");
		/*if WMT.cfg not existed, still return success and adopt the default value */
		return 0;
	}

	/*Dump the coex-related info */
	WMT_DBG_FUNC("coex_wmt:0x%x\n", pWmtGenConf->coex_wmt_ant_mode);
#if CFG_SUBSYS_COEX_NEED
	WMT_DBG_FUNC("coex_bt:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		     pWmtGenConf->coex_bt_rssi_upper_limit,
		     pWmtGenConf->coex_bt_rssi_mid_limit,
		     pWmtGenConf->coex_bt_rssi_lower_limit,
		     pWmtGenConf->coex_bt_pwr_high, pWmtGenConf->coex_bt_pwr_mid, pWmtGenConf->coex_bt_pwr_low);
	WMT_DBG_FUNC("coex_wifi:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		     pWmtGenConf->coex_wifi_rssi_upper_limit,
		     pWmtGenConf->coex_wifi_rssi_mid_limit,
		     pWmtGenConf->coex_wifi_rssi_lower_limit,
		     pWmtGenConf->coex_wifi_pwr_high, pWmtGenConf->coex_wifi_pwr_mid, pWmtGenConf->coex_wifi_pwr_low);
	WMT_DBG_FUNC("coex_ext_pta:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		     pWmtGenConf->coex_ext_pta_hi_tx_tag,
		     pWmtGenConf->coex_ext_pta_hi_rx_tag,
		     pWmtGenConf->coex_ext_pta_lo_tx_tag,
		     pWmtGenConf->coex_ext_pta_lo_rx_tag,
		     pWmtGenConf->coex_ext_pta_sample_t1,
		     pWmtGenConf->coex_ext_pta_sample_t2, pWmtGenConf->coex_ext_pta_wifi_bt_con_trx);
	WMT_DBG_FUNC("coex_misc:0x%x 0x%x 0x%x\n",
		     pWmtGenConf->coex_misc_ext_pta_on, pWmtGenConf->coex_misc_ext_feature_set);
#endif

	/*command adjustion due to WMT.cfg */
	coex_table[COEX_WMT].cmd[5] = pWmtGenConf->coex_wmt_ant_mode;
	if (gWmtDbgLvl >= WMT_LOG_DBG)
		wmt_core_dump_data(&coex_table[COEX_WMT].cmd[0], coex_table[COEX_WMT].str, coex_table[COEX_WMT].cmdSz);

#if CFG_SUBSYS_COEX_NEED
	coex_table[COEX_BT].cmd[9] = pWmtGenConf->coex_bt_rssi_upper_limit;
	coex_table[COEX_BT].cmd[10] = pWmtGenConf->coex_bt_rssi_mid_limit;
	coex_table[COEX_BT].cmd[11] = pWmtGenConf->coex_bt_rssi_lower_limit;
	coex_table[COEX_BT].cmd[12] = pWmtGenConf->coex_bt_pwr_high;
	coex_table[COEX_BT].cmd[13] = pWmtGenConf->coex_bt_pwr_mid;
	coex_table[COEX_BT].cmd[14] = pWmtGenConf->coex_bt_pwr_low;
	if (gWmtDbgLvl >= WMT_LOG_DBG)
		wmt_core_dump_data(&coex_table[COEX_BT].cmd[0], coex_table[COEX_BT].str, coex_table[COEX_BT].cmdSz);

	coex_table[COEX_WIFI].cmd[10] = pWmtGenConf->coex_wifi_rssi_upper_limit;
	coex_table[COEX_WIFI].cmd[11] = pWmtGenConf->coex_wifi_rssi_mid_limit;
	coex_table[COEX_WIFI].cmd[12] = pWmtGenConf->coex_wifi_rssi_lower_limit;
	coex_table[COEX_WIFI].cmd[13] = pWmtGenConf->coex_wifi_pwr_high;
	coex_table[COEX_WIFI].cmd[14] = pWmtGenConf->coex_wifi_pwr_mid;
	coex_table[COEX_WIFI].cmd[15] = pWmtGenConf->coex_wifi_pwr_low;
	if (gWmtDbgLvl >= WMT_LOG_DBG)
		wmt_core_dump_data(&coex_table[COEX_WIFI].cmd[0],
				   coex_table[COEX_WIFI].str, coex_table[COEX_WIFI].cmdSz);

	coex_table[COEX_PTA].cmd[5] = pWmtGenConf->coex_ext_pta_hi_tx_tag;
	coex_table[COEX_PTA].cmd[6] = pWmtGenConf->coex_ext_pta_hi_rx_tag;
	coex_table[COEX_PTA].cmd[7] = pWmtGenConf->coex_ext_pta_lo_tx_tag;
	coex_table[COEX_PTA].cmd[8] = pWmtGenConf->coex_ext_pta_lo_rx_tag;
	coex_table[COEX_PTA].cmd[9] = ((pWmtGenConf->coex_ext_pta_sample_t1 & 0xff00) >> 8);
	coex_table[COEX_PTA].cmd[10] = ((pWmtGenConf->coex_ext_pta_sample_t1 & 0x00ff) >> 0);
	coex_table[COEX_PTA].cmd[11] = ((pWmtGenConf->coex_ext_pta_sample_t2 & 0xff00) >> 8);
	coex_table[COEX_PTA].cmd[12] = ((pWmtGenConf->coex_ext_pta_sample_t2 & 0x00ff) >> 0);
	coex_table[COEX_PTA].cmd[13] = pWmtGenConf->coex_ext_pta_wifi_bt_con_trx;
	if (gWmtDbgLvl >= WMT_LOG_DBG)
		wmt_core_dump_data(&coex_table[COEX_PTA].cmd[0], coex_table[COEX_PTA].str, coex_table[COEX_PTA].cmdSz);

	osal_memcpy(&coex_table[COEX_MISC].cmd[5], &pWmtGenConf->coex_misc_ext_pta_on,
		    sizeof(pWmtGenConf->coex_misc_ext_pta_on));
	osal_memcpy(&coex_table[COEX_MISC].cmd[9], &pWmtGenConf->coex_misc_ext_feature_set,
		    sizeof(pWmtGenConf->coex_misc_ext_feature_set));

	wmt_core_dump_data(&coex_table[COEX_MISC].cmd[0], coex_table[COEX_MISC].str, coex_table[COEX_MISC].cmdSz);
#endif

	iRet = wmt_core_init_script(coex_table, ARRAY_SIZE(coex_table));

	return iRet;
}

static INT32 wmt_stp_init_epa(VOID)
{
	INT32 iRet;
	unsigned long addr;
	WMT_GEN_CONF *pWmtGenConf;

	/*Get wmt config */
	iRet = wmt_core_ctrl(WMT_CTRL_GET_WMT_CONF, &addr, 0);
	if (iRet) {
		WMT_ERR_FUNC("ctrl GET_WMT_CONF fail(%d)\n", iRet);
		return -2;
	}
	WMT_DBG_FUNC("ctrl GET_WMT_CONF ok(0x%08lx)\n", addr);

	pWmtGenConf = (P_WMT_GEN_CONF) addr;

	/*Check if WMT.cfg exists */
	if (pWmtGenConf->cfgExist == 0) {
		WMT_DBG_FUNC("cfgExist == 0, skip config chip\n");
		/*if WMT.cfg not existed, still return success and adopt the default value */
		return 0;
	}

	WMT_INFO_FUNC("epa_mode:0x%x\n", pWmtGenConf->coex_wmt_ant_mode_ex);

	if (pWmtGenConf->coex_wmt_ant_mode_ex != 1)
		return 0;

	epa_table[0].cmd[5] = pWmtGenConf->coex_wmt_ant_mode_ex;
	if (gWmtDbgLvl >= WMT_LOG_DBG)
		wmt_core_dump_data(&epa_table[0].cmd[0], epa_table[0].str, epa_table[0].cmdSz);
	iRet = wmt_core_init_script(epa_table, ARRAY_SIZE(epa_table));

	return iRet;
}

#if CFG_WMT_SDIO_DRIVING_SET
static INT32 mtk_wcn_soc_set_sdio_driving(void)
{
	INT32 ret = 0;

	unsigned long addr;
	WMT_GEN_CONF *pWmtGenConf;
	UINT32 drv_val = 0;

	/*Get wmt config */
	ret = wmt_core_ctrl(WMT_CTRL_GET_WMT_CONF, &addr, 0);
	if (ret) {
		WMT_ERR_FUNC("ctrl GET_WMT_CONF fail(%d)\n", ret);
		return -1;
	}
	WMT_INFO_FUNC("ctrl GET_WMT_CONF ok(0x%08lx)\n", addr);

	pWmtGenConf = (P_WMT_GEN_CONF) addr;

	/*Check if WMT.cfg exists */
	if (pWmtGenConf->cfgExist == 0) {
		WMT_INFO_FUNC("cfgExist == 0, skip config chip\n");
		/*if WMT.cfg not existed, still return success and adopt the default value */
		return 0;
	}

	drv_val = pWmtGenConf->sdio_driving_cfg;

	/*Dump the sdio driving related info */
	WMT_INFO_FUNC("sdio driving:0x%x\n", drv_val);

	sdio_driving_table[0].cmd[12] = (UINT8) ((drv_val & 0x00000077UL) >> 0);	/* DAT0 and DAT1 */
	sdio_driving_table[0].cmd[13] = (UINT8) ((drv_val & 0x00007700UL) >> 8);	/* DAT2 and DAT3 */
	sdio_driving_table[0].cmd[14] = (UINT8) ((drv_val & 0x00070000UL) >> 16);	/* CMD */

	ret = wmt_core_init_script(sdio_driving_table, ARRAY_SIZE(sdio_driving_table));

	return ret;
}
#endif

#if CFG_WMT_CRYSTAL_TIMING_SET
static INT32 mtk_wcn_soc_crystal_triming_set(VOID)
{
	INT32 iRet = 0;
	PUINT8 pbuf = NULL;
	UINT32 bufLen = 0;
	WMT_CTRL_DATA ctrlData;
	UINT32 uCryTimOffset = 0x6D;
	MTK_WCN_BOOL bIsNvramExist = MTK_WCN_BOOL_FALSE;
	INT8 cCrystalTimingOffset = 0x0;
	UINT8 cCrystalTiming = 0x0;
	INT32 iCrystalTiming = 0x0;
	MTK_WCN_BOOL bIsCrysTrimEnabled = MTK_WCN_BOOL_FALSE;
	UINT32 u4Res;

	bIsNvramExist = MTK_WCN_BOOL_FALSE;
	 /**/ ctrlData.ctrlId = WMT_CTRL_CRYSTAL_TRIMING_GET;
	ctrlData.au4CtrlData[0] = (UINT32) "/data/nvram/APCFG/APRDEB/WIFI";
	ctrlData.au4CtrlData[1] = (UINT32) &pbuf;
	ctrlData.au4CtrlData[2] = (UINT32) &bufLen;

	iRet = wmt_ctrl(&ctrlData);
	if (iRet != 0) {
		WMT_ERR_FUNC("0x%x: WMT_CTRL_CRYSTAL_TRIMING_GET fail:%d\n", wmt_ic_ops_soc.icId, iRet);
		bIsNvramExist = MTK_WCN_BOOL_FALSE;
		bIsCrysTrimEnabled = MTK_WCN_BOOL_FALSE;
		cCrystalTimingOffset = 0x0;
		cCrystalTiming = 0x0;
		iRet = -1;
	} else {
		WMT_DBG_FUNC("0x%x: nvram pBuf(0x%08x), bufLen(%d)\n", wmt_ic_ops_soc.icId, pbuf, bufLen);
		if (bufLen < (uCryTimOffset + 1)) {
			WMT_ERR_FUNC("0x%x: nvram len(%d) too short, crystalTimging value offset(%d)\n",
				     wmt_ic_ops_soc.icId, bufLen, uCryTimOffset);
			bIsNvramExist = MTK_WCN_BOOL_FALSE;
			bIsCrysTrimEnabled = MTK_WCN_BOOL_FALSE;
			cCrystalTimingOffset = 0x0;
			cCrystalTiming = 0x0;
		} else {
			bIsNvramExist = MTK_WCN_BOOL_TRUE;
			cCrystalTimingOffset = *(pbuf + uCryTimOffset);
			if (cCrystalTimingOffset & 0x80) {
				bIsCrysTrimEnabled = MTK_WCN_BOOL_TRUE;
				cCrystalTimingOffset = (UINT8) cCrystalTimingOffset & 0x7f;
			}
			WMT_DBG_FUNC("cCrystalTimingOffset (%d), bIsCrysTrimEnabled(%d)\n", cCrystalTimingOffset,
				     bIsCrysTrimEnabled);
		}
		ctrlData.ctrlId = WMT_CTRL_CRYSTAL_TRIMING_PUT;
		ctrlData.au4CtrlData[0] = (UINT32) "/data/nvram/APCFG/APRDEB/WIFI";
		iRet = wmt_ctrl(&ctrlData);
		if (iRet != 0) {
			WMT_ERR_FUNC("0x%x: WMT_CTRL_CRYSTAL_TRIMING_PUT fail:%d\n", wmt_ic_ops_soc.icId, iRet);
			iRet = -2;
		} else {
			WMT_DBG_FUNC("0x%x: WMT_CTRL_CRYSTAL_TRIMING_PUT succeed\n", wmt_ic_ops_soc.icId);
		}
	}
	if ((bIsNvramExist == MTK_WCN_BOOL_TRUE) && (bIsCrysTrimEnabled == MTK_WCN_BOOL_TRUE)) {
		/*get CrystalTiming value before set it */
		iRet =
		    wmt_core_tx(get_crystal_timing_script[0].cmd, get_crystal_timing_script[0].cmdSz, &u4Res,
				MTK_WCN_BOOL_FALSE);
		if (iRet || (u4Res != get_crystal_timing_script[0].cmdSz)) {
			WMT_ERR_FUNC("WMT-CORE: write (%s) iRet(%d) cmd len err(%d, %d)\n",
				     get_crystal_timing_script[0].str, iRet, u4Res, get_crystal_timing_script[0].cmdSz);
			iRet = -3;
			goto done;
		}
		/* EVENT BUF */
		osal_memset(get_crystal_timing_script[0].evt, 0, get_crystal_timing_script[0].evtSz);
		iRet = wmt_core_rx(get_crystal_timing_script[0].evt, get_crystal_timing_script[0].evtSz, &u4Res);
		if (iRet || (u4Res != get_crystal_timing_script[0].evtSz)) {
			WMT_ERR_FUNC("WMT-CORE: read (%s) iRet(%d) evt len err(rx:%d, exp:%d)\n",
				     get_crystal_timing_script[0].str, iRet, u4Res, get_crystal_timing_script[0].evtSz);
			mtk_wcn_stp_dbg_dump_package();
			iRet = -4;
			goto done;
		}

		iCrystalTiming = WMT_GET_CRYSTAL_TRIMING_EVT[5] & 0x7f;
		if (cCrystalTimingOffset & 0x40) {
			/*nagative offset value */
			iCrystalTiming = iCrystalTiming + cCrystalTimingOffset - 128;
		} else {
			iCrystalTiming += cCrystalTimingOffset;
		}
		WMT_DBG_FUNC("iCrystalTiming (0x%x)\n", iCrystalTiming);
		if (iCrystalTiming > 0x7f)
			cCrystalTiming = 0x7f;
		else if (iCrystalTiming < 0)
			cCrystalTiming = 0;
		else
			cCrystalTiming = iCrystalTiming;
		WMT_DBG_FUNC("cCrystalTiming (0x%x)\n", cCrystalTiming);
		/* set_crystal_timing_script */
		WMT_SET_CRYSTAL_TRIMING_CMD[5] = cCrystalTiming;
		WMT_GET_CRYSTAL_TRIMING_EVT[5] = cCrystalTiming;

		iRet = wmt_core_init_script(set_crystal_timing_script, osal_array_size(set_crystal_timing_script));
		if (iRet) {
			WMT_ERR_FUNC("set_crystal_timing_script fail(%d)\n", iRet);
			iRet = -5;
		} else {
			WMT_DBG_FUNC("set crystal timing value (0x%x) succeed\n", WMT_SET_CRYSTAL_TRIMING_CMD[5]);
			iRet =
			    wmt_core_init_script(get_crystal_timing_script, osal_array_size(get_crystal_timing_script));
			if (iRet) {
				WMT_ERR_FUNC("get_crystal_timing_script fail(%d)\n", iRet);
				iRet = -6;
			} else {
				WMT_INFO_FUNC("succeed, updated crystal timing value (0x%x)\n",
					      WMT_GET_CRYSTAL_TRIMING_EVT[5]);
				iRet = 0x0;
			}
		}
	}
done:
	return iRet;
}
#endif

#if CFG_WMT_MULTI_PATCH
static INT32 mtk_wcn_soc_patch_info_prepare(VOID)
{
	INT32 iRet = -1;
	WMT_CTRL_DATA ctrlData;

	ctrlData.ctrlId = WMT_CTRL_PATCH_SEARCH;
	iRet = wmt_ctrl(&ctrlData);

	return iRet;
}

static UINT32 mtk_wcn_soc_get_patch_num(VOID)
{
	ULONG ctrlPa1 = 0;
	ULONG ctrlPa2 = 0;

	wmt_core_ctrl(WMT_CTRL_GET_PATCH_NUM, &ctrlPa1, &ctrlPa2);
	WMT_DBG_FUNC("patch total num = [%d]\n", ctrlPa1);
	return ctrlPa1;
}

static INT32 mtk_wcn_soc_normal_patch_dwn(PUINT8 pPatchBuf, UINT32 patchSize, PUINT8 addressByte)
{
	INT32 iRet = -1;
	UINT32 patchSizePerFrag = 0;
	UINT32 fragSeq;
	UINT32 fragNum;
	UINT16 fragSize = 0;
	UINT16 cmdLen;
	UINT32 offset;
	UINT32 u4Res;
	UINT8 evtBuf[8];
	UINT8 addressevtBuf[12];

	patchSizePerFrag = DEFAULT_PATCH_FRAG_SIZE;
	/* reserve 1st patch cmd space before patch body
	 *        |<-WMT_CMD: 5Bytes->|<-patch body: X Bytes (X=patchSize)----->|
	 */
	pPatchBuf -= sizeof(WMT_PATCH_CMD);

	fragNum = patchSize / patchSizePerFrag;
	fragNum += ((fragNum * patchSizePerFrag) == patchSize) ? 0 : 1;

	WMT_DBG_FUNC("patch size(%d) fragNum(%d)\n", patchSize, fragNum);

	/*send wmt part patch address command */
	if (wmt_ic_ops_soc.icId == 0x6752 ||
		wmt_ic_ops_soc.icId == 0x8127 ||
		wmt_ic_ops_soc.icId == 0x7623 ||
		wmt_ic_ops_soc.icId == 0x6571 ||
		wmt_ic_ops_soc.icId == 0x0326 ||
		wmt_ic_ops_soc.icId == 0x0551 ||
		wmt_ic_ops_soc.icId == 0x0690 ||
		wmt_ic_ops_soc.icId == 0x0699 ||
		wmt_ic_ops_soc.icId == 0x0321 ||
		wmt_ic_ops_soc.icId == 0x0335 ||
		wmt_ic_ops_soc.icId == 0x0337 ||
		wmt_ic_ops_soc.icId == 0x8163 ||
		wmt_ic_ops_soc.icId == 0x6580 ||
		wmt_ic_ops_soc.icId == 0x8167 ||
		wmt_ic_ops_soc.icId == 0x0633) {
		/* ROMv2 patch RAM base */
		WMT_PATCH_ADDRESS_CMD[8] = 0x40;
		WMT_PATCH_P_ADDRESS_CMD[8] = 0xc8;
	}
	/*send wmt part patch address command */
	if (wmt_ic_ops_soc.icId == 0x0279) {
		/* ROMv3 patch RAM base */
		WMT_PATCH_ADDRESS_CMD[8] = 0x08;
		WMT_PATCH_ADDRESS_CMD[9] = 0x05;
		WMT_PATCH_P_ADDRESS_CMD[8] = 0x2c;
		WMT_PATCH_P_ADDRESS_CMD[9] = 0x0b;
	}

	if (wmt_ic_ops_soc.icId == 0x0507 ||
		wmt_ic_ops_soc.icId == 0x0713 ||
		wmt_ic_ops_soc.icId == 0x0788 ||
		wmt_ic_ops_soc.icId == 0x0688) {
		/* ROMv4 patch RAM base */
		WMT_PATCH_ADDRESS_CMD[8] = 0x18;
		WMT_PATCH_ADDRESS_CMD[9] = 0x05;
		WMT_PATCH_P_ADDRESS_CMD[8] = 0x7c;
		WMT_PATCH_P_ADDRESS_CMD[9] = 0x0b;
	}

	if (wmt_ic_ops_soc.icId == 0x6765) {
		/*send part patch address command */
		WMT_PATCH_ADDRESS_CMD_NEW[5] = addressByte[0];
		WMT_PATCH_ADDRESS_CMD_NEW[6] = addressByte[1];
		WMT_PATCH_ADDRESS_CMD_NEW[7] = addressByte[2];
		WMT_PATCH_ADDRESS_CMD_NEW[8] = addressByte[3];
		WMT_DBG_FUNC("4 bytes address command:0x%02x,0x%02x,0x%02x,0x%02x",
			     WMT_PATCH_ADDRESS_CMD_NEW[5],
			     WMT_PATCH_ADDRESS_CMD_NEW[6],
			     WMT_PATCH_ADDRESS_CMD_NEW[7],
			     WMT_PATCH_ADDRESS_CMD_NEW[8]);
		iRet = wmt_core_tx((PUINT8) &WMT_PATCH_ADDRESS_CMD_NEW[0], sizeof(WMT_PATCH_ADDRESS_CMD_NEW),
				   &u4Res, MTK_WCN_BOOL_FALSE);
		if (iRet || (u4Res != sizeof(WMT_PATCH_ADDRESS_CMD_NEW))) {
			WMT_ERR_FUNC("wmt_core:wmt part patch address CMD fail(%d),size(%d)\n", iRet, u4Res);
			return -1;
		}
		osal_memset(addressevtBuf, 0, sizeof(addressevtBuf));
		iRet = wmt_core_rx(addressevtBuf, sizeof(WMT_PATCH_ADDRESS_EVT_NEW), &u4Res);
		if (iRet || (u4Res != sizeof(WMT_PATCH_ADDRESS_EVT_NEW))) {
			WMT_ERR_FUNC("wmt_core:wmt patch address EVT fail(%d),size(%d)\n", iRet, u4Res);
			mtk_wcn_stp_dbg_dump_package();
			return -1;
		}
		goto patch_download;
	}

	/*send wmt part patch address command */
	iRet =
	    wmt_core_tx((PUINT8) &WMT_PATCH_ADDRESS_CMD[0], sizeof(WMT_PATCH_ADDRESS_CMD), &u4Res, MTK_WCN_BOOL_FALSE);
	if (iRet || (u4Res != sizeof(WMT_PATCH_ADDRESS_CMD))) {
		WMT_ERR_FUNC("wmt_core:wmt patch address CMD fail(%d),size(%d)\n", iRet, u4Res);
		return -1;
	}
	osal_memset(addressevtBuf, 0, sizeof(addressevtBuf));
	iRet = wmt_core_rx(addressevtBuf, sizeof(WMT_PATCH_ADDRESS_EVT), &u4Res);
	if (iRet || (u4Res != sizeof(WMT_PATCH_ADDRESS_EVT))) {
		WMT_ERR_FUNC("wmt_core:wmt patch address EVT fail(%d),size(%d)\n", iRet, u4Res);
		mtk_wcn_stp_dbg_dump_package();
		return -1;
	}
#if CFG_CHECK_WMT_RESULT
	if (osal_memcmp(addressevtBuf, WMT_PATCH_ADDRESS_EVT, osal_sizeof(WMT_PATCH_ADDRESS_EVT)) != 0) {
		WMT_ERR_FUNC("wmt_core: write WMT_PATCH_ADDRESS_CMD status fail\n");
		return -1;
	}
#endif

	/*send part patch address command */
	WMT_PATCH_P_ADDRESS_CMD[12] = addressByte[0];
	WMT_PATCH_P_ADDRESS_CMD[13] = addressByte[1];
	WMT_PATCH_P_ADDRESS_CMD[14] = addressByte[2];
	WMT_PATCH_P_ADDRESS_CMD[15] = addressByte[3];
	WMT_DBG_FUNC("4 bytes address command:0x%02x,0x%02x,0x%02x,0x%02x",
		      WMT_PATCH_P_ADDRESS_CMD[12],
		      WMT_PATCH_P_ADDRESS_CMD[13], WMT_PATCH_P_ADDRESS_CMD[14], WMT_PATCH_P_ADDRESS_CMD[15]);
	iRet =
	    wmt_core_tx((PUINT8) &WMT_PATCH_P_ADDRESS_CMD[0], sizeof(WMT_PATCH_P_ADDRESS_CMD), &u4Res,
			MTK_WCN_BOOL_FALSE);
	if (iRet || (u4Res != sizeof(WMT_PATCH_P_ADDRESS_CMD))) {
		WMT_ERR_FUNC("wmt_core:wmt part patch address CMD fail(%d),size(%d)\n", iRet, u4Res);
		return -1;
	}
	osal_memset(addressevtBuf, 0, sizeof(addressevtBuf));
	iRet = wmt_core_rx(addressevtBuf, sizeof(WMT_PATCH_P_ADDRESS_EVT), &u4Res);
	if (iRet || (u4Res != sizeof(WMT_PATCH_P_ADDRESS_EVT))) {
		WMT_ERR_FUNC("wmt_core:wmt patch address EVT fail(%d),size(%d)\n", iRet, u4Res);
		mtk_wcn_stp_dbg_dump_package();
		return -1;
	}
#if CFG_CHECK_WMT_RESULT
	if (osal_memcmp(addressevtBuf, WMT_PATCH_P_ADDRESS_EVT, osal_sizeof(WMT_PATCH_ADDRESS_EVT)) != 0) {
		WMT_ERR_FUNC("wmt_core: write WMT_PATCH_ADDRESS_CMD status fail\n");
		return -1;
	}
#endif

patch_download:
	/* send all fragments */
	offset = sizeof(WMT_PATCH_CMD);
	fragSeq = 0;
	while (fragSeq < fragNum) {
		WMT_DBG_FUNC("patch size(%d) fragNum(%d)\n", patchSize, fragNum);
		if (fragSeq == (fragNum - 1)) {
			/* last fragment */
			fragSize = patchSize - fragSeq * patchSizePerFrag;
			WMT_PATCH_CMD[4] = WMT_PATCH_FRAG_LAST;
		} else {
			fragSize = patchSizePerFrag;
			WMT_PATCH_CMD[4] = (fragSeq == 0) ? WMT_PATCH_FRAG_1ST : WMT_PATCH_FRAG_MID;
		}
		/* update length field in CMD:flag+frag */
		cmdLen = 1 + fragSize;
		osal_memcpy(&WMT_PATCH_CMD[2], &cmdLen, 2);
		/* copy patch CMD to buf (overwrite last 5-byte in prev frag) */
		osal_memcpy(pPatchBuf + offset - sizeof(WMT_PATCH_CMD), WMT_PATCH_CMD, sizeof(WMT_PATCH_CMD));

		/* iRet =
		*(*kal_stp_tx)(pbuf + offset - sizeof(WMT_PATCH_CMD), fragSize + sizeof(WMT_PATCH_CMD),
		*&u4Res);
		*/
		iRet =
			wmt_core_tx(pPatchBuf + offset - sizeof(WMT_PATCH_CMD), fragSize + sizeof(WMT_PATCH_CMD),
				&u4Res, MTK_WCN_BOOL_FALSE);
		if (iRet || (u4Res != fragSize + sizeof(WMT_PATCH_CMD))) {
			WMT_ERR_FUNC("wmt_core: write fragSeq(%d) size(%d, %d) fail(%d)\n", fragSeq,
				     fragSize + sizeof(WMT_PATCH_CMD), u4Res, iRet);
			iRet = -1;
			break;
		}
		WMT_DBG_FUNC("wmt_core: write fragSeq(%d) size(%d, %d) ok\n",
			     fragSeq, fragSize + sizeof(WMT_PATCH_CMD), u4Res);

		osal_memset(evtBuf, 0, sizeof(evtBuf));
		/* iRet = (*kal_stp_rx)(evtBuf, sizeof(WMT_PATCH_EVT), &u4Res); */
		iRet = wmt_core_rx(evtBuf, sizeof(WMT_PATCH_EVT), &u4Res);
		if (iRet || (u4Res != sizeof(WMT_PATCH_EVT))) {
			WMT_ERR_FUNC("wmt_core: read WMT_PATCH_EVT length(%d, %d) fail(%d)\n", sizeof(WMT_PATCH_EVT),
				     u4Res, iRet);
			mtk_wcn_stp_dbg_dump_package();
			iRet = -1;
			break;
		}
#if CFG_CHECK_WMT_RESULT
		if (osal_memcmp(evtBuf, WMT_PATCH_EVT, sizeof(WMT_PATCH_EVT)) != 0) {
			WMT_ERR_FUNC("wmt_core: compare WMT_PATCH_EVT error rx(%d):[%02X,%02X,%02X,%02X,%02X]\n",
				u4Res,
				evtBuf[0],
				evtBuf[1],
				evtBuf[2],
				evtBuf[3],
				evtBuf[4]);
			WMT_ERR_FUNC("wmt_core: exp(%d):[%02X,%02X,%02X,%02X,%02X]\n",
				sizeof(WMT_PATCH_EVT),
				WMT_PATCH_EVT[0],
				WMT_PATCH_EVT[1],
				WMT_PATCH_EVT[2],
				WMT_PATCH_EVT[3],
				WMT_PATCH_EVT[4]);
			iRet = -1;
			break;
		}
#endif
		WMT_DBG_FUNC("wmt_core: read WMT_PATCH_EVT length(%d, %d) ok\n", sizeof(WMT_PATCH_EVT), u4Res);
		offset += patchSizePerFrag;
		++fragSeq;
	}

	WMT_WARN_FUNC("wmt_core: patch dwn:%d frag(%d, %d) %s\n",
		      iRet, fragSeq, fragSize, (!iRet && (fragSeq == fragNum)) ? "ok" : "fail");

	if (fragSeq != fragNum)
		return -1;

	return 0;
}

static INT32 mtk_wcn_soc_pda_patch_dwn(PUINT8 pPatchBuf, UINT32 patchSize, PUINT8 addressByte)
{
	UINT32 u4Res;
	UINT8 evtBuf[8];
	INT32 iRet = -1;
	UINT32 fragSeq;
	UINT32 fragNum;
	UINT16 fragSize = 0;
	UINT32 patchSizePerFrag = 0;
	UINT32 offset;

	/*PDA download address*/
	WMT_PATCH_PDA_CFG_CMD[5] = addressByte[3];
	WMT_PATCH_PDA_CFG_CMD[6] = addressByte[2];
	WMT_PATCH_PDA_CFG_CMD[7] = addressByte[1];
	WMT_PATCH_PDA_CFG_CMD[8] = addressByte[0];

	/*PDA download size*/
	WMT_PATCH_PDA_CFG_CMD[9] = patchSize & 0xFF000000 >> 24;
	WMT_PATCH_PDA_CFG_CMD[10] = patchSize & 0xFF0000 >> 16;
	WMT_PATCH_PDA_CFG_CMD[11] = patchSize & 0xFF00 >> 8;
	WMT_PATCH_PDA_CFG_CMD[12] = patchSize & 0xFF;

	iRet = wmt_core_tx((PUINT8) &WMT_PATCH_PDA_CFG_CMD[0], sizeof(WMT_PATCH_PDA_CFG_CMD), &u4Res,
			MTK_WCN_BOOL_FALSE);
	if (iRet || (u4Res != sizeof(WMT_PATCH_PDA_CFG_CMD))) {
		WMT_ERR_FUNC("wmt_core:wmt part patch PDA config CMD fail(%d),size(%d)\n",
				iRet, u4Res);
		return -1;
	}
	osal_memset(evtBuf, 0, sizeof(evtBuf));
	iRet = wmt_core_rx(evtBuf, sizeof(WMT_PATCH_PDA_CFG_EVT), &u4Res);
	if (iRet || (u4Res != sizeof(WMT_PATCH_PDA_CFG_EVT))) {
		WMT_ERR_FUNC("wmt_core:wmt patch PDA config EVT fail(%d),size(%d)\n",
				iRet, u4Res);
		mtk_wcn_stp_dbg_dump_package();
		return -1;
	}

	patchSizePerFrag = DEFAULT_PATCH_FRAG_SIZE;
	fragNum = patchSize / patchSizePerFrag;
	fragNum += ((fragNum * patchSizePerFrag) == patchSize) ? 0 : 1;

	/* send all fragments */
	offset = 0;
	fragSeq = 0;
	while (fragSeq < fragNum) {
		WMT_DBG_FUNC("patch size(%d) fragNum(%d)\n", patchSize, fragNum);
		if (fragSeq == (fragNum - 1)) {
			/* last fragment */
			fragSize = patchSize - fragSeq * patchSizePerFrag;
		} else
			fragSize = patchSizePerFrag;

		iRet = wmt_core_tx(pPatchBuf + offset, fragSize, &u4Res, MTK_WCN_BOOL_TRUE);
		if (iRet || (u4Res != fragSize)) {
			WMT_ERR_FUNC("wmt_core: write fragSeq(%d) size(%d, %d) fail(%d)\n",
					fragSeq, fragSize, u4Res, iRet);
			iRet = -1;
			break;
		}

		WMT_DBG_FUNC("wmt_core: write fragSeq(%d) size(%d, %d) ok\n",
			     fragSeq, fragSize, u4Res);

		offset += patchSizePerFrag;
		++fragSeq;
	}

	WMT_WARN_FUNC("wmt_core: patch dwn:%d frag(%d, %d) %s\n",
		      iRet, fragSeq, fragSize, (!iRet && (fragSeq == fragNum)) ? "ok" : "fail");

	if (fragSeq != fragNum)
		return -1;

	return 0;
}

static INT32 mtk_wcn_soc_patch_dwn(UINT32 index)
{
	INT32 iRet = -1;
	P_WMT_PATCH patchHdr = NULL;
	PUINT8 pBuf = NULL;
	PUINT8 pPatchBuf = NULL;
	UINT32 patchSize;
	PINT8 cDataTime = NULL;
	UINT16 u2HwVer = 0;
	UINT16 u2SwVer = 0;
	UINT32 u4PatchVer = 0;
	WMT_CTRL_DATA ctrlData;
	P_CONSYS_EMI_ADDR_INFO emiInfo;
	UINT8 addressByte[4];

	/*1.check hardware information */
	if (gp_soc_info == NULL) {
		WMT_ERR_FUNC("null gp_soc_info!\n");
		return -1;
	}

	osal_memset(gFullPatchName, 0, osal_sizeof(gFullPatchName));

	ctrlData.ctrlId = WMT_CTRL_GET_PATCH_INFO;
	ctrlData.au4CtrlData[0] = index + 1;
	ctrlData.au4CtrlData[1] = (SIZE_T)&gFullPatchName;
	ctrlData.au4CtrlData[2] = (SIZE_T)&addressByte;
	iRet = wmt_ctrl(&ctrlData);
	WMT_DBG_FUNC("the %d time valid patch found: (%s)\n", index + 1, gFullPatchName);

	/* <2.2> read patch content */
	ctrlData.ctrlId = WMT_CTRL_GET_PATCH;
	ctrlData.au4CtrlData[0] = (SIZE_T)NULL;
	ctrlData.au4CtrlData[1] = (SIZE_T)&gFullPatchName;
	ctrlData.au4CtrlData[2] = (SIZE_T)&pBuf;
	ctrlData.au4CtrlData[3] = (SIZE_T)&patchSize;
	iRet = wmt_ctrl(&ctrlData);
	if (iRet) {
		WMT_ERR_FUNC("wmt_core: WMT_CTRL_GET_PATCH fail:%d\n", iRet);
		iRet -= 1;
		goto done;
	}

	/* |<-BCNT_PATCH_BUF_HEADROOM(8) bytes dummy allocated->|<-patch file->| */
	/* patch file with header:
	 * |<-patch header: 28 Bytes->|<-patch body: X Bytes ----->|
	 */
	pPatchBuf = osal_malloc(patchSize);
	if (pPatchBuf == NULL) {
		WMT_ERR_FUNC("vmalloc pPatchBuf for patch download fail\n");
		return -2;
	}
	osal_memcpy(pPatchBuf, pBuf, patchSize);
	/* check patch file information */
	patchHdr = (P_WMT_PATCH) pPatchBuf;

	cDataTime = patchHdr->ucDateTime;
	u2HwVer = patchHdr->u2HwVer;
	u2SwVer = patchHdr->u2SwVer;
	u4PatchVer = patchHdr->u4PatchVer;

	cDataTime[15] = '\0';
	if (index == 0) {
		WMT_INFO_FUNC("[Patch]BuiltTime=%s,HVer=0x%x,SVer=0x%x,PhVer=0x%04x,Platform=%c%c%c%c\n",
				cDataTime, ((u2HwVer & 0x00ff) << 8) | ((u2HwVer & 0xff00) >> 8),
				((u2SwVer & 0x00ff) << 8) | ((u2SwVer & 0xff00) >> 8),
				((u4PatchVer & 0xff000000) >> 24) | ((u4PatchVer & 0x00ff0000) >> 16),
				patchHdr->ucPLat[0], patchHdr->ucPLat[1],
				patchHdr->ucPLat[2], patchHdr->ucPLat[3]);
	}
	osal_memcpy(&gp_soc_patch_info, patchHdr, osal_sizeof(WMT_PATCH));

	/* remove patch header:
	 * |<-patch body: X Bytes (X=patchSize)--->|
	 */
	if (patchSize < sizeof(WMT_PATCH)) {
		WMT_ERR_FUNC("error patch size\n");
		iRet = -1;
		goto done;
	}
	patchSize -= sizeof(WMT_PATCH);
	pPatchBuf += sizeof(WMT_PATCH);

	if (wmt_ic_ops_soc.icId == 0x6765) {
		/* remove patch checksum:
		 * |<-patch checksum: 2Bytes->|<-patch body: X Bytes (X=patchSize)--->|
		 */
		pPatchBuf += BCNT_PATCH_BUF_CHECKSUM;
		patchSize -= BCNT_PATCH_BUF_CHECKSUM;
	}

	emiInfo = mtk_wcn_consys_soc_get_emi_phy_add();
	if (!emiInfo) {
		WMT_ERR_FUNC("get emi info fail!\n");
		iRet = -1;
		goto done;
	}

	if (emiInfo->pda_dl_patch_flag)
		iRet = mtk_wcn_soc_pda_patch_dwn(pPatchBuf, patchSize, addressByte);
	else
		iRet = mtk_wcn_soc_normal_patch_dwn(pPatchBuf, patchSize, addressByte);

done:
	if (patchHdr != NULL) {
		osal_free(patchHdr);
		pPatchBuf = NULL;
		patchHdr = NULL;
	}

	/* WMT_CTRL_FREE_PATCH always return 0 */
	ctrlData.ctrlId = WMT_CTRL_FREE_PATCH;
	ctrlData.au4CtrlData[0] = index + 1;
	wmt_ctrl(&ctrlData);

	return iRet;
}

#else
static INT32 mtk_wcn_soc_patch_dwn(VOID)
{
	INT32 iRet = -1;
	P_WMT_PATCH patchHdr;
	PUINT8 pbuf;
	UINT32 patchSize;
	UINT32 fragSeq;
	UINT32 fragNum;
	UINT16 fragSize = 0;
	UINT16 cmdLen;
	UINT32 offset;
	UINT32 u4Res;
	UINT8 evtBuf[8];
	PINT8 cDataTime = NULL;
	/*PINT8 cPlat = NULL; */
	UINT16 u2HwVer = 0;
	UINT16 u2SwVer = 0;
	UINT32 u4PatchVer = 0;
	UINT32 patchSizePerFrag = 0;
	WMT_CTRL_DATA ctrlData;

	/*1.check hardware information */
	if (gp_soc_info == NULL) {
		WMT_ERR_FUNC("null gp_soc_info!\n");
		return -1;
	}
	/* <2> search patch and read patch content */
	/* <2.1> search patch */
	ctrlData.ctrlId = WMT_CTRL_PATCH_SEARCH;
	iRet = wmt_ctrl(&ctrlData);
	if (iRet == 0) {
		/* patch with correct Hw Ver Major Num found */
		ctrlData.ctrlId = WMT_CTRL_GET_PATCH_NAME;
		ctrlData.au4CtrlData[0] = (UINT32) &gFullPatchName;
		iRet = wmt_ctrl(&ctrlData);

		WMT_INFO_FUNC("valid patch found: (%s)\n", gFullPatchName);
		/* <2.2> read patch content */
		ctrlData.ctrlId = WMT_CTRL_GET_PATCH;
		ctrlData.au4CtrlData[0] = (UINT32) NULL;
		ctrlData.au4CtrlData[1] = (UINT32) &gFullPatchName;

	} else {
		iRet -= 1;
		return iRet;
	}
	ctrlData.au4CtrlData[2] = (UINT32) &pbuf;
	ctrlData.au4CtrlData[3] = (UINT32) &patchSize;
	iRet = wmt_ctrl(&ctrlData);
	if (iRet) {
		WMT_ERR_FUNC("wmt_core: WMT_CTRL_GET_PATCH fail:%d\n", iRet);
		iRet -= 1;
		goto done;
	}

	/* |<-BCNT_PATCH_BUF_HEADROOM(8) bytes dummy allocated->|<-patch file->| */
	pbuf += BCNT_PATCH_BUF_HEADROOM;
	/* patch file with header:
	 * |<-patch header: 28 Bytes->|<-patch body: X Bytes ----->|
	 */
	patchHdr = (P_WMT_PATCH) pbuf;
	/* check patch file information */

	cDataTime = patchHdr->ucDateTime;
	u2HwVer = patchHdr->u2HwVer;
	u2SwVer = patchHdr->u2SwVer;
	u4PatchVer = patchHdr->u4PatchVer;
	/*cPlat = &patchHdr->ucPLat[0]; */

	cDataTime[15] = '\0';
	WMT_DBG_FUNC("===========================================\n");
	WMT_INFO_FUNC("[ConsysPatch]BuiltTime = %s, HVer = 0x%x, SVer = 0x%x, PhVer = 0x%04x,Platform = %c%c%c%c\n",
	cDataTime, ((u2HwVer & 0x00ff) << 8) | ((u2HwVer & 0xff00) >> 8),
	((u2SwVer & 0x00ff) << 8) | ((u2SwVer & 0xff00) >> 8),
	((u4PatchVer & 0xff000000) >> 24) | ((u4PatchVer & 0x00ff0000) >> 16),
	patchHdr->ucPLat[0], patchHdr->ucPLat[1], patchHdr->ucPLat[2], patchHdr->ucPLat[3]);
	WMT_DBG_FUNC("[Consys Patch] Hw Ver = 0x%x\n", ((u2HwVer & 0x00ff) << 8) | ((u2HwVer & 0xff00) >> 8));
	WMT_DBG_FUNC("[Consys Patch] Sw Ver = 0x%x\n", ((u2SwVer & 0x00ff) << 8) | ((u2SwVer & 0xff00) >> 8));
	WMT_DBG_FUNC("[Consys Patch] Ph Ver = 0x%04x\n",
		      ((u4PatchVer & 0xff000000) >> 24) | ((u4PatchVer & 0x00ff0000) >> 16));
	WMT_DBG_FUNC("[Consys Patch] Platform = %c%c%c%c\n", patchHdr->ucPLat[0], patchHdr->ucPLat[1],
		      patchHdr->ucPLat[2], patchHdr->ucPLat[3]);
	WMT_DBG_FUNC("===========================================\n");

	/* remove patch header:
	 * |<-patch body: X Bytes (X=patchSize)--->|
	 */
	if (patchSize < sizeof(WMT_PATCH)) {
		WMT_ERR_FUNC("error patch size\n");
		return -1;
	}
	patchSize -= sizeof(WMT_PATCH);
	pbuf += sizeof(WMT_PATCH);
	patchSizePerFrag = DEFAULT_PATCH_FRAG_SIZE;
	/* reserve 1st patch cmd space before patch body
	 *        |<-WMT_CMD: 5Bytes->|<-patch body: X Bytes (X=patchSize)----->|
	 */
	pbuf -= sizeof(WMT_PATCH_CMD);

	fragNum = patchSize / patchSizePerFrag;
	fragNum += ((fragNum * patchSizePerFrag) == patchSize) ? 0 : 1;

	WMT_DBG_FUNC("patch size(%d) fragNum(%d)\n", patchSize, fragNum);

	/* send all fragments */
	offset = sizeof(WMT_PATCH_CMD);
	fragSeq = 0;
	while (fragSeq < fragNum) {
		WMT_DBG_FUNC("patch size(%d) fragNum(%d)\n", patchSize, fragNum);
		if (fragSeq == (fragNum - 1)) {
			/* last fragment */
			fragSize = patchSize - fragSeq * patchSizePerFrag;
			WMT_PATCH_CMD[4] = WMT_PATCH_FRAG_LAST;
		} else {
			fragSize = patchSizePerFrag;
			WMT_PATCH_CMD[4] = (fragSeq == 0) ? WMT_PATCH_FRAG_1ST : WMT_PATCH_FRAG_MID;
		}
		/* update length field in CMD:flag+frag */
		cmdLen = 1 + fragSize;
		osal_memcpy(&WMT_PATCH_CMD[2], &cmdLen, 2);
		/* copy patch CMD to buf (overwrite last 5-byte in prev frag) */
		osal_memcpy(pbuf + offset - sizeof(WMT_PATCH_CMD), WMT_PATCH_CMD, sizeof(WMT_PATCH_CMD));

		/* iRet =
		*	(*kal_stp_tx)(pbuf + offset - sizeof(WMT_PATCH_CMD), fragSize + sizeof(WMT_PATCH_CMD),
		*	&u4Res);
		*/
		iRet =
		    wmt_core_tx(pbuf + offset - sizeof(WMT_PATCH_CMD), fragSize + sizeof(WMT_PATCH_CMD), &u4Res,
				MTK_WCN_BOOL_FALSE);
		if (iRet || (u4Res != fragSize + sizeof(WMT_PATCH_CMD))) {
			WMT_ERR_FUNC("wmt_core: write fragSeq(%d) size(%d, %d) fail(%d)\n", fragSeq,
				     fragSize + sizeof(WMT_PATCH_CMD), u4Res, iRet);
			iRet -= 1;
			break;
		}
		WMT_DBG_FUNC("wmt_core: write fragSeq(%d) size(%d, %d) ok\n",
			     fragSeq, fragSize + sizeof(WMT_PATCH_CMD), u4Res);

		osal_memset(evtBuf, 0, sizeof(evtBuf));
		/* iRet = (*kal_stp_rx)(evtBuf, sizeof(WMT_PATCH_EVT), &u4Res); */
		iRet = wmt_core_rx(evtBuf, sizeof(WMT_PATCH_EVT), &u4Res);
		if (iRet || (u4Res != sizeof(WMT_PATCH_EVT))) {
			WMT_ERR_FUNC("wmt_core: read WMT_PATCH_EVT length(%d, %d) fail(%d)\n", sizeof(WMT_PATCH_EVT),
				     u4Res, iRet);
			mtk_wcn_stp_dbg_dump_package();
			iRet -= 1;
			break;
		}
#if CFG_CHECK_WMT_RESULT
		if (osal_memcmp(evtBuf, WMT_PATCH_EVT, sizeof(WMT_PATCH_EVT)) != 0) {
			WMT_ERR_FUNC("wmt_core: compare WMT_PATCH_EVT error rx(%d):[%02X,%02X,%02X,%02X,%02X]\n",
				u4Res,
				evtBuf[0],
				evtBuf[1],
				evtBuf[2],
				evtBuf[3],
				evtBuf[4]);
			WMT_ERR_FUNC("wmt_core: exp(%d):[%02X,%02X,%02X,%02X,%02X]\n",
				sizeof(WMT_PATCH_EVT),
				WMT_PATCH_EVT[0],
				WMT_PATCH_EVT[1],
				WMT_PATCH_EVT[2],
				WMT_PATCH_EVT[3],
				WMT_PATCH_EVT[4]);
			iRet -= 1;
			break;
		}
#endif
		WMT_DBG_FUNC("wmt_core: read WMT_PATCH_EVT length(%d, %d) ok\n", sizeof(WMT_PATCH_EVT), u4Res);
		offset += patchSizePerFrag;
		++fragSeq;
	}

	WMT_WARN_FUNC("wmt_core: patch dwn:%d frag(%d, %d) %s\n",
		      iRet, fragSeq, fragSize, (!iRet && (fragSeq == fragNum)) ? "ok" : "fail");

	if (fragSeq != fragNum)
		iRet -= 1;
done:
	/* WMT_CTRL_FREE_PATCH always return 0 */
	wmt_core_ctrl(WMT_CTRL_FREE_PATCH, NULL, NULL);

	return iRet;
}

#endif

INT32 mtk_wcn_soc_rom_patch_dwn(UINT32 ip_ver)
{
	INT32 iRet = -1;
	struct wmt_rom_patch *patchHdr = NULL;
	PUINT8 pBuf = NULL;
	PUINT8 pPatchBuf = NULL;
	UINT32 patchSize;
	UINT8 addressByte[4];
	PINT8 cDataTime = NULL;
	UINT16 u2HwVer = 0;
	UINT16 u2SwVer = 0;
	UINT32 u4PatchType = 0;
	UINT32 type;
	UINT32 patchEmiOffset;
	PUINT8 patchAddr;
	WMT_CTRL_DATA ctrlData;
	P_CONSYS_EMI_ADDR_INFO emiInfo;

	for (type = WMTDRV_TYPE_BT; type < WMTDRV_TYPE_ANT; type++) {
		osal_memset(gFullPatchName, 0, osal_sizeof(gFullPatchName));

		ctrlData.ctrlId = WMT_CTRL_GET_ROM_PATCH_INFO;
		ctrlData.au4CtrlData[0] = type;
		ctrlData.au4CtrlData[1] = (SIZE_T)&gFullPatchName;
		ctrlData.au4CtrlData[2] = (SIZE_T)&addressByte;
		ctrlData.au4CtrlData[3] = ip_ver;
		iRet = wmt_ctrl(&ctrlData);
		if (iRet > 0) {
			WMT_INFO_FUNC("There is no need to download (%d) type patch!\n", type);
			continue;
		}

		/* <2.2> read patch content */
		ctrlData.ctrlId = WMT_CTRL_GET_PATCH;
		ctrlData.au4CtrlData[0] = (SIZE_T)NULL;
		ctrlData.au4CtrlData[1] = (SIZE_T)&gFullPatchName;
		ctrlData.au4CtrlData[2] = (SIZE_T)&pBuf;
		ctrlData.au4CtrlData[3] = (SIZE_T)&patchSize;
		iRet = wmt_ctrl(&ctrlData);
		if (iRet) {
			WMT_ERR_FUNC("wmt_core: WMT_CTRL_GET_PATCH fail:%d\n", iRet);
			iRet = -1;
			goto done;
		}

		/* |<-BCNT_PATCH_BUF_HEADROOM(8) bytes dummy allocated->|<-patch file->|
		 * patch file with header:
		 * |<-patch header: 32 Bytes->|<-patch body: X Bytes ----->|
		 */
		pPatchBuf = osal_malloc(patchSize);
		if (pPatchBuf == NULL) {
			WMT_ERR_FUNC("vmalloc pPatchBuf for patch download fail\n");
			iRet = -2;
			goto done;
		}
		osal_memcpy(pPatchBuf, pBuf, patchSize);
		/* check patch file information */
		patchHdr = (struct wmt_rom_patch *) pPatchBuf;

		cDataTime = patchHdr->ucDateTime;
		u2HwVer = patchHdr->u2HwVer;
		u2SwVer = patchHdr->u2SwVer;
		u4PatchType = patchHdr->u4PatchType;

		cDataTime[15] = '\0';
		WMT_INFO_FUNC("[RomPatch]BTime=%s,HVer=0x%x,SVer=0x%x,Platform=%c%c%c%c\n,Type=%x\n",
				cDataTime,
				((u2HwVer & 0x00ff) << 8) | ((u2HwVer & 0xff00) >> 8),
				((u2SwVer & 0x00ff) << 8) | ((u2SwVer & 0xff00) >> 8),
				patchHdr->ucPLat[0], patchHdr->ucPLat[1],
				patchHdr->ucPLat[2], patchHdr->ucPLat[3],
				u4PatchType);

		/* remove patch header:
		 * |<-patch body: X Bytes (X=patchSize)--->|
		 */
		if (patchSize < sizeof(struct wmt_rom_patch)) {
			WMT_ERR_FUNC("error patch size\n");
			iRet = -3;
			goto done;
		}
		patchSize -= sizeof(struct wmt_rom_patch);
		pPatchBuf += sizeof(struct wmt_rom_patch);

		patchEmiOffset = (addressByte[2] << 16) | (addressByte[1] << 8) | addressByte[0];

		emiInfo = mtk_wcn_consys_soc_get_emi_phy_add();
		if (!emiInfo) {
			WMT_ERR_FUNC("get emi info fail!\n");
			iRet = -4;
			goto done;
		}

		if (patchEmiOffset + patchSize < emiInfo->emi_size) {
			WMT_INFO_FUNC("[Rom Patch]Name=%s,EmiOffset=0x%x,Size=0x%x\n",
					gFullPatchName, patchEmiOffset, patchSize);

			patchAddr = ioremap_nocache(emiInfo->emi_phy_addr + patchEmiOffset, patchSize);
			if (patchAddr) {
				osal_memcpy_toio(patchAddr, pPatchBuf, patchSize);
				iounmap(patchAddr);
			} else
				WMT_ERR_FUNC("ioremap_nocache fail\n");
		} else
			WMT_ERR_FUNC("The rom patch is too big to overflow on EMI\n");

done:
		if (patchHdr != NULL) {
			osal_free(patchHdr);
			pPatchBuf = NULL;
			patchHdr = NULL;
		}

		/* WMT_CTRL_FREE_PATCH always return 0 */
		ctrlData.ctrlId = WMT_CTRL_FREE_PATCH;
		ctrlData.au4CtrlData[0] = type;
		wmt_ctrl(&ctrlData);
		if (iRet)
			break;
	}

	return iRet;
}
