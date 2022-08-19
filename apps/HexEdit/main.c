#include <extapp_api.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SMALLTEXT_WIDTH 7
#define SMALLTEXT_HEIGHT 14
#define NB_ROWS 16

enum InputMode {
    Value,
    Goto,
} ;

static uint32_t s_addrStart = 0x00200000;
static int s_activeRow;
static uint32_t s_inputBuffer;
static enum InputMode s_inputMode;

void byte2hex(uint8_t byte, char *hex) {
	for (int i = 0; i < 2; i++) {
		uint8_t b = (byte & 0xF0) >> 4;

		if (b < 10)
			hex[i] = '0' + b;
		else
			hex[i] = 'a' + b - 10;

		byte <<= 4;
	}
}

void word2hex(uint32_t word, char *hex)
{
	byte2hex((word >> 24) & 0xFF, hex);
	byte2hex((word >> 16) & 0xFF, hex+2);
	byte2hex((word >> 8 ) & 0xFF, hex+4);
	byte2hex((word      ) & 0xFF, hex+6);
}

void draw_bitfield(uint32_t value, int x, int y)
{
    for (int i = 0; i < 32; i++) {
        bool on = (value & (1 << (31-i))) != 0;
        extapp_pushRectUniform(x+i*4+(i/4+i/8+i/16)*2, y+1, 2, SMALLTEXT_HEIGHT-2, on ? 0xF700 : 0x0000);
    }
}

void draw_dump()
{
    for (int row = 0; row < NB_ROWS; row++) {
        uint32_t value = *((uint32_t*)(s_addrStart + 4*row));
        char buf[9];
        buf[8] = 0;
        word2hex(value, buf);
        extapp_drawTextSmall(buf, 13*SMALLTEXT_WIDTH, row*SMALLTEXT_HEIGHT, 0, 0xFFFF, false);

        draw_bitfield(value, 22*SMALLTEXT_WIDTH, row*SMALLTEXT_HEIGHT);
    }
}

void draw_status_bar()
{
    const char* mode;
    switch (s_inputMode) {
        case Value:
            mode = "V";
            break;
        case Goto:
            mode = "G";
            break;
    }

    extapp_drawTextSmall(mode, 0, LCD_HEIGHT-SMALLTEXT_HEIGHT, 0xFFFF, 0, false);

    for (int i = 0; i < 8; i++) {
        int digit = (s_inputBuffer >> (4*(7-i))) & 0xF;
        char buf[2];
        buf[0] = digit < 10 ? '0'+digit : 'A'+digit-10;
        buf[1] = 0;
        extapp_drawTextSmall(buf, SMALLTEXT_WIDTH*(2+i), LCD_HEIGHT-SMALLTEXT_HEIGHT, 0x0, 0xFFFF, false);
    }
    draw_bitfield(s_inputBuffer, 22*SMALLTEXT_WIDTH, LCD_HEIGHT-SMALLTEXT_HEIGHT);
}

void draw_ui()
{
    for (int row = 0; row < NB_ROWS; row++) {
        uint32_t value = s_addrStart + 4*row;
        bool active = s_activeRow == row;
        char buf[12];
        buf[0] = '0';
        buf[1] = 'x';
        buf[10] = ':';
        buf[11] = 0;
        word2hex(value, buf+2);
        extapp_drawTextSmall(buf, 0, row*SMALLTEXT_HEIGHT, active ? 0 : 0xFFFF, active ? 0xFFFF : 0, false);
    }

    draw_status_bar();
}

void handle_enter()
{
    switch (s_inputMode) {
        case Value:
            *(uint32_t*)s_addrStart = s_inputBuffer;
            s_inputBuffer = 0;
            break;
        case Goto:
            s_addrStart = s_inputBuffer & 0xFFFFFFFC;
            s_inputBuffer = 0;
            draw_ui();
            draw_dump();
            break;
    }
}

void handle_scroll(int delta) {
    if (s_activeRow + delta < 0) {
        s_addrStart -= 4;
        draw_dump();
    }
    else if (s_activeRow + delta >= NB_ROWS) {
        s_addrStart += 4;
        draw_dump();
    }
    else {
        s_activeRow += delta;
    }
    draw_ui();
}

void extapp_main(int argc, char * argv[])
{
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, 0xFFFF);
    draw_ui();
    draw_dump();
    uint64_t oldkey = 0;
    while (true) {
        uint64_t key = extapp_scanKeyboard();
        uint64_t pressed = (~oldkey) & key;
        switch (pressed) {
            case SCANCODE_Zero:
                s_inputBuffer = (s_inputBuffer << 4) + 0;
                break;
            case SCANCODE_One:
                s_inputBuffer = (s_inputBuffer << 4) + 1;
                break;
            case SCANCODE_Two:
                s_inputBuffer = (s_inputBuffer << 4) + 2;
                break;
            case SCANCODE_Three:
                s_inputBuffer = (s_inputBuffer << 4) + 3;
                break;
            case SCANCODE_Four:
                s_inputBuffer = (s_inputBuffer << 4) + 4;
                break;
            case SCANCODE_Five:
                s_inputBuffer = (s_inputBuffer << 4) + 5;
                break;
            case SCANCODE_Six:
                s_inputBuffer = (s_inputBuffer << 4) + 6;
                break;
            case SCANCODE_Seven:
                s_inputBuffer = (s_inputBuffer << 4) + 7;
                break;
            case SCANCODE_Eight:
                s_inputBuffer = (s_inputBuffer << 4) + 8;
                break;
            case SCANCODE_Nine:
                s_inputBuffer = (s_inputBuffer << 4) + 9;
                break;
            case SCANCODE_Exp:
                s_inputBuffer = (s_inputBuffer << 4) + 0xA;
                break;
            case SCANCODE_Ln:
                s_inputBuffer = (s_inputBuffer << 4) + 0xB;
                break;
            case SCANCODE_Log:
                s_inputBuffer = (s_inputBuffer << 4) + 0xC;
                break;
            case SCANCODE_Imaginary:
                s_inputBuffer = (s_inputBuffer << 4) + 0xD;
                break;
            case SCANCODE_Comma:
                s_inputBuffer = (s_inputBuffer << 4) + 0xE;
                break;
            case SCANCODE_Power:
                s_inputBuffer = (s_inputBuffer << 4) + 0xF;
                break;
            case SCANCODE_Backspace:
                s_inputBuffer = s_inputBuffer >> 4;
                break;
            case SCANCODE_Sine:
                s_inputMode = Goto;
                break;
            case SCANCODE_Cosine:
                s_inputMode = Value;
                break;
            case SCANCODE_Square:
                draw_dump();
                break;
            case SCANCODE_EXE:
                handle_enter();
                break;
            case SCANCODE_Up:
                handle_scroll(-1);
                break;
            case SCANCODE_Down:
                handle_scroll(1);
                break;
            case SCANCODE_Home:
                return;
        }
        oldkey = key;
        extapp_waitForVBlank();
        draw_status_bar();
    }
}
