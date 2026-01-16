#include "ssd1306.h"
#include "config/ssd1306_config.h"
#include "font/oled_f8x16.h"
#include <stddef.h>
#include <stdint.h>

static void SSD1306_WriteCommand(SSD1306ObjectType *ssd1306,
                                 uint8_t Command); // 命令操作
static void SSD1306_WriteData(SSD1306ObjectType *ssd1306,
                              uint8_t data); // 数据操作
static void SSD1306_SetCursor(SSD1306ObjectType *ssd1306, uint8_t Y,
                              uint8_t X);         // 设置光标位置
static uint32_t oled_pow(uint32_t X, uint32_t Y); // 返回X的Y次方

SSD1306ErrotType SSD1306_ObjectInit(SSD1306ObjectType *ssd1306,
                                    SSD1306_IICWrite send,
                                    SSD1306_DelayMs delay, uint8_t addr) {
  uint8_t len =
      sizeof(ssd1306_defauleconfig) / sizeof(ssd1306_defauleconfig[0]);
  /*检查所需函数是否存在*/
  if ((send == NULL) && (delay == NULL)) {
    return SSD1306_Init_Absent;
  }

  /*注入*/
  ssd1306->IICWrite = send;
  ssd1306->DelayMs = delay;
  ssd1306->addr = addr;

  /*初始化*/
  ssd1306->DelayMs(50);
  for (int i = 0; i < len; i++) {
    SSD1306_WriteCommand(ssd1306, *(ssd1306_defauleconfig + i));
  }
  
  return SSD1306_Init_Ok;
}

/**
 * @brief OLED清屏
 *
 * @param ssd1306 要操作的ssd1306对象
 */
void OLED_Clear(SSD1306ObjectType *ssd1306) {
  uint8_t i, j;
  for (j = 0; j < 8; j++) {
    SSD1306_SetCursor(ssd1306, j, 0);
    for (i = 0; i < 128; i++) {
      SSD1306_WriteData(ssd1306, 0x00);
    }
  }
}

/**
 * @brief OLED显示一个字符
 *
 * @param ssd1306 要操作的ssd1306对象
 * @param Line 行位置，范围：1~4
 * @param Column 列位置，范围：1~16
 * @param Char 要显示的一个字符，范围：ASCII可见字符
 */
void OLED_ShowChar(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                   char Char) {
  uint8_t i;
  SSD1306_SetCursor(ssd1306, (Line - 1) * 2,
                    (Column - 1) * 8); // 设置光标位置在上半部分
  for (i = 0; i < 8; i++) {
    SSD1306_WriteData(ssd1306, OLED_F8x16[Char - ' '][i]); // 显示上半部分内容
  }
  SSD1306_SetCursor(ssd1306, (Line - 1) * 2 + 1,
                    (Column - 1) * 8); // 设置光标位置在下半部分
  for (i = 0; i < 8; i++) {
    SSD1306_WriteData(ssd1306,
                      OLED_F8x16[Char - ' '][i + 8]); // 显示下半部分内容
  }
}

/**
 * @brief OLED显示字符串
 *
 * @param ssd1306 要操作的ssd1306对象
 * @param Line Line 起始行位置，范围：1~4
 * @param Column Column 起始列位置，范围：1~16
 * @param String String 要显示的字符串，范围：ASCII可见字符
 */
void OLED_ShowString(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                     char *String) {
  uint8_t i;
  for (i = 0; String[i] != '\0'; i++) {
    OLED_ShowChar(ssd1306, Line, Column + i, String[i]);
  }
}

/**
 * @brief OLED显示数字（十进制，正数）
 *
 * @param ssd1306 要操作的ssd1306对象
 * @param Line Line 起始行位置，范围：1~4
 * @param Column Column 起始列位置，范围：1~16
 * @param Number Number 要显示的数字，范围：0~4294967295
 * @param Length Length 要显示数字的长度，范围：1~10
 */
void OLED_ShowNum(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                  uint32_t Number, uint8_t Length) {
  uint8_t i;
  for (i = 0; i < Length; i++) {
    OLED_ShowChar(ssd1306, Line, Column + i,
                  Number / oled_pow(10, Length - i - 1) % 10 + '0');
  }
}

