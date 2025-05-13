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

include(pico_sdk_import.cmake)

project(microphone_dma C CXX ASM)

pico_sdk_init()

add_executable(microphone_dma microphone_dma.c)

pico_set_program_name(microphone_dma "microphone_dma")
pico_set_program_version(microphone_dma "0.1")

pico_generate_pio_header(microphone_dma ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

pico_enable_stdio_uart(microphone_dma 0)
pico_enable_stdio_usb(microphone_dma 1)

target_include_directories(microphone_dma PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(microphone_dma
    pico_stdlib
    hardware_dma
    hardware_timer
    hardware_adc
    hardware_pio
    hardware_clocks
)

pico_add_extra_outputs(microphone_dma)
