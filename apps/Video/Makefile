JPEG=./libjpeg-turbo
API=../../api
CC=arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib
OBJCOPY=arm-none-eabi-objcopy
AR=arm-none-eabi-ar
CFLAGS=-DNDEBUG -ggdb3 -I$(API) -I$(JPEG)/src -I. -Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions
LDFLAGS=-Wl,-L$(API) -Wl,-L$(JPEG)/build -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nosys.specs -nostartfiles -Wl,-Ur -lapi -ljpeg

OBJS = main.o peripherals.o selector.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

app.elf: $(OBJS) ${JPEG}/build/libjpeg.a
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -lm

${JPEG}/build/libjpeg.a:
	cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -S ${JPEG} -B ${JPEG}/build
	make -C ${JPEG}/build jpeg-static

clean:
	rm -f *.elf *.o
	rm -rf output
