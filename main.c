#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "sdl_wrapper.h"

#define ever ;;
#define BOARD_SIZE 256

void sleepMillis(int millis) {
    struct timespec t;
    t.tv_sec = millis / 1000;
    t.tv_nsec = (millis % 1000) * 1000000;
    nanosleep(&t, NULL);
}

uint8_t cells[BOARD_SIZE+2][BOARD_SIZE+2];
uint8_t neighborCounts[BOARD_SIZE+2][BOARD_SIZE+2]; // enable buffer 0 space outside board to make out-of-bounds checks unnecessary

void printNeighbors() {
    int x;
    int y;
    for (x = 1; x < BOARD_SIZE + 1; x++) {
        for (y = 1; y < BOARD_SIZE + 1; y++) {
            putchar(neighborCounts[x][y] + '0');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

void printCells() {
    int x;
    int y;
    for (x = 1; x < BOARD_SIZE + 1; x++) {
        for (y = 1; y < BOARD_SIZE + 1; y++) {
            putchar(cells[x][y] + '0');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

void updateBoard(uint8_t cells[BOARD_SIZE+2][BOARD_SIZE+2], uint8_t neighborCounts[BOARD_SIZE+2][BOARD_SIZE+2], struct screenPackage screen) {
    int x;
    int y;
    int occupied;
    int dataLine;
    int latch;
    int count;
    // sum neighbors
    for (x = 1; x < BOARD_SIZE + 1; x++) {
        for (y = 1; y < BOARD_SIZE + 1; y++) {
            occupied = cells[x][y];
            // use unrolled for loop to avoid branching in the center case (don't count self as neighbor)
            neighborCounts[x-1][y-1] += occupied;
            neighborCounts[x][y-1] += occupied;
            neighborCounts[x+1][y-1] += occupied;
            neighborCounts[x-1][y] += occupied;
            // neighborCounts[x][y]++; This is the case we skip
            neighborCounts[x+1][y] += occupied;
            neighborCounts[x-1][y+1] += occupied;
            neighborCounts[x][y+1] += occupied;
            neighborCounts[x+1][y+1] += occupied;
        }
    }

    // update board based on neighbor count

    for (x = 1; x < BOARD_SIZE + 1; x++) {
        for (y = 1; y < BOARD_SIZE + 1; y++) {
            count = neighborCounts[x][y];
            dataLine = count == 3;
            latch = count != 2;
            // only branch
            if (latch) {
                cells[x][y] = dataLine;
            }
            neighborCounts[x][y] = 0;
            setPixel(screen, (point) {x, y}, (color) {255 * cells[x][y], 255 * cells[x][y], 255 * cells[x][y]});
        }
    }
}

int main() {
    struct screenPackage screen = initVideo(BOARD_SIZE, BOARD_SIZE);
    int x;
    int y;
    for (x = 0; x < BOARD_SIZE + 2; x++) {
        for (y = 0; y < BOARD_SIZE + 2; y++) {
            cells[x][y] = 0;
            neighborCounts[x][y] = 0;
        }
    }
    cells[5][5] = 1;
    cells[5][6] = 1;
    cells[3][6] = 1;
    cells[5][7] = 1;
    cells[4][7] = 1;
    for(ever) {
        updateBoard(cells, neighborCounts, screen);
        updateScreen(screen);
        // sleepMillis(100);
    }
}
