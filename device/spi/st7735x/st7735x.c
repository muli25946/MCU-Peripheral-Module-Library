#include "st7735x.h"
#include "font/font.h"
#include <stddef.h>
#include <stdint.h>

// 写操作
static uint8_t ST7735_WriteCommandData(ST7735ObjectType *st7735,
                                       uint8_t *Command, uint8_t *data,
                                       uint8_t len);
// 初始化屏幕
static ST7735ErrorType ST7735_Init(ST7735ObjectType *st7735);
// 指定绘画区域
static void ST7735_SetShowArea(ST7735ObjectType *st7735, uint16_t x1,
                               uint16_t y1, uint16_t x2, uint16_t y2);
/**
 * @brief st7735对象初始化
 *
 * @param st7735 st7735句柄
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

  /*初始化屏幕*/
  return ST7735_Init(st7735);
}

/**
 * @brief ST7735清屏函数
 *
 * @param st7735 st7735句柄
 * @param color 填充的颜色
 */
void ST7735_Clear(ST7735ObjectType *st7735, uint16_t color) {
  uint8_t color_buff[2];
  color_buff[0] = color >> 8;
  color_buff[1] = color & 0xFF;

  ST7735_SetShowArea(st7735, 0, 0, 130, 162);

  for (uint8_t i = 0; i < 130; i++) {
    for (uint8_t j = 0; j < 162; j++) {
      ST7735_WriteCommandData(st7735, NULL, color_buff, 2);
    }
  }
}

/**
 * @brief ST7735画点函数
 *
 * @param st7735 st7735句柄
 * @param x 点的x坐标
 * @param y 点的y坐标
 * @param color 点的颜色
 */
void ST7735_DrawPoint(ST7735ObjectType *st7735, uint16_t x, uint16_t y,
                      uint16_t color) {
  ST7735_SetShowArea(st7735, x, y, x, y);
  uint8_t color_buff[2];
  color_buff[0] = color >> 8;
  color_buff[1] = color & 0xFF;
  ST7735_WriteCommandData(st7735, NULL, color_buff, 2);
}

/**
 * @brief ST7735显示控制函数
 *
 * @param st7735 st7735句柄
 * @param mode 开/关，参数见ST7735DisplayMode
 */
void ST7735_DisplaySet(ST7735ObjectType *st7735, ST7735DisplayMode mode) {
  if (mode == ST7735_DISPLAY_ON) {
    ST7735_WriteCommandData(st7735, (uint8_t[]){0x29}, NULL, 0);
  } else {
    ST7735_WriteCommandData(st7735, (uint8_t[]){0x28}, NULL, 0);
  }
}

/**
 * @brief ST7735在指定区域内填充单个颜色
 *
 * @param st7735 st7735句柄
 * @param x1 起始点的横坐标
 * @param y1 起始点的纵坐标
 * @param x2 终点的横坐标
 * @param y2 终点的纵坐标
 * @param color 填充的颜色
 * @note (x1,y1),(x2,y2):填充矩形对角坐标,区域大小为:(x2-x1+1)*(y2-y1+1)
 */
void ST7735_Fill(ST7735ObjectType *st7735, uint16_t x1, uint16_t y1,
                 uint16_t x2, uint16_t y2, uint16_t color) {
  uint8_t color_buff[2];
  color_buff[0] = color >> 8;
  color_buff[1] = color & 0xFF;

  ST7735_SetShowArea(st7735, x1, y1, x2, y2);

  for (uint8_t i = 0; i < (x2 - x1 + 1); i++) {
    for (uint8_t j = 0; j < (y2 - y1 + 1); j++) {
      ST7735_WriteCommandData(st7735, NULL, color_buff, 2);
    }
  }
}

/**
 * @brief ST7735画线函数，使用Bresenham算法
 *
 * @param st7735 st7735句柄
 * @param x1 起始点的横坐标
 * @param y1 起始点的纵坐标
 * @param x2 终点的横坐标
 * @param y2 终点的纵坐标
 * @param color 线条颜色
 */
