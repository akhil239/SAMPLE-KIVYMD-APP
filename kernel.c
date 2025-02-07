#include <stdint.h>

#define VIDEO_MEMORY (char *)0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

char *vidmem = VIDEO_MEMORY;
int cursor_x = 0, cursor_y = 0;

void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i + 1] = 0x07;
    }
    cursor_x = cursor_y = 0;
}

void print(const char *str) {
    while (*str) {
        if (*str == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else {
            int pos = (cursor_y * SCREEN_WIDTH + cursor_x) * 2;
            vidmem[pos] = *str;
            vidmem[pos + 1] = 0x07;
            cursor_x++;
        }
        if (cursor_x >= SCREEN_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
        str++;
    }
}

void kernel_main() {
    clear_screen();
    print("64-bit OS Loaded Successfully!\n");
    while (1);
}
