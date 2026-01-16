/**
 * @file ssd1306.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief
 * 用于ssd1306驱动的oled屏幕，即市面上常见的0.96寸单色oled屏幕。该驱动参考江协科技
 * @version 0.1
 * @date 2026-01-16
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef SSD1306_OLED_H_
#define SSD1306_OLED_H_

#include <stdint.h>

/*SSD1306错误类型*/
typedef enum {
  SSD1306_Init_Ok,
  SSD1306_Init_Error,
  SSD1306_Init_Absent
} SSD1306ErrotType;

/*SSD1306对象*/
typedef struct {
  /*data*/
  uint8_t addr;
  /*function*/
  uint8_t (*IICWrite)(uint8_t addr, uint8_t select,
                      uint8_t data); // IIC写数据操作
  void (*DelayMs)(uint32_t ms);      // 延时函数
} SSD1306ObjectType;

/*需要实现的接口*/
typedef uint8_t (*SSD1306_IICWrite)(uint8_t addr, uint8_t select, uint8_t data);
typedef void (*SSD1306_DelayMs)(uint32_t ms);

/*可用函数*/
SSD1306ErrotType SSD1306_ObjectInit(SSD1306ObjectType *ssd1306,
                                    SSD1306_IICWrite send,
                                    SSD1306_DelayMs delay, uint8_t addr);
void OLED_Clear(SSD1306ObjectType *ssd1306);
void OLED_ShowChar(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                   char Char);
void OLED_ShowString(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                     char *String);
void OLED_ShowNum(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                  uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(SSD1306ObjectType *ssd1306, uint8_t Line,
                        uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                     uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                     uint32_t Number, uint8_t Length);

#endif
