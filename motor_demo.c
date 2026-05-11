#include <stdio.h>
#include <unistd.h>
#include "cmsis_os2.h"      
#include "app_init.h"  
#include "i2c.h"
#include "motor_control.h"
#include "pinctrl.h" 

// 电机测试任务
static void MotorDemoTask(void *arg) {
    (void)arg;
    printf("[Motor Demo] Task Started!\r\n");
    
    uapi_i2c_init(I2C_MASTER_BUS_ID, 400000);

    // 1. 初始化 PWM 频率 (根据实验报告指导)
    pwm_write(0x16);
    osDelay(10); // 等待初始化生效 (鸿蒙OS延时API，1单位通常等于10ms，根据实际SDK可能不同)

    while (1) {
        // --- 动作1：前进 ---
        printf("[Motor Demo] Move Forward...\r\n");
        left_wheel_set(800, 1000, true);   // 占空比 800/1000
        right_wheel_set(800, 1000, true);
        osDelay(200); // 运行约2秒 (假设 tick=10ms)

        // --- 动作2：停止 ---
        printf("[Motor Demo] Stop!\r\n");
        left_wheel_set(0, 1000, true);
        right_wheel_set(0, 1000, true);
        osDelay(100); // 停1秒缓冲

        // --- 动作3：后退 ---
        printf("[Motor Demo] Move Backward...\r\n");
        left_wheel_set(800, 1000, false);
        right_wheel_set(800, 1000, false);
        osDelay(200);

        // --- 动作4：停止 ---
        printf("[Motor Demo] Stop!\r\n");
        left_wheel_set(0, 1000, true);
        right_wheel_set(0, 1000, true);
        osDelay(100);
    }
}

// 入口函数：创建任务

static void MotorDemoEntry(void) {
    osThreadAttr_t attr = {0};
    attr.name = "MotorDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4; // 栈大小 4KB
    attr.priority = osPriorityNormal;

    if (osThreadNew(MotorDemoTask, NULL, &attr) == NULL) {
        printf("[Motor Demo] Failed to create task!\r\n");
    }
}

// 注册启动入口（根据你们 SDK 实际的宏定义可能是 SYS_RUN 或 APP_FEATURE_INIT）
app_run(MotorDemoEntry);    // <--- 将原本的 APP_FEATURE_INIT 替换成这句
