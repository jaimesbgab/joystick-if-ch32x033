# Minimum CMake version required for this project
cmake_minimum_required(VERSION 3.10)

# Set the toolchain binary path from external variable (must be defined before running cmake)
# This path should point to the directory containing RISC-V GCC compiler binaries
set(ENV{TOOLCHAIN_BIN_PATH} "${TOOLCHAIN_BIN_PATH}")
message(STATUS "ENV{TOOLCHAIN_BIN_PATH} = $ENV{TOOLCHAIN_BIN_PATH}")

# Add cmake module directory to search path for custom CMake scripts
# This allows us to use custom find modules and configuration files
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(USE_THIRD_PARTY_HAL_DRIVER 0)
if(USE_THIRD_PARTY_HAL_DRIVER)
  add_definitions( -DUSE_THIRD_PARTY_HAL_DRIVER=1 )
else()
  add_definitions( -DUSE_THIRD_PARTY_HAL_DRIVER=0 )
endif()

# Include HAL drivers configuration (loads CH32V00x peripheral drivers and linker script)
include(FindHalDrivers)
include(FindUsbDrivers)
#include(FindMiddewares)
include(ch32x033)

# Display build configuration type (Debug, Release, etc.)
message("Build type: "              ${CMAKE_BUILD_TYPE})

# C language standard: C11 with GNU extensions enabled
# C11 provides modern C features while maintaining compatibility with embedded systems
set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)

# C++ language standard: C++17 with GNU extensions enabled
# C++17 provides modern C++ features (though this project primarily uses C)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)

# Export compile commands for IDE integration and static analysis tools
# Generates compile_commands.json for tools like clangd, clang-tidy, etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS	ON)

# Store the project root directory path for later reference
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})

# Specify the toolchain file for cross-compilation to RISC-V
# This file configures the RISC-V GCC compiler and related tools
set(CMAKE_TOOLCHAIN_FILE            riscv-wch-elf-gcc)

# Define the project name (generates 'example.elf' binary)
project(example)

# Enable C, C++, and Assembly language support
# Assembly is required for the startup code (startup_ch32v00X.S)
enable_language(C CXX ASM)

message(STATUS "Build directory is: ${CMAKE_BINARY_DIR}")

# Configure output directory for the compiled binary (.elf file)
# The final executable will be placed in build/bin/
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Recursively collect all C source files from the Core/src directory
# These are your application-specific source files (main.c, interrupts, syscalls, etc.)
file(GLOB_RECURSE PROJ_SRC
    "${PROJ_PATH}/Core/src/*.c"
)

# Recursively collect all header files from the Core/inc directory
# These are your application-specific headers
file(GLOB_RECURSE PROJ_INC
    "${PROJ_PATH}/Core/inc/*.h"
)

# Define include directories for the project
# These paths will be added with -I flags during compilation
set(PROJ_INC_DIRS
    "${PROJ_PATH}/Core/inc"
)

#message(STATUS "- PROJ_SRC = ${PROJ_SRC}")
#message(STATUS "- PROJ_INC = ${PROJ_INC}")
#message(STATUS "- PROJ_INC_DIRS = ${PROJ_INC_DIRS}")

# Set the executable name (same as project name)
set(EXECUTABLE ${CMAKE_PROJECT_NAME})

# Create the executable target by combining:
# - HAL_DRIVERS_SRC: CH32V00x peripheral drivers and startup code
# - PROJ_SRC: Application source files
add_executable(${EXECUTABLE}
    ${HAL_DRIVERS_SRC}
    ${USB_DRIVERS_SRC}
    ${PROJ_SRC}
)

# Add include directories to the target
# PRIVATE means these includes are only used when compiling this target
# Includes both HAL driver headers and application headers
target_include_directories(${EXECUTABLE}
    PRIVATE
        Sigma
        ${HAL_DRIVERS_INC_DIRS}
        ${DRIVERS_INC_DIRS}
        ${PROJ_INC_DIRS}
)

message(STATUS "MCU_FLAGS: ${MCU_FLAGS}")

# Compiler options for the target
# These flags control how the code is compiled
target_compile_options(${EXECUTABLE} PRIVATE
  ${MCU_FLAGS}              # MCU-specific flags from ch32v007.cmake:
                            # -march=rv32ec: RISC-V 32-bit with embedded & compressed extensions
                            # -mabi=ilp32e: Integer/Long/Pointer 32-bit embedded ABI (16 registers)
                            # -msmall-data-limit=8: Place data <= 8 bytes in small data section
                            # -mno-save-restore: Inline prologue/epilogue instead of function calls
                            # -Os: Optimize for size (critical for small flash devices)
                            # -fmessage-length=0: Unlimited error message length
                            # -fsigned-char: Make 'char' type signed by default
                            # -ffunction-sections: Place each function in its own section
                            # -fdata-sections: Place each data item in its own section
                            # -Wunused: Warn about unused variables
                            # -Wuninitialized: Warn about uninitialized variables
                            # -g: Include debug information

  -Wall                     # Enable all common warnings
  #-Wpedantic                # Warn about non-standard C/C++ usage
  -Wno-unused-parameter     # Suppress warnings for unused function parameters
)

# Linker options for the target
# These flags control how the final binary is linked
target_link_options(${EXECUTABLE} PRIVATE
  -T${LINKER_SCRIPT_SRC}    # Use custom linker script (Link.ld) that defines memory layout
                            # The linker script specifies FLASH and RAM regions for CH32V007

  ${MCU_FLAGS}              # Include same MCU flags as compiler (architecture, ABI, etc.)

  -Wl,-Map=${CMAKE_PROJECT_NAME}.map  # Generate a map file showing symbol addresses and memory usage

#  -Wl,--start-group         # (Optional) Allow circular dependencies between libraries
#  -Wl,--end-group           # Must be used as a pair with --start-group

  -Wl,--print-memory-usage  # Print memory region usage summary after linking
                            # Shows FLASH and RAM consumption

#  -Xlinker                  # Pass the next argument directly to the linker
#  --gc-sections             # Remove unused sections (pairs with -ffunction-sections/-fdata-sections)
                            # Critical for reducing binary size on small devices

#  --specs=nosys.specs       # Use newlib-nano without system calls
                            # Provides minimal C library suitable for bare-metal embedded systems
#  -nostartfiles             # Do not use standard system startup files

  -lgcc
  -Wl,--gc-sections,--build-id=none
)

# Generate .hex file from the .elf binary
# This creates an Intel HEX format file that can be used for flashing the microcontroller
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${TOOLCHAIN_BIN_PATH}/${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE}.elf ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE}.hex
    COMMENT "Generating HEX file: ${EXECUTABLE}.hex"
)

# Generate .bin file from the .elf binary
# This creates a raw binary file that can be used for flashing the microcontroller
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${TOOLCHAIN_BIN_PATH}/${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE}.elf ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE}.bin
    COMMENT "Generating BIN file: ${EXECUTABLE}.bin"
)

# Custom target to program the device using chprog
# Usage: make chprog (or cmake --build . --target flash)
add_custom_target(flash
    COMMAND chprog ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE}.bin
    DEPENDS ${EXECUTABLE}
    COMMENT "Programming device with chprog: ${EXECUTABLE}.bin"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
