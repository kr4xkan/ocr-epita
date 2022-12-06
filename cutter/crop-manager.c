#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdlib.h>

#include "../utils.h"
#include "crop-manager.h"
#include "cutter.h"

#include <string.h>

#define pi 3.1415926535


Intersection* FindIntersections(SDL_Surface *surface, unsigned int *normalSpace, size_t vertLen, size_t horiLen){
    unsigned int w = surface->w, h = surface->h;

    size_t len = vertLen*horiLen;
    Intersection *coords = calloc(len, sizeof(Intersection));

    //Fill the arrays with the intersections
    size_t j = 0;
    unsigned int x = 0, y = 0;
    for (size_t i = 0; i < w*h; i++){
        if (normalSpace[i] >= 2 && !DontAdd(coords, x, y, j)){
            coords[j].x = x;
            coords[j].y = y;
            j++;
        }
        x++;
        if (x == w){
            x = 0;
            y++;
        }
    }


    size_t start = 0;
    for (size_t i = 1; i < len; i++){
        size_t j = i;
        if (i % vertLen == 0){
            start = i;
        }
        while (j > start && coords[j-1].x >  coords[j].x){
            Intersection tmp = coords[j-1];
            coords[j-1] = coords[j];
            coords[j] = tmp;
            j--;
        }

    }

    vertLen = vertLen > 10 ? 10 : vertLen;
    horiLen = horiLen > 10 ? 10 : horiLen;


    printf("top-left:      (%u, %u)\n", coords[0].x, coords[0].y);
    printf("top-right:     (%u, %u)\n", coords[vertLen-1].x, coords[vertLen-1].y);
    printf("bottom-left:   (%u, %u)\n", coords[vertLen*horiLen-vertLen].x, coords[vertLen*horiLen-vertLen].y);
    printf("bottom-right:  (%u, %u)\n", coords[vertLen*horiLen-1].x, coords[vertLen*horiLen-1].y);

    return coords;
}


int DontAdd(Intersection *coords, size_t x, size_t y, size_t nbIntersections) {
    /**
     * Check if a line is similar to an already existing one
     * Return 1 if the line already exist, 2 if a bigger one is found, 0 else
     */

    for (size_t i = 0; i < nbIntersections; i++) {
        Intersection iCoord = coords[i];

        int dx = abs((int)(iCoord.x) - (int)x);
        int dy = abs((int)(iCoord.y) - (int)y);

        if (dx <= 30 && dy <= 30) {
            return 1;
        }
    }
    return 0;
}




void CropSquares(SDL_Surface *surface, Intersection *coords, size_t vertLen, size_t horiLen){
    for (size_t y = 0; y < horiLen-1; y++){
        for (size_t x = 0; x < vertLen-1; x++){
            Intersection current = coords[y*vertLen + x];

            unsigned int squareWidth = coords[y*vertLen + x+1].x - current.x;
            unsigned int squareHeight = coords[(y+1)*vertLen + x].y - current.y;

            SDL_Surface *square = CropSurface(surface, current, squareWidth,
                    squareHeight);


            char name[] = {x+'1', '-', y+'1', '.', 'p', 'n', 'g', '\0'};
            char *newStr = malloc((strlen(name) + 15) * sizeof(char));
            strcpy(newStr, "squares/");
            strcat(newStr, name);
            IMG_SavePNG(square, newStr);
            SDL_FreeSurface(square);
            free(newStr);
        }
    }
    printf("All squares have been cropped\n");
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


void ManualCrop(SDL_Surface *surface, Intersection topLeft, Intersection topRight, Intersection bottomLeft, Intersection bottomRight){
    Intersection *coords = malloc(100*sizeof(Intersection));

    Intersection *leftSide = FindPoints(topLeft, bottomLeft);
    Intersection *rightSide = FindPoints(topRight, bottomRight);

    for (size_t i = 0; i < 10; i++){
        Intersection *line = FindPoints(leftSide[i], rightSide[i]);
        printf("%lu x:%u  y:%u\n", i, leftSide[i].x, leftSide[i].y);
        for (size_t j = 0; j < 10; j++){
            coords[i*10+j] = line[j];
        }
        free(line);
    }
    free(leftSide);
    free(rightSide);
    
    for(size_t i = 0; i < 10; i++){
        //printf("\ny:%u -> ", coords[i*10].y);
        for(size_t j = 0; j < 10; j++){
            //printf("%5u", coords[i*10 + j].x);
             
        }
    }

    CropSquares(surface, coords, 10, 10);
    free(coords);
}



Intersection *FindPoints(Intersection a, Intersection b){
    Intersection *res = malloc(10*sizeof(Intersection));
    res[0] = a;

    Intersection vect = {abs((int)(b.x - a.x)/10), abs((int)(b.y - a.y)/10)};
    for (size_t i = 1; i < 9; i++){
        res[i].x = res[i-1].x + vect.x;
        res[i].y = res[i-1].y + vect.y;
    }
    res[9] = b;
    return res;
}



