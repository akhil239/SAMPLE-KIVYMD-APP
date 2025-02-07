#include <stdint.h>
#include <stddef.h>

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
        if (cursor_y >= SCREEN_HEIGHT) {
            clear_screen();
        }
        str++;
    }
}

void newline() {
    cursor_x = 0;
    cursor_y++;
}

void input(char *buffer, int size) {
    int i = 0;
    while (i < size - 1) {
        char ch;
        asm volatile("int $0x16" : "=a"(ch) : "a"(0));
        
        if (ch == 13) break;  // Enter key
        if (ch == 8 && i > 0) {  // Backspace
            i--;
            cursor_x--;
            vidmem[(cursor_y * SCREEN_WIDTH + cursor_x) * 2] = ' ';
        } else {
            buffer[i++] = ch;
            vidmem[(cursor_y * SCREEN_WIDTH + cursor_x) * 2] = ch;
            cursor_x++;
        }
    }
    buffer[i] = '\0';
    newline();
}

void reboot() {
    asm volatile ("outb %0, %1" : : "a"(0xFE), "Nd"(0x64));
}

void cli() {
    char buffer[100];
    print("CLI OS Ready. Type 'help' for commands.\n");

    while (1) {
        print("OS> ");
        input(buffer, sizeof(buffer));

        if (strcmp(buffer, "help") == 0) {
            print("Commands: help, echo <msg>, cls, version, shutdown, reboot");
        } else if (strncmp(buffer, "echo ", 5) == 0) {
            print(buffer + 5);
        } else if (strcmp(buffer, "cls") == 0) {
            clear_screen();
        } else if (strcmp(buffer, "version") == 0) {
            print("CLI OS v0.1");
        } else if (strcmp(buffer, "reboot") == 0) {
            print("Rebooting...");
            reboot();
        } else if (strcmp(buffer, "shutdown") == 0) {
            print("Shutting down...");
            asm volatile("hlt");
        } else {
            print("Invalid command.");
        }
        newline();
    }
}

void kernel_main() {
    clear_screen();
    cli();
}
