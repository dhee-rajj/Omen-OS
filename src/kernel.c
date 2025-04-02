#include "kernel.h"
#include "stdint.h"
#include "stddef.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;


int strlen(const char* str) {
    int len = 0;
    while(*str) {
        len++;
        str++;
    }
    return len;

}

uint16_t terminal_makechar(char c, char color) {
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_makechar(c, color);
}

void terminal_writechar(char c, char color) {
    if (c == '\n') {
        terminal_row++;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    
    if (terminal_row >= VGA_WIDTH) {
        terminal_row++;
        terminal_col = 0;
    }
}

void terminal_intialize() {
    video_mem = (uint16_t*) (0xB8000);
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void print(const char* str) {
    size_t len = strlen(str);
    for (int i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }
}

void kernel_main() {
    terminal_intialize();
    print("HI, WELCOME TO OMEN OS");
}