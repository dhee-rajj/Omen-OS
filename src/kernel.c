#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "disk/streamer.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;


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

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main() {
    terminal_intialize();
    print("HI, WELCOME TO OMEN OS\n");

    //Intialize heap
    kheap_init();

    //Intialize the Interrupt descriptor Table
    idt_init();

    //Search and initialize the disk
    disk_search_and_init();

    //setup paging
    kernel_chunk = paging_new_4gb_chunk(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    //switch to kernel paging chunk 
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // enable paging
    enable_paging();

    //Enable System Interrupts
    enable_interrupts();
}