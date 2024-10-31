#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define GRID_SIZE 9
#define SUBGRID_SIZE 3
#define NUM_THREADS 4


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t completing = PTHREAD_MUTEX_INITIALIZER;
int threadsCompleted = 0;
int delay = 0;

// Structure to pass parameters to threads
struct SubRowThread {
    int id;
    int rows[3];
    int subGrids[3];
    int (*Sol)[GRID_SIZE];
    int *Row;
    int *Sub;
    int *Counter;
};

struct ColumnThread {
    int id;
    int columns[9];
    int (*Sol)[GRID_SIZE];
    int *Col;
    int *Counter;
};

void printValidationResults(int Row[], int Col[], int Sub[], int Counter);
void printMatrix(int Sol[][GRID_SIZE]);
void *validateSubRows(void *arg);
void initializeSudokuSolution(char *filename, int Sol[][GRID_SIZE]);
void printArray(int arr[], int length);
void *validateColumns(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <solution_file> <delay>\n", argv[0]);
        return 1;
    }

    int Sol[GRID_SIZE][GRID_SIZE];
    int Row[GRID_SIZE] = {0};
    int Col[GRID_SIZE] = {0};
    int Sub[GRID_SIZE] = {0};
    int Counter = 0;

    char *filename = argv[1];
    delay = atoi(argv[2]);
    

    initializeSudokuSolution(filename, Sol);
    
    pthread_t threads[NUM_THREADS];

    struct SubRowThread thread1 = {1, {1,2,3},{1,2,3}, Sol, Row, Sub, &Counter};
    pthread_create(&threads[0], NULL, validateSubRows, (void *)&thread1);
    struct SubRowThread thread2 = {2, {4,5,6},{4,5,6}, Sol, Row, Sub, &Counter};
    pthread_create(&threads[1], NULL, validateSubRows, (void *)&thread2);
    struct SubRowThread thread3 = {3, {7,8,9},{7,8,9}, Sol, Row, Sub, &Counter};
    pthread_create(&threads[2], NULL, validateSubRows, (void *)&thread3);

    struct ColumnThread thread4 = {4, {1,2,3,4,5,6,7,8,9}, Sol, Col, &Counter};
    pthread_create(&threads[3], NULL, validateColumns, (void *)&thread4);

    // Wait for threads to finish
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // printMatrix(Sol);
    printValidationResults(Row, Col, Sub, Counter);
    
    return 0;
}

void *validateSubRows(void *arg) {
    struct SubRowThread *currentThread = (struct SubRowThread*)arg;
    //printf("%d\n", currentThread->id);
    int num = 0;

    //Validates Rows
    for (int i = currentThread->rows[0]; i <= currentThread->rows[2]; i++) 
    {
        int valid = 1;
        int rowCount[GRID_SIZE + 1] = {0};
        //printf("thread ID: %d curRow %d: ",currentThread->id ,i);
        for (int j = 0; j < GRID_SIZE; j++)
        {
            num = currentThread->Sol[i - 1][j];
            //printf(" %d " , num);

            if (num < 1 || num > GRID_SIZE || rowCount[num] > 0) 
            {
                valid = 0;
            }

            rowCount[num]++;
        }
        //printf("\n");
        if(valid)
        {
            pthread_mutex_lock(&mutex);
            currentThread->Row[i-1] = 1;
            (*currentThread->Counter)++;
            //printf("\ncounter %d\n", *currentThread->Counter);
            pthread_mutex_unlock(&mutex);
        }
    }


    
    //Validates subgrids
    for (int curSub = currentThread->subGrids[0]; curSub <= currentThread->subGrids[2]; curSub++) 
    {
        //printf("current subgrid: %d\n", curSub - 1);
        int startRow = ((curSub - 1) / 3)*3;
        int startCol = (curSub - 1 - ((startRow/3) * 3))*3;
        int valid = 1;
        int subgridCount[GRID_SIZE + 1] = {0};
        //printf("subgrid: %d startRow is %d\n", curSub, startRow);
        //printf("subgrid: %d startCol is %d\n", scurSub, startCol); 
        for (int i = startRow; i < startRow+3; i++) 
        {
            for (int j = startCol; j < startCol+3; j++) 
            {
                num = currentThread->Sol[i][j];
                //printf(" %d " , num);
                if (num < 1 || num > GRID_SIZE || subgridCount[num] > 0) 
                {
                    //printf("problem: %d\n", num);
                    valid = 0;
                }
                subgridCount[num]++;
                
            }
            //printf("\n");
        }
        if(valid)
        {
            pthread_mutex_lock(&mutex);
            currentThread->Sub[curSub - 1] = 1;
            (*currentThread->Counter)++;
            //printf("\ncounter %d\n", *currentThread->Counter);
            pthread_mutex_unlock(&mutex);
        }
    }
    sleep(delay);

    pthread_mutex_lock(&completing);

    threadsCompleted++;
    //printf("\nthreads completed: %d    ",threadsCompleted);
    if(threadsCompleted == 4)
    {
        printf("Thread ID-%d: last thread", currentThread->id);
    }
    pthread_mutex_unlock(&completing);

    //terminating
    return 0;

}



