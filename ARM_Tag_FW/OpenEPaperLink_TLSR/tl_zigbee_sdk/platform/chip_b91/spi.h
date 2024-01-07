/********************************************************************************************************
 * @file	spi.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef SPI_H
#define SPI_H

#include <reg_include/register.h>
#include "gpio.h"
#include "dma.h"
/**	@page SPI
 *
 *	Introduction
 *	===============
 *	B91 supports two spi
 *
 *	API Reference
 *	===============
 *	Header File: spi.h
 */

typedef enum{
	SPI_RXFIFO_OR_INT_EN        =BIT(0),
	SPI_TXFIFO_UR_INT_EN        =BIT(1),
	SPI_RXFIFO_INT_EN 	        =BIT(2),
	SPI_TXFIFO_INT_EN		    =BIT(3),
	SPI_END_INT_EN              =BIT(4),
	SPI_SLV_CMD_EN	            =BIT(5),
}spi_irq_mask;

typedef enum{
	SPI_RXF_OR_INT	    =BIT(2),
	SPI_TXF_UR_INT	    =BIT(3),
	SPI_RXF_INT         =BIT(4),
	SPI_TXF_INT 	    =BIT(5),
	SPI_END_INT         =BIT(6),
    SPI_SLV_CMD_INT	    =BIT(7),
}spi_irq_status_e;

typedef enum{
	PSPI_MODULE = 0,
    HSPI_MODULE = 1 ,
}spi_sel_e;

/**
 * @brief  Define the work mode.
 */
typedef enum{
    SPI_MODE0 = 0,
/**
 * change enum SPI_MODE1 value to 1 from 2,
 * 		  enum SPI_MODE2 value to 2 from 1,
 * changed by chaofan.20210113.
 */
    SPI_MODE1,
    SPI_MODE2,
    SPI_MODE3,
} spi_mode_type_e;

/**
 * @brief  Define the mode for SPI io mode.
 */
typedef enum{
	SPI_SINGLE_MODE	 = 0,
    SPI_DUAL_MODE	 = 1 ,
    HSPI_QUAD_MODE 	 = 2,
	SPI_3_LINE_MODE	 = 3
} spi_io_mode_e;

typedef enum{
	SPI_NOMAL = 0,
    SPI_3LINE = 3,
} spi_nomal_3line_mode_e;

typedef enum{
	PSPI_SINGLE = 0,
    PSPI_DUAL 	= 1,
	PSPI_3LINE	= 3
} pspi_single_dual_mode_e;

typedef enum{
	HSPI_SINGLE = 0,
    HSPI_DUAL	= 1,
    HSPI_QUAD 	= 2,
	HSPI_3LINE	= 3
} hspi_single_dual_quad_mode_e;


/**
 * @brief  Define the SPI command & translate mode.
 */
typedef enum{
	SPI_MODE_WRITE_AND_READ = 0,//write and read at the same.must enbale CmdEn
	SPI_MODE_WRITE_ONLY,//write
	SPI_MODE_READ_ONLY,// read must enbale CmdEn
	SPI_MODE_WRITE_READ,//write_ read
	SPI_MODE_READ_WRITE,//read_write
	SPI_MODE_WRITE_DUMMY_READ,//write_dummy_read
	SPI_MODE_READ_DUMMY_WRITE,//read_ dummy_write must enbale CmdEn
	SPI_MODE_NONE_DATA,//must enbale CmdEn
	SPI_MODE_DUMMY_WRITE,//dummy_write
	SPI_MODE_DUMMY_READ,//dummy_read
	SPI_MODE_RESERVED,
}spi_tans_mode_e;

typedef enum{
	SPI_MODE_WR_WRITE_ONLY  = 1,//write
	SPI_MODE_WR_DUMMY_WRITE = 8,//dummy_write
}spi_wr_tans_mode_e;

typedef enum{
	SPI_MODE_RD_READ_ONLY  = 2,//must enbale CmdEn
	SPI_MODE_RD_DUMMY_READ = 9,//dummy_read
}spi_rd_tans_mode_e;

typedef enum{
	SPI_MODE_WR_RD 		 = 3,//must enbale CmdEn
	SPI_MODE_WR_DUMMY_RD = 5,//write_dummy_read
}spi_wr_rd_tans_mode_e;

typedef struct{
	hspi_single_dual_quad_mode_e  hspi_io_mode;//set spi interface mode
	unsigned char hspi_dummy_cnt;//set dummy cnt if tans_mode have dummy .
	unsigned char hspi_cmd_en;//enable cmd phase
	unsigned char hspi_addr_en;//enable address phase
	unsigned char hspi_addr_len;//enable address phase
	unsigned char hspi_cmd_fmt_en;//if cmd_en enable cmd fmt will follow the interface (dual/quad)
	unsigned char hspi_addr_fmt_en;//if addr_en enable addr fmt will follow the interface (dual/quad)
}hspi_config_t;

typedef struct{
	pspi_single_dual_mode_e  pspi_io_mode;//set spi interface mode
	unsigned char pspi_dummy_cnt;//set dummy cnt if tans_mode have dummy .
    _Bool  pspi_cmd_en;//enable cmd phase
}pspi_config_t;


typedef enum{
	SPI_SLAVE_WRITE_DATA_CMD = 0x00,
	SPI_SLAVE_WRITE_DATA_DUAL_CMD = FLD_SPI_CMD_DATA_DUAL,
	SPI_SLAVE_WRITE_ADDR_DUAL_CMD = FLD_SPI_CMD_ADDR_DUAL,

	SPI_SLAVE_WRITE_DATA_DUAL_4CYC_CMD = FLD_SPI_CMD_DATA_DUAL | FLD_SPI_CMD_RD_DUMMY_4CYCLE,
	SPI_SLAVE_WRITE_ADDR_DUAL_4CYC_CMD = FLD_SPI_CMD_ADDR_DUAL | FLD_SPI_CMD_RD_DUMMY_4CYCLE,
	SPI_SLAVE_WRITE_DATA_AND_ADDR_DUL_4CYC_CMD = FLD_SPI_CMD_ADDR_DUAL | FLD_SPI_CMD_DATA_DUAL | FLD_SPI_CMD_RD_DUMMY_4CYCLE,
}spi_slave_write_cmd_e;

