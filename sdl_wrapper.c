#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <math.h>

struct screenPackage initVideo(int width, int height) {
    struct {int width; int height;} screenSize = {width, height};
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        perror( "SDL failed" );
        exit(1);
    }
    struct screenPackage screen;
    screen.width = screenSize.width;
    screen.height = screenSize.height;

    screen.window = SDL_CreateWindow( "Video", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenSize.width, screenSize.height, SDL_WINDOW_SHOWN );
    if ( screen.window == NULL ) {
        printf("%s\n", SDL_GetError());
    }
    screen.renderer = SDL_CreateRenderer(screen.window, -1, SDL_RENDERER_ACCELERATED);
    if (screen.renderer == 0) {
        printf("%s\n", SDL_GetError());
    }
    return screen;
}

void destroyVideo(struct screenPackage screen) {
    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    SDL_Quit();
}

void updateScreen(struct screenPackage screen) {
    SDL_RenderPresent(screen.renderer);
}

void clearScreen(struct screenPackage screen, color c) {
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(screen.renderer);
}

void setPixel(struct screenPackage screen, point p, color c) {
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(screen.renderer, p.x, p.y);
}

void drawCircle(struct screenPackage screen, circle circ, color c) {
    int maxArea = (circ.radius * circ.radius * 4) ; // area of the rectangle this circle is inscribed inside; Avoiding float multiplication and any potential buffer overflow with imprecise circle area calculations.
    SDL_Point* points = malloc(maxArea * sizeof(SDL_Point));
    int frontPointer = 0;
    // fill in semicircle and mirror
    for (int x = -circ.radius; x < circ.radius + 1; x++) {
        for (int y = 0; y * y < (circ.radius * circ.radius - (x * x)); y++) {
            points[frontPointer++] = (SDL_Point) {x + circ.center.x, y + circ.center.y};
            points[frontPointer++] = (SDL_Point) {x + circ.center.x, -y + circ.center.y};
        }
    }
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoints(screen.renderer, points, frontPointer);
    free(points);
}

void drawRect(struct screenPackage screen, rectangle rect, color c) {
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawRect(screen.renderer, &rect);
}
void drawSegment(struct screenPackage screen, segment s, color c) {
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(screen.renderer, s.p1.x, s.p1.y, s.p2.x, s.p2.y);
}

void drawPolygon(struct screenPackage screen, point* points, color c, int pointCount) {
    point* finalPoints = malloc(sizeof(point) * pointCount + 1);
    memcpy(finalPoints, points, pointCount * sizeof(point));
    finalPoints[pointCount] = finalPoints[0];
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLines(screen.renderer, (SDL_Point*)finalPoints, pointCount + 1);
}
