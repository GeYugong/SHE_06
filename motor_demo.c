#include <stdio.h>
#include "soc_osal.h"
#include "app_init.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "i2c.h"
#include "motor_control.h"
#include "pinctrl.h"

#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x0
#define I2C_SET_BAUDRATE 100000

#define MOTOR_TASK_STACK_SIZE 0x2000
#define MOTOR_TASK_PRIO 20
#define PWM_INIT_DELAY_MS 500
#define MOTOR_RUN_TIME_MS 2000
#define MOTOR_STOP_TIME_MS 1000
#define DIAG_RETRY_DELAY_MS 5000

static void motor_diag_read_pwm(void)
{
    uint8_t buffer[21] = {0};
    i2c_data_t data = {0};
    data.receive_buf = buffer;
    data.receive_len = sizeof(buffer);

    errcode_t ret = uapi_i2c_master_read(I2C_MASTER_BUS_ID, PWM_I2C_ADDR, &data);
    if (ret != 0) {
        printf("[Motor Demo] pwm diag read addr 0x%02X failed, ret=%0x\r\n", PWM_I2C_ADDR, ret);
        return;
    }

    printf("[Motor Demo] pwm diag read ok:");
    for (uint32_t i = 0; i < sizeof(buffer); i++) {
        printf(" %02X", buffer[i]);
    }
    printf("\r\n");
}

// 电机测试任务
static void *MotorDemoTask(const char *arg)
{
    unused(arg);
    printf("[Motor Demo] Task Started!\r\n");

    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);

    errcode_t ret = uapi_i2c_master_init(I2C_MASTER_BUS_ID, I2C_SET_BAUDRATE, I2C_MASTER_ADDR);
    if (ret != 0) {
        printf("[Motor Demo] i2c init failed, ret = %0x\r\n", ret);
        return NULL;
    }
    printf("[Motor Demo] i2c init success, bus=%d, baudrate=%d\r\n", I2C_MASTER_BUS_ID, I2C_SET_BAUDRATE);

    motor_diag_read_pwm();

    // 初始化 PWM 频率
    if (!pwm_write_checked(0x16)) {
        while (1) {
            printf("[Motor Demo] PWM chip not responding. Check car power, I2C pins, and STM8S/PWM board.\r\n");
            osal_msleep(DIAG_RETRY_DELAY_MS);
            motor_diag_read_pwm();
            if (pwm_write_checked(0x16)) {
                break;
            }
        }
    }
    osal_msleep(PWM_INIT_DELAY_MS);

    while (1) {
        // --- 动作1：前进 ---
        printf("[Motor Demo] Move Forward...\r\n");
        left_wheel_set(500, 500, true);
        right_wheel_set(500, 500, true);
        osal_msleep(MOTOR_RUN_TIME_MS);

        // --- 动作2：停止 ---
        printf("[Motor Demo] Stop!\r\n");
        left_wheel_set(0, 0, true);
        right_wheel_set(0, 0, true);
        osal_msleep(MOTOR_STOP_TIME_MS);

        // --- 动作3：后退 ---
        printf("[Motor Demo] Move Backward...\r\n");
        left_wheel_set(500, 500, false);
        right_wheel_set(500, 500, false);
        osal_msleep(MOTOR_RUN_TIME_MS);

        // --- 动作4：停止 ---
        printf("[Motor Demo] Stop!\r\n");
        left_wheel_set(0, 0, true);
        right_wheel_set(0, 0, true);
        osal_msleep(MOTOR_STOP_TIME_MS);
    }

    return NULL;
}

// 入口函数：创建任务
static void MotorDemoEntry(void)
{
    osal_task *taskid = NULL;

    osal_kthread_lock();
    taskid = osal_kthread_create((osal_kthread_handler)MotorDemoTask, NULL, "MotorDemoTask", MOTOR_TASK_STACK_SIZE);
    if (taskid == NULL) {
        printf("[Motor Demo] Failed to create task!\r\n");
        osal_kthread_unlock();
        return;
    }

    if (osal_kthread_set_priority(taskid, MOTOR_TASK_PRIO) != OSAL_SUCCESS) {
        printf("[Motor Demo] Failed to set task priority!\r\n");
    }
    osal_kthread_unlock();
}

app_run(MotorDemoEntry);
