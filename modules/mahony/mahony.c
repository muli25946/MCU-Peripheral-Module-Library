#include "mahony.h"
#include <math.h>

float InvSqrt(float x);

/**
 * @brief 初始化mahony滤波器
 *
 * @param ahrs MahonyFilterType
 */
void MahonyFilterCoreInit(MahonyFilterType *ahrs) {
  // 四元数初始化
  ahrs->filter.q[0] = 1.0f;
  ahrs->filter.q[1] = 0.0f;
  ahrs->filter.q[2] = 0.0f;
  ahrs->filter.q[3] = 0.0f;

  // 增益系数
  ahrs->filter.twoKp = 2.0f * 4.3f;
  ahrs->filter.twoKi = 2.0f * 0.1f;

  // 误差积分初始化
  ahrs->filter.integralFB[0] = 0.0f;
  ahrs->filter.integralFB[1] = 0.0f;
  ahrs->filter.integralFB[2] = 0.0f;

  // 配置项
  ahrs->samplePeriod = 0.0f;
  ahrs->useMagnetometer = 0; // 默认信任磁力计
  ahrs->lastUpdateTime = 0;
}

/**
 * @brief mahony进行一次计算求得四元数
 *
 * @param ahrs MahonyFilterType
 * @param input MahonyInput,应放入采集到的姿态数据。
 * @param dt 计算间隔时间
 */
void MahonyUpdateAHRS(MahonyFilterType *ahrs, MahonyInput *input, float dt) {
  float recipNorm;
  float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
  float hx, hy, bx, bz;
  float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
  float halfex, halfey, halfez;
  float qa, qb, qc;

  // 将陀螺仪度/秒转换为弧度/秒
  input->gyro.x *= 0.0174533f;
  input->gyro.y *= 0.0174533f;
  input->gyro.z *= 0.0174533f;

  // 如果磁力计测量无效，则使用纯IMU算法
  if ((input->mag.x == 0.0f) && (input->mag.y == 0.0f) &&
      (input->mag.z == 0.0f)) {
    MahonyUpdateAHRSIMU(ahrs, input, dt);
    return;
  }

  // 仅在加速度计测量有效时计算反馈（避免加速度计归一化中出现 NaN）
  if (!((input->accel.x == 0.0f) && (input->accel.y == 0.0f) &&
        (input->accel.z == 0.0f))) {

    // 加速度计测量归一化
    recipNorm = InvSqrt(input->accel.x * input->accel.x +
                        input->accel.y * input->accel.y +
                        input->accel.z * input->accel.z);
    input->accel.x *= recipNorm;
    input->accel.y *= recipNorm;
    input->accel.z *= recipNorm;

    // 将磁强计测量归一化
    recipNorm =
        InvSqrt(input->mag.x * input->mag.x + input->mag.y * input->mag.y +
                input->mag.z * input->mag.z);
    input->mag.x *= recipNorm;
    input->mag.y *= recipNorm;
    input->mag.z *= recipNorm;

    // 辅助变量避免重复运算
    q0q0 = ahrs->filter.q[0] * ahrs->filter.q[0];
    q0q1 = ahrs->filter.q[0] * ahrs->filter.q[1];
    q0q2 = ahrs->filter.q[0] * ahrs->filter.q[2];
    q0q3 = ahrs->filter.q[0] * ahrs->filter.q[3];
    q1q1 = ahrs->filter.q[1] * ahrs->filter.q[1];
    q1q2 = ahrs->filter.q[1] * ahrs->filter.q[2];
    q1q3 = ahrs->filter.q[1] * ahrs->filter.q[3];
    q2q2 = ahrs->filter.q[2] * ahrs->filter.q[2];
    q2q3 = ahrs->filter.q[2] * ahrs->filter.q[3];
    q3q3 = ahrs->filter.q[3] * ahrs->filter.q[3];

    // 地球磁场的参考方向
    hx = 2.0f * (input->mag.x * (0.5f - q2q2 - q3q3) +
                 input->mag.y * (q1q2 - q0q3) + input->mag.z * (q1q3 + q0q2));
    hy = 2.0f *
         (input->mag.x * (q1q2 + q0q3) + input->mag.y * (0.5f - q1q1 - q3q3) +
          input->mag.z * (q2q3 - q0q1));
    bx = sqrtf(hx * hx + hy * hy);
    bz = 2.0f * (input->mag.x * (q1q3 - q0q2) + input->mag.y * (q2q3 + q0q1) +
                 input->mag.z * (0.5f - q1q1 - q2q2));

    // 重力和磁场的估计方向
    halfvx = q1q3 - q0q2;
    halfvy = q0q1 + q2q3;
    halfvz = q0q0 - 0.5f + q3q3;
    halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
    halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
    halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

    // 误差是估计方向与测量的场矢量方向之和
    halfex = (input->accel.x * halfvz - input->accel.z * halfvy) +
             (input->mag.y * halfwz - input->mag.z * halfwy);
    halfey = (input->accel.z * halfvx - input->accel.x * halfvz) +
             (input->mag.z * halfwx - input->mag.x * halfwz);
    halfez = (input->accel.x * halfvy - input->accel.y * halfvx) +
             (input->mag.x * halfwy - input->mag.y * halfwx);

    // 如果启用，计算并应用积分反馈
    if (ahrs->filter.twoKi > 0.0f) {
      // 按 Ki 缩放的积分误差
      ahrs->filter.integralFB[0] += ahrs->filter.twoKi * halfex / dt;
      ahrs->filter.integralFB[1] += ahrs->filter.twoKi * halfey / dt;
      ahrs->filter.integralFB[2] += ahrs->filter.twoKi * halfez / dt;
      input->gyro.x += ahrs->filter.integralFB[0]; // 应用积分反馈
      input->gyro.y += ahrs->filter.integralFB[1];
      input->gyro.z += ahrs->filter.integralFB[2];
    } else {
      ahrs->filter.integralFB[0] = 0.0f; // 防止整体卷绕
      ahrs->filter.integralFB[1] = 0.0f;
      ahrs->filter.integralFB[2] = 0.0f;
    }

    // 应用比例反馈
    input->gyro.x += ahrs->filter.twoKp * halfex;
    input->gyro.y += ahrs->filter.twoKp * halfey;
    input->gyro.z += ahrs->filter.twoKp * halfez;
  }

  // 积分四元数的变化率
  input->gyro.x *= (0.5f / dt); // 预乘公因数
  input->gyro.y *= (0.5f / dt);
  input->gyro.z *= (0.5f / dt);
  qa = ahrs->filter.q[0];
  qb = ahrs->filter.q[1];
  qc = ahrs->filter.q[2];
  ahrs->filter.q[0] += (-qb * input->gyro.x - qc * input->gyro.y -
                        ahrs->filter.q[3] * input->gyro.z);
  ahrs->filter.q[1] += (qa * input->gyro.x + qc * input->gyro.z -
                        ahrs->filter.q[3] * input->gyro.y);
  ahrs->filter.q[2] += (qa * input->gyro.y - qb * input->gyro.z +
                        ahrs->filter.q[3] * input->gyro.x);
  ahrs->filter.q[3] +=
      (qa * input->gyro.z + qb * input->gyro.y - qc * input->gyro.x);

  // 四元数规范化
  recipNorm = InvSqrt(ahrs->filter.q[0] * ahrs->filter.q[0] +
                      ahrs->filter.q[1] * ahrs->filter.q[1] +
                      ahrs->filter.q[2] * ahrs->filter.q[2] +
                      ahrs->filter.q[3] * ahrs->filter.q[3]);
  ahrs->filter.q[0] *= recipNorm;
  ahrs->filter.q[1] *= recipNorm;
  ahrs->filter.q[2] *= recipNorm;
  ahrs->filter.q[3] *= recipNorm;
}

