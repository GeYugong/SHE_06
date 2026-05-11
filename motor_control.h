#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
// 根据实验报告引用的头文件
#include "pinctrl.h"
#include "common_def.h"

#define I2C_MASTER_BUS_ID 1
#define PWM_I2C_ADDR 0x5A

// 声明我们要用到的函数
void pwm_write(uint8_t reg_data);
bool pwm_write_checked(uint8_t reg_data);
bool pwm_writes(uint8_t* reg_data, uint32_t len);
void left_wheel_set(uint16_t CRR, uint16_t limit, bool dir);
void right_wheel_set(uint16_t CRR, uint16_t limit, bool dir);

#endif // MOTOR_CONTROL_H
