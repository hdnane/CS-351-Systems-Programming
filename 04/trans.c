/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i,j,r,c;
    int temp[2][4];
    switch (M)
    {
    case 32:
        #define STEP 8
        for(r = 0; r < N; r += STEP)
        {
            for(c = 0; c < M; c += STEP)
            {
                for(i = r; i < r+STEP && i < N; i++)
                {
                    for(j = c; j < c+STEP && j < M; j++)
                    {
                        if(i!=j)
                            B[j][i]=A[i][j];
                        else
                            temp[0][0]=A[i][j];
                    }
                    if(r==c)
                        B[i][i]=temp[0][0];
                }
            }
        }
        #undef STEP
        break;
    case 64:
        for(r = 0; r < N; r += 8)
        {
            for(c = 0; c < M; c += 8)
            {
                for(j=c;j<c+4;j++)
                {
                    for(i=r;i<r+4;i++)
                        temp[0][i-r]=A[j][i];
                    for(i=r;i<r+4;i++)
                        temp[1][i-r]=A[j][i+4];
                    for (i = r;i<r + 4;i++)
                        B[i][j]=temp[0][i - r];
                    for (i = r;i<r + 4;i++)
                        B[i][j+4]=temp[1][i - r];
                }
                for(j=r;j<r+4;j++)
                {
                    for(i=c;i<c+4;i++)
                        temp[0][i-c]=B[j][i+4];
                    for(i=c;i<c+4;i++)
                        temp[1][i-c]=A[i+4][j];
                    for (i = c;i<c + 4;i++)
                        B[j][i+4]=temp[1][i - c];
                    for (i = c;i<c + 4;i++)
                        B[j+4][i]=temp[0][i - c];

                }
                for(j=r+4;j<r+8;j++)
                {
                    for(i=c+4;i<c+8;i++)
                        temp[0][i-c-4]=A[i][j];
                    for (i = c + 4;i<c + 8;i++)
                        B[j][i]=temp[0][i - c-4];
                }
            }
        }
        break;
    default:
        #define STEP 16
        for(r = 0; r < N; r += STEP)
        {
            for(c = 0; c < M; c += STEP)
            {
                for(i = r; i < r+STEP && i < N; i++)
                {
                    for(j = c; j < c+STEP && j < M; j++)
                    {
                        if(i!=j)
                            B[j][i]=A[i][j];
                        else
                            temp[0][0]=A[i][j];
                    }
                    if(r==c)
                        B[i][i]=temp[0][0];
                }
            }
        }
        #undef STEP
        break;
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