/**
 * @brief mahony进行一次计算求得四元数。(当磁力计不可靠时)
 *
 * @param ahrs MahonyFilterType
 * @param input MahonyInput,应放入采集到的姿态数据。
 * @param dt 计算间隔时间
 */
void MahonyUpdateAHRSIMU(MahonyFilterType *ahrs, MahonyInput *input, float dt) {
  float recipNorm;
  float halfvx, halfvy, halfvz;
  float halfex, halfey, halfez;
  float qa, qb, qc;

  // 仅在加速度计测量有效时计算反馈（避免加速度计归一化中出现 NaN）
  if (!((input->accel.x == 0.0f) && (input->accel.y == 0.0f) &&
        (input->accel.z == 0.0f))) {

    // 使加速度计测量归一化
    recipNorm = InvSqrt(input->accel.x * input->accel.x +
                        input->accel.y * input->accel.y +
                        input->accel.z * input->accel.z);
    input->accel.x *= recipNorm;
    input->accel.y *= recipNorm;
    input->accel.z *= recipNorm;

    // 估计重力方向
    halfvx = ahrs->filter.q[1] * ahrs->filter.q[3] -
             ahrs->filter.q[0] * ahrs->filter.q[2];
    halfvy = ahrs->filter.q[0] * ahrs->filter.q[1] +
             ahrs->filter.q[2] * ahrs->filter.q[3];
    halfvz = ahrs->filter.q[0] * ahrs->filter.q[0] - 0.5f +
             ahrs->filter.q[3] * ahrs->filter.q[3];

    // 误差是估计重力方向 与测量的重力方向的乘积之和
    halfex = (input->accel.y * halfvz - input->accel.z * halfvy);
    halfey = (input->accel.z * halfvx - input->accel.x * halfvz);
    halfez = (input->accel.x * halfvy - input->accel.y * halfvx);

    // 如果启用，计算并应用积分反馈
    if (ahrs->filter.twoKi > 0.0f) {
      // 按 Ki 缩放的积分误差
      ahrs->filter.integralFB[0] += ahrs->filter.twoKi * halfex / dt;
      ahrs->filter.integralFB[1] += ahrs->filter.twoKi * halfey / dt;
      ahrs->filter.integralFB[2] += ahrs->filter.twoKi * halfez / dt;
      input->gyro.x += ahrs->filter.integralFB[0]; // 应用积分反馈
      input->gyro.y += ahrs->filter.integralFB[1];
      input->gyro.z += ahrs->filter.integralFB[2];
    } else {
      ahrs->filter.integralFB[0] = 0.0f; // 防止整体卷绕
      ahrs->filter.integralFB[1] = 0.0f;
      ahrs->filter.integralFB[2] = 0.0f;
    }

    // 应用比例反馈
    input->gyro.x += ahrs->filter.twoKp * halfex;
    input->gyro.y += ahrs->filter.twoKp * halfey;
    input->gyro.z += ahrs->filter.twoKp * halfez;
  }

  // 积分四元数的变化率
  input->gyro.x *= (0.5f / dt); // 预乘公因数
  input->gyro.y *= (0.5f / dt);
  input->gyro.z *= (0.5f / dt);
  qa = ahrs->filter.q[0];
  qb = ahrs->filter.q[1];
  qc = ahrs->filter.q[2];
  ahrs->filter.q[0] += (-qb * input->gyro.x - qc * input->gyro.y -
                        ahrs->filter.q[3] * input->gyro.z);
  ahrs->filter.q[1] += (qa * input->gyro.x + qc * input->gyro.z -
                        ahrs->filter.q[3] * input->gyro.y);
  ahrs->filter.q[2] += (qa * input->gyro.y - qb * input->gyro.z +
                        ahrs->filter.q[3] * input->gyro.x);
  ahrs->filter.q[3] +=
      (qa * input->gyro.z + qb * input->gyro.y - qc * input->gyro.x);

  // 四元数规范化
  recipNorm = InvSqrt(ahrs->filter.q[0] * ahrs->filter.q[0] +
                      ahrs->filter.q[1] * ahrs->filter.q[1] +
                      ahrs->filter.q[2] * ahrs->filter.q[2] +
                      ahrs->filter.q[3] * ahrs->filter.q[3]);
  ahrs->filter.q[0] *= recipNorm;
  ahrs->filter.q[1] *= recipNorm;
  ahrs->filter.q[2] *= recipNorm;
  ahrs->filter.q[3] *= recipNorm;
}