typedef enum{
	SPI_SLAVE_READ_DATA_CMD = FLD_SPI_CMD_RD_EN,
	SPI_SLAVE_READ_DATA_DUAL_CMD = FLD_SPI_CMD_RD_EN | FLD_SPI_CMD_DATA_DUAL,
	SPI_SLAVE_READ_ADDR_DUAL_CMD = FLD_SPI_CMD_RD_EN | FLD_SPI_CMD_ADDR_DUAL,

	SPI_SLAVE_READ_DATA_DUAL_4CYC_CMD = FLD_SPI_CMD_RD_EN | FLD_SPI_CMD_DATA_DUAL | FLD_SPI_CMD_RD_DUMMY_4CYCLE,
	SPI_SLAVE_READ_ADDR_DUAL_4CYC_CMD = FLD_SPI_CMD_RD_EN | FLD_SPI_CMD_ADDR_DUAL | FLD_SPI_CMD_RD_DUMMY_4CYCLE,

	SPI_SLAVE_READ_DATA_AND_ADDR_DUL_4CYC_CMD = FLD_SPI_CMD_RD_EN | FLD_SPI_CMD_ADDR_DUAL | FLD_SPI_CMD_DATA_DUAL | FLD_SPI_CMD_RD_DUMMY_4CYCLE,

}spi_slave_read_cmd_e;

typedef enum{
	SPI_READ_STATUS_SINGLE_CMD  = 0x05,
	SPI_READ_STATUS_DUAL_CMD 	= 0x15,
	HSPI_READ_STATUS_QUAD_CMD   = 0x25,
	SPI_READ_DATA_SINGLE_CMD    = 0x0B,
	SPI_READ_DATA_DUAL_CMD 		= 0x0C,
	HSPI_READ_DATA_QUAD_CMD 	= 0x0E,
	SPI_WRITE_DATA_SINGLE_CMD   = 0x51,
	SPI_WRITE_DATA_DUAL_CMD 	= 0x52,
	HSPI_WRITE_DATA_QUAD_CMD 	= 0x54,
}pspi_hspi_cmd_e;
/**
 * @brief Define APS1604M-3SQR QSPI PSRAM CMD.
 */
typedef enum{
	PSRAM_READ_CMD				  = 0x03,
	PSRAM_FAST_READ_CMD			  = 0x0B,
	PSRAM_FAST_READ_QUAD_CMD	  = 0xEB,

	PSRAM_WRITE_CMD				  = 0x02,
	PSRAM_QUAD_WRITE_CMD		  = 0x38,//
	PSRAM_WRAPPED_READ_CMD		  = 0x8B,
	PSRAM_WRAPPED_WRITE_CMD		  = 0x82,

	PSRAM_MODE_RG_READ_CMD		  = 0xB5,
	PSRAM_MODE_RG_WRITE_CMD		  = 0xB1,

	PSRAM_ENTER_QUAD_MODE_CMD	  = 0x35,
	PSRAM_EXIT_QUAD_MODE_CMD	  = 0xF5,

	PSRAM_REST_ENABLE_CMD		  = 0x66,
	PSRAM_REST_CMD	    		  = 0x99,
	PSRAM_BURST_LENGTH_TOGGLE_CMD = 0xC0,
	PSRAM_READ_ID_CMD  		  	  = 0x95,

}spi_xip_cmd_e;

/**
 * @brief Define panel 2data_lane_mode
 */
typedef enum{
	HSPI_2DATA_LANE_CLOSE	= 0x00,
	HSPI_2DATA_LANE_RGB565  = 0x01,
	HSPI_2DATA_LANE_RGB666  = 0x03,
	HSPI_2DATA_LANE_RGB888  = 0x07,
}hspi_panel_2data_lane_mode_e;

/**
 * @brief  Define the SPI io.
 */
typedef enum{
	HSPI_CLK_PB4 = GPIO_PB4,
	HSPI_CLK_PA2 = GPIO_PA2,
}hspi_clk_pin_def_e;

typedef enum{
	HSPI_CSN_PB6 = GPIO_PB6,
	HSPI_CSN_PA1 = GPIO_PA1,
}hspi_csn_pin_def_e;

typedef enum{
	HSPI_MOSI_IO0_PB3 = GPIO_PB3,
	HSPI_MOSI_IO0_PA4 = GPIO_PA4,
}hspi_mosi_io0_pin_def_e;

typedef enum{
	HSPI_MISO_IO1_PB2 = GPIO_PB2,
	HSPI_MISO_IO1_PA3 = GPIO_PA3,
}hspi_miso_io1_pin_def_e;

typedef enum{
	HSPI_WP_IO2_PB1 = GPIO_PB1,
}hspi_wp_io2_pin_def_e;

typedef enum{
	HSPI_HOLD_IO3_PB0 = GPIO_PB0,
}hspi_hold_io3_pin_def_e;

typedef enum{
	HSPI_CLK_PB4_PIN = GPIO_PB4,
	HSPI_CLK_PA2_PIN = GPIO_PA2,

	HSPI_CSN_PB6_PIN = GPIO_PB6,
	HSPI_CSN_PA1_PIN = GPIO_PA1,

	HSPI_MOSI_IO0_PB3_PIN = GPIO_PB3,
	HSPI_MOSI_IO0_PA4_PIN = GPIO_PA4,

	HSPI_MISO_IO1_PB2_PIN = GPIO_PB2,
	HSPI_MISO_IO1_PA3_PIN = GPIO_PA3,

	HSPI_WP_IO2_PB1_PIN   = GPIO_PB1,
	HSPI_HOLD_IO3_PB0_PIN = GPIO_PB0,
	HSPI_NONE_PIN = 0xfff,
}hspi_pin_def_e;

