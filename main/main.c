/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "mpu6050.h"

struct led_task_arg {
    int gpio;
    int delay;
};

void led_task(void *p) {
    struct led_task_arg *a = (struct led_task_arg *)p;

    gpio_init(a->gpio);
    gpio_set_dir(a->gpio, GPIO_OUT);
    while (true) {
        gpio_put(a->gpio, 1);
        vTaskDelay(pdMS_TO_TICKS(a->delay));
        gpio_put(a->gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(a->delay));
    }
}

void imu_task(void *p) {
    imu_c imu;
    mpu6050_set_config(&imu, i2c0, 4, 5, 0); // ±2G

    if (!mpu6050_init(imu)) {
        printf("MPU6050 init failed!\n");
        vTaskDelete(NULL);
    }

    mpu6050_reset(imu);
    vTaskDelay(pdMS_TO_TICKS(100)); // Aguarda reset

    while (1) {
        int16_t acc[3], gyro[3], temp;
        if (mpu6050_read_acc(imu, acc) &&
            mpu6050_read_gyro(imu, gyro) &&
            mpu6050_read_temp(imu, &temp)) {

            printf("ACC  : X=%d Y=%d Z=%d\n", acc[0], acc[1], acc[2]);
            printf("GYRO : X=%d Y=%d Z=%d\n", gyro[0], gyro[1], gyro[2]);
            printf("TEMP : %.2f C\n", (temp / 340.0f) + 36.53f);
        } else {
            printf("Failed to read IMU\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Leitura a cada 1s
    }
}

int main() {
    stdio_init_all();
    printf("Start LED blink\n");

    struct led_task_arg arg1 = {20, 100};
    xTaskCreate(led_task, "LED_Task 1", 256, &arg1, 1, NULL);

    struct led_task_arg arg2 = {21, 200};
    xTaskCreate(led_task, "LED_Task 2", 256, &arg2, 1, NULL);

    struct led_task_arg arg3 = {22, 300};
    xTaskCreate(led_task, "LED_Task 3", 256, &arg3, 1, NULL);

    xTaskCreate(imu_task, "IMU_Task", 512, NULL, 2, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