void ST7735_DrawLine(ST7735ObjectType *st7735, uint16_t x1, uint16_t y1,
                     uint16_t x2, uint16_t y2, uint16_t color) {
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1; // 计算坐标增量
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;

  // 设置单步方向
  if (delta_x > 0) {
    incx = 1;
  } else if (delta_x == 0) // 垂直线
  {
    incx = 0;
  } else {
    incx = -1;
    delta_x = -delta_x;
  }

  if (delta_y > 0) {
    incy = 1;
  } else if (delta_y == 0) // 水平线
  {
    incy = 0;
  } else {
    incy = -1;
    delta_y = -delta_y;
  }

  if (delta_x > delta_y) // 选取基本增量坐标轴
  {
    distance = delta_x;
  } else {
    distance = delta_y;
  }

  for (t = 0; t <= distance + 1; t++) // 画线输出
  {
    ST7735_DrawPoint(st7735, uRow, uCol, color); // 画点
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      uRow += incx;
    }

    if (yerr > distance) {
      yerr -= distance;
      uCol += incy;
    }
  }
}

/**
 * @brief ST7735画矩形函数
 * 
 * @param st7735 st7735句柄
 * @param x1 起始点的横坐标
 * @param y1 起始点的纵坐标
 * @param x2 终点的横坐标
 * @param y2 终点的纵坐标
 * @param color 矩形颜色
 */
void ST7735_DrawRectangle(ST7735ObjectType *st7735, uint16_t x1, uint16_t y1,
                          uint16_t x2, uint16_t y2, uint16_t color) {
  ST7735_DrawLine(st7735, x1, y1, x2, y1, color);
  ST7735_DrawLine(st7735, x1, y1, x1, y2, color);
  ST7735_DrawLine(st7735, x1, y2, x2, y2, color);
  ST7735_DrawLine(st7735, x2, y1, x2, y2, color);
}

/**
 * @brief ST7735画圆函数，使用Bresenham算法
 * 
 * @param st7735 st7735句柄
 * @param x0 圆心的横坐标
 * @param y0 圆心的纵坐标
 * @param r 圆的半径
 * @param color 圆的颜色
 */
void ST7735_DrawCircle(ST7735ObjectType *st7735, uint16_t x0, uint16_t y0, uint8_t r,
                     uint16_t color) {
  int a, b;
  int di;
  a = 0;
  b = r;
  di = 3 - (r << 1); // 判断下个点位置的标志
  while (a <= b) {
    ST7735_DrawPoint(st7735,x0 + a, y0 - b, color);
    ST7735_DrawPoint(st7735,x0 + b, y0 - a, color);
    ST7735_DrawPoint(st7735,x0 + b, y0 + a, color);
    ST7735_DrawPoint(st7735,x0 + a, y0 + b, color);
    ST7735_DrawPoint(st7735,x0 - a, y0 + b, color);
    ST7735_DrawPoint(st7735,x0 - b, y0 + a, color);
    ST7735_DrawPoint(st7735,x0 - a, y0 - b, color);
    ST7735_DrawPoint(st7735,x0 - b, y0 - a, color);
    a++;
    // 使用Bresenham算法画圆
    if (di < 0) {
      di += 4 * a + 6;
    } else {
      di += 10 + 4 * (a - b);
      b--;
    }
  }
}

/**
 * @brief ST7735画实心圆函数，使用Bresenham算法
 * 
 * @param st7735 st7735句柄
 * @param Xpos 圆心横坐标
 * @param Ypos 圆心纵坐标
 * @param Radius 圆半径
 * @param Color 颜色
 */
void ST7735_DrawFullCircle(ST7735ObjectType *st7735, uint16_t Xpos, uint16_t Ypos,
                        uint16_t Radius, uint16_t Color) {
  uint16_t x, y, r = Radius;
  for (y = Ypos - r; y < Ypos + r; y++) {
    for (x = Xpos - r; x < Xpos + r; x++) {
      if (((x - Xpos) * (x - Xpos) + (y - Ypos) * (y - Ypos)) <= r * r) {
        ST7735_DrawPoint(st7735, x, y, Color);
      }
    }
  }
}