typedef struct{
	hspi_clk_pin_def_e      	      hspi_clk_pin;
	hspi_csn_pin_def_e         		  hspi_csn_pin;
	hspi_mosi_io0_pin_def_e           hspi_mosi_io0_pin;
	hspi_miso_io1_pin_def_e           hspi_miso_io1_pin;
	hspi_wp_io2_pin_def_e             hspi_wp_io2_pin;
	hspi_hold_io3_pin_def_e           hspi_hold_io3_pin;
}hspi_pin_config_t;


typedef enum{
	PSPI_CLK_PC5 = GPIO_PC5,
	PSPI_CLK_PB5 = GPIO_PB5,
	PSPI_CLK_PD1 = GPIO_PD1,
}pspi_clk_pin_def_e;

typedef enum{
	PSPI_CSN_PC4 = GPIO_PC4,
	PSPI_CSN_PC0 = GPIO_PC0,
	PSPI_CSN_PD0 = GPIO_PD0,
}pspi_csn_pin_def_e;

typedef enum{
	PSPI_MOSI_IO0_PC7 = GPIO_PC7,
	PSPI_MOSI_IO0_PB7 = GPIO_PB7,
	PSPI_MOSI_IO0_PD3 = GPIO_PD3,
}pspi_mosi_io0_pin_def_e;

typedef enum{
	PSPI_MISO_IO1_PC6 = GPIO_PC6,
	PSPI_MISO_IO1_PB6 = GPIO_PB6,
	PSPI_MISO_IO1_PD2 = GPIO_PD2,
}pspi_miso_io1_pin_def_e;

typedef enum{
	PSPI_CLK_PC5_PIN = GPIO_PC5,
	PSPI_CLK_PB5_PIN = GPIO_PB5,
	PSPI_CLK_PD1_PIN = GPIO_PD1,

	PSPI_CSN_PC4_PIN = GPIO_PC4,
	PSPI_CSN_PC0_PIN = GPIO_PC0,
	PSPI_CSN_PD0_PIN = GPIO_PD0,

	PSPI_MOSI_IO0_PC7_PIN = GPIO_PC7,
	PSPI_MOSI_IO0_PB7_PIN = GPIO_PB7,
	PSPI_MOSI_IO0_PD3_PIN = GPIO_PD3,

	PSPI_MISO_IO1_PC6_PIN = GPIO_PC6,
	PSPI_MISO_IO1_PB6_PIN = GPIO_PB6,
	PSPI_MISO_IO1_PD2_PIN = GPIO_PD2,
	PSPI_NONE_PIN = 0xfff,
}pspi_pin_def_e;

typedef struct{
	pspi_clk_pin_def_e      pspi_clk_pin;
	pspi_csn_pin_def_e      pspi_csn_pin;
	pspi_mosi_io0_pin_def_e pspi_mosi_io0_pin;
	pspi_miso_io1_pin_def_e pspi_miso_io1_pin;
}pspi_pin_config_t;


/**
 * @brief    This function reset HSPI module.
 * @return   none
 */
static inline void hspi_reset(void)
{
	reg_rst0 &= (~FLD_RST0_HSPI);
	reg_rst0 |= FLD_RST0_HSPI;
	reg_spi_status(HSPI_MODULE)|=FLD_HSPI_SOFT_RESET;
}
/**
 * @brief  This function reset PSPI module.
 * @return none
 */
static inline void pspi_reset(void)
{
	reg_rst1 &= (~FLD_RST1_PSPI);
	reg_rst1 |= FLD_RST1_PSPI;
	reg_spi_status(PSPI_MODULE)|=FLD_HSPI_SOFT_RESET;
}

/**
 * @brief 		This function get tx_fifo number.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		ntx_fifo 	- number that wait to be sent.
 */
static inline  unsigned char spi_get_txfifo_num(spi_sel_e spi_sel)
{
   return (reg_spi_fifo_num(spi_sel) & FLD_SPI_TXF_NUM) >> 4;
}

/**
 * @brief 		This function get rx_fifo number.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		rx_fifo 	- num that have been received.
 */
static inline unsigned char spi_get_rxfifo_num(spi_sel_e spi_sel)
{
   return reg_spi_fifo_num(spi_sel) & FLD_SPI_RXF_NUM;
}

/**
 * @brief 		This function set SPI rx_cnt.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	cnt 		- rx amount of data.
 * @return 		none
 */
static inline void spi_rx_cnt(spi_sel_e spi_sel, unsigned int cnt)
{
	reg_spi_rx_cnt2(spi_sel) = ((cnt - 1) >> 16) & 0xff;
	reg_spi_rx_cnt1(spi_sel) = ((cnt - 1) >> 8) & 0xff;
	reg_spi_rx_cnt0(spi_sel) = (cnt - 1) & 0xff;

}

/**
 * @brief 		This function set SPI tx_cnt.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	cnt 		- tx amount of data.
 * @return 		none
 */
static inline void spi_tx_cnt(spi_sel_e spi_sel, unsigned int cnt)
{
	reg_spi_tx_cnt2(spi_sel) = ((cnt - 1) >> 16) & 0xff;
	reg_spi_tx_cnt1(spi_sel) = ((cnt - 1) >> 8) & 0xff;
    reg_spi_tx_cnt0(spi_sel) = (cnt - 1) & 0xff;

}

/**
 * @brief 		This function clear tx_fifo.
 * @param[in] 	spi_sel - the spi module.
 * @return 		none
 */
static inline void spi_tx_fifo_clr(spi_sel_e spi_sel)
{
	BM_SET(reg_spi_fifo_state(spi_sel), FLD_SPI_TXF_CLR);
}

/**
 * @brief 		This function clear rx_fifo.
 * @param[in]	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_rx_fifo_clr(spi_sel_e spi_sel)
{
	BM_SET(reg_spi_fifo_state(spi_sel), FLD_SPI_RXF_CLR);
}

/**
 * @brief 		This function set hspi command content.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	cmd 		- command content.
 * @return 		none
 */
static inline void spi_set_cmd(spi_sel_e spi_sel, unsigned char cmd)
{
	reg_spi_trans1(spi_sel) = cmd;
}

