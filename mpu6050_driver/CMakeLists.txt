add_library(mpu6050 STATIC 
    mpu6050.c
    mpu6050.h
)

target_include_directories(mpu6050 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mpu6050 PUBLIC hardware_i2c pico_stdlib)