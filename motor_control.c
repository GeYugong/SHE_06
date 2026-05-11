#include "motor_control.h"
#include "i2c.h"         // SDK中I2C相关API
#include <stdio.h>

// 发送单字节PWM控制指令 (用于初始化)
void pwm_write(uint8_t reg_data) {
    uint8_t buffer[] = {reg_data};
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer);
    
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, 0x5A, &data);
    if (ret != 0) {
        printf("pwm_write: failed, %0X!\n", ret);
        return;
    }
    printf("pwm_write: success! address:0x5A\r\n");
}

// 发送多字节PWM控制指令 (已修复原版 sizeof(指针) 的 Bug)
void pwm_writes(uint8_t* reg_data, uint32_t len) {
    i2c_data_t data = {0};
    data.send_buf = reg_data;
    data.send_len = len; // 修复：使用传入的正确长度，而不是 sizeof
    
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, 0x5A, &data);
    if (ret != 0) {
        printf("pwm_writes: failed, %0X!\n", ret);
        return;
    }
    // 取消频繁打印，防止打印导致时序卡顿
    // printf("pwm_writes: success!\r\n");
}

// 左轮控制函数
void left_wheel_set(uint16_t CRR, uint16_t limit, bool dir) {
    uint16_t actual_crr = (CRR > limit) ? limit : CRR;
    uint8_t crrh = (uint8_t)((actual_crr >> 8) & 0xFF);
    uint8_t crrl = (uint8_t)(actual_crr & 0xFF);
    
    uint8_t cmd_a0[3] = {0xA0, 0x00, 0x00};
    uint8_t cmd_90[3] = {0x90, 0x00, 0x00};
    
    if (dir) {
        cmd_90[1] = crrh;
        cmd_90[2] = crrl;
    } else {
        cmd_a0[1] = crrh;
        cmd_a0[2] = crrl;
    }
    
    pwm_writes(cmd_a0, 3);
    pwm_writes(cmd_90, 3);
}

// 右轮控制函数
void right_wheel_set(uint16_t CRR, uint16_t limit, bool dir) {
    uint16_t actual_crr = (CRR > limit) ? limit : CRR;
    uint8_t crrh = (uint8_t)((actual_crr >> 8) & 0xFF);
    uint8_t crrl = (uint8_t)(actual_crr & 0xFF);
    
    uint8_t cmd_70[3] = {0x70, 0x00, 0x00};
    uint8_t cmd_80[3] = {0x80, 0x00, 0x00};
    
    if (dir) {
        cmd_80[1] = crrh;
        cmd_80[2] = crrl;
    } else {
        cmd_70[1] = crrh;
        cmd_70[2] = crrl;
    }
    
    pwm_writes(cmd_70, 3);
    pwm_writes(cmd_80, 3);
}