/**
 * @brief 快速平方根倒数算法
 * @param  x               要求算的数字
 * @return float
 */
float InvSqrt(float x) {
  float halfx = 0.5f * x;
  union {
    float f;
    long l;
  } i;
  i.f = x;
  i.l = 0x5f3759df - (i.l >> 1);
  float y = i.f;
  y = y * (1.5f - (halfx * y * y));
  y = y * (1.5f - (halfx * y * y));
  return y;
}

/**
 * @brief 使用Mahony算法解算欧拉角(FRD载体系)
 * @param  mpu6050          指向mpu6050_struct的地址，用于读取数据。
 * @param  ist8310          指向ist8310_struct的地址，用于读取数据。
 * @param  filter           指向MahonyFilterType的地址，用于数据计算。
 * @param  mahony           指向MahonyType的地址，用于存储计算结果。
 */
void MahonyGetEuler(MahonyFilterType *ahrs, MahonyInput *input, float dt) {

  MahonyUpdateAHRS(ahrs, input, dt);
  // 四元数结算弧度
  ahrs->frd.rollRad = atan2f(ahrs->filter.q[0] * ahrs->filter.q[1] +
                                 ahrs->filter.q[2] * ahrs->filter.q[3],
                             0.5f - ahrs->filter.q[1] * ahrs->filter.q[1] -
                                 ahrs->filter.q[2] * ahrs->filter.q[2]);
  ahrs->frd.pitchRad = asinf(-2.0f * (ahrs->filter.q[1] * ahrs->filter.q[3] -
                                      ahrs->filter.q[0] * ahrs->filter.q[2]));
  ahrs->frd.yawRad = atan2f(ahrs->filter.q[1] * ahrs->filter.q[2] +
                                ahrs->filter.q[0] * ahrs->filter.q[3],
                            0.5f - ahrs->filter.q[2] * ahrs->filter.q[2] -
                                ahrs->filter.q[3] * ahrs->filter.q[3]);
  // 弧度转角度
  ahrs->frd.roll = ahrs->frd.rollRad * 57.29578f;
  ahrs->frd.pitch = ahrs->frd.pitchRad * 57.29578f;
  ahrs->frd.yaw = ahrs->frd.yawRad * 57.29578f;

  // 限制航向角0-360°
  if (ahrs->frd.yaw < 0.0f) {
    ahrs->frd.yaw += 180.0f;
  }

  // 只保留整数
  ahrs->frd.roll = floor(ahrs->frd.roll);
  ahrs->frd.pitch = floor(ahrs->frd.pitch);
  ahrs->frd.yaw = floor(ahrs->frd.yaw);
}
