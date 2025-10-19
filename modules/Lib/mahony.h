#ifndef MAHONY_H
#define MAHONY_H

#include <stdint.h>

/*前右下坐标系(Euler-FRD)*/
typedef struct {
  float pitch, roll, yaw;          // 角度值
  float pitchRad, rollRad, yawRad; // 弧度值
} FRDEulerAngle;

/*北东地坐标系(Euler-NED)*/
typedef struct {
  float pitch, roll, yaw;
} NEDEulerAngle;

/*Mahony滤波器核心*/
typedef struct {
  float q[4];          // 四元数状态[q0, q1, q2, q3]
  float twoKp;         // 2 * 比例增益
  float twoKi;         // 2 * 积分增益
  float integralFB[3]; // 积分误差项[x, y, z]
  uint8_t initialized; // 状态标志
} MahonyFilterCore;

/*mahony解算器数据类型*/
typedef struct MahonyFilter {
  MahonyFilterCore filter;
  FRDEulerAngle frd;       // 载体系
  NEDEulerAngle ned;       // 导航系
  float samplePeriod;      // 计算周期
  uint8_t useMagnetometer; // 是否使用磁力计
  float lastUpdateTime;    // 最后更新时间
} MahonyFilterType;

/*传入数据接口*/
typedef struct {
  struct {
    float x, y, z; // 加速度 (单位m/s²)
  } accel;

  struct {
    float x, y, z; // 角速度 (单位rad/s)
  } gyro;

  struct {
    float x, y, z; // 磁场 (uT)
  } mag;

  uint32_t timestamp; // 时间戳
} MahonyInput;

/*函数声明*/

void MahonyFilterCoreInit(MahonyFilterType *ahrs);
void MahonyUpdateAHRSIMU(MahonyFilterType *ahrs, MahonyInput *input, float dt);
void MahonyUpdateAHRS(MahonyFilterType *ahrs, MahonyInput *input, float dt);

#endif // !MAHONY_H
