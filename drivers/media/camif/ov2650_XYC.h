#define D50HZ

#define	OV2650_I2C_WADDR		0x30
#define OV2650_I2C_RADDR		0x30

uint16_t	OV2650_3000	= 0x0030;
uint16_t	OV2650_3002	= 0x0230;
uint16_t	OV2650_3003	= 0x0330;
uint16_t	OV2650_3013 	= 0x1330;
uint16_t	OV2650_3014	= 0x1430;
uint16_t	OV2650_3028	= 0x2830;
uint16_t	OV2650_3029	= 0x2930;
uint16_t	OV2650_302d	= 0x2d30;
uint16_t	OV2650_302e	= 0x2e30;
uint16_t	OV2650_302a	= 0x2a30;
uint16_t	OV2650_302b	= 0x2b30;
uint16_t	OV2650_3070	= 0x7030;
uint16_t	OV2650_3071	= 0x7130;
uint16_t	OV2650_3072	= 0x7230;
uint16_t	OV2650_3073	= 0x7330;
uint16_t	OV2650_301d	= 0x1d30;
uint16_t	OV2650_301c	= 0x1c30;
uint16_t	OV2650_3011	= 0x1130;

uint16_t	OV2650_PIDH = 0x0A30;
uint16_t	OV2650_PIDL = 0X0B30;

struct ov2650_regval_list {
	unsigned char  reg_high;
	unsigned char  reg_low;
	unsigned char  value;
};

struct ov2650_regval_list  ov2650_before[] = {
	{0x30, 0xb1, 0x00},
};

struct ov2650_regval_list  ov2650_after[] = {
	{0x30, 0xb1, 0xff},
};

struct ov2650_regval_list  ov2650_svga_low_regs[] = {
	{0x30, 0x0e, 0x36},
};

struct ov2650_regval_list  ov2650_to_preview_320_240_regs[] = {
	{0x30, 0x2e, 0x00},
	{0x30, 0x2d, 0x00},
	{0x30, 0x11, 0x00},
	{0x30, 0x0e, 0x36},
	{0x30, 0x12, 0x10},
	{0x30, 0x2a, 0x02},
	{0x30, 0x2b, 0x6a},
	{0x30, 0x6f, 0x14},
	{0x30, 0x20, 0x01},
	{0x30, 0x21, 0x18},
	{0x30, 0x22, 0x00},
	{0x30, 0x23, 0x06},
	{0x30, 0x24, 0x06},
	{0x30, 0x25, 0x58},
	{0x30, 0x26, 0x02},
	{0x30, 0x27, 0x61},
	{0x30, 0x88, 0x01},
	{0x30, 0x89, 0x40},
	{0x30, 0x8a, 0x00},
	{0x30, 0x8b, 0xf0},
	{0x33, 0x16, 0x64},
	{0x33, 0x17, 0x25},
	{0x33, 0x18, 0x80},
	{0x33, 0x19, 0x08},
	{0x33, 0x1a, 0x14},
	{0x33, 0x1b, 0x0f},
	{0x33, 0x1c, 0x00},
	{0x33, 0x1d, 0x3c},
	{0x33, 0x02, 0x11},
};

struct ov2650_regval_list  ov2650_to_preview_640_480_regs[] = {
	{0x30, 0x2e, 0x00},
	{0x30, 0x2d, 0x00},
	{0x30, 0x11, 0x00},
	{0x30, 0x0e, 0x36},
	{0x30, 0x12, 0x10},
	{0x30, 0x2a, 0x02},
	{0x30, 0x2b, 0x6a},
	{0x30, 0x6f, 0x14},
	{0x30, 0x20, 0x01},
	{0x30, 0x21, 0x18},
	{0x30, 0x22, 0x00},
	{0x30, 0x23, 0x06},
	{0x30, 0x24, 0x06},
	{0x30, 0x25, 0x58},
	{0x30, 0x26, 0x02},
	{0x30, 0x27, 0x61},
	{0x30, 0x88, 0x02},
	{0x30, 0x89, 0x80},
	{0x30, 0x8a, 0x01},
	{0x30, 0x8b, 0xe0},
	{0x33, 0x16, 0x64},
	{0x33, 0x17, 0x25},
	{0x33, 0x18, 0x80},
	{0x33, 0x19, 0x08},
	{0x33, 0x1a, 0x28},
	{0x33, 0x1b, 0x1e},
	{0x33, 0x1c, 0x00},
	{0x33, 0x1d, 0x3c},
	{0x33, 0x02, 0x11},
};