/**
 * @brief 		This function servers to enable cmd which will sent a byte cmd.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_cmd_en(spi_sel_e spi_sel)
{
	BM_SET(	reg_spi_mode2(spi_sel), FLD_SPI_CMD_EN);
}

/**
 * @brief 		This function servers to disable cmd which will not sent cmd.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_cmd_dis(spi_sel_e spi_sel)
{

	BM_CLR(reg_spi_mode2(spi_sel), FLD_SPI_CMD_EN);
}

/**
 * @brief 	This function servers enable cmd  format,the format of cmd phase is the same as the data phase(Dual/Quad).
 * @return 	none
 */
static inline void hspi_cmd_fmt_en()
{
	BM_SET(	reg_spi_mode2(HSPI_MODULE), FLD_HSPI_CMD_FMT);
}

/**
 * @brief 	This function servers disable cmd  format.
 * @return 	none
 */
static inline void hspi_cmd_fmt_dis()
{
	BM_CLR(reg_spi_mode2(HSPI_MODULE), FLD_HSPI_CMD_FMT);
}

/**
 * @brief 	This function servers to enable hspi quad mode.
 * @return 	none
 */
static inline void hspi_quad_mode_en()
{
	BM_SET(reg_spi_mode2(HSPI_MODULE), FLD_HSPI_QUAD);
}

/**
 * @brief 		This function servers to disable hspi quad mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void hspi_quad_mode_dis(spi_sel_e spi_sel)
{
	BM_CLR(reg_spi_mode2(spi_sel), FLD_HSPI_QUAD);
}

/**
 * @brief 		This function servers to enable hspi dual mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_dual_mode_en(spi_sel_e spi_sel)
{
	BM_SET(reg_spi_mode0(spi_sel), FLD_SPI_DUAL);
}

/**
 * @brief 		This function servers to disable hspi dual mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_dual_mode_dis(spi_sel_e spi_sel)
{
	BM_CLR(reg_spi_mode0(spi_sel), FLD_SPI_DUAL);
}

/**
 * @brief 		This function servers to enable hspi 3line mode.
 * @param[in] 	spi_sel - the spi module.
 * @return 		none
 */
static inline void spi_3line_mode_en(spi_sel_e spi_sel)
{
	BM_SET(reg_spi_mode0(spi_sel), FLD_SPI_3LINE);
}

/**
 * @brief 		This function servers to disable hspi 3line mode..
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_3line_mode_dis(spi_sel_e spi_sel)
{
	BM_CLR(reg_spi_mode0(spi_sel), FLD_SPI_3LINE);
}

/**
 * @brief 	This function to enable address format.the format of addr phase is the same as the data phase(Dual/Quad).
 * @return 	none
 */
static inline void hspi_addr_fmt_en(void)
{
	BM_SET(reg_hspi_xip_ctrl, FLD_HSPI_ADDR_FMT);

}

/**
 * @brief 	This function to disable address format.
 * @return 	none
 */
static inline void hspi_addr_fmt_dis(void)
{
	BM_CLR(	reg_hspi_xip_ctrl, FLD_HSPI_ADDR_FMT);

}

/**
 * @brief 		This function to determine whether the bus is busy.
 * @param[in] 	spi_sel 	- the spi module.
 * @return   	1:Indicates that the bus is busy. 0:Indicates that the bus is free.
 */
static inline _Bool spi_is_busy(spi_sel_e spi_sel)
{
	return  reg_spi_status(spi_sel) & FLD_HSPI_BUSY;

}

/**
 * @brief 		This function enable tx dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_tx_dma_en(spi_sel_e spi_sel)
{
	BM_SET(reg_spi_trans2(spi_sel), FLD_SPI_TX_DMA_EN);
}

/**
 * @brief 		This function disable tx dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_tx_dma_dis(spi_sel_e spi_sel)
{
	BM_CLR(reg_spi_trans2(spi_sel), FLD_SPI_TX_DMA_EN);
}

/**
 * @brief 		This function enable rx dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_rx_dma_en(spi_sel_e spi_sel)
{
	BM_SET(reg_spi_trans2(spi_sel), FLD_SPI_RX_DMA_EN);
}

/**
 * @brief 		This function disable rx dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_rx_dma_dis(spi_sel_e spi_sel)
{
	BM_CLR(reg_spi_trans2(spi_sel), FLD_SPI_RX_DMA_EN);
}

/**
 * @brief 		This function set xip write command content.
 * @param[in] 	wr_cmd 		- write command content.
 * @return 		none
 */
static inline void hspi_xip_set_wr_cmd(unsigned char wr_cmd)
{
	reg_hspi_xip_wr_cmd	= wr_cmd;
}

/**
 * @brief 		This function set xip read command content.
 * @param[in] 	rd_cmd		- read command content.
 * @return 		none
 */
static inline void hspi_xip_set_rd_cmd(unsigned char rd_cmd)
{

	reg_hspi_xip_rd_cmd	= rd_cmd;
}

/**
 * @brief 		This function set xip address offset.
 * @param[in] 	addr_offset 	- the offset of data.
 * @return 		none
 */
static inline void hspi_xip_addr_offset(unsigned int addr_offset)
{
    reg_hspi_xip_addr_offset0 = addr_offset & 0xff;
	reg_hspi_xip_addr_offset1 = (addr_offset >> 8) & 0xff;
	reg_hspi_xip_addr_offset2 = (addr_offset >> 16) & 0xff;
	reg_hspi_xip_addr_offset3 = (addr_offset >> 24) & 0xff;
}

/**
 * @brief 		This function servers to set xip read transfer mode.
 * @param[in] 	rd_mode 	- read mode in enum hspi_tans_mode_e.
 * @return  	none
 */
static inline void hspi_xip_read_transmode(unsigned char rd_mode)
{
	reg_hspi_xip_trans_mode = ((reg_hspi_xip_trans_mode & (~FLD_HSPI_XIP_RD_TRANS_MODE)) | ((rd_mode & 0xf) << 4));
}

/**
 * @brief 		This function servers to set xip write transfer mode.
 * @param[in] 	wr_mode 	- write mode in enum hspi_tans_mode_e.
 * @return  	none
 */
