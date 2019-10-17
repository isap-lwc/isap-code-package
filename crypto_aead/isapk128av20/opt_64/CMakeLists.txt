cmake_minimum_required (VERSION 3.3.2)
project (ISAP)

########################
#   COMPILE COMMANDS   #
########################

#set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2")
set(CMAKE_C_FLAGS "-O3 -funroll-all-loops -march=native -mtune=native")

add_library(
  CRYPTO_AEAD
  STATIC
  crypto_aead.c
  Isap.c
)
set_target_properties(CRYPTO_AEAD PROPERTIES OUTPUT_NAME "isap")

add_executable(EXE_GENKAT_AEAD genkat_aead.c)
target_link_libraries(EXE_GENKAT_AEAD CRYPTO_AEAD)
set_target_properties(EXE_GENKAT_AEAD PROPERTIES OUTPUT_NAME "genkat_aead" SUFFIX "")
