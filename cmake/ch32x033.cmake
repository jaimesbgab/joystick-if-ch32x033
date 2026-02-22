set(MCU_FLAGS ${MCU_FLAGS}
  -march=rv32imac
  -mabi=ilp32
  -g
  -Os
  -flto
  -ffunction-sections
  -fdata-sections 
  -fno-builtin -nostdlib
  -Wno-pedantic
)

message(STATUS "- MCU_FLAGS = ${MCU_FLAGS}")