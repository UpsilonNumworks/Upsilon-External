API=../../api
CC=arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib
OBJCOPY=arm-none-eabi-objcopy
AR=arm-none-eabi-ar
CFLAGS=-DNDEBUG -ggdb3 -I$(API) -Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions
LDFLAGS=-Wl,-L$(API) -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nosys.specs -nostartfiles -Wl,-Ur -lapi

MINIZ_PATH = src/miniz
CPPFLAGS += -I$(MINIZ_PATH)
CFLAGS += -I$(MINIZ_PATH)
CFLAGS += -DSPNG_USE_MINIZ


%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $<
	@echo "Compiled $<"

app.elf: main.o ./src/libspng/spng.c ./src/libspng/spng.h src/miniz/miniz.c src/miniz/miniz.h
	@echo "Building app.elf"
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -lm
	@echo "Built app.elf"

clean:
	@echo "Cleaning up"
	rm -f *.elf *.o
	rm -rf output
	@echo "Cleaned up"