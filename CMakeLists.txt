cmake_minimum_required(VERSION 3.10)
project(MyDriverProject LANGUAGES C CXX)

# 生成编译数据库给clangd使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加头文件搜索路径
include_directories(
    device/iic/
    device/iic/ist8310/
    device/iic/mpu6050/
    device/spi/
    device/spi/nrf24l01/
    device/uart/
    device/uart/hc05/
    modules/
    modules/mahony/
    modules/pid/
    modules/vofa/
)

# 分别收集每个模块的源文件
file(GLOB IIC_IST8310_SOURCES "device/iic/ist8310/*.c")
file(GLOB IIC_MPU6050_SOURCES "device/iic/mpu6050/*.c")
file(GLOB SPI_NRF24L01_SOURCES "device/spi/nrf24l01/*.c")
file(GLOB UART_HC05_SOURCES "device/uart/hc05/*.c")
file(GLOB MAHONY_SOURCES "modules/mahony/*.c")
file(GLOB PID_SOURCES "modules/pid/*.c")
file(GLOB VOFA_SOURCES "modules/vofa/*.c")

# 创建库驱动库
add_library(MyDriver STATIC
    ${IIC_IST8310_SOURCES}
    ${IIC_MPU6050_SOURCES}
    ${SPI_NRF24L01_SOURCES}
    ${UART_HC05_SOURCES}
    ${MAHONY_SOURCES}
    ${PID_SOURCES}
    ${VOFA_SOURCES}
)

# 如果有main.c，创建一个简单的测试程序
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
    add_executable(MyDriverTest main.c)
    target_link_libraries(MyDriverTest MyDriver)
endif()