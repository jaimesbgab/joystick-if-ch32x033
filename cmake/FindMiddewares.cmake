# Define the root middlewares path
set(MIDDLEWARES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party")

file(GLOB_RECURSE LITTLEFS_SRC
  "${MIDDLEWARES_DIR}/littlefs/*.c"
)

file(GLOB_RECURSE LITTLEFS_INC
  "${MIDDLEWARES_DIR}/littlefs/*.h"
)

set (LITTLEFS_INC_DIRS
  "${MIDDLEWARES_DIR}/littlefs"
)

set(THIRD_PARTY_SRC
  ${LITTLEFS_SRC}
)

set(THIRD_PARTY_INC
  ${LITTLEFS_INC}
)

set(THIRD_PARTY_INC_DIRS
  ${LITTLEFS_INC_DIRS}
)