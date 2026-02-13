cmake_minimum_required(VERSION 3.10)

# 设置尝试编译目标类型为静态库
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(MyDriverProject LANGUAGES C CXX)

# 生成编译数据库给clangd使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 检测主机操作系统
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(HOST_OS "Windows")
    # 设置交叉编译器路径
    set(CMAKE_C_COMPILER "C:/.Drivers/Arm GNU Toolchain arm-none-eabi/14.2 rel1/bin/arm-none-eabi-gcc.exe")
    set(CMAKE_CXX_COMPILER "C:/.Drivers/Arm GNU Toolchain arm-none-eabi/14.2 rel1/bin/arm-none-eabi-g++.exe")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(HOST_OS "Linux")
    # 设置交叉编译器路径
    set(CMAKE_C_COMPILER "/home/muli/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc")
    set(CMAKE_CXX_COMPILER "/home/muli/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++")    
else()
    message(WARNING "Unsupported host OS: ${CMAKE_HOST_SYSTEM_NAME}. Assuming Linux-like toolchain.")
    set(HOST_OS ${CMAKE_HOST_SYSTEM_NAME})
    set(TOOLCHAIN_EXE_SUFFIX "")
    set(TOOLCHAIN_PATH "/usr/bin" CACHE PATH "ARM toolchain bin directory")
endif()

# 添加头文件搜索路径
include_directories(
    device/iic/
    device/iic/ist8310/
    device/iic/mpu6050/
    device/iic/ssd1306/
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
file(GLOB IIC_SSD1306_SOURCES "device/iic/ssd1306/*.c")
file(GLOB SPI_NRF24L01_SOURCES "device/spi/nrf24l01/*.c")
file(GLOB UART_HC05_SOURCES "device/uart/hc05/*.c")
file(GLOB MAHONY_SOURCES "modules/mahony/*.c")
file(GLOB PID_SOURCES "modules/pid/*.c")
file(GLOB VOFA_SOURCES "modules/vofa/*.c")

# 创建库驱动库
add_library(MyDriver STATIC
    ${IIC_IST8310_SOURCES}
    ${IIC_MPU6050_SOURCES}
    ${IIC_SSD1306_SOURCES}
    ${SPI_NRF24L01_SOURCES}
    ${UART_HC05_SOURCES}
    ${MAHONY_SOURCES}
    ${PID_SOURCES}
    ${VOFA_SOURCES}
)

target_link_libraries(MyDriver m)

# 如果有main.c，创建一个简单的测试程序
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
    add_executable(MyDriverTest main.c)
    target_link_libraries(MyDriverTest MyDriver)
endif()