struct ov2650_regval_list  ov2650_svga_high_regs[] = {
	{0x30, 0x0e, 0x38},
};

struct ov2650_regval_list  ov2650_xuga_regs[] = {
	{0x30, 0x0e, 0x34},
	{0x30, 0x12, 0x00},
	{0x30, 0x2a, 0x04},
	{0x30, 0x2b, 0xd4},
	{0x30, 0x6f, 0x54},
	{0x30, 0x20, 0x01},
	{0x30, 0x21, 0x18},
	{0x30, 0x22, 0x00},
	{0x30, 0x23, 0x0a},
	{0x30, 0x24, 0x06},
	{0x30, 0x25, 0x58},
	{0x30, 0x26, 0x04},
	{0x30, 0x27, 0xbc},
};

struct ov2650_regval_list  ov2650_xuga_high_regs[] = {
	{0x30, 0x11, 0x01},
	{0x30, 0x88, 0x06},
	{0x30, 0x89, 0x40},
	{0x30, 0x8a, 0x04},
	{0x30, 0x8b, 0xb0},
	{0x33, 0x16, 0x64},
	{0x33, 0x17, 0x4b},
	{0x33, 0x18, 0x00},
	{0x33, 0x19, 0x2c},
	{0x33, 0x1a, 0x64},
	{0x33, 0x1b, 0x4b},
	{0x33, 0x1c, 0x00},
	{0x33, 0x1d, 0x4c},
	{0x33, 0x02, 0x01},
};

struct ov2650_regval_list  ov2650_xuga_upmid_regs[] = {
	{0x30, 0x11, 0x01},
	{0x30, 0x88, 0x03},
	{0x30, 0x89, 0x20},
	{0x30, 0x8a, 0x02},
	{0x30, 0x8b, 0x58},
	{0x33, 0x16, 0x64},
	{0x33, 0x17, 0x4b},
	{0x33, 0x18, 0x00},
	{0x33, 0x19, 0x2c},
	{0x33, 0x1a, 0x32},
	{0x33, 0x1b, 0x25},
	{0x33, 0x1c, 0x80},
	{0x33, 0x1d, 0x4c},
	{0x33, 0x02, 0x11},
};

struct ov2650_regval_list  ov2650_xuga_mid_regs[] = {
	{0x30, 0x11, 0x01},
	{0x30, 0x88, 0x02},
	{0x30, 0x89, 0x80},
	{0x30, 0x8a, 0x01},
	{0x30, 0x8b, 0xe0},
	{0x33, 0x16, 0x64},
	{0x33, 0x17, 0x4b},
	{0x33, 0x18, 0x00},
	{0x33, 0x19, 0x2c},
	{0x33, 0x1a, 0x28},
	{0x33, 0x1b, 0x1e},
	{0x33, 0x1c, 0x00},
	{0x33, 0x1d, 0x4c},
	{0x33, 0x02, 0x11},
};

struct ov2650_regval_list  ov2650_xuga_low_regs[] = {
	{0x30, 0x88, 0x01},
	{0x30, 0x89, 0x40},
	{0x30, 0x8a, 0x00},
	{0x30, 0x8b, 0xf0},
	{0x33, 0x1a, 0x14},
	{0x33, 0x1b, 0x0f},
	{0x33, 0x1c, 0x00},
	{0x33, 0x1d, 0x4c},
	{0x33, 0x02, 0x11},
};

struct ov2650_regval_list ov2650_stop_regs[] = {
	{0x30, 0xAB, 0x00},
	{0x30, 0xAD, 0x0A},
	{0x30, 0xAE, 0x27},
	{0x36, 0x3B, 0x01},
};

struct ov2650_regval_list  ov2650_init_regs[] = {
	{0x30, 0x12, 0x80},
	                   
