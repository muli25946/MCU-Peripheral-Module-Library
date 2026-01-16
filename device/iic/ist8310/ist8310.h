/**
 * @file ist8310.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief IST8310驱动，使用函数注入的方式，以隔离处理逻辑和底层硬件，抹平平台差异
 * @version 0.1
 * @date 2025-10-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef IST8310_H
#define IST8310_H

/*std*/
#include <stdint.h>

/*整形向uT转换*/
#define MAG_SEN 0.3f

/*定义IST8310错误枚举*/
typedef enum {
  IST8310_NoError,
  IST8310_InitError,
  IST8310_Absent
} IST8310ErrorType;

/*ist8310存储数据结构体*/
typedef struct IST8310 {
  int16_t raw_x;
  int16_t raw_y;
  int16_t raw_z;
  float x;
  float y;
  float z;
  float redirect_x;
  float redirect_y;
  float redirect_z;
} ist8310_struct;

/*IST8310对象类型*/
typedef struct {
  /*data*/
  ist8310_struct data;
  /*functions*/
  uint8_t (*ReadBuf)(uint8_t reg, uint8_t *pRxBuf,
                     uint8_t len); // 声明读取指定寄存器上多个字节的函数
  uint8_t (*WriteBuf)(uint8_t reg, uint8_t *pTxBuf,
                      uint8_t len); // 声明写入指定寄存器指定长度数据的函数
  void (*Delayms)(uint32_t nTime);  // 声明毫秒延迟函数
} IST8310ObjectType;

typedef uint8_t (*IST8310_ReadBuf)(
    uint8_t reg, uint8_t *pRxBuf,
    uint8_t len); // 声明读取指定寄存器上多个字节的函数
typedef uint8_t (*IST8310_WriteBuf)(
    uint8_t reg, uint8_t *pTxBuf,
    uint8_t len); // 声明写入指定寄存器指定长度数据的函数
typedef void (*IST8310_Delayms)(uint32_t nTime); // 声明毫秒延迟函数

/*声明函数*/
IST8310ErrorType IST8310_ObjectInit(IST8310ObjectType *ist, IST8310_ReadBuf read,
                                   IST8310_WriteBuf write, IST8310_Delayms delay);
void IST8310_GetRawData(IST8310ObjectType *ist);
void IST8310_GetMegVal(IST8310ObjectType *ist);

#endif // !IST8310_H
