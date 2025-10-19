/**
 * @file mpu6050.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief
 * MPU6050驱动，使用函数注入的方式，以隔离处理裸机和底层硬件，抹平平台差异
 * @version 0.1
 * @date 2025-10-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

/*MPU6050错误类型*/
typedef enum {
  MPU6050_NoError,
  MPU6050_InitError,
  MPU6050_Absent
} MPU6050ErrorType;

/*物理值结构体*/
typedef struct physicalValue {
  float x;
  float y;
  float z;
} PhysicalType;

/*数据结构体*/
typedef struct MPU6050 {
  int16_t acc[3];  // 加速度[x,y,z]
  int16_t gyro[3]; // 陀螺仪[x,y,z]
  uint16_t temp;   // 温度
  PhysicalType redirectAcc;
  PhysicalType redirectGyro;
} mpu6050_struct;

/*MPU6050对象类型*/
typedef struct {
  /*data*/
  mpu6050_struct data;
  /*functions*/
  uint8_t (*ReadBuf)(uint8_t reg, uint8_t *pRxBuf,
                     uint8_t len); // 声明读取指定寄存器上多个字节的函数
  uint8_t (*WriteBuf)(uint8_t reg, uint8_t *pTxBuf,
                      uint8_t len); // 声明写入指定寄存器指定长度数据的函数
  void (*Delayms)(uint32_t nTime);  // 声明毫秒延迟函数
} MPU6050ObjectType;

/*需要实现的接口*/
typedef uint8_t (*MPU6050ReadBuf)(
    uint8_t reg, uint8_t *pRxBuf,
    uint8_t len); // 声明读取指定寄存器上多个字节的函数
typedef uint8_t (*MPU6050WriteBuf)(
    uint8_t reg, uint8_t *pTxBuf,
    uint8_t len); // 声明写入指定寄存器指定长度数据的函数
typedef void (*MPU6050Delayms)(uint32_t nTime); // 声明毫秒延迟函数

/*声明函数*/
MPU6050ErrorType MPU6050ObjectInit(MPU6050ObjectType *mpu6050,
                                   MPU6050ReadBuf read, MPU6050WriteBuf write,
                                   MPU6050Delayms delay);
void MPU6050GetRedirectValue(MPU6050ObjectType *mpu6050);

#endif // !MPU6050_H
