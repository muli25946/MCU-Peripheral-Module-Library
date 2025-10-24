#ifndef PID_H
#define PID_H

#include <stdint.h>

// pid控制器核心
typedef struct {
  float kp;           // 比例系数
  float ki;           // 积分系数
  float kd;           // 微分系数
  float integral;     // 积分累积值
  float prev_error;   // 上一次误差
  float output_limit; // PID输出限幅
} PIDControllerCore;

/*mahony控制器数据类型*/
typedef struct {
  PIDControllerCore core;
  float out; // pid输出的控制量
} PIDControllerType;

void PID_Init(PIDControllerType *pid, float kp, float ki, float kd,
              float limit);
void PIDUpdate(PIDControllerType *pid, float error, float dt);

#endif // !PID_H
