#pragma once
#include <SDL2/SDL_image.h>

int MainCutter(char *path);
    


// -----------------------------Lines--------------------------------------
typedef struct Line{
    unsigned int theta; 
    unsigned int rho; 
    unsigned int value; 
    size_t accuPos; 
} Line;

unsigned int *CreateAccumulator(SDL_Surface *surface);
void FillAcumulator(SDL_Surface *surface, unsigned int *accumulator);
unsigned int FindMinPeak(unsigned int *accumulator);
Line* DetectLines(unsigned int *accumulator);
int CheckPeak(unsigned int *accumulator, size_t i,
        unsigned int val);
int AlreadyExist(Line *lines, Line line, size_t len, int maxGap,
        unsigned int *accumulator);
Line* FilterLines(unsigned int *accumulator, Line* lines);
size_t FindGap(unsigned int *histo, size_t histoSize, size_t range);
void Remove(unsigned int *accumulator, Line *lines, size_t capacity, size_t *size, size_t gap);



// -----------------------------Rotation--------------------------------------
SDL_Surface *CheckRotation(SDL_Surface *surface, unsigned int *accumulator);
SDL_Surface *RotateSurface(SDL_Surface *origine, float angle);



// -----------------------------Intersections--------------------------------------
unsigned int *CreateSpace(SDL_Surface *surface,
                                  Line* lines);
void ComputeLine(unsigned int *linesArray, long int w, long int h, long int x1,
                 long int y1, long int x2, long int y2);



// -----------------------------UTILS-------------------------------------
void DrawIntersections(SDL_Surface *surface, unsigned int *accumulator);
void PrintMat(unsigned int *accumulator);
void DrawLines(SDL_Surface *surface, unsigned int *accumulator, int *pixels);
void DrawLine(int *pixels, long int w, long int h, long int x1, long int y1,
              long int x2, long int y2, Uint32 color);
