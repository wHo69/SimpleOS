/*
Notes: 
1. Conversion from pos to actual index: pos*2
   - Pos starts from 0
2. index: character data, index + 1: color data
*/

#include "ports.h"
#include "screen.h"

#include "../libc/mem.h"
#include "../libc/string.h"

/* Function Decleration */
void move_cursor(int pos);
int getPos(int row, int col);
int getIndex(int pos);
int get_cursor();

/* Public Functions */

// Clears the screen
// Usage: clear_screen()
void clear_screen() {
    // LETS GO IT WORKS :OOOOO
    unsigned char* v_mem = (unsigned char*) VIDEO_MEMORY;

    // Loop thru every row and col and set it to ' '
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            int index = getIndex(getPos(row, col));

            v_mem[index] = ' ';
            v_mem[++index] = BLACK_ON_WHITE;
        }
    }

    // Move cursor to start of screen
    move_cursor(0);
}

// Prints a char at current cursor location
// Usage: tprint_char(char)
void tprint_char(char c) {
    // Calculate pos and index
    int pos = get_cursor();
    int index = getIndex(pos);

    // Get vidmem
    unsigned char* v_mem = (unsigned char*) VIDEO_MEMORY;

    switch (c) {
        // Handle \n
        case '\n':
            // Move down 1 row
            pos += MAX_COLS;
            // Move to start of row
            pos -= (pos % MAX_COLS);

            break;

        // Handle backspace
        case 0x08:
            v_mem[index-2] = ' ';
            v_mem[index-1] = BLACK_ON_WHITE;

            // If cursor is not at start of screen, move pos back by 1
            if (pos > 0) {
                pos--;
            }

            // Set cursor
            move_cursor(pos);

            break;
    
        default:
            // Write to vid mem
            v_mem[index] = c;
            v_mem[++index] = BLACK_ON_WHITE;

            pos++;
            break;
    }

    // Scroll screen down
    if (pos >= MAX_ROWS * MAX_COLS) {
        // Move lines 1 up
        for (int i = 1; i < MAX_ROWS; i++) {
            memory_copy((char*) getIndex(getPos(i, 0)) + VIDEO_MEMORY, (char*) getIndex(getPos(i - 1, 0)) + VIDEO_MEMORY, MAX_COLS * 2);
        }

        // Blank last line
        for (int col = 0; col < MAX_COLS; col++) {
            index = getIndex(getPos(MAX_ROWS - 1, col));
            v_mem[index] = ' ';
            v_mem[++index] = BLACK_ON_WHITE;
        }

        // Move cursor to start of last line
        move_cursor(getPos(MAX_ROWS - 1, 0));
        return;
    }

    // Update cursor position
    move_cursor(pos);
}

// Prints a backspace
void tprint_backspace() {
    tprint_char(0x08);  
}

// Prints a string at current cursor location
void tprint(char* msg) {
    int i = 0;
    while (msg[i] != '\0') {
        tprint_char(msg[i++]);
    }
}

/* Private Functions */
// Moves cursor to specified pos
void move_cursor(int pos) {
    outb(CMD_PORT, HIGH_BYTE);
    outb(DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(CMD_PORT, LOW_BYTE);
    outb(DATA_PORT, pos & 0x00FF);
}

// Returns the pos of cursor
int get_cursor() {
    outb(CMD_PORT, HIGH_BYTE);
    int pos = inb(DATA_PORT) << 8;
    outb(CMD_PORT, LOW_BYTE);
    pos += inb(DATA_PORT);

    return pos;
}

// Gets the raw memory index from pos
int getIndex(int pos) {
    return pos * 2;
}

// Gets the pos value from a given row and col
int getPos(int row, int col) {
    return (row * MAX_COLS) + col;
}