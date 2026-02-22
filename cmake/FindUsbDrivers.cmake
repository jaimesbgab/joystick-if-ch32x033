# Define the root middlewares path
set(USB_DRIVERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/usb")

file(GLOB_RECURSE USB_DRIVERS_SRC
  "${USB_DRIVERS_DIR}/src/*.c"
)

file(GLOB_RECURSE DRIVERS_INC
  "${USB_DRIVERS_DIR}/inc/*.h"
)

set (DRIVERS_INC_DIRS
  "${USB_DRIVERS_DIR}/inc"
)