static inline void hspi_xip_write_transmode(unsigned char wr_mode)
{
	reg_hspi_xip_trans_mode = ((reg_hspi_xip_trans_mode & (~FLD_HSPI_XIP_WR_TRANS_MODE)) | (wr_mode & FLD_HSPI_XIP_WR_TRANS_MODE));
}

/**
 * @brief 	This function enable address phase.
 * @return 	none
 */
static inline void hspi_addr_en(void)
{
	BM_SET(reg_hspi_xip_ctrl, FLD_HSPI_ADDR_EN);
}

/**
 * @brief 	This function disable address phase.
 * @return 	none
 */
static inline void hspi_addr_dis(void)
{
	BM_CLR(reg_hspi_xip_ctrl, FLD_HSPI_ADDR_EN);
}

/**
 * @brief 		This function servers to set hspi address length.
 * @param[in] 	len 	The number of bytes of address, range is 1~4.
 * @return 		none
 */
static inline void hspi_set_addr_len(unsigned char len)
{
	reg_hspi_xip_ctrl |= ((len - 1) & 0x3) << 2;
}

/**
 * @brief 	This function servers to enable xip sequential mode.
 * @return 	none
 */
static inline void hspi_xip_seq_mode_en(void)
{
	BM_SET(reg_hspi_xip_ctrl, FLD_HSPI_XIP_MODE);
}

/**
 * @brief 	This function servers to disable xip sequential mode.
 * @return 	none
 */
static inline void hspi_xip_seq_mode_dis(void)
{
	BM_CLR(reg_hspi_xip_ctrl, FLD_HSPI_XIP_MODE);
}

/**
 * @brief 	This function servers to enable xip.
 * @return 	none
 */
static inline void hspi_xip_en(void)
{
	BM_SET(reg_hspi_xip_ctrl, FLD_HSPI_XIP_ENABLE);
}

/**
 * @brief 	This function servers to disable xip.
 * @return 	none
 */
static inline void hspi_xip_dis(void)
{
	BM_CLR(reg_hspi_xip_ctrl, FLD_HSPI_XIP_ENABLE);
}

/**
 * @brief 	This function servers to stop xip.
 * @return 	none
 */
static inline void hspi_xip_stop(void)
{
	BM_SET(reg_hspi_xip_ctrl, FLD_HSPI_XIP_STOP);
}

/**
 * @brief 	This function servers to enable xip timeout that set hight level.
 * @return 	none
 */
static inline void hspi_xip_timeout_mode_en(void)
{
	BM_SET(reg_hspi_xip_ctrl, FLD_HSPI_XIP_TIMEOUT_MODE);
}

/**
 * @brief 	This function servers to disable xip timeout.
 * @return 	none
 */
static inline void hspi_xip_timeout_mode_dis(void)
{
	BM_CLR(reg_hspi_xip_ctrl, FLD_HSPI_XIP_TIMEOUT_MODE);
}

/**
 * @brief 		This function servers to set xip timeout cnt,when two data frame intervals exceed spi_clock_out_period*timeout_cnt,cs will set high level.
 * @param[in] 	cnt 	- xip timeout cnt.
 * @return 		none
 */
static inline void hspi_xip_timeout_cnt(unsigned char cnt)
{
	reg_hspi_xip_timeout_cnt = cnt & 0xff;
}

/**
 * @brief 		This function servers to set page_size.
 * @param[in] 	page_size_i 	- page boundary 2^page_size_i bytes.
 * @return 		none
 */
static inline void hspi_xip_page_size(unsigned char page_size_i)
{
	reg_hspi_page_size = page_size_i;
}

/**
 * @brief 		This function servers to reply master slave is ready .
 * When slave_ready is set to 1, after the master sends a read status command to the slave,
 * the slave will automatically return to 0x5a by the hardware.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_slave_ready_en(spi_sel_e spi_sel)
{
	  BM_SET(reg_spi_status(spi_sel), FLD_HSPI_SLAVE_READY);
}

/**
 * @brief 		This function servers to reply master slave is not ready.
 * When slave_ready is set to 0, after the master sends a read status command to the slave,
 * the slave will automatically return to 0x00 by the hardware.
 * @param[in] 	spi_sel 	- the spi module.
 * @return 		none
 */
static inline void spi_slave_ready_dis(spi_sel_e spi_sel)
{
	  BM_CLR(reg_spi_status(spi_sel), FLD_HSPI_SLAVE_READY);
}

/**
 * @brief 		This function servers to read cmd from master for slave.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	cmd transferd by master.
 */
static inline unsigned char spi_slave_get_cmd(spi_sel_e spi_sel)
{
	return reg_spi_trans1(spi_sel);
}

/**
 * @brief      	The function of this API is to set the number of bytes to triggered the receive and transmit interrupt.
 *            	Its default value is 4. recommend setting 4.
 * @param[in] 	spi_sel - the spi module.
 * @param[in]  	cnt 	- the interrupt trigger level.
 * @return   	none
 */
static inline void spi_rx_tx_irq_trig_cnt(spi_sel_e spi_sel, unsigned char cnt)
{
	reg_spi_status(spi_sel) = ((reg_spi_status(spi_sel) & (~FLD_HSPI_FIFO_THRES)) | ((cnt & 7) << 4));
}

/**
 * @brief 		This function servers to get irq status.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	status 	- the irq status.
 * @return    - the value of status is be set.
 */
static inline unsigned char spi_get_irq_status(spi_sel_e spi_sel,spi_irq_status_e status )
{
	return reg_spi_irq_state(spi_sel)&status;
}

/**
 * @brief 		This function servers to clear irq status.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	status 	- the irq status.
 * @return  	none.
 */
static inline void spi_clr_irq_status(spi_sel_e spi_sel, spi_irq_status_e status)
{
	reg_spi_irq_state(spi_sel) = status;
}

/**
 * @brief 		This function servers to set irq mask.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mask 	- the irq mask.
 * @return  	cmd 	- transferd by master.
 */
static inline void spi_set_irq_mask(spi_sel_e spi_sel, spi_irq_mask mask)
{
   BM_SET(reg_spi_trans2(spi_sel), mask);
}
/**
 * @brief 		This function servers to get irq mask.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mask 	- the irq mask.
 * @return  	irq mask.
 */