	{0x30, 0x8c, 0x80},
	{0x30, 0x8d, 0x0e},
	{0x36, 0x0b, 0x00},
	{0x30, 0xb0, 0xff},
	{0x30, 0xb1, 0xff},
	{0x30, 0xb2, 0x24},
	{0x30, 0x0f, 0xa6},
	{0x30, 0x10, 0x81},
	{0x30, 0x82, 0x01},
	{0x30, 0xf4, 0x01},
	{0x30, 0x90, 0x33},
	{0x30, 0x91, 0xc0},
	{0x30, 0xac, 0x42},
	{0x30, 0xd1, 0x08},
	{0x30, 0xa8, 0x56},
	{0x30, 0x15, 0x01},
	{0x30, 0x93, 0x00},
	{0x30, 0x7e, 0xe5},
	{0x30, 0x79, 0x00},
	{0x30, 0xaa, 0x42},
	{0x30, 0x17, 0x40},
	{0x30, 0xf3, 0x82},
	{0x30, 0x6a, 0x0c},
	{0x30, 0x6d, 0x00},
	{0x33, 0x6a, 0x3c},
	{0x30, 0x76, 0x6a},
	{0x30, 0xd9, 0x95},
	{0x30 ,0x16, 0x82},
	{0x36, 0x01, 0x30},
	{0x30, 0x4e, 0x88},
	{0x30, 0xf1, 0x82},
	{0x30, 0x6f, 0x14},
	{0x30, 0x2a, 0x02},
	{0x30, 0x2b, 0x6a},
	{0x30, 0x12, 0x10},

	{0x30, 0x18, 0x80},
	{0x30, 0x19, 0x70},
	{0x30, 0x1a, 0xb4},

	{0x30, 0xaf, 0x00},
	{0x30, 0x48, 0x1f},
	{0x30, 0x49, 0x4e},
	{0x30, 0x4a, 0x20},
	{0x30, 0x4f, 0x20},
	{0x30, 0x4b, 0x02},
	{0x30, 0x4c, 0x00},
	{0x30, 0x4d, 0x02},
	{0x30, 0x4f, 0x20},
	{0x30, 0xa3, 0x10},
	{0x30, 0x13, 0xf7},
#ifdef D50HZ
	{0x30, 0x14, 0x84},
#endif
#ifdef D60HZ
	{0x30, 0x14, 0x04},
#endif
	{0x30, 0x71, 0x00},
	{0x30, 0x70, 0x8b},
	{0x30, 0x73, 0x00},
	{0x30, 0x72, 0x73},
	{0x30, 0x1c, 0x03},	
	{0x30, 0x1d, 0x04},
	{0x30, 0x4d, 0x42},
	{0x30, 0x4a, 0x40},
	{0x30, 0x4f, 0x40},
	{0x30, 0x95, 0x07},
	{0x30, 0x96, 0x16},
	{0x30, 0x97, 0x1d},
	                   
	{0x30, 0x20, 0x01},
	{0x30, 0x21, 0x18},
	{0x30, 0x22, 0x00},
	{0x30, 0x23, 0x06},
	{0x30, 0x24, 0x06},
	{0x30, 0x25, 0x58},
	{0x30, 0x26, 0x02},
	{0x30, 0x27, 0x61},
	{0x30, 0x88, 0x02},
	{0x30, 0x89, 0x80},
	{0x30, 0x8a, 0x01},
	{0x30, 0x8b, 0xe0},
	{0x33, 0x16, 0x64},
	{0x33, 0x17, 0x25},
	{0x33, 0x18, 0x80},
	{0x33, 0x19, 0x08},
	{0x33, 0x1a, 0x28},
	{0x33, 0x1b, 0x1e},
	{0x33, 0x1c, 0x00},
	{0x33, 0x1d, 0x38},
	{0x31, 0x00, 0x00},

/*
 * 	AWB
 */
	{0x33, 0x20, 0xfa},
	{0x33, 0x21, 0x11},
	{0x33, 0x22, 0x92},
	{0x33, 0x23, 0x01},
	{0x33, 0x24, 0x97},
	{0x33, 0x25, 0x02},
	{0x33, 0x26, 0xff},
	{0x33, 0x27, 0x0c},
	{0x33, 0x28, 0x10},
	{0x33, 0x29, 0x0f},
	{0x33, 0x2a, 0x51},
	{0x33, 0x2b, 0x6a},
	{0x33, 0x2c, 0xc1},
	{0x33, 0x2d, 0x90},
	{0x33, 0x2e, 0x18},
	{0x33, 0x2f, 0x36},
	{0x33, 0x30, 0x36},
	{0x33, 0x31, 0x28},
	{0x33, 0x32, 0xf0},
	{0x33, 0x33, 0x0a},
	{0x33, 0x34, 0xf0},
	{0x33, 0x35, 0xf0},
	{0x33, 0x36, 0xf0},
	{0x33, 0x37, 0x40},
	{0x33, 0x38, 0x40},
	{0x33, 0x39, 0x40},
	{0x33, 0x3a, 0x00},
	{0x33, 0x3b, 0x00},

