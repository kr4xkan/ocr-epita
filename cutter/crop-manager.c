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

/*
    printf("top-left:      (%u, %u)\n", coords[0].x, coords[0].y);
    printf("top-right:     (%u, %u)\n", coords[vertLen-1].x, coords[vertLen-1].y);
    printf("bottom-left:   (%u, %u)\n", coords[vertLen*horiLen-vertLen].x, coords[vertLen*horiLen-vertLen].y);
    printf("bottom-right:  (%u, %u)\n", coords[vertLen*horiLen-1].x, coords[vertLen*horiLen-1].y);
*/
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




SDL_Surface **CropSquares(SDL_Surface *surface, Intersection *coords, size_t vertLen, size_t horiLen){
    SDL_Surface **res = malloc(81*sizeof(SDL_Surface*));

    for (size_t y = 0; y < horiLen-1; y++){
        for (size_t x = 0; x < vertLen-1; x++){
            Intersection current = coords[y*vertLen + x];

            unsigned int squareWidth = coords[y*vertLen + x+1].x - current.x;
            unsigned int squareHeight = coords[(y+1)*vertLen + x].y - current.y;

            SDL_Surface *square = CropSurface(surface, current, squareWidth,
                    squareHeight);

            res[y*(vertLen-1)+x] = square;
            /*
            char name[] = {x+'1', '-', y+'1', '.', 'p', 'n', 'g', '\0'};
            char *newStr = malloc((strlen(name) + 15) * sizeof(char));
            strcpy(newStr, "squares/");
            strcat(newStr, name);
            IMG_SavePNG(square, newStr);
            SDL_FreeSurface(square);
            free(newStr);
            */
        }
    }
    return res;
}




SDL_Surface **ManualCrop(SDL_Surface *surface, Intersection *corners){
    Intersection *coords = malloc(100*sizeof(Intersection));

    // Change to 0 2 | 1 3 if manual crop is not working
    // Intersection *leftSide = FindPoints(corners[0], corners[2]);
    // Intersection *rightSide = FindPoints(corners[1], corners[3]);
    Intersection *leftSide = FindPoints(corners[0], corners[1]);
    Intersection *rightSide = FindPoints(corners[2], corners[3]);

    for (size_t i = 0; i < 10; i++){
        Intersection *line = FindPoints(leftSide[i], rightSide[i]);
        for (size_t j = 0; j < 10; j++){
            coords[i*10+j] = line[j];
        }
        free(line);
    }

    free(leftSide);
    free(rightSide);
    return CropSquares(surface, coords, 10, 10);
}



Intersection *FindPoints(Intersection a, Intersection b){
    Intersection *res = malloc(10*sizeof(Intersection));
    res[0] = a;

    int dx = ((int)b.x - (int)a.x)/9;
    int dy = ((int)b.y - (int)a.y)/9;
    for (size_t i = 1; i < 9; i++){
        res[i].x = res[i-1].x + dx;
        res[i].y = res[i-1].y + dy;
    }
    res[9] = b;
    return res;
}





SDL_Surface *CropSurface(SDL_Surface *surface, Intersection current, int width,
                         int height){
    SDL_Surface *newSurface = SDL_CreateRGBSurface(
        surface->flags, width, height, surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask, surface->format->Bmask,
        surface->format->Amask);

    SDL_Rect rect = {current.x, current.y, width, height};
    SDL_BlitSurface(surface, &rect, newSurface, NULL);


    SDL_Surface *res = SDL_CreateRGBSurface(
        surface->flags, 28, 28, surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask, surface->format->Bmask,
        surface->format->Amask);

    StretchBlit(newSurface, res);
    SDL_FreeSurface(newSurface);

    return res;
}


void StretchBlit(SDL_Surface* src,SDL_Surface* dest){
    SDL_Surface* img = SDL_CreateRGBSurface(SDL_SWSURFACE,dest->w,dest->h,32,0,0,0,0);
    StretchLinear(src,img);
    SDL_BlitSurface(img,NULL,dest,NULL);
    SDL_FreeSurface(img);
}


void StretchLinear(SDL_Surface* src,SDL_Surface* dest){
    double rx = dest->w * 1.0 / src->w;
    double ry = dest->h * 1.0 / src->h;

    for(int i = 0; i < dest->w; i++){
        for(int j = 0; j < dest->h; j++){
            unsigned char pix;
            double valx = i / rx;
            double valy = j / ry;
            int minx = (int) valx;
            int miny = (int) valy;
            int maxx = minx+1;

            if (maxx>=src->w)
                maxx--;

            int maxy = miny+1;
            if (maxy>=src->h)
                maxy--;

            double fx = valx-minx;
            double fy = valy-miny;

            for(int k = 0; k < 3; k++){
                pix = (unsigned char)(GetPixelComp32(src,minx,miny,k)*(1-fx)*(1-fy) + GetPixelComp32(src,maxx,miny,k)*fx*(1-fy)
                    + GetPixelComp32(src,minx,maxy,k)*(1-fx)*fy + GetPixelComp32(src,maxx,maxy,k)*fx*fy);
                PutPixelComp32(dest,i,j,k,pix);
            }
        }
    }
}

unsigned char GetPixelComp32(SDL_Surface* surface,int x,int y,int c){ 
    unsigned char *p = ((unsigned char*)surface->pixels) + y * surface->pitch + x * 4;
    return p[c];
}

void PutPixelComp32(SDL_Surface* surface,int x,int y,int c,unsigned char val){ 
    unsigned char *p = ((unsigned char*)surface->pixels) + y * surface->pitch + x * 4;
    p[c] = val;
}