/**
 * @brief ST7735显示一个字符函数
 * 
 * @param st7735 st7735句柄
 * @param x 起始点的横坐标
 * @param y 起始点的纵坐标
 * @param num 要显示的字符ASCII码值
 * @param size 字体大小，支持12/16/24
 * @param mode 叠加方式(1)/非叠加方式(0)
 * @param pen_color 画笔颜色
 * @param back_color 背景颜色
 */
void ST7735_ShowChar(ST7735ObjectType *st7735, uint16_t x, uint16_t y, uint8_t num,
                  uint8_t size, uint8_t mode,
                  uint16_t pen_color, uint16_t back_color) {
  uint8_t temp, t1, t;
  uint16_t y0 = y;
  uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) *
                        (size / 2); // 得到字体一个字符对应点阵集所占的字节数
  num = num - ' '; // 得到偏移后的值（ASCII字库是从空格开始取模，所以-'
                   // '就是对应字符的字库）

  for (t = 0; t < csize; t++) {
    if (size == 12) // 调用1206字体
    {
      temp = asc2_1206[num][t];
    } else if (size == 16) // 调用1608字体
    {
      temp = asc2_1608[num][t];
    } else if (size == 24) // 调用2412字体
    {
      temp = asc2_2412[num][t];
    } else
      return; // 没有的字库

    for (t1 = 0; t1 < 8; t1++) {
      if (temp & 0x80) {
        ST7735_DrawPoint(st7735, x, y, pen_color);
      } else if (mode == 0) {
        ST7735_DrawPoint(st7735, x, y, back_color);
      }
      temp <<= 1;
      y++;

      if (y >= 240) // 超区域
      {
        return;
      }

      if ((y - y0) == size) {
        y = y0;
        x++;
        if (x >= 320) // 超区域
        {
          return;
        }
        break;
      }
    }
  }
}

/**
 * @brief ST7735显示字符串函数
 * 
 * @param st7735 st7735句柄
 * @param x 起始点的横坐标
 * @param y 起始点的纵坐标
 * @param width 显示区域宽度
 * @param height 显示区域高度
 * @param size 字体大小，支持12/16/24
 * @param p 要显示的字符串指针
 * @param pen_color 画笔颜色
 * @param back_color 背景颜色
 */
void ST7735_ShowString(ST7735ObjectType*st7735,uint16_t x, uint16_t y, uint16_t width,
                    uint16_t height, uint8_t size, uint8_t *p,
                    uint16_t pen_color, uint16_t back_color) {
  uint8_t x0 = x;
  width += x;
  height += y;
  while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
  {
    if (x >= width) {
      x = x0;
      y += size;
    }

    if (y >= height) // 退出
    {
      break;
    }

    ST7735_ShowChar(st7735, x, y, *p, size, 0, pen_color, back_color);
    x += size / 2;
    p++;
  }
}

/**
 * @brief 写命令+数据操作
 *
 * @param st7735 st7735句柄
 * @param Command 命令,纯数据时该值填NULL
 * @param data 数据数组首地址，纯命令时该值填NULL
 * @param len 数组长度
 * @return uint8_t 数据传输返回值
 */
static uint8_t ST7735_WriteCommandData(ST7735ObjectType *st7735,
                                       uint8_t *Command, uint8_t *data,
                                       uint8_t len) {
  uint8_t status = 0;

  if (Command != NULL) {                   /*发送命令*/
    st7735->ChipSelect(ST7735_CS_ENABLE);  // 使能片选
    st7735->dc(ST7735_MODE_CMD);           // 选择命令模式
    status += st7735->Write(Command, 1);   // 发送命令数据
    st7735->ChipSelect(ST7735_CS_DISABLE); // 失能片选
  }
  if (data == NULL) {
    return status;
  } else {
    /*写入数据*/
    st7735->ChipSelect(ST7735_CS_ENABLE);  // 使能片选
    st7735->dc(ST7735_MODE_DATA);          // 选择数据模式
    status += st7735->Write(data, len);    // 发送数据
    st7735->ChipSelect(ST7735_CS_DISABLE); // 失能片选
    return status;
  }
}

/**
 * @brief st7735初始化函数
 *
 * @param st7735 st7735句柄
 * @return ST7735ErrorType 错误类型，初始化失败返回ST7735_ERROR_INIT
 */
