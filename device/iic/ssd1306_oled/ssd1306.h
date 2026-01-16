/**
 * @file ssd1306.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief 用于ssd1306驱动的oled屏幕，即市面上常见的0.96寸单色oled屏幕
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
  uint8_t (*WriteCommand)(uint8_t cmd); // 写命令操作
  uint8_t (*WriteData)(uint8_t data);   // 写数据操作
} SSD1306ObjectType;

/*需要实现的接口*/
typedef uint8_t (*SSD1306_WriteCommand)(uint8_t cmd);
typedef uint8_t (*SSD1306_WriteData)(uint8_t data);

#endif
