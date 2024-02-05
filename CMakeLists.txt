add_library(pico_ssd1306
        ssd1306.c)

target_link_libraries(pico_ssd1306
        hardware_i2c
        pico_stdlib
        )
target_include_directories (pico_ssd1306 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})