static ST7735ErrorType ST7735_Init(ST7735ObjectType *st7735) {

  uint8_t buff[16] = {0x00}; // 用于初始化数据的临时数组
  uint8_t status = 0;
  /*复位*/
  st7735->Reset(ST7735_RST_ENABLE);
  st7735->DelayMs(10);
  st7735->Reset(ST7735_RST_DISABLE);
  st7735->DelayMs(200);

  /*寄存器配置*/
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0x11}, NULL, 0); // Sleep out
  st7735->DelayMs(120);

  buff[0] = 0x05;
  buff[1] = 0x3C;
  buff[2] = 0x3C;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xB1}, buff, 3);

  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xB2}, buff, 3);

  buff[3] = 0x05;
  buff[4] = 0x3C;
  buff[5] = 0x3C;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xB3}, buff, 6);

  buff[0] = 0x03;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xB4}, buff, 1);

  buff[0] = 0x28;
  buff[1] = 0x08;
  buff[2] = 0x04;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xC0}, buff, 3);

  buff[0] = 0xC0;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xC1}, buff, 1);

  buff[0] = 0x0D;
  buff[1] = 0x00;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xC2}, buff, 2);

  buff[0] = 0x8D;
  buff[1] = 0x2A;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xC3}, buff, 2);

  buff[0] = 0x8D;
  buff[1] = 0xEE;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xC4}, buff, 2);

  buff[0] = 0x1A;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xC5}, buff, 1);

  buff[0] = 0xC0;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0x36}, buff, 1);

  buff[0] = 0x04;
  buff[1] = 0x22;
  buff[2] = 0x07;
  buff[3] = 0x0A;
  buff[4] = 0x2E;
  buff[5] = 0x30;
  buff[6] = 0x25;
  buff[7] = 0x2A;
  buff[8] = 0x28;
  buff[9] = 0x26;
  buff[10] = 0x2E;
  buff[11] = 0x3A;
  buff[12] = 0x00;
  buff[13] = 0x01;
  buff[14] = 0x03;
  buff[15] = 0x13;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xE0}, buff, 16);

  buff[0] = 0x04;
  buff[1] = 0x16;
  buff[2] = 0x06;
  buff[3] = 0x0D;
  buff[4] = 0x2D;
  buff[5] = 0x26;
  buff[6] = 0x23;
  buff[7] = 0x27;
  buff[8] = 0x27;
  buff[9] = 0x25;
  buff[10] = 0x2D;
  buff[11] = 0x3B;
  buff[12] = 0x00;
  buff[13] = 0x01;
  buff[14] = 0x04;
  buff[15] = 0x13;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0xE1}, buff, 16);

  buff[0] = 0x05;
  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0x3A}, buff, 1);

  status += ST7735_WriteCommandData(st7735, (uint8_t[]){0x29}, NULL, 0);

  if (status == 0) {
    return ST7735_ERROR_NONE;
  } else {
    return ST7735_ERROR_INIT;
  }
}

/**
 * @brief 设置显示区域
 *
 * @param st7735 st7735句柄
 * @param x1 起始点横坐标
 * @param y1 起始点纵坐标
 * @param x2 终点横坐标
 * @param y2 终点纵坐标
 * @note
 * 设置显示区域后，后续写入的数据会自动填充到该区域，直到再次设置显示区域。区域范围(x1,y1)->(x2,y2)的矩形区域，坐标原点在左上角，x轴向右递增，y轴向下递增
 */
static void ST7735_SetShowArea(ST7735ObjectType *st7735, uint16_t x1,
                               uint16_t y1, uint16_t x2, uint16_t y2) {
  uint8_t data[4];
  data[0] = x1 >> 8;
  data[1] = x1 & 0xFF;
  data[2] = x2 >> 8;
  data[3] = x2 & 0xFF;
  ST7735_WriteCommandData(st7735, (uint8_t[]){0x2A}, data, 4);

  data[0] = y1 >> 8;
  data[1] = y1 & 0xFF;
  data[2] = y2 >> 8;
  data[3] = y2 & 0xFF;
  ST7735_WriteCommandData(st7735, (uint8_t[]){0x2B}, data, 4);

  ST7735_WriteCommandData(st7735, (uint8_t[]){0x2C}, NULL, 0);
}
