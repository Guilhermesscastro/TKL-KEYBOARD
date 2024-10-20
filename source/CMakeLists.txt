cmake_minimum_required(VERSION 3.13)

add_compile_definitions(CFG_TUSB_OS=OPT_OS_PICO)

# set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(picotkl_proj)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()



# rest of your project
add_executable(picotkl
    main.c
    keyboard.c
    led.c
    usb_descriptors.c
    bongocat.c
)

pico_generate_pio_header(picotkl ${CMAKE_CURRENT_LIST_DIR}/encoder.pio)

target_include_directories(picotkl PRIVATE include)

# enable usb output, disable uart output
pico_enable_stdio_usb(picotkl 0)
pico_enable_stdio_uart(picotkl 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(picotkl)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(picotkl pico_stdlib tinyusb_device)

#add_subdirectory(pico-ssd1306)
add_subdirectory(pico-ssd1306)
target_link_libraries(picotkl
pico_ssd1306
# you will also need hardware i2c library for communication with the display
hardware_i2c
hardware_pio
)
