#pragma once

void print(int grid[9][9]);
int check_safe(int grid[9][9], int row, int col, int num);
int solver(int grid[9][9], int row, int col);
void argv_to_grid(char *filename, int grid[9][9]);
