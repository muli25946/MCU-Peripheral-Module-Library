/**
 * @file nrf24l01.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief NRF24L01驱动，使用函数注入的方式，以隔离处理逻辑和底层硬件，抹平平台差异
 * @version 0.1
 * @date 2025-10-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>

/*定义NRF24L01错误枚举*/
typedef enum NRF24L01Error {
  NRF24L01_NoError,
  NRF24L01_InitError,
  NRF24L01_Absent
} NRF24L01ErrorType;

/* 定义片选信号枚举 */
typedef enum NRF24L01CS {
  NRF24L01CS_Enable,
  NRF24L01CS_Disable
} NRF24L01CSType;

/* 定义使能信号枚举 */
typedef enum NRF24L01CE {
  NRF24L01CE_Enable,
  NRF24L01CE_Disable
} NRF24L01CEType;

/*定义模式枚举*/
typedef enum NRF24L01Mode {
  NRF24L01TxMode = 0,
  NRF24L01RxMode = 1
} NRF24L01ModeType;

/* 定义NRF24L01对象类型 */
typedef struct NRF24L01Object {
  /*data*/
  uint8_t reg[8];                           // 记录前8个配置寄存器
  /*functions*/
  uint8_t (*ReadWriteByte)(uint8_t TxData); // 声明向nRF24L01读写一个字节的函数
  void (*ChipSelect)(NRF24L01CSType cs);    // 声明片选操作函数
  void (*ChipEnable)(NRF24L01CEType en);    // 声明使能及模式操作函数
  uint8_t (*GetIRQ)(void);                  // 声明中断获取函数
  void (*Delayms)(volatile uint32_t nTime); // 毫秒延时操作指针
} NRF24L01ObjectType;

typedef uint8_t (*NRF24L01ReadWriteByte)(
    uint8_t TxData); // 声明向nRF24L01读写一个字节的函数
typedef void (*NRF24L01ChipSelect)(NRF24L01CSType cs); // 声明片选操作函数
typedef void (*NRF24L01ChipEnable)(NRF24L01CEType en); // 声明使能及模式操作函数
typedef uint8_t (*NRF24L01GetIRQ)(void);               // 声明中断获取函数
typedef void (*NRF24L01Delayms)(volatile uint32_t nTime); // 毫秒延时操作指针

/*启动NRF24L01发送一次数据包*/
uint8_t NRF24L01TransmitPacket(NRF24L01ObjectType *nrf, uint8_t *txbuf);

/*启动NRF24L01接收一次数据包*/
uint8_t NRF24L01ReceivePacket(NRF24L01ObjectType *nrf, uint8_t *rxbuf);

/*nRF24L01对象初始化函数*/
NRF24L01ErrorType
NRF24L01Initialization(NRF24L01ObjectType *nrf,            // nRF24L01对象
                       NRF24L01ReadWriteByte spiReadWrite, // SPI读写函数指针
                       NRF24L01ChipSelect cs,  // 片选信号操作函数指针
                       NRF24L01ChipEnable ce,  // 使能信号操作函数指针
                       NRF24L01GetIRQ irq,     // 中断信号获取函数指针
                       NRF24L01Delayms delayms // 毫秒延时
);

/*定义操作状态*/
#define MAX_TX 0x10 // 达到最大发送次数中断
#define TX_OK 0x20  // TX发送完成中断
#define RX_OK 0x40  // 接收到数据中断

//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH 5    // 5字节的地址宽度
#define RX_ADR_WIDTH 5    // 5字节的地址宽度
#define TX_PLOAD_WIDTH 32 // 32字节的用户数据宽度
#define RX_PLOAD_WIDTH 32 // 32字节的用户数据宽度

#endif