/**
 * @brief OLED显示数字（十进制，带符号数）
 *
 * @param ssd1306 要操作的ssd1306对象
 * @param Line Line 起始行位置，范围：1~4
 * @param Column Column 起始列位置，范围：1~16
 * @param Number Number 要显示的数字，范围：-2147483648~2147483647
 * @param Length Length 要显示数字的长度，范围：1~10
 */
void OLED_ShowSignedNum(SSD1306ObjectType *ssd1306, uint8_t Line,
                        uint8_t Column, int32_t Number, uint8_t Length) {
  uint8_t i;
  uint32_t Number1;
  if (Number >= 0) {
    OLED_ShowChar(ssd1306, Line, Column, '+');
    Number1 = Number;
  } else {
    OLED_ShowChar(ssd1306, Line, Column, '-');
    Number1 = -Number;
  }
  for (i = 0; i < Length; i++) {
    OLED_ShowChar(ssd1306, Line, Column + i + 1,
                  Number1 / oled_pow(10, Length - i - 1) % 10 + '0');
  }
}

/**
 * @brief OLED显示数字（十六进制，正数）
 *
 * @param ssd1306 要操作的ssd1306对象
 * @param Line 起始行位置，范围：1~4
 * @param Column Column 起始列位置，范围：1~16
 * @param Number Number 要显示的数字，范围：0~0xFFFFFFFF
 * @param Length Length 要显示数字的长度，范围：1~8
 */
void OLED_ShowHexNum(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                     uint32_t Number, uint8_t Length) {
  uint8_t i, SingleNumber;
  for (i = 0; i < Length; i++) {
    SingleNumber = Number / oled_pow(16, Length - i - 1) % 16;
    if (SingleNumber < 10) {
      OLED_ShowChar(ssd1306, Line, Column + i, SingleNumber + '0');
    } else {
      OLED_ShowChar(ssd1306, Line, Column + i, SingleNumber - 10 + 'A');
    }
  }
}

/**
 * @brief OLED显示数字（二进制，正数）
 *
 * @param ssd1306 要操作的ssd1306对象
 * @param Line 起始行位置，范围：1~4
 * @param Column 起始列位置，范围：1~16
 * @param Number 要显示的数字，范围：0~1111 1111 1111 1111
 * @param Length 要显示数字的长度，范围：1~16
 */
void OLED_ShowBinNum(SSD1306ObjectType *ssd1306, uint8_t Line, uint8_t Column,
                     uint32_t Number, uint8_t Length) {
  uint8_t i;
  for (i = 0; i < Length; i++) {
    OLED_ShowChar(ssd1306, Line, Column + i,
                  Number / oled_pow(2, Length - i - 1) % 2 + '0');
  }
}

/**
 * @brief 设置OLED光标位置
 *
 * @param Y 以左上角为原点，向下方向的坐标，范围：0~7
 * @param X 以左上角为原点，向右方向的坐标，范围：0~127
 */
static void SSD1306_SetCursor(SSD1306ObjectType *ssd1306, uint8_t Y,
                              uint8_t X) {
  SSD1306_WriteCommand(ssd1306, 0xb0 | Y);                 // 设置Y位置
  SSD1306_WriteCommand(ssd1306, 0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
  SSD1306_WriteCommand(ssd1306, 0x00 | (X & 0x0F));        // 设置X位置低4位
}

/**
 * @brief 向OLED写命令
 *
 * @param ssd1306 SSD1306ObjectType类型地址
 * @param Command 要写入的命令
 */
static void SSD1306_WriteCommand(SSD1306ObjectType *ssd1306, uint8_t Command) {
  ssd1306->IICWrite(ssd1306->addr, 0x00, Command);
}

/**
 * @brief 向OLED写数据
 *
 * @param ssd1306 SSD1306ObjectType类型地址
 * @param data 要写入的数据
 */
static void SSD1306_WriteData(SSD1306ObjectType *ssd1306, uint8_t data) {
  ssd1306->IICWrite(ssd1306->addr, 0x40, data);
}

/**
 * @brief 求Y次方函数
 *
 * @param X 底数
 * @param Y 指数
 * @return uint32_t X的Y次方
 */
static uint32_t oled_pow(uint32_t X, uint32_t Y) {
  uint32_t Result = 1;
  while (Y--) {
    Result *= X;
  }
  return Result;
}
