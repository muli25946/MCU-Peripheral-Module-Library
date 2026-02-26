#include "st7735s.h"
#include <stddef.h>

// 写操作
static void ST7735_WriteCommand(ST7735ObjectType *st7735, uint8_t *msg, uint8_t len);

/**
 * @brief st7735s对象初始化
 *
 * @param st7735 st7735s句柄
 * @param write spi写函数注入
 * @param chipselect spi片选函数注入
 * @param reset 复位函数注入
 * @param blk 背光控制函数注入，可选pwm调光，开关控制，不需要控制传入NULL即可
 * @param dc DC控制函数注入，控制命令-数据模式
 * @param delay 延时函数注入
 * @return ST7735ErrorType 错误类型
 */
ST7735ErrorType ST7735_ObjectInit(ST7735ObjectType *st7735,
                                  ST7735_SPIWrite write,
                                  ST7735_SPIChipSelect chipselect,
                                  ST7735_Reset reset, ST7735_Blk blk,
                                  ST7735_DC dc, ST7735_DelayMs delay) {
  /*检查所需函数是否存在*/
  if (st7735 == NULL || write == NULL || reset == NULL || dc == NULL ||
      delay == NULL) {
    return ST7735_ERROR_INVALID_PARAMETER;
  }

  /*检查可能不需要控制的选项*/
  if (chipselect == NULL) {
    st7735->ChipSelect = NULL;
  }

  if (blk == NULL) {
    st7735->Blk = NULL;
  }

  /*函数注入*/
  st7735->Write = write;
  st7735->ChipSelect = chipselect;
  st7735->Reset = reset;
  st7735->Blk = blk;
  st7735->dc = dc;
  st7735->DelayMs = delay;

  return ST7735_ERROR_NONE;
}


static void ST7735_WriteCommand(ST7735ObjectType *st7735, uint8_t *Command, uint8_t len){

}
