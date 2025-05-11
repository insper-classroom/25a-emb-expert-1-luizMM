#include "mpu6050.h"

static int write_reg(imu_c config, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return i2c_write_blocking(config.i2c, MPU6050_ADDR, data, 2, false) == 2;
}

static int read_regs(imu_c config, uint8_t reg, uint8_t *buf, uint len) {
    return i2c_write_blocking(config.i2c, MPU6050_ADDR, &reg, 1, true) == 1 &&
           i2c_read_blocking(config.i2c, MPU6050_ADDR, buf, len, false) == len;
}

void mpu6050_set_config(imu_c *config, i2c_inst_t *i2c, int pin_sda, int pin_scl, int acc_scale) {
    config->i2c = i2c;
    config->pin_sda = pin_sda;
    config->pin_scl = pin_scl;
    config->acc_scale = acc_scale;
}

int mpu6050_init(imu_c config) {
    i2c_init(config.i2c, 400 * 1000);
    gpio_set_function(config.pin_sda, GPIO_FUNC_I2C);
    gpio_set_function(config.pin_scl, GPIO_FUNC_I2C);
    gpio_pull_up(config.pin_sda);
    gpio_pull_up(config.pin_scl);

    sleep_ms(100);
    if (!write_reg(config, 0x6B, 0x00)){ 
        return 0; // Wake up
    }
    if (!write_reg(config, 0x1C, (config.acc_scale << 3))){ 
        return 0; // Acc scale
    }
    if (!write_reg(config, 0x1B, 0x00)){ 
        return 0; // Gyro scale ±250 deg/s
    }
    return 1;
}

int mpu6050_reset(imu_c config) {
    return write_reg(config, 0x6B, 0x80); // Reset
}

int mpu6050_read_acc(imu_c config, int16_t accel[3]) {
    uint8_t buf[6];
    if (!read_regs(config, 0x3B, buf, 6)){ 
        return 0;
    }
    for (int i = 0; i < 3; i++)
        accel[i] = (int16_t)((buf[i*2] << 8) | buf[i*2 + 1]);
    return 1;
}

int mpu6050_read_gyro(imu_c config, int16_t gyro[3]) {
    uint8_t buf[6];
    if (!read_regs(config, 0x43, buf, 6)){ 
        return 0;
    }
    for (int i = 0; i < 3; i++)
        gyro[i] = (int16_t)((buf[i*2] << 8) | buf[i*2 + 1]);
    return 1;
}

int mpu6050_read_temp(imu_c config, int16_t *temp) {
    uint8_t buf[2];
    if (!read_regs(config, 0x41, buf, 2)){ 
        return 0;
    }
    *temp = (int16_t)((buf[0] << 8) | buf[1]);
    return 1;
}