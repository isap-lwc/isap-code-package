
LDSCRIPT   = ../libopencm3/lib/stm32/f4/stm32f405x6.ld
LIBNAME    = opencm3_stm32f4
ARCH_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
DEFINES    = -DSTM32F4 -DISAP128 -DKECCAKP400
OBJS	   = stm32f4_wrapper.o crypto_aead.o Isap.o KeccakP-400-armv7m-le-gcc.s

include ./make.mk
