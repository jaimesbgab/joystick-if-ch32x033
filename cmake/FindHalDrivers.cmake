
if(USE_THIRD_PARTY_HAL_DRIVER)
  set(HAL_DRIVERS_TARGET ch32x033_tp)
else()
  set(HAL_DRIVERS_TARGET ch32x033)
endif()

# Define the root drivers path
set(HAL_DRIVERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${HAL_DRIVERS_TARGET}")

# Collect all C source files recursively from the 'src' directory
# and store them in the HAL_DRIVERS_SRC variables
file(GLOB_RECURSE HAL_DRIVERS_SRC
    "${HAL_DRIVERS_DIR}/Startup/*.S"
    "${HAL_DRIVERS_DIR}/Startup/*.s"
    "${HAL_DRIVERS_DIR}/Core/*.c"
    "${HAL_DRIVERS_DIR}/Debug/*.c"
    "${HAL_DRIVERS_DIR}/Peripheral/src/*.c"
)

# Collect all header files recursively from the 'inc' directory
# and store them in the HAL_DRIVERS_INC variable
file(GLOB_RECURSE HAL_DRIVERS_INC 
    "${HAL_DRIVERS_DIR}/Core/*.h"
    "${HAL_DRIVERS_DIR}/Peripheral/inc/*.h"
    "${HAL_DRIVERS_DIR}/Debug/*.h"
)

set (HAL_DRIVERS_INC_DIRS
    "${HAL_DRIVERS_DIR}/Core"
    "${HAL_DRIVERS_DIR}/Peripheral/inc"
    "${HAL_DRIVERS_DIR}/Debug"
)

#message(STATUS "- HAL_DRIVERS_DIR = ${HAL_DRIVERS_DIR}")
#message(STATUS "- HAL_DRIVERS_SRC = ${HAL_DRIVERS_SRC}")
message(STATUS "- HAL_DRIVERS_INC = ${HAL_DRIVERS_INC}")
#message(STATUS "- HAL_DRIVERS_INC_DIRS = ${HAL_DRIVERS_INC_DIRS}")

# Validate HAL_DRIVERS_SRC
if(NOT DEFINED HAL_DRIVERS_SRC OR HAL_DRIVERS_SRC STREQUAL "")
    message(FATAL_ERROR "HAL_DRIVERS_SRC is not defined or empty!")
endif()

# Validate HAL_DRIVERS_INC
if(NOT DEFINED HAL_DRIVERS_INC OR HAL_DRIVERS_INC STREQUAL "")
    message(FATAL_ERROR "HAL_DRIVERS_INC is not defined or empty!")
endif()

# Validate HAL_DRIVERS_INC_DIRS
if(NOT DEFINED HAL_DRIVERS_INC_DIRS OR HAL_DRIVERS_INC_DIRS STREQUAL "")
    message(FATAL_ERROR "HAL_DRIVERS_INC_DIRS is not defined or empty!")
endif()

# Linker File
set(LINKER_SCRIPT_SRC "${HAL_DRIVERS_DIR}/Ld/Link.ld")
message(STATUS "- LINKER_SCRIPT_SRC = ${LINKER_SCRIPT_SRC}")

# Validate HAL_DRIVERS_INC_DIRS
if(NOT DEFINED LINKER_SCRIPT_SRC OR LINKER_SCRIPT_SRC STREQUAL "")
    message(FATAL_ERROR "LINKER_SCRIPT_SRC is not defined or empty!")
endif()

message(STATUS "Loaded FindHalDrivers.cmake")
message(STATUS "- HAL_DRIVERS_TARGET = ${HAL_DRIVERS_TARGET}")
