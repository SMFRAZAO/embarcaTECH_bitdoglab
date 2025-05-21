# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(dma_usb C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adiciona o executável com o nome correto do arquivo fonte
add_executable(dma_usb dma_usb.c ssd1306.c)

# Define nome e versão do programa
pico_set_program_name(dma_usb "dma_usb")
pico_set_program_version(dma_usb "0.1")

# Configura saída stdio
pico_enable_stdio_uart(dma_usb 0)
pico_enable_stdio_usb(dma_usb 1)

# Bibliotecas padrão e periféricos usados
target_link_libraries(dma_usb
    pico_stdlib
    pico_stdio_usb
    hardware_dma
    hardware_adc
    hardware_i2c
)


# Diretório com os headers
target_include_directories(dma_usb PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Gera UF2, bin, etc
pico_add_extra_outputs(dma_usb)