static inline unsigned char spi_get_irq_mask(spi_sel_e spi_sel, spi_irq_mask mask)
{
	return reg_spi_trans2(spi_sel)&mask;
}

/**
 * @brief 		This function servers to clear irq mask.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mask 	- the irq mask.
 * @return  	cmd 	- transferd by master.
 */
static inline void spi_clr_irq_mask(spi_sel_e spi_sel, spi_irq_mask mask)
{
	BM_CLR(reg_spi_trans2(spi_sel), mask);
}

/**
 * @brief 		This function enable 3line_dcx module which is use for panel(lcd oled..).
 * @return 		none
 */
static inline void hspi_3line_dcx_en(void)
{
	BM_SET(reg_hspi_panel_ctrl, FLD_HSPI_PANEL_3LINE_DCX_EN);
}

/**
 * @brief 		This function disable 3line_dcx module which is use for panel(lcd oled..).
 * @return 		none
 */
static inline void hspi_3line_dcx_dis(void)
{
	BM_CLR(reg_hspi_panel_ctrl, FLD_HSPI_PANEL_3LINE_DCX_EN);
}

/**
 * @brief 		This function set 3line_dcx translate data.
 * @return 		none
 */
static inline void hspi_set_3line_dcx_data(void)
{
	BM_SET(reg_hspi_panel_ctrl, FLD_HSPI_PANEL_3LINE_DCX);
}

/**
 * @brief 		This function set 3line_dcx translate command.
 * @return 		none
 */
static inline void hspi_set_3line_dcx_cmd(void)
{
	BM_CLR(reg_hspi_panel_ctrl, FLD_HSPI_PANEL_3LINE_DCX);
}

/**
 * @brief 		This function set 2data_lane mode for panel.
 * @return 		none
 */
static inline void hspi_set_panel_2data_lane_mode(hspi_panel_2data_lane_mode_e mode)
{
	reg_hspi_panel_ctrl = ((reg_hspi_panel_ctrl & (~FLD_HSPI_PANEL_2DATA_LANE)) | ((mode & 0xf) << 2));
}

/**
 * @brief   This function selects  pin  for hspi master or slave.
 * @return  none
 */
void spi_slave_set_pin(void);
/**
 * @brief     	This function configures hspi pin.
 * @param[in] 	config - the pointer of pin config struct.
 * @return  	none
 */
void hspi_set_pin(hspi_pin_config_t *config);

/**
 * @brief     	This function configures pspi pin.
 * @param[in] 	config - the pointer of pin config struct.
 * @return 		none
 */
void pspi_set_pin(pspi_pin_config_t *config);

/**
 * @brief     	This function enable hspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void hspi_cs_pin_en(hspi_csn_pin_def_e pin);

/**
 * @brief     	This function disable hspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void hspi_cs_pin_dis(hspi_csn_pin_def_e pin);

/**
 * @brief     	This function change hspi csn pin.
 * @param[in] 	current_csn_pin - the current csn pin.
 * @param[in] 	next_csn_pin - the next csn pin.
 * @return 		none.
 */
void hspi_change_csn_pin(hspi_csn_pin_def_e current_csn_pin, hspi_csn_pin_def_e next_csn_pin);
/**
 * @brief     	This function enable pspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void pspi_cs_pin_en(pspi_csn_pin_def_e pin);

/**
 * @brief     	This function disable pspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void pspi_cs_pin_dis(pspi_csn_pin_def_e pin);

/**
 * @brief     	This function change pspi csn pin.
 * @param[in] 	current_csn_pin - the current csn pin.
 * @param[in] 	next_csn_pin - the next csn pin.
 * @return 		none.
 */
void pspi_change_csn_pin(pspi_csn_pin_def_e current_csn_pin, pspi_csn_pin_def_e next_csn_pin);
/**
 * @brief     	This function configures the clock and working mode for SPI interface.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	div_clock 	- the division factor for SPI module.
 *           	spi_clock_out = ahb_clock / ((div_clock+1)*2)
 * @param[in]	 mode 		- the selected working mode of SPI module.
 *           	bit5:CPHA-Clock Polarity ; bit6:CPOL:CPHA-Clock Phase
 *        	    MODE0:  CPOL = 0, CPHA =0;
 *        	    MODE1:  CPOL = 0, CPHA =1;
 *        	    MODE2:  CPOL = 1, CPHA =0;
 *        	    MODE3:  CPOL = 1, CPHA =1;
 * @return  	none
 */
void spi_master_init(spi_sel_e spi_sel, unsigned char div_clock, spi_mode_type_e mode);

/**
 * @brief     	This function configures the clock and working mode for SPI interface.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mode 	- the selected working mode of SPI module.
 *          	bit5:CPHA-Clock Polarity ; bit6:CPOL:CPHA-Clock Phase
 *          	  MODE0:  CPOL = 0, CPHA =0;
 *          	  MODE1:  CPOL = 0, CPHA =1;
 *          	  MODE2:  CPOL = 1, CPHA =0;
 *          	  MODE3:  CPOL = 1, CPHA =1;
 * @return  	none
 * @note  		spi_clock_in  (spi_slave_clock frequency)/3
 */
void spi_slave_init(spi_sel_e spi_sel, spi_mode_type_e mode);

/**
 * @brief     	This function servers to set dummy cycle cnt.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	dummy_cnt 	- the cnt of dummy clock.
 * @return  	none
 */
void spi_set_dummy_cnt(spi_sel_e spi_sel, unsigned char dummy_cnt);

/**
 * @brief     	This function servers to set slave address hspi only.
 * @param[in] 	addr 	- address of slave.
 * @return  	none
 */
void hspi_set_address(unsigned int addr);

/**
 * @brief     	This function servers to set spi transfer mode.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mode 	- transfer mode.
 * @return  	none
 */
void spi_set_transmode(spi_sel_e spi_sel, spi_tans_mode_e mode);


/**
 * @brief     	This function servers to set normal mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	none
 */
void spi_set_normal_mode(spi_sel_e spi_sel);

