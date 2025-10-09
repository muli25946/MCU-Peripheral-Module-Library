cmake_minimum_required(VERSION 3.10)
project(MyDriverProject LANGUAGES C CXX)  # 根据您的开发语言启用C和/或C++

# 核心命令：指示CMake生成编译数据库，这是clangd工作的基础:cite[2]
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加头文件搜索路径
include_directories(
    device/iic/Lib
    device/iic/Reg
    device/spi/Lib
    device/spi/Reg
    device/uart/Lib
    modules/Lib
    # 如果还有其他自定义头文件目录，请在此继续添加
)

# 递归查找所有源文件，并创建目标
file(GLOB_RECURSE ALL_SOURCES
    "device/iic/Src/*.c"
    "device/spi/Src/*.c"
    "device/uart/Src/*.c"
    "modules/Src/*.c"
)

# 根据找到的源文件创建可执行文件。如果目标是静态库，可使用 add_library(... STATIC)
add_executable(MyDriverTarget main.c ${ALL_SOURCES})

# 如果需要链接第三方库，可以取消注释以下命令
# target_link_libraries(MyDriverTarget ${XXX_LIBRARIES})