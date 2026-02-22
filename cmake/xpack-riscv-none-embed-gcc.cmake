set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Make this globally visible
# Recommend toolchain for this project https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases

# Validate TOOLCHAIN_BIN_PATH
if(NOT DEFINED ENV{TOOLCHAIN_BIN_PATH} OR ENV{TOOLCHAIN_BIN_PATH} STREQUAL "")
    message(FATAL_ERROR "TOOLCHAIN_BIN_PATH is not defined or empty!")
endif()

set(ENV{PATH} "$ENV{TOOLCHAIN_BIN_PATH}:$ENV{PATH}")

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                riscv-none-embed-)
set(C_FLAGS                         "-fdata-sections -ffunction-sections --specs=nano.specs -Wl,--gc-sections")

# Define compiler settings
set(CMAKE_C_COMPILER    ${TOOLCHAIN_PREFIX}gcc ${C_FLAGS})
set(CMAKE_ASM_COMPILER  ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PREFIX}g++ ${C_FLAGS} ${CPP_FLAGS})
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE          ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C   ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
