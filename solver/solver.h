#pragma once


void print(int grid[10][10]);
int check_safe(int grid[10][10], int row, int col, int num);
int solver(int grid[10][10], int row, int col);
void argv_to_grid(char *filename, int grid[10][10]);
