#include "motor_control.h"
#include "i2c.h"         // SDK中I2C相关API
#include <stdio.h>

static uint16_t g_pwm_i2c_addr = PWM_I2C_ADDR;

static errcode_t pwm_i2c_write(i2c_data_t *data)
{
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, g_pwm_i2c_addr, data);
    if (ret == 0) {
        return ret;
    }

    uint16_t fallback_addr = (g_pwm_i2c_addr == 0x5A) ? 0x2D : 0x5A;
    errcode_t fallback_ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, fallback_addr, data);
    if (fallback_ret == 0) {
        printf("pwm_i2c_write: switch address 0x%02X -> 0x%02X\r\n", g_pwm_i2c_addr, fallback_addr);
        g_pwm_i2c_addr = fallback_addr;
        return fallback_ret;
    }

    printf("pwm_i2c_write: addr 0x%02X failed %0X, addr 0x%02X failed %0X\r\n",
        g_pwm_i2c_addr, ret, fallback_addr, fallback_ret);
    return ret;
}

// 发送单字节PWM控制指令 (用于初始化)
void pwm_write(uint8_t reg_data) {
    (void)pwm_write_checked(reg_data);
}

bool pwm_write_checked(uint8_t reg_data)
{
    uint8_t buffer[] = {reg_data};
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer);
    
    errcode_t ret = pwm_i2c_write(&data);
    if (ret != 0) {
        printf("pwm_write: failed, %0X!\n", ret);
        return false;
    }
    printf("pwm_write: success! address:0x%02X\r\n", g_pwm_i2c_addr);
    return true;
}

// 发送多字节PWM控制指令 (已修复原版 sizeof(指针) 的 Bug)
bool pwm_writes(uint8_t* reg_data, uint32_t len) {
    i2c_data_t data = {0};
    data.send_buf = reg_data;
    data.send_len = len; // 修复：使用传入的正确长度，而不是 sizeof
    
    errcode_t ret = pwm_i2c_write(&data);
    if (ret != 0) {
        printf("pwm_writes: failed, %0X!\n", ret);
        return false;
    }
    // 取消频繁打印，防止打印导致时序卡顿
    // printf("pwm_writes: success!\r\n");
    return true;
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
        pwm_writes(cmd_90, 3);
        pwm_writes(cmd_a0, 3);
    } else {
        cmd_a0[1] = crrh;
        cmd_a0[2] = crrl;
        pwm_writes(cmd_90, 3);
        pwm_writes(cmd_a0, 3);
    }
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
        pwm_writes(cmd_70, 3);
        pwm_writes(cmd_80, 3);
    } else {
        cmd_70[1] = crrh;
        cmd_70[2] = crrl;
        pwm_writes(cmd_70, 3);
        pwm_writes(cmd_80, 3);
    }
}
