# Added for Upsilon

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_RC_COMPILER arm-none-eabi-gcc)

SET(CMAKE_C_FLAGS "-O3 -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions -nostdlib")
SET(CMAKE_CXX_FLAGS "-O3 -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions -nostdlib")
