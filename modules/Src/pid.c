#include "pid.h"
#include <math.h>

/**
 * @brief 初始化pid控制器
 * @param  pid pid环
 * @param  kp 填入初始化值
 * @param  ki 填入初始化值
 * @param  kd 填入初始化值
 * @param  limit 填入初始化值
 */
void PID_Init(PIDControllerType *pid, float kp, float ki, float kd,
              float limit) {
  pid->core.kp = kp;
  pid->core.ki = ki;
  pid->core.kd = kd;
  pid->core.integral = 0.0f;
  pid->core.prev_error = 0.0f;
  pid->core.output_limit = limit;
}

/**
 * @brief PID计算
 * @param  pid pid环
 * @param  error 误差输入
 * @param  dt 时间间隔
 */
void PIDUpdate(PIDControllerType *pid, float error, float dt) {
  // 积分项
  pid->core.integral += error * dt;

  // 积分限幅
  if (pid->core.ki > 0) {
    float max_integral = pid->core.output_limit / pid->core.ki;
    if (pid->core.integral > max_integral)
      pid->core.integral = max_integral;
    if (pid->core.integral < -max_integral)
      pid->core.integral = -max_integral;
  }

  // 微分项
  float derivative = (error - pid->core.prev_error) / dt;
  pid->core.prev_error = error;

  // PID输出
  pid->out = pid->core.kp * error + pid->core.ki * pid->core.integral +
             pid->core.kd * derivative;

  // 限幅
  if (pid->out > pid->core.output_limit)
    pid->out = pid->core.output_limit;
  if (pid->out < -pid->core.output_limit)
    pid->out = -pid->core.output_limit;
}