/**
 * @brief     	This function servers to set dual mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	none
 */
void spi_set_dual_mode(spi_sel_e spi_sel);

/**
 * @brief	This function servers to set quad mode.
 * @return	none
 */
void hspi_set_quad_mode();

/**
 * @brief     	This function servers to set 3line mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	none
 */
void spi_set_3line_mode(spi_sel_e spi_sel);

/**
 * @brief     	This function servers to set hspi io  mode.
 * @param[in] 	spi_sel - the spi module.
 * @param[in]	mode 	- single/dual/quad /3line.
 * @return  	none
  */
void spi_set_io_mode(spi_sel_e spi_sel, spi_io_mode_e mode);

/**
 * @brief     	This function servers to config normal mode.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mode 	- nomal ,mode 3line.
 * @return  	none
 */
void spi_master_config(spi_sel_e spi_sel, spi_nomal_3line_mode_e mode);

/**
 * @brief     	This function servers to config hspi special mode.
 * @param[in] 	config 	- the pointer of pin special config struct.
 * @return  	none
 */
void hspi_master_config_plus(hspi_config_t *config);

/**
 * @brief     	This function servers to config pspi special mode.
 * @param[in] 	config 	- the pointer of pin special config struct.
 * @return  	none
 */
void pspi_master_config_plus(pspi_config_t *config);

/**
 * @brief     	This function servers to write hspi fifo.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	data 	- the pointer to the data for write.
 * @param[in] 	len 	- write length.
 * @return    	none
 */
void spi_write(spi_sel_e spi_sel, unsigned char *data, unsigned int len);

/**
 * @brief     	This function servers to read hspi fifo.
 * @param[in] 	spi_sel	- the spi module.
 * @param[in] 	data 	- the pointer to the data for read.
 * @param[in] 	len 	- write length.
 * @return    	none
 */
void spi_read(spi_sel_e spi_sel, unsigned char *data, unsigned int len);

/**
 * @brief     	This function serves to normal write data in normal.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	data 	- the pointer to the data for write.
 * @param[in] 	len 	- write length.
 * @return  	none
 */
void spi_master_write(spi_sel_e spi_sel, unsigned char *data, unsigned int len);

/**
 * @brief     	This function serves to normal write and read data.
 * This interface cannot be used for full duplex.
 * rd_len shouldn't set to 0. Must write first, then read.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	wr_data - the pointer to the data for write.
 * @param[in] 	wr_len 	- write length.
 * @param[in] 	rd_data - the pointer to the data for read.
 * @param[in] 	rd_len 	- read length.
 * @return  	none
 */
void spi_master_write_read(spi_sel_e spi_sel, unsigned char *wr_data, unsigned int wr_len, unsigned char *rd_data, unsigned int rd_len);

/**
 * @brief     	This function serves to single/dual/quad write to the SPI slave.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	cmd 		- cmd one byte will first write.
 * @param[in] 	addr 		- the address of slave.
 * @param[in] 	data 		-  pointer to the data need to write.
 * @param[in] 	data_len 	- length in byte of the data need to write.
 * @param[in] 	wr_mode 	- write mode.dummy or not dummy.
 * @return  	none
 */
void spi_master_write_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *data, unsigned int data_len, spi_wr_tans_mode_e wr_mode);

/**
 * @brief     	This function serves to single/dual/quad  read from the SPI slave.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	data 		- pointer to the data need to read.
 * @param[in]  	data_len 	- the length of data.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_read_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *data, unsigned int data_len, spi_rd_tans_mode_e rd_mode);

/**
 * @brief      	This function serves to write address, then read data from the SPI slave.
 * This interface cannot be used for full duplex.
 * @param[in]  	spi_sel	 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addrs 		- pointer to the address of slave.
 * @param[in]  	addr_len 	- the length of address.
 * @param[in]  	data 		- the pointer to the data for read.
 * @param[in]  	data_len 	- read length.
 * @param[in] 	wr_mode 	- write mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_write_read_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned char *addrs, unsigned int addr_len, unsigned char *data, unsigned int data_len, spi_rd_tans_mode_e wr_mode);

/**
 * @brief     	This function serves to set tx_dma channel and config dma tx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 * @note        In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
 *              If you must do this, you must perform the following sequence:
 *              1.hspi_reset() 2.spi_tx_fifo_clr(HSPI_MODULE)/spi_rx_fifo_clr(HSPI_MODULE) 3. dma_chn_dis(s_hspi_tx_dma_chn) 4.spi_master_write_dma(HSPI_MODULE)/spi_master_write_read_dma(HSPI_MODULE)/spi_master_write_dma_plus(HSPI_MODULE)/spi_master_write_read_dma_plus(HSPI_MODULE)/spi_set_tx_dma(HSPI_MODULE).
 */
void hspi_set_tx_dma_config(dma_chn_e chn);

/**
 * @brief     	This function serves to set rx_dma channel and config dma rx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 * @note        In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
 *              If you must do this, you must perform the following sequence:
 *              1.hspi_reset() 2.spi_rx_fifo_clr(HSPI_MODULE)/spi_tx_fifo_clr(HSPI_MODULE) 3. dma_chn_dis(s_hspi_rx_dma_chn) 4.spi_master_write_read_dma(HSPI_MODULE)/spi_master_read_dma_plus(HSPI_MODULE)/spi_master_write_read_dma_plus(HSPI_MODULE)//spi_set_rx_dma(HSPI_MODULE).
 */
void hspi_set_rx_dma_config(dma_chn_e chn);

/**
 * @brief     	This function serves to set tx_dam channel and config dma tx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 * @note        In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
 *              If you must do this, you must perform the following sequence:
 *              1.pspi_reset() 2.spi_tx_fifo_clr(PSPI_MODULE)/spi_rx_fifo_clr(PSPI_MODULE) 3. dma_chn_dis(s_pspi_tx_dma_chn) 4.spi_master_write_dma(PSPI_MODULE)/spi_master_write_read_dma(PSPI_MODULE)/spi_master_write_dma_plus(PSPI_MODULE)/spi_master_write_read_dma_plus(PSPI_MODULE)/spi_set_tx_dma(PSPI_MODULE).
 */
