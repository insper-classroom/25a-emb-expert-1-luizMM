#include "mpu6050.h"
#include <stdio.h>

int main() {
    // Inicialização serial
    stdio_init_all();

    // Objeto de configuração do sensor
    imu_c mpu;
    mpu6050_set_config(&mpu, i2c0, 4, 5, 0);

    if (!mpu6050_init(mpu)) {
        printf("Erro ao iniciar MPU6050.\n");
        return 1;
    }

    printf("MPU6050 inicializado com sucesso!\n");

    while (1) {
        int16_t acc[3];
        int16_t gyro[3];
        int16_t temperature;

        // Leitura dos dados
        if (mpu6050_read_acc(mpu, acc) &&
            mpu6050_read_gyro(mpu, gyro) &&
            mpu6050_read_temp(mpu, &temperature)) {

            printf("ACC  -> X=%d | Y=%d | Z=%d\n", acc[0], acc[1], acc[2]);
            printf("GYRO -> X=%d | Y=%d | Z=%d\n", gyro[0], gyro[1], gyro[2]);
            printf("TEMP -> %.2f °C\n", (temperature / 340.0f) + 36.53f);
            printf("--------------------------\n");
        } else {
            printf("Falha na leitura dos dados do sensor.\n");
        }
        sleep_ms(100);
    }

    return 0;
}
