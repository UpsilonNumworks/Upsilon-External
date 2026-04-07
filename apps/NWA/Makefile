API=../../api
EADK=eadk
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
AR=arm-none-eabi-ar
CFLAGS=-DNDEBUG -ggdb3 -I$(API) -Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions -ffreestanding
# LDFLAGS=-Wl,-L$(API) -Wl,-L$(EADK) -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nosys.specs -nostartfiles -Wl,-Ur -lapi -leadk -Wl,--allow-multiple-definition
LDFLAGS=-Wl,-L$(API) -Wl,-L$(EADK) -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nosys.specs -nostartfiles -Wl,-Ur -lapi -leadk

%.o: %.c
	$(CC) $(CFLAGS) -c $<

app.elf: app.nwa $(EADK)/libeadk.a
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	make -C $(EADK) clean

$(EADK)/libeadk.a:
	make -C $(EADK)

clean:
	rm -f *.elf *.o
