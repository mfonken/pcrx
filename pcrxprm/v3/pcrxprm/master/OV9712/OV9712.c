/************************************************************************
 *                             Includes                      *
 ***********************************************************************/
#include "OV9712.h"

/************************************************************************
 *                       Local Configuration                            *
 ***********************************************************************/
static register_t OV9712_regs[] =
{
  {DVP_CTRL_00,	0xb0}, // [7:6]VSYNC - vsync_old(b00), vsync_new(b01), or vsync3(b10)|[5]pclk_gate_en|[4]vsync_gate|[3]vsync3_w_sel|[2]pclk reverse|[1]href reverse|[0]vsync reverse
  {REG5C,	REG5C_V}, // [6:5]PLL Pre-divider - /1(b0x), /2(b10), or /4(b11)|[4:0]Pll-multiplier CLK2=CLK1 x (32-[4:0])
//  {REG5D,     	REG5D_V}, // [5:4]Output drive capability - 1x(b00), 2x(b01), 3x(b10), or 4x(b11)

  {REG57,		 REG57_V},
  {REG58,		 REG58_V},
  {REG59,		 REG59_V},

  //{REG58,				(0xc8)},//0x19}, // DSP Output Vertical Size MSBs
  //{REG59,				(0xa0/40)},//0x08}, // DSP Output Horizontal Size MSBs
  //{LENC_CTRL_23, 0x05}, // [2]V_skip|[0]H_skip - Normal image output(b0) or Sub-sampling output(b1)

  /*
  {COM7,        0x42}, // [1]Color bar with pixel overlay
  {DSP_CTRL_1,	0x2a}, // [7]SMPH Mean enable|[3]Color bar without pixel overlay|[1:0]Patterns

  {HSTART, 			0x13},
  {AHSIZE, 			0x51},
  //{REG32, 			0x00},

  {VSTART, 			0x00},
  {AVSIZE, 			0x64},
  {REG03, 			0x01},
  */
  {ENDR}
};

/************************************************************************
 *                      Local Function Definitions                      *
 ***********************************************************************/
void OV9712_Init( OV9712_t * ov9712, I2C_Handle_t * i2c_port, OV9712_pins_t * pins )
{
  ov9712->CAM_I2C_PORT = i2c_port;
  if( pins == NULL) return;
  ov9712->Pins = pins;
  OV9712_Functions.Enable(ov9712);
  HAL_Delay(1);
  register_t reg;
  for( int i = 0; i < sizeof(OV9712_regs) / sizeof(OV9712_regs[0]); i++ )
  {
    reg = OV9712_regs[i];
    if( reg.Address == ENDR ) break;
    OV9712_Functions.Write( ov9712, reg.Address, reg.Value );
  }
}
static void OV9712_Write( OV9712_t * ov9712, uint8_t r, uint8_t v )
{
  uint8_t data[2] = { r, v };
  PlatformFunctions.I2C.Transmit( ov9712->CAM_I2C_PORT, ov9712->ADDR, data, 2, 100);
}

static void OV9712_Enable( OV9712_t * ov9712 )
{
  if( ov9712->Pins == NULL) return;
  PlatformFunctions.GPIO.Write( &ov9712->Pins->ENABLE, GPIO_PIN_SET);
  PlatformFunctions.GPIO.Write( &ov9712->Pins->LOW_VOLTAGE, GPIO_PIN_SET);
  PlatformFunctions.GPIO.Write( &ov9712->Pins->POWER_DOWN, GPIO_PIN_RESET);
  PlatformFunctions.GPIO.SetPortMode( &ov9712->Pins->MASTER_CLOCK, GPIO_MODE_AF_PP);
}

static void OV9712_Disable( OV9712_t * ov9712 )
{
  if( ov9712->Pins == NULL) return;
  PlatformFunctions.GPIO.Write( &ov9712->Pins->ENABLE, GPIO_PIN_RESET);
  PlatformFunctions.GPIO.Write( &ov9712->Pins->LOW_VOLTAGE, GPIO_PIN_RESET);
  PlatformFunctions.GPIO.Write( &ov9712->Pins->POWER_DOWN, GPIO_PIN_SET);
  PlatformFunctions.GPIO.SetPortMode( &ov9712->Pins->MASTER_CLOCK, GPIO_MODE_INPUT);
}