	{0x33, 0x80, 0x27},
	{0x33, 0x81, 0x5c},
	{0x33, 0x82, 0x0a},
	{0x33, 0x83, 0x2f},
	{0x33, 0x84, 0xc7},
	{0x33, 0x85, 0xf7},
	{0x33, 0x86, 0xea},
	{0x33, 0x87, 0xe7},
	{0x33, 0x88, 0x03},
	{0x33, 0x89, 0x98},
	{0x33, 0x8a, 0x01},

	{0x33, 0x40, 0x08},
	{0x33, 0x41, 0x16},
	{0x33, 0x42, 0x2f},
	{0x33, 0x43, 0x45},
	{0x33, 0x44, 0x56},
	{0x33, 0x45, 0x66},
	{0x33, 0x46, 0x72},
	{0x33, 0x47, 0x7c},
	{0x33, 0x48, 0x86},
	{0x33, 0x49, 0x96},
	{0x33, 0x4a, 0xa3},
	{0x33, 0x4b, 0xaf},
	{0x33, 0x4c, 0xc4},
	{0x33, 0x4d, 0xd7},
	{0x33, 0x4e, 0xe8},
	{0x33, 0x4f, 0x20},

#if 0 
/*
 * 	lens shading
 */

// R
	{0x33, 0x50, 0x2e}, // X
	{0x33, 0x51, 0x23}, // Y
	{0x33, 0x52, 0x00},
	{0x33, 0x53, 0x16}, // Gain
	{0x33, 0x54, 0x00},
	{0x33, 0x55, 0x85},
// G
	{0x33, 0x56, 0x2c}, // X
	{0x33, 0x57, 0x21}, // Y
	{0x33, 0x58, 0x00},
	{0x33, 0x59, 0x15}, // Gain
	{0x33, 0x5a, 0x00},
	{0x33, 0x5b, 0x85},
// B
	{0x33, 0x5c, 0x2c}, // X
	{0x33, 0x5d, 0x21}, // Y
	{0x33, 0x5e, 0x00}, 
	{0x33, 0x5f, 0x16}, // Gain
	{0x33, 0x60, 0xff},
	{0x33, 0x61, 0x85},

	{0x33, 0x63, 0x01},
	{0x33, 0x64, 0x03},
	{0x33, 0x65, 0x02},
	{0x33, 0x66, 0x00},
#else
/*
 * 	lens shading
 */

// R
	{0x33, 0x50, 0x2e}, // X
	{0x33, 0x51, 0x23}, // Y
	{0x33, 0x52, 0x00},
	{0x33, 0x53, 0x16}, // Gain
	{0x33, 0x54, 0x00},
	{0x33, 0x55, 0x85},
// G
	{0x33, 0x56, 0x2d}, // X
	{0x33, 0x57, 0x21}, // Y
	{0x33, 0x58, 0x40},
	{0x33, 0x59, 0x15}, // Gain
	{0x33, 0x5a, 0x00},
	{0x33, 0x5b, 0x85},
// B
	{0x33, 0x5c, 0x30}, // X
	{0x33, 0x5d, 0x1f}, // Y
	{0x33, 0x5e, 0x00}, 
	{0x33, 0x5f, 0x16}, // Gain
	{0x33, 0x60, 0xff},
	{0x33, 0x61, 0x85},

	{0x33, 0x63, 0x01},
	{0x33, 0x64, 0x03},
	{0x33, 0x65, 0x02},
	{0x33, 0x66, 0x00},
#endif
	                   
	{0x33, 0x01, 0xff},
	{0x33, 0x8b, 0x11},
	{0x33, 0x8c, 0x10},
	{0x33, 0x8d, 0x40},
	                   
	{0x33, 0x70, 0xd0},
	{0x33, 0x71, 0x00},
	{0x33, 0x72, 0x00},
#if 0
	{0x33, 0x73, 0x30},
	{0x33, 0x74, 0x10},
	{0x33, 0x75, 0x10},
	{0x33, 0x76, 0x05},
	{0x33, 0x77, 0x00},
	{0x33, 0x78, 0x04},
	{0x33, 0x79, 0x80},
#else
	{0x33, 0x73, 0x30},
	{0x33, 0x74, 0x10},
	{0x33, 0x75, 0x10},
	{0x33, 0x76, 0x01},
	{0x33, 0x77, 0x00},
	{0x33, 0x78, 0x02},
	{0x33, 0x79, 0xf0},
#endif

