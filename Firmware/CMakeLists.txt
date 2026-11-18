cmake_minimum_required(VERSION 3.22)

project(PolarityChecker C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)       # IntelliSense / clangd

file(GLOB APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/Src/*.c)

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${CMAKE_SOURCE_DIR}/system_stm32c5xx.c
    ${CMAKE_SOURCE_DIR}/startup_stm32c551xx.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS_Device
)

target_compile_definitions(${PROJECT_NAME} PRIVATE STM32C551xx)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -ffunction-sections -fdata-sections
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-Os -g>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/stm32c551xe_flash.ld
    --specs=nano.specs
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--print-memory-usage
)

# .hex / .bin next to the .elf, and a size summary after each build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
