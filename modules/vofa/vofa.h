#ifndef VOFA_H
#define VOFA_H

/*std*/
#include <stdint.h>

// vofa使用justfloat协议通信时的帧尾
const uint8_t fTail[4] = {0x00, 0x00, 0x80, 0x7f};
// 接收验证帧头，自定义
#define VOFA_FRAME_HEAD 0x66

/*vofa错误类型*/
typedef enum {
  vofa_Ok,
  vofa_InitError,
  vofa_Absent,
  vofa_Error,
  vofa_DataError,
} VOFAErrorType;

/*vofa接收数据的状态*/
typedef enum {
  new,          // 数据是新的
  default_init, // 默认初始化
  used,         // 数据已被使用
  cover,        // 数据被覆盖
  error,        // 数据错误
} VOFARxState;

/*vofa发送数据帧 -- justfloat协议*/
typedef struct {
  uint8_t fLen; // 载荷数据长度，用于发送
  float *fData; // 载荷数据首地址，发送用指针
} VOFATxFrameType;

/*vofa接收数据帧 -- justfloat协议*/
typedef struct {
  uint8_t fHead; // 帧头,用于接收
  uint8_t fID;   // 帧ID，由于接收
  float fData;   // 接收只需要一个float数据
} VOFARxFrameType;

/*vofa数据接收帧的缓冲表*/
typedef struct {
  VOFARxState state; // 数据状态
  uint8_t fID;       // 帧ID
  float data;        // 解析的数据
} VOFARxTable;

/*vofa控制器类型*/
typedef struct {
  /*data*/
  VOFATxFrameType txFrame;
  VOFARxFrameType rxFrame;
  /*function*/
  uint8_t (*TxMessage)(uint8_t *pBuff,
                       uint8_t len); // 绑定串口发送多字节数据的函数到vofa对象
  uint8_t (*RxMessage)(uint8_t *pBuff,
                       uint8_t len); // 绑定串口接收多字节的函数到vofa对象
} VOFAContronllerType;

/*需要实现的供vofa使用的串口函数接口*/
typedef uint8_t (*VofaUartTx)(uint8_t *pBuff, uint8_t len);
typedef uint8_t (*VofaUartRx)(uint8_t *pBuff, uint8_t len);

/*功能函数*/
VOFAErrorType VOFAContronllerInit(VOFAContronllerType *vofa, VofaUartTx tx,
                                  VofaUartRx rx);
VOFAErrorType VOFATxFrame(VOFAContronllerType *vofa);
float VOfAReadDataFromBuffer(uint8_t id);

#endif