	{0x30, 0x69, 0x84},
	{0x30, 0x7c, 0x12},
	{0x30, 0x87, 0x02},
	                   
	{0x33, 0x00, 0xfc},
	{0x33, 0x02, 0x11},
	{0x34, 0x00, 0x00},
	{0x36, 0x06, 0x20},
	{0x36, 0x01, 0x30},
	{0x30, 0x0e, 0x36},
	{0x30, 0x11, 0x00},
	{0x30, 0xf3, 0x83},
	{0x30, 0x4e, 0x88},

	{0x30, 0x90, 0x0B},
	{0x30, 0xaa, 0x32},
	{0x30, 0xa3, 0x80},
	{0x30, 0xa1, 0x41},
	{0x36, 0x3b, 0x01},
	{0x36, 0x3c, 0xf2},
	{0x30, 0x86, 0x0f},

	{0x30, 0x86, 0x00},
};

/*****************************************************\
 *************** Special Effects *********************
\*****************************************************/

struct ov2650_regval_list  ov2650_sepia_regs[] = {
	{0x33, 0x91, 0x18},
	{0x33, 0x96, 0x40},
	{0x33, 0x97, 0xa6},
};

struct ov2650_regval_list  ov2650_bluish_regs[] = {
	{0x33, 0x91, 0x18},
	{0x33, 0x96, 0xa0},
	{0x33, 0x97, 0x40},
};

struct ov2650_regval_list  ov2650_greenish_regs[] = {
	{0x33, 0x91, 0x18},
	{0x33, 0x96, 0x60},
	{0x33, 0x97, 0x60},
};

struct ov2650_regval_list  ov2650_reddish_regs[] = {
	{0x33, 0x91, 0x18},
	{0x33, 0x96, 0x80},
	{0x33, 0x97, 0xc0},
};

struct ov2650_regval_list  ov2650_yellowish_regs[] = {
	{0x33, 0x91, 0x18},
	{0x33, 0x96, 0x30},
	{0x33, 0x97, 0x90},
};

struct ov2650_regval_list  ov2650_bandw_regs[] = {
	{0x33, 0x91, 0x20},
};

struct ov2650_regval_list  ov2650_negative_regs[] = {
	{0x33, 0x91, 0x40},
};

struct ov2650_regval_list  ov2650_normal_regs[] = {
	{0x33, 0x91, 0x00},
};

/*****************************************************\
 *************** Light Mode **************************
\*****************************************************/

struct ov2650_regval_list  ov2650_auto_regs[] = {
	{0x33, 0x06, 0x00},
};

struct ov2650_regval_list  ov2650_sunny_regs[] = {
	{0x33, 0x06, 0x02},
	{0x33, 0x37, 0x5e},
	{0x33, 0x38, 0x40},
	{0x33, 0x39, 0x46},
};

struct ov2650_regval_list  ov2650_cloudy_regs[] = {
	{0x33, 0x06, 0x02},
	{0x33, 0x37, 0x68},
	{0x33, 0x38, 0x40},
	{0x33, 0x39, 0x4e},
};

struct ov2650_regval_list  ov2650_office_regs[] = {
	{0x33, 0x06, 0x02},
	{0x33, 0x37, 0x52},
	{0x33, 0x38, 0x40},
	{0x33, 0x39, 0x58},
};

struct ov2650_regval_list  ov2650_home_regs[] = {
	{0x33, 0x06, 0x02},
	{0x33, 0x37, 0x44},
	{0x33, 0x38, 0x40},
	{0x33, 0x39, 0x70},
};

/*****************************************************\
 *************** Color Saturation ********************
\*****************************************************/

struct ov2650_regval_list  ov2650_saturation_0_regs[] = {
	{0x33, 0x91, 0x02},
	{0x33, 0x94, 0x70},
	{0x33, 0x95, 0x70},
};

struct ov2650_regval_list  ov2650_saturation_1_regs[] = {
	{0x33, 0x91, 0x02},
	{0x33, 0x94, 0x50},
	{0x33, 0x95, 0x50},
};

struct ov2650_regval_list  ov2650_saturation_2_regs[] = {
	{0x33, 0x91, 0x02},
	{0x33, 0x94, 0x40},
	{0x33, 0x95, 0x40},
};

