#pragma once
#include <SDL2/SDL_image.h>

int MainCutter(char *path);
    


// -----------------------------Lines--------------------------------------
unsigned int *DetectLines(SDL_Surface *surface);
void FillAcumulator(SDL_Surface *surface, unsigned int *accumulator);
unsigned int FindMinPeak(unsigned int *accumulator, int accumulatorSize);
void FilterLines(unsigned int *accumulator, int accumulatorSize);
int CheckPeak(unsigned int *accumulator, int accumulatorSize, int i,
              unsigned int val);

typedef struct Line{
    unsigned int theta; 
    unsigned int rho; 
    unsigned int value; 
    unsigned int accuPos; 
} Line;
int AlreadyExist(Line *lines, Line line, size_t len, int maxGap,
        unsigned int *accumulator);



// -----------------------------Rotation--------------------------------------
SDL_Surface *CheckRotation(SDL_Surface *surface, unsigned int *accumulator);
SDL_Surface *RotateSurface(SDL_Surface *origine, float angle);



// -----------------------------Intersections--------------------------------------
unsigned int *DetectIntersections(SDL_Surface *surface, 
                                        unsigned int *accumulator);
void ComputeLine(unsigned int *linesArray, long int w, long int h, long int x1,
                 long int y1, long int x2, long int y2);



// -----------------------------UTILS-------------------------------------
void DrawIntersections(SDL_Surface *surface, unsigned int *accumulator);
void PrintMat(unsigned int *accumulator);
void DrawLines(SDL_Surface *surface, unsigned int *accumulator, int *pixels);
void DrawLine(int *pixels, long int w, long int h, long int x1, long int y1,
              long int x2, long int y2, Uint32 color);