void pspi_set_tx_dma_config(dma_chn_e chn);

/**
 * @brief     	This function serves to set rx_dma channel and config dma rx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 * @note        In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
 *              If you must do this, you must perform the following sequence:
 *              1.pspi_reset() 2.spi_rx_fifo_clr(PSPI_MODULE)/spi_tx_fifo_clr(PSPI_MODULE) 3. dma_chn_dis(s_pspi_rx_dma_chn) 4.spi_master_write_read_dma(PSPI_MODULE)/spi_master_read_dma_plus(PSPI_MODULE)/spi_master_write_read_dma_plus(PSPI_MODULE)/spi_set_rx_dma(PSPI_MODULE).
 */
void pspi_set_rx_dma_config(dma_chn_e chn);





/**
 * @brief   	this  function set spi tx dma channel.
 * @param[in]  	spi_sel     - the spi module.
 * @param[in]  	src_addr 	- the address of source.
 * @param[in]  	len 		- the length of data.
 * */
_attribute_ram_code_sec_  void spi_set_tx_dma(spi_sel_e spi_sel, unsigned char* src_addr,unsigned int len);


/**
 * @brief   	this  function set spi rx dma channel.
 * @param[in]  	spi_sel     - the spi module.
 * @param[in]  	dst_addr 	- the address of destination.
 * @param[in]  	len 		- the length of data.
 * */
_attribute_ram_code_sec_ void spi_set_rx_dma(spi_sel_e spi_sel, unsigned char* dst_addr,unsigned int len);
/**
 * @brief     	This function serves to normal write data by dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	src_addr 	- the pointer to the data for write.
 * @param[in] 	len 		- write length.
 * @return  	none
 */
void spi_master_write_dma(spi_sel_e spi_sel, unsigned char *src_addr, unsigned int len);

/**
 * @brief     	This function serves to normal write cmd and address, then read data by dma.
 * The interface does not support full-duplex communication.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	addr 		- the pointer to the cmd and address for write.
 * @param[in] 	addr_len 	- write length.
 * @param[in] 	data 		- the pointer to the data for read.
 * @param[in] 	data_len 	- read length.
 * @return  	none
 */
void spi_master_write_read_dma(spi_sel_e spi_sel, unsigned char *addr, unsigned int addr_len, unsigned char *data, unsigned int data_len);

/**
 * @brief      	This function serves to single/dual/quad  write to the SPI slave by dma.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	data 		- pointer to the data need to write.
 * @param[in]  	data_len 	- length in byte of the data need to write.
 * @param[in]  	wr_mode 	- write mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_write_dma_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *data, unsigned int data_len, spi_wr_tans_mode_e wr_mode);

/**
 * @brief      	This function serves to single/dual/quad  read from the SPI slave by dma.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	dst_addr 	- pointer to the buffer that will cache the reading out data.
 * @param[in]  	data_len 	- length in byte of the data need to read.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_read_dma_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *dst_addr, unsigned int data_len, spi_rd_tans_mode_e rd_mode);

/**
 * @brief      	This function serves to single/dual/quad write address and read from the SPI slave by dma.
 * This interface could be used for full duplex.
 * When this interface is used for full-duplex communication, it can only be used on the master side.
 * 1.the spi_master_config() interface must be called first to disable hardware cmd and hardware address
 * 2.must cmd is 0,addr_len is equal to rd_len,rd_mode is SPI_MODE_WRITE_AND_READ.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	addr_len 	- the length of address.
 * @param[in]  	rd_data 	- pointer to the buffer that will cache the reading out data.
 * @param[in]  	rd_len	 	- length in byte of the data need to read.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_write_read_dma_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned char *addr, unsigned int addr_len, unsigned char *rd_data, unsigned int rd_len, spi_rd_tans_mode_e rd_mode);

/**
 * @brief      	This function serves to single/dual (quad) write to the SPI slave by xip.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr_offset - offset of xip base address.
 * @param[in]  	data 		- pointer to the data need to write.
 * @param[in]  	data_len 	- length in byte of the data need to write.
 * @param[in]  	wr_mode 	- write mode  dummy or not dummy.
 * @return   	none
 */
void hspi_master_write_xip(unsigned char cmd, unsigned int addr_offset, unsigned char *data, unsigned int data_len, spi_wr_tans_mode_e wr_mode);

/**
 * @brief      	This function serves to single/dual (quad) read from the SPI slave by xip.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr_offset - offset of xip base address.
 * @param[in]  	data 		- pointer to the data need to read.
 * @param[in]  	data_len 	- length in byte of the data need to read.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void hspi_master_read_xip(unsigned char cmd, unsigned int addr_offset, unsigned char *data, unsigned int data_len, spi_rd_tans_mode_e rd_mode);

/**
 * @brief      	This function serves to a cmd and one data write to the SPI slave by xip.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr_offset - offset of xip base address.
 * @param[in]  	data_in 	- data need to write.
 * @param[in]  	wr_mode 	- write mode  dummy or not dummy.
 * @return   	none
 */
void hspi_master_write_xip_cmd_data(unsigned char cmd, unsigned int addr_offset, unsigned char data_in, spi_wr_tans_mode_e wr_mode);
/**
 * @brief      	This function serves to write and read data simultaneously.
 * This interface can only be used by the master.
 * When initializing the master, call the spi_master_config() interface to disable the hardware cmd and hardware address,
 * and then start sending and receiving data.
 * The default chunk size sent and received by this interface is 8byte.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	write_data  - write data pointer.
 * @param[in]  	read_data 	- read data pointer.
 * @param[in]  	len 	    - write/read length.
 * @return   	none
 */
void spi_master_write_read_full_duplex(spi_sel_e spi_sel,unsigned char *write_data, unsigned char *read_data, unsigned int len);
/**
 * @brief      	This function serves to read data in normal.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	data     	- read data pointer.
 * @param[in]  	len 	    - read length.
 * @return   	none
 */
void spi_master_read(spi_sel_e spi_sel, unsigned char *data, unsigned int len);
#endif


