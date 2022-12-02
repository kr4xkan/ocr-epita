#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdlib.h>

#include "../utils.h"
#include "crop-manager.h"

#include <string.h>

#define pi 3.1415926535


Intersection* DetectIntersections(SDL_Surface *surface, unsigned int *normalSpace, size_t *length){
    int w = surface->w, h = surface->h;

    size_t len = 200;
    *length = len;
    Intersection *coords = calloc(len*len, sizeof(Intersection));
    size_t nbIntersection = 0;

    //Fill the arrays with the intersections
    int arrayX = 0, arrayY = -1;
    unsigned int x = w + 1;
    unsigned int y = 0;
    for (int i = 0; i < w * h; i++) {
        if (normalSpace[i] >= 2) {
            unsigned int xi = i % w;
            if (xi < x) {
                arrayY++;
                arrayX = 0;
            } else {
                arrayX++;
            }
            x = xi;
            coords[arrayY*len + arrayX].x = x;
            coords[arrayY*len + arrayX].y = y;
            nbIntersection++;
        }
        if (i % w == 0)
            y++;
    }
    if (TryCrop(surface, len, coords, nbIntersection))
        return;


    // Remove the Intersections that are not on white points
    x = 0, y = 0;
    while (coords[y*len].x){
        while (coords[y*len + x].x) {
            Intersection current = coords[y*len + x];
    
            char count = 0;
            Uint8 r, g, b;
            for (int i = -3; i <= 3; i++){
                for (int j = -3; j <= 3; j++){
                    GetPixelColor(surface, current.x+i, current.y+j, &r, &g, &b);
                    if ((r+g+b)/3 >= 200){
                        count++;
                    }
                }
            }
            if (count < 6){
                normalSpace[current.y*w + current.x] = 0;
                nbIntersection--;
            } 
            x++;
        }
        y++;
        x = 1;
    }
    TryCrop(surface, len, coords, nbIntersection);

    free(coords);
}

SDL_Surface *CropSurface(SDL_Surface *surface, Intersection current, int width,
                         int height){
    SDL_Surface *newSurface = SDL_CreateRGBSurface(
        surface->flags, width, height, surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask, surface->format->Bmask,
        surface->format->Amask);

    SDL_Rect rect = {current.x, current.y, width, height};
    SDL_BlitSurface(surface, &rect, newSurface, NULL);
    return newSurface;
}


char TryCrop(SDL_Surface *surface, size_t len, Intersection *coords,
                                                    size_t nbIntersection){

    if (nbIntersection != 100){
        printf("Couldn't crop\n");
        return 0;
    }
    
    size_t x = 1, y = 1;
    while (coords[y*len].x){
        while (coords[y*len + x].x) {
            Intersection current = coords[y*len + x];

            unsigned int squareWidth = current.x - coords[y*len + x-1].x;
            unsigned int squareHeight = current.y - coords[(y-1)*len + x].y;

            SDL_Surface *square = CropSurface(surface, current, squareWidth,
                    squareHeight);

            char name[] = {x+'0', '-', y+'0', '.', 'p', 'n', 'g', '\0'};

            char *newStr = malloc((strlen(name) + 15) * sizeof(char));
            strcpy(newStr, "squares/");
            strcat(newStr, name);
            IMG_SavePNG(square, newStr);
            SDL_FreeSurface(square);
            free(newStr);

            x++;
        }
        y++;
        x = 1;
    }
    printf("All squares have been cropped\n");
    return 1;

}



size_t Ouaip(SDL_Surface *surface, Intersection *coords, size_t len){
    unsigned int *histo = calloc(surface->w, sizeof(unsigned int));
    size_t x = 1, y = 1;
    while (coords[y*len].x){
        while (coords[y*len + x].x) {
            Intersection current = coords[y*len + x];

            size_t squareWidth = current.x - coords[y*len + x-1].x;
            size_t squareHeight = current.y - coords[(y-1)*len + x].y;
            histo[squareWidth]++;
            histo[squareHeight]++;
            x++;
        }
        y++;
        x = 1;
    }

    size_t current = 0;
    size_t range = 3;
    for (size_t i = 0; i <= range*2; i++)
        current += histo[i];
    
    size_t gap = range;
    size_t max = current;
    for (size_t i = range+2; i < surface->w-range; i++){
        current -= histo[i-range-1];
        current += histo[i+range];
        if (current > max){
            gap = i;
            max = current;
        }
    }
    free(histo);
    
    return gap;
}



char IsValid(Intersection *coords, size_t len, size_t x, size_t y, size_t gap){
    /**
     * Checking if every neighbour are at the right distance
     **/
    size_t delta = 10;


    char vertical = 0;
    char horizontal = 0;

    Intersection current = coords[y*len + x];
    //Left and Top
    if (x != 0){
        Intersection left = coords[y*len + x-1];
        if (left.x != 0){
            size_t dx = (left.x - current.x) % gap; 
            if (dx <= delta || dx >= gap-delta)
                return 1;
        }
    } 
    else 
        horizontal = 1;

    if (y != 0){
        Intersection top = coords[(y-1)*len + x];
        if (top.x != 0){
            size_t dy = (top.y - current.y) % gap; 
            if (dy <= delta || dy >= gap-delta)
                return 1;
        }
    } 
    else 
        vertical = 1;

    //Right and Bottom
    Intersection right = coords[y*len + x+1];
    if (right.x != 0){
        size_t dx = (right.x - current.x) % gap; 
        if (dx <= delta || dx >= gap-delta)
            return 1;
    }
    else 
        horizontal = 1;

    Intersection bottom = coords[(y+1)*len + x];
    if (bottom.x != 0){
        size_t dy = (right.y - current.y) % gap; 
        if (dy <= delta || dy >= gap-delta)
            return 1;
    }
    else 
        vertical = 1;


    return vertical == horizontal;
}
