#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
// 根据实验报告引用的头文件
#include "pinctrl.h"
#include "common_def.h"

#define I2C_MASTER_BUS_ID 1

// 声明我们要用到的函数
void pwm_write(uint8_t reg_data);
void pwm_writes(uint8_t* reg_data, uint32_t len); 
void left_wheel_set(uint16_t CRR, uint16_t limit, bool dir);
void right_wheel_set(uint16_t CRR, uint16_t limit, bool dir);

#endif // MOTOR_CONTROL_H