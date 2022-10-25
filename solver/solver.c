#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#define lines 11
#define length 13 


void print(int grid[10][10]){
	for (size_t i =0; i < 9; i++){
		if (i == 3 || i == 6){
			printf("\n");
		}
		for (size_t j = 0; j < 9; j++){
			if (j == 3 || j == 6){
				printf(" ");
			}
			printf("%i", grid[i][j]);
		}
		printf("\n");
	}
}


void print_file(int grid[10][10]){
	FILE * file;
	file = fopen("result.txt", "w");
	for(size_t i = 0; i < 9 ; i++){
		if (i == 3 || i == 6){
				fprintf(file, "\n");
		}
		for(size_t j =0; j < 9; j++){
			if(j == 3 || j == 6){
				fprintf(file, " ");
			}
			fprintf(file, "%i", grid[i][j]);
		}
		fprintf(file,"\n");
	}
	fclose(file);
}


int check_safe(int grid[10][10], int row, int col, int num){
	for (int i = 0; i < 9 ; i ++){
		if (grid[row][i] == num){
			return 0;
		}
	}
	for (int i = 0; i < 9; i++){
		if (grid[i][col] == num){
			return 0;
		}
	}
	int start_sqr_row = row - row % 3;
	int start_sqr_col = col - col % 3;

	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (grid[i + start_sqr_row][j+ start_sqr_col] == num){
				return 0;
			}
		}
	}
	return 1;
}
		


int solver( int grid[10][10], int row, int col){
	if (row == 8 && col == 9){
		return 1;
	}
	if (col == 9){
		row++;
		col = 0;
	}

	if (grid[row][col] >0 ){
		return solver(grid, row , col +1);
	}

	for (int num = 1; num <= 9; num++){
		if (check_safe(grid, row, col, num) ==1){
			grid[row][col] = num;
			if (solver(grid, row, col+1) == 1){
				return 1;
			}
		}
		grid[row][col] = 0;
	}
	return 0;
}


void argv_to_grid( char *filename, int grid[10][10]){
	FILE * file;
	file = fopen(filename, "r");
	if (!file){
		printf("Unable to open file\n");
	}
	else {
		char buffer[lines][length];
		int i = 0;
		while(!feof(file)){
			fgets(buffer[i], length, file);
			i++;
		}
		size_t row =0;
		size_t col = 0;
		for(size_t a = 0; a < lines; a++){
			for(size_t b = 0; b < strlen(buffer[a]); b++){
				if ( b == 11){
					grid[row][9] = '\0';
					row++;
					col=0;
				}
				else {
					if (buffer[a][b] != ' ' && buffer[a][b] != '\n'){
						if (buffer[a][b] == '.'){
							grid[row][col] = 0;
						}
						else{
							grid[row][col] = buffer[a][b] - 48;
						}
						col++;
					}
				}
			}
		}
		grid[9][0] = '\0';
	}
}



int main(int argc, char **argv){
	if (argc != 2){
		printf("no valid grid entered\n");
		return 1;

	}
	else {
		int grid[10][10];
		argv_to_grid(argv[1], grid);
		if (solver(grid, 0, 0) == 1){
			print_file(grid);
		}
		else{
			printf("no solution found\n");
		}
		return 0;
	}
}