struct ov2650_regval_list  ov2650_saturation_3_regs[] = {
	{0x33, 0x91, 0x02},
	{0x33, 0x94, 0x30},
	{0x33, 0x95, 0x30},
};

struct ov2650_regval_list  ov2650_saturation_4_regs[] = {
	{0x33, 0x91, 0x02},
	{0x33, 0x94, 0x20},
	{0x33, 0x95, 0x20},
};

/*****************************************************\
 *************** Brightness **************************
\*****************************************************/

struct ov2650_regval_list  ov2650_brightness_0_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x41},
	{0x33, 0x9a, 0x30},
};

struct ov2650_regval_list  ov2650_brightness_1_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x41},
	{0x33, 0x9a, 0x20},
};

struct ov2650_regval_list  ov2650_brightness_2_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x41},
	{0x33, 0x9a, 0x10},
};

struct ov2650_regval_list  ov2650_brightness_3_regs[] = {
	{0x33, 0x91, 0x00},
	{0x33, 0x90, 0x41},
	{0x33, 0x9a, 0x00},
};

struct ov2650_regval_list  ov2650_brightness_4_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x49},
	{0x33, 0x9a, 0x10},
};

struct ov2650_regval_list  ov2650_brightness_5_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x49},
	{0x33, 0x9a, 0x20},
};

struct ov2650_regval_list  ov2650_brightness_6_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x49},
	{0x33, 0x9a, 0x30},
};

/*****************************************************\
 *************** Contrast ****************************
\*****************************************************/

struct ov2650_regval_list  ov2650_contrast_0_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x45},
	{0x33, 0x98, 0x2c},
	{0x33, 0x99, 0x2c},
};

struct ov2650_regval_list  ov2650_contrast_1_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x45},
	{0x33, 0x98, 0x28},
	{0x33, 0x99, 0x28},
};

struct ov2650_regval_list  ov2650_contrast_2_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x45},
	{0x33, 0x98, 0x24},
	{0x33, 0x99, 0x24},
};

struct ov2650_regval_list  ov2650_contrast_3_regs[] = {
	{0x33, 0x91, 0x00},
	{0x33, 0x90, 0x41},
	{0x33, 0x98, 0x20},
	{0x33, 0x99, 0x20},
};

struct ov2650_regval_list  ov2650_contrast_4_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x45},
	{0x33, 0x98, 0x1c},
	{0x33, 0x99, 0x1c},
};

struct ov2650_regval_list  ov2650_contrast_5_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x45},
	{0x33, 0x98, 0x18},
	{0x33, 0x99, 0x18},
};

struct ov2650_regval_list  ov2650_contrast_6_regs[] = {
	{0x33, 0x91, 0x04},
	{0x33, 0x90, 0x45},
	{0x33, 0x98, 0x14},
	{0x33, 0x99, 0x14},
};

/*****************************************************\
 *************** Sharpness ***************************
\*****************************************************/

struct ov2650_regval_list  ov2650_sharpness_0_regs[] = {
	{0x33, 0x06, 0x08},
	{0x33, 0x71, 0x00},
};

struct ov2650_regval_list  ov2650_sharpness_1_regs[] = {
	{0x33, 0x06, 0x08},
	{0x33, 0x71, 0x01},
};

struct ov2650_regval_list  ov2650_sharpness_2_regs[] = {
	{0x33, 0x06, 0x08},
	{0x33, 0x71, 0x02},
};

struct ov2650_regval_list  ov2650_sharpness_3_regs[] = {
	{0x33, 0x06, 0x08},
	{0x33, 0x71, 0x03},
};

struct ov2650_regval_list  ov2650_sharpness_4_regs[] = {
	{0x33, 0x06, 0x08},
	{0x33, 0x71, 0x04},
};

struct ov2650_regval_list  ov2650_sharpness_auto_regs[] = {
	{0x33, 0x06, 0x00},
};

/*****************************************************\
 *************** Night Mode **************************
\*****************************************************/

struct ov2650_regval_list  ov2650_night_mode_on_regs[] = {
	{0x30, 0x14, 0x0c},
	{0x30, 0x15, 0x50},
	{0x30, 0x2e, 0x00},
	{0x30, 0x2d, 0x00},
};

struct ov2650_regval_list  ov2650_night_mode_off_regs[] = {
	{0x30, 0x14, 0x04},
	{0x30, 0x15, 0x00},
	{0x30, 0x2e, 0x00},
	{0x30, 0x2d, 0x00},
};
