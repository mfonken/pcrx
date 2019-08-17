/*******************************************************************************
* File Name: ov9712.h
* Author: Matthew Fonken
******************************************************************************/

#ifndef _OV9712_H    /* Guard against multiple inclusion */
#define _OV9712_H

/************************************************************************
 *                           Includes                                   *
 ***********************************************************************/
#include <stdint.h>

/************************************************************************
 *                        OV9712 Registers                              *
 ***********************************************************************/
#define OV9712_ADDR     0x60

//<editor-fold defaultstate="collapsed" desc="OV9712 Registers">
#define GAIN		 	0x00
#define BLUE		 	0x01
#define RED     	    0x02
#define REG03		    0x03
#define REG04		 	0x04
#define GREEN		 	0x05
#define BRL             0x06
#define GL              0x07
#define REG08		 	0x08
#define COM2		 	0x09
#define PIDH		 	0x0a
#define PIDL		 	0x0b
#define COM3		 	0x0c
#define COM4		 	0x0d
#define COM5		 	0x0e
#define COM6		 	0x0f
#define AEC             0x10
#define CLK             0x11
#define COM7		 	0x12
#define COM8		 	0x13
#define COM9		 	0x14
#define COM10		 	0x15
#define AECH		 	0x16
#define HSTART		    0x17
#define AHSIZE          0x18
#define VSTART		    0x19
#define AVSIZE		    0x1a
#define PSHIFT	        0x1b
#define MIDH		 	0x1c
#define MIDL		 	0x1d
#define COM11		 	0x1e
#define REG1F		 	0x1f
#define COM12		 	0x20
#define COM13		 	0x21
#define COM14		 	0x22
#define COM15		 	0x23
#define WPT             0x24
#define BPT             0x25
#define VPT             0x26
#define COM16		 	0x27
#define COM17		 	0x28
#define COM18		 	0x29
#define REG2A		 	0x2a
#define REG2B		 	0x2b
#define COM19		 	0x2c
#define REG2D		 	0x2d
#define REG2E		 	0x2e
#define YAVG		 	0x2f
#define REG30		 	0x30
#define REG31		 	0x31
#define REG32		 	0x32
#define COM24		 	0x37
#define COM25		 	0x38
#define COM28		 	0x3c
#define RENDL		 	0x3d
#define RENDH		 	0x3e
#define REG41		 	0x41
#define REG42		 	0x42
#define REG43		 	0x43
#define ROFFS		 	0x44
#define BOFFS		 	0x45
#define GROFFS		    0x46
#define GBOFFS		    0x47
#define HOFFS		 	0x48
#define REG49		 	0x49
#define REG4A		 	0x4a
#define REG4E		 	0x4e
#define REG4F		 	0x4f
#define REG50		 	0x50
#define REG51		 	0x51
#define REG55		 	0x55
#define REG56		 	0x56
#define REG57		 	0x57
#define REG58		 	0x58
#define REG59		 	0x59
#define REG5C		 	0x5c
#define REG5D		 	0x5d
#define REG72		 	0x72
#define REG73		 	0x73
#define REG74		 	0x74
#define REG75		 	0x75
#define REG76		 	0x76
#define REG77		 	0x77
#define REG78		 	0x78
#define REG79		 	0x79
#define DSP_CTRL_0 	    0x96
#define DSP_CTRL_1 	    0x97
#define DSP_CTRL_2 	    0x98
#define DSP_CTRL_3      0x99
#define DSP_CTRL_4      0x9a
#define DSP_CTRL_5      0x9b
#define DSP_CTRL_6      0x9c
#define DSP_CTRL_7      0x9d
#define LENC_CTRL_0     0x9e
#define LENC_CTRL_1     0x9f
#define LENC_CTRL_2     0xa0
#define LENC_CTRL_3     0xa1
#define LENC_CTRL_4     0xa2
#define LENC_CTRL_5     0xa3
#define LENC_CTRL_6     0xa4
#define LENC_CTRL_7     0xa5
#define LENC_CTRL_8     0xa6
#define LENC_CTRL_9     0xa7
#define LENC_CTRL_10    0xa8
#define LENC_CTRL_11    0xa9
#define LENC_CTRL_12    0xaa
#define LENC_CTRL_13    0xab
#define LENC_CTRL_14    0xac
#define LENC_CTRL_15    0xad
#define LENC_CTRL_16    0xae
#define LENC_CTRL_17    0xaf
#define LENC_CTRL_18    0xb0
#define LENC_CTRL_19    0xb1
#define LENC_CTRL_20    0xb2
#define LENC_CTRL_21    0xb3
#define LENC_CTRL_22    0xb4
#define LENC_CTRL_23    0xb5
#define AWB_CTRL_0	    0xb6
#define AWB_CTRL_1		0xb7
#define AWB_CTRL_2	    0xb8
#define AWB_CTRL_3		0xb9
#define AWB_CTRL_4		0xba
#define AWB_CTRL_5		0xbb
#define WBC_CTRL_0		0xbc
#define YAVG_CTRL_0		0xbd
#define YAVG_CTRL_1		0xbe
#define YAVG_CTRL_2		0xbf
#define YAVG_CTRL_3		0xc0
#define YAVG_CTRL_4		0xc1
#define DVP_CTRL_00		0xc2
#define DVP_CTRL_01		0xc3
#define DVP_CTRL_02		0xc4
#define DVP_CTRL_03		0xc5
#define DVP_CTRL_04		0xc6
#define DVP_CTRL_05		0xc7
#define DVP_CTRL_06		0xc8
#define DVP_CTRL_07		0xc9
#define DVP_CTRL_08		0xca
#define DVP_CTRL_09		0xcb
#define DVP_CTRL_0a		0xcc
#define DVP_CTRL_0b		0xcd
#define DVP_CTRL_0c		0xce
#define DVP_CTRL_0d		0xcf
#define DVP_CTRL_0e		0xd0
#define DVP_CTRL_0f		0xd1
#define DVP_CTRL_10		0xd2
#define DVP_CTRL_11		0xd3
#define DVP_CTRL_12		0xd4
#define SC_CTRL_0		0xd6
#define ENDR			0xff

/*	TERMS *
* AEC - Auto Exposure Control
* AGC - Auto Gain Control
* BLC - Black level Calibration
**/

#endif

/* *****************************************************************************
End of File
*/
