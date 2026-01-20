API=../../api
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
AR=arm-none-eabi-ar
CFLAGS=-DNDEBUG -ggdb3 -I$(API) -Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions
LDFLAGS=-Wl,-L$(API) -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nosys.specs -nostartfiles -Wl,-Ur -lapi

%.o: %.c
	$(CC) $(CFLAGS) -c $<

ifeq ($(OS),Windows_NT)
# Windows Version
all: app.elf app.icon

$(API)/libapi.a:
	$(MAKE) -C $(API)

app.elf: main.o board.o moves.o minimax.o ui.o $(API)/libapi.a
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

app.icon: icon.png
	python ../../tool/png2icon.py $< $@

clean:
	rm -f *.elf *.o *.icon

else
# Linux Version
app.elf: main.o board.o moves.o minimax.o ui.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.elf *.o
endif
