#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct {
    int x;
    int y;
} point;

struct screenPackage {
    SDL_Window *window;
    SDL_Renderer *renderer;
    uint16_t width;
    uint16_t height;
};

typedef struct {
    char r;
    char g;
    char b;
    char a;
} color;

typedef struct {
    int radius;
    point center;
} circle;

typedef SDL_Rect rectangle;

typedef struct {
    point p1;
    point p2;
} segment;

typedef SDL_Event event;

struct screenPackage initVideo(int width, int height);
void destroyVideo(struct screenPackage screen);

void setPixel(struct screenPackage screen, point p, color c);
void updateScreen(struct screenPackage screen);
void clearScreen(struct screenPackage screen, color c);
void drawCircle(struct screenPackage screen, circle cir, color c);
void drawRect(struct screenPackage screen, rectangle r, color c);
void drawSegment(struct screenPackage screen, segment s, color c);
void drawPolygon(struct screenPackage screen, point* points, color c, int pointCount);

#define pollEvent SDL_PollEvent

#endif
