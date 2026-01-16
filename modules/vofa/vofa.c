#include "vofa.h"
#include <stdint.h>
#include <string.h>

// 配置表,根据实际使用更改
// |数据就绪状态|数据对应的标识ID|数据|
static VOFARxTable rxTable[] = {
    {default_init, 00, 00},
};

static uint8_t Frame_float2uint8(float *pSrc, uint8_t *pDst,
                                 uint8_t len); // float数据转uint8数据

/**
 * @brief vofa控制器的初始化，同时初始化自动管理的收发帧
 *
 * @param vofa VOFAContronllerType类型地址
 * @param tx 串口发送的函数指针
 * @param rx 串口接收的函数指针
 * @param irq 中断获取函数指针
 * @return VOFAErrorType
 * @note 解析配置表需要手动修改(vofa.c)
 */
VOFAErrorType VOFAContronllerInit(VOFAContronllerType *vofa, VofaUartTx tx,
                                  VofaUartRx rx) {

  /*检查注入函数是否空缺*/
  if (tx == NULL || rx == NULL) {
    return vofa_Absent;
  }

  /*帧内容初始化*/
  vofa->rxFrame = (VOFARxFrameType){
      .fHead = 0xFF,
      .fID = 0xFF,
      .fData = 0,
  };
  vofa->txFrame = (VOFATxFrameType){
      .fLen = 0x00,
      .fData = NULL,
  };

  /*注入函数*/
  vofa->TxMessage = tx;
  vofa->RxMessage = rx;

  return vofa_Ok;
}

/**
 * @brief vofa发送一帧数据
 *
 * @param vofa VOFAContronllerType类型
 * @return uint8_t
 * @note
 * 使用前务必将vofa->txFrame.fData指向要发送的数组,使用vofa->txFrame.fLen指定发送数据长度。不建议超16个，默认64个
 */
VOFAErrorType VOFATxFrame(VOFAContronllerType *vofa) {
  if (vofa->txFrame.fData == NULL || vofa->txFrame.fLen > 64) {
    return vofa_DataError;
  }

  /*数据转换*/
  uint8_t status = 0;
  uint8_t temp[4 * vofa->txFrame.fLen];

  /*发送表征信息,vofa上位机不需要，不发送*/
  // status += vofa->TxMessage(&vofa->txFrame.fHead, 1);
  // status += vofa->TxMessage(&vofa->txFrame.fID, 1);
  // status += vofa->TxMessage(&vofa->txFrame.fLen, 1);

  /*数据转换*/
  status += Frame_float2uint8(vofa->txFrame.fData, temp, vofa->txFrame.fLen);
  /*发送载荷数据*/
  status += vofa->TxMessage(temp, 4 * vofa->txFrame.fLen);

  /*发送帧尾*/
  status += vofa->TxMessage((uint8_t *)fTail, 4);

  if (status != 0) {
    return vofa_Error;
  }
  return vofa_Ok;
}

/**
 * @brief 从解析数据缓冲区获得需要的数据
 *
 * @param id 需要查询的数据对应的id
 * @return float 查询到的结果;若一直为0则错误
 */
float VOfAReadDataFromBuffer(uint8_t id) {
  uint8_t cnt = 0;

  /*遍历表*/
  for (; cnt < (sizeof(rxTable) / sizeof(VOFARxTable)); cnt++) {
    if (id == rxTable[cnt].fID) {
      rxTable[cnt].state = used;
      return rxTable[cnt].data;
    }
  }

  return 0;
}

/**
 * @brief vofa解码接收到的数据并存入缓冲配置表
 *
 * @param vofa VOFAContronllerType类型地址
 * @return VOFARxState 数据接受后的处理结果
 * @note 接收的数据应为6字节。|帧头|ID|float载荷数据|
 */
VOFARxState VOFADecodeFrame(VOFAContronllerType *vofa) {

  uint8_t tID;     // ID匹配暂存
  uint8_t cnt = 0; // 表索引
  uint8_t rxBytesBuf[6];

  /*获取数据*/
  vofa->RxMessage(rxBytesBuf, 6);
  /*转为vofa帧格式*/
  memcpy(&vofa->rxFrame.fHead, &rxBytesBuf[0], sizeof(uint8_t));
  memcpy(&vofa->rxFrame.fID, &rxBytesBuf[1], sizeof(uint8_t));
  memcpy(&vofa->rxFrame.fData, &rxBytesBuf[2], sizeof(float));

  /*帧头检验*/
  if (vofa->rxFrame.fHead != VOFA_FRAME_HEAD) {
    return error;
  }

  tID = vofa->rxFrame.fID;

  /*ID匹配*/
  for (; cnt < (sizeof(rxTable) / sizeof(VOFARxTable)); cnt++) {
    /*数据状态检查*/
    if (tID == rxTable[cnt].fID &&
        (rxTable[cnt].state == default_init || rxTable[cnt].state == used ||
         rxTable[cnt].state == new)) {
      memcpy(&rxTable[cnt].data, &vofa->rxFrame.fData, sizeof(float));

      /*数据覆盖判断*/
      if (rxTable[cnt].state == new) {
        rxTable[cnt].state = cover;
      } else {
        /*标记数据可用*/
        rxTable[cnt].state = new;
      }

      /*返回处理结果*/
      return rxTable[cnt].state;
    }
  }

  return error;
}

/**
 * @brief 将float的数组数据转换为uint8数组
 *
 * @param pSrc 指向原来的float数组
 * @param pTar 指向转换后的uint8数组
 * @param len 转换的float数组长度
 * @return uint8_t
 */
static uint8_t Frame_float2uint8(float *pSrc, uint8_t *pDst, uint8_t len) {
  if (pSrc == NULL || pDst == NULL || len == 0) {
    return 1;
  }

  memcpy(pDst, pSrc, 4 * len);

  return 0;
}