/************************************************************************
 *                       Dummy Configuration                            *
 ***********************************************************************/
#ifdef DUMMY
register_t dummy[] =
{
  /* Clock Selection */
  {REG5C,               0x59}, // [6:5]PLL Pre-divider - /1(b0x), /2(b10), or /4(b11)|[4:0]Pll-multiplier CLK2=CLK1 x (32-[4:0])

  /* Driving Capability and Direction Control for I/O Pins */
  {REG5D,               0xc4}, // [5:4]Output drive capability - 1x(b00), 2x(b01), 3x(b10), or 4x(b11)
  {REG56,               0x1f}, // [7:5]Reserved|[4]HREF|[3]VSYNC|[2]PCLK|[1]Y9|[0]Y8

  /* Histogram-Based AEC Algorithm Adjustment Controls */
  {REG72,               0x60}, // BP Level   - Upper limit for B pixels
  {REG73,               0x49}, // WP Level   - Lower limit for W pixels
  {REG74,               0xe0}, // VBP Level  - Upper limit for VB pixels
  {REG75,               0xe0}, // VWP Level  - Lower limit for VW pixels
  {REG76,               0xc0}, // BP Number  - Lower probability for B pixels to trigger auto AEC/AGC
  {REG77,               0xc0}, // WP Number  - Lower probability for W pixels to trigger auto AEC/AGC
  {REG78,               0xff}, // VBP Number - Lower probability for B pixels to trigger fast AEC/AGC
  {REG79,	        0xff}, // VWP Number - Lower probability for W pixels to trigger fast AEC/AGC

  /* Test Pattern Selection Control */
  {COM7,                0x42}, // [1]Color bar with pixel overlay

  /* Histogram-Based AEC/AGC Reference Area Option */
  {COM8,                0x85}, // [5]Banding on/off|[3]LAEC on/off
  {COM9,                0x10}, // [3]Enable AEC below banding value (0 - 1/100s<exposure<1/120s, 1 - allow shorter/longer exposure under strong light conditions)
  {COM5,                0x40}, // [3]VAEC on/off (Add frame)
  {REG03,               0x02}, // [7:5]VAEC Ceiling - 1(b001),2(b010),3(b011), or 7(b1XX)
  {COM13,               0x03}, // [5:4]Auto trigger VAEC when gain is greater than 2x(b00),4x(b01),8x(b10), or 16x(b11)
  {COM14,               0x00}, // [5:0]Maximum banding steps in terms of row exposure
  {REG4A,               0xd0}, // [1:0]Banding filter value BDst[9:8]
  {REG49,               0x00}, // [7:0]Banding filter value BDst[7:0]

  /* BLC Control Functions */
  {REG41,               0x82}, // [3:0]Target black level for algorithm
  {REG43,               0xc0}, // [7:6]BLC line selection - R/Gr offset(bX0), R/Gb offset(b01), or all offsets(b11)
  {COM24,               0x02}, // [3]Manual BLC trigger for 64fr|[2]BLC always on
  {HOFFS,               0x00}, // [7:6]BLC offset for Gb[9:8]|[5:4]BLC offset for Gr[9:8]|[3:2]BLC offset for B[9:8]|[1:0]BLC offset for R[9:8]
  {ROFFS,               0x00}, // [7:0]BLC offset for R[7:0]
  {BOFFS,               0x00}, // [7:0]BLC offset for B[7:0]
  {GROFFS,              0x00}, // [7:0]BLC offset for Gr[7:0]
  {GBOFFS,              0x00}, // [7:0]BLC offset for Gb[7:0]

  /* Digital Gain Control */
  {GAIN,                0x00}, // [7]Apply gain if gain >= 2x(b0), Apply gain if gain >= 4x(b1)

  /* Pre-DSP Registers */
  {DSP_CTRL_1,		0x8a}, // [7]SMPH Mean enable|[3]Color bar without pixel overlay|[1:0]Patterns
  {DSP_CTRL_2,		0x00}, // [7:0]pre_out_hoff[7:0]
  {DSP_CTRL_3,		0x00}, // [7:0]pre_out_voff[7:0]
  {DSP_CTRL_4,		0x00}, // [5:4]pre_out_voff[9:8]|[2:0]pre_out_hoff[10:8]
  {DSP_CTRL_5,		0x04}, // [7:4]pre_pad_vrgt|[3:0]pre_pad_hrgt

  /* LEN Registers */
  {DSP_CTRL_0, 		0xf9}, // [7]black_en|[6]white_en|[5]AWB gain enable|[4]AWB enable|[3]LENC correction enable|[2]lenc_gain_coef_en|[0]ISP Enable
  {LENC_CTRL_0,		0x00}, // def_coef_man
  {LENC_CTRL_1,		0x00}, // def_coef_th
  {LENC_CTRL_2,		0xf1}, // [7:4]lenc_gain_thre2[3:0]|[3:0]lenc_gain_thre1[3:0]
  {LENC_CTRL_3,		0x6a}, // [4]Manual center of RGB circle|[3]Round off last two bits of LENC input data|[2]def_coef_man_en|[1]lenc_gain_thre2[4]|[0]lenc_gain_thre1[4]
  {LENC_CTRL_4,		0x00}, // Black level manual setting

  {LENC_CTRL_7,		0x12}, // [5:4]LCC_Y[9:8] for R|[2:0]LCC_X[9:8] for R
  {LENC_CTRL_5,		0x80}, // LCC_X[7:0] for R
  {LENC_CTRL_6,		0x90}, // LCC_Y[7:0] for R
  {LENC_CTRL_8, 	0x22}, // [6:0]First group of factors for R
  {LENC_CTRL_9, 	0xc2}, // [6:0]Second group of factors for R
  {LENC_CTRL_10, 	0x87}, // [7:4]Second group of factors for R|[3:0]First group of factors for R

  {LENC_CTRL_13,	0x12}, // [5:4]LCC_Y[9:8] for G|[2:0]LCC_X[9:8] for G
  {LENC_CTRL_11,	0x80}, // LCC_X[7:0] for G
  {LENC_CTRL_12,	0x90}, // LCC_Y[7:0] for G
  {LENC_CTRL_14, 	0x22}, // [6:0]First group of factors for G
  {LENC_CTRL_15, 	0xc2}, // [6:0]Second group of factors for G
  {LENC_CTRL_16, 	0x87}, // [7:4]Second group of factors for G|[3:0]First group of factors for G

  {LENC_CTRL_19,	0x12}, // [5:4]LCC_Y[9:8] for B|[2:0]LCC_X[9:8] for B
  {LENC_CTRL_17,	0x80}, // LCC_X[7:0] for B
  {LENC_CTRL_18,	0x90}, // LCC_Y[7:0] for B
  {LENC_CTRL_20, 	0x22}, // [6:0]First group of factors for B
  {LENC_CTRL_21, 	0xc2}, // [6:0]Second group of factors for B
  {LENC_CTRL_22, 	0x87}, // [7:4]Second group of factors for B|[3:0]First group of factors for B

  {LENC_CTRL_23,	0x00}, // [2]V_skip|[0]H_skip - Normal image output(b0) or Sub-sampling output(b1)

  /* White Balance Function */
  {COM25,               0x10}, // [4]Manual WB control(b0)| or ISP auto WB(b1)
  {BLUE,                0x40}, // Blue[11:4]
  {GREEN,               0x00}, // Green[11:4]
  {RED,                 0x40}, // Red[11:4]
  {BRL,                 0x00}, // [7:4]Blue[3:0]|[3:0]Red[3:0]
  {GL,                  0x00}, // [7:4]Green[3:0]
  {AWB_CTRL_0,		0x07}, // [2]AWB fast mode enable|[1]AWB bias enable|[0]AWB bias plus enable - add bias after AWB gain is calculated
  {AWB_CTRL_2,		0x04}, // AWB stable range 1
  {AWB_CTRL_3,		0x08}, // AWB stable range 2
  {AWB_CTRL_4,		0x00}, // AWB frame count
  {AWB_CTRL_5,		0x20}, // AWB delta - steps toward stable range

  /* YAVG Registers */
  {DSP_CTRL_6,		0x00}, // blk_thresh
  {DSP_CTRL_7,		0xff}, // wht_thresh
  {YAVG_CTRL_0,		0xa0}, // yavg_winh
  {YAVG_CTRL_1,		0xc8}, // yavg_winv
  {YAVG_CTRL_2,		0x00}, // yavg_winofh[7:0]
  {YAVG_CTRL_3,		0x00}, // yavg_winofv[7:0]
  {YAVG_CTRL_4,		0x00}, // [7]yavg_win_man|[5:4]yavg_winofv[9:8]|[2:0]yavg_winh[10:8]

  /* System Control Registers */
  {SC_CTRL_0,           0x0c}, // [7]Bypass DVP|[5]snr_rst_ctrl_for DVP|[4]snr_rst_ctrl for DSP|[3]DVP gated clock|[2]DSP gated clock|[1]DVP reset|[0]DSP reset

  /* DVP Control Register */
  {DVP_CTRL_00,		0x80}, // [7:6]VSYNC - vsync_old(b00), vsync_new(b01), or vsync3(b10)|[5]pclk_gate_en|[4]vsync_gate|[3]vsync3_w_sel|[2]pclk reverse|[1]href reverse|[0]vsync reverse
  {DVP_CTRL_01,		0x20}, // [7]ccir656_en|[6]sync_code_sel|[5]blk_656|[4]eav_first|[3:1]dvp_data_sequence - See datasheet table 6-2|[0]chg_flag
  {DVP_CTRL_02,		0xab}, // FS for CCIR656
  {DVP_CTRL_03,		0xb6}, // FE for CCIR656
  {DVP_CTRL_04,		0x80}, // FS for CCIR656
  {DVP_CTRL_05,		0x9d}, // LE for CCIR656
  {DVP_CTRL_06,		0x00}, // [7]blk_tog|[6]vsync_width_sel|[5:3]h2v delay|[2:0]v2h delay
  {DVP_CTRL_07,		0x80}, // vsync_width_l
  {DVP_CTRL_08,		0x24}, // [7:6]vsync_width_h|[5:4]sof2h delay offset|[3]ddr_phase|[2]Test pattern 10bit(b0) or 8bit(b1)[1:0]No pattern(b00), 1,2,4,8,?(b01), or 1,1,2,2,?(b10)
  {DVP_CTRL_09,         0xa6}, // [7]sof_rst_en,[6:5]chg_sel for recalculating DVP timing for HSYNC mode|[4]hsync_en|[3]hsync_dvp_en|[2]hsync_1st_en|[1:0]snr(b00), reg(b01), or auto(b10)
  {DVP_CTRL_0a,		0x40}, // [7:4]FIFO buffer size|[3:0]dmy_line_nu
  {DVP_CTRL_0b,		0x00}, // eof2v delay[23:16]
  {DVP_CTRL_0c,		0x01}, // eof2v delay[15:8]
  {DVP_CTRL_0d,		0x00}, // eof2v delay[7:0]
  {DVP_CTRL_0e,		0x00}, // line_length[15:8]
  {DVP_CTRL_0f,		0x00}, // line_length[7:0]
  {DVP_CTRL_10,		0x00}, // clip_max[9:2]
  {DVP_CTRL_11,		0x00}, // clip_min[9:2]
  {DVP_CTRL_12,		0x00}, // [5]f_tog_en|[4]ddr_en|[3:2]clipmin[1:0]|[1:0]clip_max[1:0]

  {ENDR}
};
#endif
