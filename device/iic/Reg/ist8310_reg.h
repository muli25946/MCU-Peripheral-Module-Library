#ifndef IST8310_REG_H
#define IST8310_REG_H

//设备地址
#define IST8310_I2C_ADDR 0x0E

/* IST8310 三轴磁力计寄存器地址定义 */

/* 设备识别寄存器 */
#define IST8310_CHIP_ID_ADDR 0x00    /* 芯片ID寄存器地址 */
#define IST8310_CHIP_ID_VAL 0x10     /* 芯片ID期望值 */

/* 状态寄存器1 */
#define IST8310_STAT1_ADDR 0x02      /* 状态寄存器1地址 */

/* 3轴磁力计数据寄存器 - 每个轴16位数据(高8位+低8位) */
#define IST8310_DATA_XL_ADDR 0x03    /* X轴数据低字节地址 */
#define IST8310_DATA_XH_ADDR 0x04    /* X轴数据高字节地址 */
#define IST8310_DATA_YL_ADDR 0x05    /* Y轴数据低字节地址 */
#define IST8310_DATA_YH_ADDR 0x06    /* Y轴数据高字节地址 */
#define IST8310_DATA_ZL_ADDR 0x07    /* Z轴数据低字节地址 */
#define IST8310_DATA_ZH_ADDR 0x08    /* Z轴数据高字节地址 */

/* 状态寄存器2 */
#define IST8310_STAT2_ADDR 0x09      /* 状态寄存器2地址 */

/* 控制寄存器1 - 设备工作模式控制 */
#define IST8310_CNTL1_ADDR 0x0A      /* 控制寄存器1地址 */
#define IST8310_CNTL1_SLEEP 0x00     /* 睡眠模式 */
#define IST8310_CNTL1_SINGLE 0x01    /* 单次测量模式 */
#define IST8310_CNTL1_CONTINUE 0x0B  /* 连续测量模式 */

/* 控制寄存器2 */
#define IST8310_CNTL2_ADDR 0x0B      /* 控制寄存器2地址 */

/* 状态寄存器2标志位 */
#define IST8310_STAT2_NONE_ALL 0x00  /* 无状态标志 */

/* 自检寄存器 */
#define IST8310_SELF_CHECK_ADDR 0x0C /* 自检寄存器地址 */

/* 温度数据寄存器 */
#define IST8310_TEMPL_ADDR 0x1C      /* 温度数据低字节地址 */
#define IST8310_TEMPH_ADDR 0x1D      /* 温度数据高字节地址 */

/* 平均采样控制寄存器 */
#define IST8310_AVGCNTL_ADDR 0x41    /* 平均采样控制寄存器地址 */
#define IST8310_AVGCNTL_TWICE 0x09   /* 2次采样平均 */
#define IST8310_AVGCNTL_FOURTH 0x12  /* 4次采样平均 */

#endif
