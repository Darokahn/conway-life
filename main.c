#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "sdl_wrapper.h"
#include <signal.h>

#define ever ;;
#define SCREEN_SIZE 900

#define PIXEL_SIZE 30

#if PIXEL_SIZE > 1
void DRAW_CELL(struct screenPackage screen, int x, int y, color c) {
    x -= 1;
    y -= 1;
    x *= PIXEL_SIZE;
    y *= PIXEL_SIZE;
    drawRect(screen, (rectangle) {x, y, PIXEL_SIZE, PIXEL_SIZE}, c);
}
#else
#define DRAW_CELL(screen, x-1, y-1, color) setPixel(screen, (point) {x, y}, color)
#endif

#define BOARD_SIZE (SCREEN_SIZE / PIXEL_SIZE)

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
    for (y = 1; y < BOARD_SIZE + 1; y++) {
        for (x = 1; x < BOARD_SIZE + 1; x++) {
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
    for (y = 1; y < BOARD_SIZE + 1; y++) {
        for (x = 1; x < BOARD_SIZE + 1; x++) {
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
    
    // Update board based on neighbor count
    // Can be modeled with each cell as a latch with a data line and a latch signal.
    // 2 live neighbors means don't latch (keep state), 3 neighbors means set data line to 1.

    for (x = 1; x < BOARD_SIZE + 1; x++) {
        for (y = 1; y < BOARD_SIZE + 1; y++) {
            count = neighborCounts[x][y];
            dataLine = count == 3;
            latch = count != 2;
            if (latch) {
                cells[x][y] = dataLine;
            }
            neighborCounts[x][y] = 0;
            DRAW_CELL(screen, x, y, ((color) {255 * cells[x][y], 255 * cells[x][y], 255 * cells[x][y], 255}));
        }
    }
}

int main(int argc, char** argv) {
    signal(SIGINT, exit);
    struct screenPackage screen = initVideo(SCREEN_SIZE, SCREEN_SIZE);
    int x;
    int y;
    for (x = 0; x < BOARD_SIZE + 2; x++) {
        for (y = 0; y < BOARD_SIZE + 1; y++) {
            cells[x][y] = 0;
            neighborCounts[x][y] = 0;
        }
    }
    SDL_Event e;
    int placing;
    int thisCell;
placing:
    placing = 1;
    updateScreen(screen);
    while (placing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                x = e.button.x / PIXEL_SIZE;
                y = e.button.y / PIXEL_SIZE;
                x++;
                y++;
                thisCell = !cells[x][y];
                cells[x][y] = thisCell;
                DRAW_CELL(screen, x, y, ((color) {255 * thisCell, 255 * thisCell, 255 * thisCell, 255}));
                updateScreen(screen);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_RETURN || e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    placing = 0;
                }
            }
            else if (e.type == SDL_QUIT) {
                exit(0);
            }
        }
    }
    for(ever) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_RETURN || e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    goto placing; // sorry
                }
            }
        }
        updateScreen(screen);
        updateBoard(cells, neighborCounts, screen);
        sleepMillis(100);
    }
}