void *validateColumns(void *arg) {
    //printf("\n\n");
    struct ColumnThread *currentThread = (struct ColumnThread*)arg;
    //printf("%d\n", currentThread->id);
    int num = 0;

    for (int j = currentThread->columns[0]; j <= currentThread->columns[GRID_SIZE - 1]; j++) 
    {
        int valid = 1;
        int colCount[GRID_SIZE + 1] = {0};
        //printf("thread ID: %d curRow %d: ",currentThread->id ,i);
        for (int i = 0; i < GRID_SIZE; i++)
        {
            num = currentThread->Sol[i][j - 1];
            //printf(" %d " , num);

            if (num < 1 || num > GRID_SIZE || colCount[num] > 0) 
            {
                //printf("problem: %d \n", num);
                //printf("count: %d \n", colCount[num]);
                valid = 0;
            }

            colCount[num]++;
        }
        //printf("\n");
        if(valid)
        {
            pthread_mutex_lock(&mutex);
            //printf("valid\n");
            currentThread->Col[j-1] = 1;
            (*currentThread->Counter)++;
            //printf("\ncounter %d\n", *currentThread->Counter);
            pthread_mutex_unlock(&mutex);
        }
    }
    sleep(delay);


    pthread_mutex_lock(&completing);

    threadsCompleted++;
    //printf("\nthreads completed: %d    ",threadsCompleted);
    if(threadsCompleted == 4)
    {
        printf("Thread ID-%d: last thread", currentThread->id);
    }
    pthread_mutex_unlock(&completing);

    //terminating
    return 0;
}

void initializeSudokuSolution(char *filename, int Sol[][GRID_SIZE]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (fscanf(file, "%d", &Sol[i][j]) != 1) {
                fprintf(stderr, "Error reading from file\n");
                fclose(file);
                exit(1);
            }
        }
    }

    fclose(file);
}

void printValidationResults(int Row[], int Col[], int Sub[], int Counter) {
    // printf("Row: ");
    // printArray(Row, GRID_SIZE);
    // printf("Col: ");
    // printArray(Col, GRID_SIZE);
    // printf("Sub: ");
    // printArray(Sub, GRID_SIZE);
    int valid = 1;
    for(int rowSubThreads = 0; rowSubThreads < 3; rowSubThreads++)
    {
        valid = 1;
        printf("\nThread ID-%d:", rowSubThreads + 1);
        for(int i = 0; i < 3; i++)
        {
            //printf("\nrow/sub %d: %d", (rowSubThreads*3) + i, Row[(rowSubThreads*3) + i]);
            if(Row[(rowSubThreads*3) + i] == 0)
            {
                valid = 0;
                printf(" row %d,", (rowSubThreads*3) + i + 1);
            }
            if(Sub[(rowSubThreads*3) + i] == 0)
            {
                valid = 0;
                printf(" sub %d,", (rowSubThreads*3) + i + 1);
            }
        }
        if(valid)
        {
            printf(" valid");
        }
        else
        {
            printf(" are invalid");

        }
    }

    printf("\nThread ID-4:");
    
    valid = 1;
    for(int i = 0; i < GRID_SIZE; i++)
    {
        if(Col[i] == 0)
        {
            valid = 0;
            printf(" col %d,", i + 1);
        }
        
    }
    if(valid)
    {
        printf(" valid");
    }
    else
    {
        printf(" are invalid");

    }
    printf("\nThere are %d valid sub-grids, columns and rows. The solution is ", Counter);
    if(Counter == 27)
    {
        printf("valid.");
    }
    else
    {
        printf("invalid.");
    }

    printf("\n");
}

void printMatrix(int Sol[][GRID_SIZE])
{
    for(int x = 0 ; x < GRID_SIZE ; x++) 
    {
        printf(" (");
        for(int y = 0 ; y < GRID_SIZE ; y++){
            printf("%d     ", Sol[x][y]);
        }
        printf(")\n");
    }
}

void printArray(int arr[], int length)
{
    for (int i = 0; i < length; i++) 
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}