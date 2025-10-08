/*dependence*/
#include "hc05.h"
#include <stdint.h>

#if defined(VOFA)
const uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7f};
#endif

/**
 * @brief 初始化HC05对象
 *
 * @param hc05 hc05对象实体
 * @param tx 串口发送数据函数指针
 * @param rx 串口接收数据函数指针
 * @param delay 延时函数指针
 * @return HC05ErrorType
 */
HC05ErrorType HC05ObjectInit(HC05ObjectType *hc05, HC05TxData tx,
                             HC05RxData rx) {

  /*检查注入函数*/
  if ((tx == NULL) || (rx == NULL)) {
    return HC05_InitError;
  }

  /*注入函数*/
  hc05->TxData = tx;
  hc05->RxData = rx;

  return HC05_Ok;
}

/**
 * @brief 使用hc05发送出一包数据
 *
 * @param hc05 hc05对象实体
 * @param txPacket 要发送的数据包
 * @return uint8_t
 */
uint8_t HC05TxPacket(HC05ObjectType *hc05, HC05Packet *txPacket) {
  uint8_t status;

  status += hc05->TxData(&txPacket->head, 1);
  status += hc05->TxData(&txPacket->id, 1);
  status += hc05->TxData(txPacket->pData, txPacket->len);
#if defined(VOFA)
  status += hc05->TxData(tail, 4);
#endif
  return status;
}

/**
 * @brief 使用hc05接收定长数据
 *
 * @param hc05 hc05对象实体
 * @param rxData 存放接收数据的数组地址
 * @param len 要接收的数据长度
 * @return uint8_t
 */
uint8_t HC05RxPacket(HC05ObjectType *hc05, uint8_t *rxData, uint8_t len) {
  uint8_t status;
  status += hc05->RxData(rxData, len);
  return status;
}
