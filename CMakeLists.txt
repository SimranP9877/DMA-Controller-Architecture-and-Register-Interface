
cmake_minimum_required(VERSION 3.16)

project(DMA-Controller-Architecture-and-Register-Interface LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SystemCLanguage CONFIG REQUIRED
    PATHS "C:/SystemC/install/lib/cmake/SystemCLanguage"
    NO_DEFAULT_PATH
)

add_executable(DMA-Controller-Architecture-and-Register-Interface 
    src/sc_main.cpp
    src/cpu.cpp
    src/dma_controller.cpp
    src/memory.cpp
    tests/test_dma_transfer.cpp
)

target_include_directories(DMA-Controller-Architecture-and-Register-Interface 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(DMA-Controller-Architecture-and-Register-Interface 
    PRIVATE SystemC::systemc
)
