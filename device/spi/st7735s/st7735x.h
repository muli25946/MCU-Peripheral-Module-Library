/**
 * @file st7735x.h
 * @author muli25946 (muli25946@qq.com)
 * @brief 该文件为st7735x的驱动头文件
 * @version 0.1
 * @date 2026-02-18
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef ST735S_H
#define ST735S_H

#include <stdint.h>

/*ST775S错误类型*/
typedef enum {
  ST7735_ERROR_NONE = 0,
  ST7735_ERROR_INVALID_PARAMETER,
  ST7735_ERROR_TIMEOUT,
  ST7735_ERROR_INIT
} ST7735ErrorType;

/*ST7735背光控制*/
typedef enum {
  ST7735_BLK_OFF,
  ST7735_BLK_ON,
  ST7735_BLK_10 = 10,
  ST7735_BLK_20 = 20,
  ST7735_BLK_30 = 30,
  ST7735_BLK_40 = 40,
  ST7735_BLK_50 = 50,
  ST7735_BLK_60 = 60,
  ST7735_BLK_70 = 70,
  ST7735_BLK_80 = 80,
  ST7735_BLK_90 = 90,
} ST7735BlkStatus;

/*ST7735命令-数据模式*/
typedef enum {
  ST7735_MODE_CMD,
  ST7735_MODE_DATA,
} ST7735ModeType;

/*ST7735片选控制*/
typedef enum {
  ST7735_CS_ENABLE,
  ST7735_CS_DISABLE,
} ST7735CSEnableType;

/*ST7735复位控制-低电平有效*/
typedef enum {
  ST7735_RST_ENABLE,
  ST7735_RST_DISABLE,
} ST7735RSTEnableType;

/*ST7735显示控制*/
typedef enum {
  ST7735_DISPLAY_ON,
  ST7735_DISPLAY_OFF,
} ST7735DisplayMode;

/*ST7735S对象*/
typedef struct {
  /*data*/
  ST7735ErrorType err; // 错误类型
  /*function*/
  uint8_t (*Write)(uint8_t *data, uint8_t len);     // SPI写操作
  uint8_t (*ChipSelect)(ST7735CSEnableType status); // SPI片选操作
  uint8_t (*Reset)(ST7735RSTEnableType status);     // SPI复位操作
  uint8_t (*Blk)(ST7735BlkStatus blk);              // 背光控制操作
  void (*dc)(ST7735ModeType mode);                  // DC控制操作
  void (*DelayMs)(uint32_t ms);                     // 延时函数
} ST7735ObjectType;

/*需要实现的接口*/
typedef uint8_t (*ST7735_SPIWrite)(uint8_t *msg,
                                   uint8_t len); // ST7735的SPI写数据操作
typedef uint8_t (*ST7735_SPIChipSelect)(
    ST7735CSEnableType
        status); // ST7735的片选操作,硬件片选可忽略同时传入初始化参数NULL即可
typedef uint8_t (*ST7735_Reset)(ST7735RSTEnableType status); // ST7735的复位操作
typedef uint8_t (*ST7735_Blk)(
    ST7735BlkStatus blk); // ST7735的背光控制操作，如果不想控制背光可缺省
typedef void (*ST7735_DC)(ST7735ModeType mode); // ST7735的DC控制操作
typedef void (*ST7735_DelayMs)(uint32_t ms);    // ST7735的延时函数

/*可用函数*/
ST7735ErrorType ST7735_ObjectInit(ST7735ObjectType *st7735,
                                  ST7735_SPIWrite write,
                                  ST7735_SPIChipSelect chipselect,
                                  ST7735_Reset reset, ST7735_Blk blk,
                                  ST7735_DC dc, ST7735_DelayMs delay);
void ST7735_Clear(ST7735ObjectType *st7735, uint16_t color);
void ST7735_DrawPoint(ST7735ObjectType *st7735, uint16_t x, uint16_t y,
                      uint16_t color);
void ST7735_DisplaySet(ST7735ObjectType *st7735, ST7735DisplayMode mode);
void ST7735_Fill(ST7735ObjectType *st7735, uint16_t x1, uint16_t y1,
                 uint16_t x2, uint16_t y2, uint16_t color);
void ST7735_DrawLine(ST7735ObjectType *st7735, uint16_t x1, uint16_t y1,
                     uint16_t x2, uint16_t y2, uint16_t color);
void ST7735_DrawRectangle(ST7735ObjectType *st7735, uint16_t x1, uint16_t y1,
                          uint16_t x2, uint16_t y2, uint16_t color);
void ST7735_DrawFullCircle(ST7735ObjectType *st7735, uint16_t Xpos,
                           uint16_t Ypos, uint16_t Radius, uint16_t Color);
void ST7735_ShowChar(ST7735ObjectType *st7735, uint16_t x, uint16_t y,
                     uint8_t num, uint8_t size, uint8_t mode,
                     uint16_t pen_color, uint16_t back_color);
void ST7735_ShowString(ST7735ObjectType *st7735, uint16_t x, uint16_t y,
                       uint16_t width, uint16_t height, uint8_t size,
                       uint8_t *p, uint16_t pen_color, uint16_t back_color);

#endif
