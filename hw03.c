/**
 * A pthread program which determines the validity of
 * a sudoku puzzle by checking each row, column, and sub-grid.
 * This is achieved by creating a total of 27 worker threads.
 *
 * Most Unix/Linux/OS X users
 * gcc hw03.c -o <name of executable file> -lpthread
 *
 * @author Justin Fulner
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_CHILD_THREADS 27
#define MAX 9

typedef int bool;

#define TRUE 1
#define FALSE 0

/* this data is shared by the thread(s) */
int  sudokuPuz[MAX][MAX];
bool col[MAX];
bool row[MAX];
bool subGrid[MAX];

/* the threads */
void *checkCols(void *param);
void *checkRows(void *param);
void *checkSubGrids(void *param);

/* helper functions */
void populatePuz();
bool numExists(int arr[], int num);
bool puzIsValid(bool arr[], bool arr1[], bool arr2[]);

typedef struct{
int topRow;
int botRow;
int leftCol;
int rightCol;
int tNum;
} run_param;

int main(int argc, char *argv[])
{
	int i;
	populatePuz();

	/* coordinate information for columns */
	run_param colCoords[MAX];
	for (i = 0; i < MAX; i++) {
		colCoords[i].topRow = 0;
		colCoords[i].botRow = 8;
		colCoords[i].leftCol = i;
		colCoords[i].rightCol = i;
		colCoords[i].tNum = i;
	}

	/* coordinate information for rows */
	run_param rowCoords[MAX];
	for (i = 0; i < MAX; i++) {
		rowCoords[i].topRow = i;
		rowCoords[i].botRow = i;
		rowCoords[i].leftCol = 0;
		rowCoords[i].rightCol = 8;
		colCoords[i].tNum = i;
	}

	/* I know it's dirty, but it was going to take me too long to figure out how to loop this. */
	run_param subGridCoords[MAX];
	subGridCoords[0].tNum = 0;
	subGridCoords[0].topRow = 0;
	subGridCoords[0].botRow = 2;
	subGridCoords[1].tNum = 1;
	subGridCoords[1].topRow = 0;
	subGridCoords[1].botRow = 2;
	subGridCoords[2].tNum = 2;
	subGridCoords[2].topRow = 0;
	subGridCoords[2].botRow = 2;

	subGridCoords[3].tNum = 3;
	subGridCoords[3].topRow = 3;
	subGridCoords[3].botRow = 5;
	subGridCoords[4].tNum = 4;
	subGridCoords[4].topRow = 3;
	subGridCoords[4].botRow = 5;
	subGridCoords[5].tNum = 5;
	subGridCoords[5].topRow = 3;
	subGridCoords[5].botRow = 5;

	subGridCoords[6].tNum = 6;
	subGridCoords[6].topRow = 6;
	subGridCoords[6].botRow = 8;
	subGridCoords[7].tNum = 7;
	subGridCoords[7].topRow = 6;
	subGridCoords[7].botRow = 8;
	subGridCoords[8].tNum = 8;
	subGridCoords[8].topRow = 6;
	subGridCoords[8].botRow = 8;

	subGridCoords[0].leftCol = 0;
	subGridCoords[0].rightCol = 2;
	subGridCoords[3].leftCol = 0;
	subGridCoords[3].rightCol = 2;
	subGridCoords[6].leftCol = 0;
	subGridCoords[6].rightCol = 2;
	
	subGridCoords[1].leftCol = 3;
	subGridCoords[1].rightCol = 5;
	subGridCoords[4].leftCol = 3;
	subGridCoords[4].rightCol = 5;
	subGridCoords[7].leftCol = 3;
	subGridCoords[7].rightCol = 5;
	
	subGridCoords[2].leftCol = 6;
	subGridCoords[2].rightCol = 8;
	subGridCoords[5].leftCol = 6;
	subGridCoords[5].rightCol = 8;
	subGridCoords[8].leftCol = 6;
	subGridCoords[8].rightCol = 8;

	
	/* the thread identifier for child threads */
	pthread_t tid_row[MAX];
	pthread_t tid_column[MAX];
	pthread_t tid_subgrid[MAX];
	pthread_attr_t attr[NUM_CHILD_THREADS]; /* set of attributes for the thread */


	/* get the default attributes */
	for (i = 0; i < NUM_CHILD_THREADS; i++)
		pthread_attr_init(&(attr[i]));

	/* create threads to check columns */
	for (i = 0; i < MAX; i++) {
		pthread_create( &(tid_column[i]), &(attr[i]), checkCols, &(colCoords[i]) );
	}

	/* now wait for column threads to exit */
	for (i = 0; i < MAX; i++) {
		pthread_join(tid_column[i],NULL);
	}
	printf("\n");

	// create threads to check rows
	for (i = 0; i < MAX; i++) {
		pthread_create( &(tid_row[i]), &(attr[i]), checkRows, &(rowCoords[i]) );
	}

	// wait for row threads to exit
	for (i = 0; i < MAX; i++) {
		pthread_join(tid_row[i],NULL);
	}
	printf("\n");

	// create threads to check subgrids
	for (i = 0; i < MAX; i++) {
		pthread_create( &(tid_subgrid[i]), &(attr[i]), checkSubGrids, &(subGridCoords[i]) );
	}

	// wait for subgrid threads to exit
	for (i = 0; i < MAX; i++) {
		pthread_join(tid_subgrid[i],NULL);
	}
	printf("\n");

	for (i = 0; i < MAX; i++ ) {
		if (col[i] == TRUE)  printf("Column: 0x%02X --- Valid.\n", (unsigned) tid_column[i]); 
		else  printf("Column: 0x%02X --- Invalid.\n", (unsigned) tid_column[i]); 
	}
	printf("\n");

	for (i = 0; i < MAX; i++ ) {
		if (row[i] == TRUE)  printf("Row: 0x%02X --- Valid.\n", (unsigned) tid_row[i]); 
		else  printf("Row: 0x%02X --- Invalid.\n", (unsigned) tid_row[i]); 
	}
	printf("\n");

	for (i = 0; i < MAX; i++ ) {
		if (subGrid[i] == TRUE)  printf("Sub-grid: 0x%02X --- Valid.\n", (unsigned) tid_subgrid[i]); 
		else  printf("Sub-grid: 0x%02X --- Invalid.\n", (unsigned) tid_subgrid[i]); 
	}
	printf("\n");

	if ( puzIsValid(col,row,subGrid) == FALSE ) printf("Sudoku Puzzle: Invalid.");
	else  printf("Sudoku Puzzle: Valid.");

	return 0;
}

