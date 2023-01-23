CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
AR=arm-none-eabi-ar
CFLAGS=-Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions

TARGETS=extapp_api.o extapp_startup.o drawing.o

libapi.a: $(TARGETS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.a
