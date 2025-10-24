/**
 * @file hc05.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief HC05驱动，使用函数注入的方式，以隔离处理逻辑和底层硬件，抹平平台差异
 * @version 0.1
 * @date 2025-10-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef HC05_H
#define HC05_H

#include <stdint.h>

/*HC05错误类型*/
typedef enum {
  HC05_Ok,
  HC05_InitError,
  HC05_Absent,
} HC05ErrorType;

/*HC05数据包*/
typedef struct {
  uint8_t head;   // 包头
  uint8_t id;     // 数据帧代号
  uint8_t *pData; // 载荷数据头
  uint8_t len;    // 数据长度
} HC05Packet;

/*HC05对象类型*/
typedef struct {
  /*functions*/
  uint8_t (*TxData)(uint8_t *txBuf,
                    uint8_t len); // 绑定串口发送多个字节的函数到设备HC05
  uint8_t (*RxData)(uint8_t *rxBuf,
                    uint8_t len); // 绑定串口接收多个字节的函数到设备HC05

} HC05ObjectType;

/*需要实现的供HC05使用的串口函数接口*/
typedef uint8_t (*HC05TxData)(uint8_t *txBuf,
                              uint8_t len); // 声明使用串口发送多个字节的函数
typedef uint8_t (*HC05RxData)(uint8_t *rxBuf,
                              uint8_t len); // 声明使用串口接收多个字节的函数

/*声明调用函数*/
HC05ErrorType HC05ObjectInit(HC05ObjectType *hc05, HC05TxData tx,
                             HC05RxData rx);
uint8_t HC05TxPacket(HC05ObjectType *hc05, HC05Packet *txPacket);
uint8_t HC05RxPacket(HC05ObjectType *hc05, uint8_t *rxData, uint8_t len);

#endif