void *checkCols(void *param) {
	run_param *inP;
	int i, top, bot, left, right, val;
	int numsSeen[MAX] = {0};
	pthread_t self = pthread_self();

	inP = (run_param *)param;
	top = inP->topRow;
	bot = inP->botRow;
	left = inP->leftCol;
	right = inP->rightCol;

	for (i = 0; i <= bot; i++) {
		val = sudokuPuz[i][left];
		if (val >= 1 && val <= 9) {
			if( numExists(numsSeen,val) == TRUE ) {
				col[left] = FALSE;

				printf("0x%02X: TRow: %d, BRow: %d, LCol: %d, RCol: %d --- Invalid! \n", (unsigned) self, top, bot, left, right );

				pthread_exit(0);
			} 
			else {
				numsSeen[i] = val;
			}
		}
	}
	col[left] = TRUE;

	printf("0x%02X: TRow: %d, BRow: %d, LCol: %d, RCol: %d --- Valid! \n",(unsigned) self, top, bot, left, right );

	pthread_exit(0);
}

void *checkRows(void *param) {
	run_param *inP;
	int i, top, bot, left, right, val;
	int numsSeen[MAX] = {0};
	pthread_t self = pthread_self();

	inP = (run_param *)param;
	top = inP->topRow;
	bot = inP->botRow;
	left = inP->leftCol;
	right = inP->rightCol;

	for (i = 0; i <= right; i++) {
		val = sudokuPuz[top][i];
		if (val >= 1 && val <= 9) {
			if ( numExists(numsSeen,val) == TRUE) {
				row[top] = FALSE;

				printf("0x%02X: TRow: %d, BRow: %d, LCol: %d, RCol: %d --- Invalid! \n", (unsigned)self, top, bot, left, right );

				pthread_exit(0);
			}
			else {
				numsSeen[i] = val;
			}
		}
	}
	row[top] = TRUE;

	printf("0x%02X: TRow: %d, BRow: %d, LCol: %d, RCol: %d --- Valid! \n", (unsigned)self, top, bot, left, right );

	pthread_exit(0);
}

void *checkSubGrids(void *param) {
	run_param *inP;
	int j, k, top, bot, left, right, val, arrNum;
	int i = 0;
	int numsSeen[MAX] = {0};
	pthread_t self = pthread_self();

	inP = (run_param *)param;
	top = inP->topRow;
	bot = inP->botRow;
	left = inP->leftCol;
	right = inP->rightCol;
	arrNum = inP-> tNum;

	for (j = top; j <= bot; j++) {
		for (k = left; k <= right; k++) {
			val = sudokuPuz[j][k];
			if(val >= 1 && val <= 9) {
				if ( numExists(numsSeen,val) == TRUE ) {
					subGrid[arrNum] = FALSE;

					printf("0x%02X: TRow: %d, BRow: %d, LCol: %d, RCol: %d --- Invalid! \n", (unsigned)self, top, bot, left, right );

					pthread_exit(0);
				}
				else {
					numsSeen[i] = val;
					i++;
				}
			}
		}
	}
	subGrid[arrNum] = TRUE;

	printf("0x%02X: TRow: %d, BRow: %d, LCol: %d, RCol: %d --- Valid! \n", (unsigned)self, top, bot, left, right );

	pthread_exit(0);
}

/* returns TRUE on first occurence of 'num' within 'arr' */
bool numExists(int arr[], int num) {
	int i;
	for (i = 0; i < MAX; i++) {
		if (arr[i] == num) return TRUE;
	}
	return FALSE;
}

/* retrieves puzzle data from 'SudokuPuzzle.txt' and populates global var 'sudokuPuz' */
void populatePuz() {
	int nums[81] = {0};
	int i = 0;
	int j;
	int k = 0;
	FILE *fp;

	if ( fp = fopen("./SudokuPuzzle.txt", "r") ) {
		while ( fscanf(fp, "%d", &nums[i]) != EOF ) {
			++i;
		}
	}

	for(i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			sudokuPuz[i][j] = nums[k++];
		}

	}

	fclose(fp);

	for (j = 0; j < MAX; j++) {
		for (k = 0; k < MAX; k++) {
			printf(" %d ", sudokuPuz[j][k]);
		}
		printf("\n");
	}
}

bool puzIsValid(bool arr[], bool arr1[], bool arr2[]) {
	int i;
	for (i = 0; i < MAX; i++) {
		if(arr[i] == FALSE) return FALSE;
		else if(arr1[i] == FALSE) return FALSE;
		else if(arr2[i] == FALSE) return FALSE;
	}
	return TRUE;
}