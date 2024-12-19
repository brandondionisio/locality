#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "uarray2b.h"

void print(int col, int row, UArray2b_T array2b, void *elem, void *cl);

int main(int argc, char *argv[]) 
{
        (void) argc;
        (void) argv;
        printf("HELLOOO WORRRLDDD\n");
        // test the new function in UArray2b

        printf("Making a new UArray2b\n");
        // width, height, size, blocksize
        UArray2b_T array = UArray2b_new(13, 10, 8, 8);
        printf("Finished making a new UArray2b\n");
        (void) array;

        // printf("Making a 64K UArray2b\n");
        // UArray2b_T array2 = UArray2b_new_64K_block(13, 10, 7);
        // printf("Finished making a 64K UArray2b\n");
        // (void) array2;

        int counter = 1;
        printf("Testing at function\n");

        for (int j = 0; j < UArray2b_height(array); j++) {
                for (int i = 0; i < UArray2b_width(array); i++) {
                        printf("width is: %d and height is: %d\n", i, j);
                        int *p = UArray2b_at(array, i, j);
                        *p = counter;
                        counter++;
                }
        }

        printf("Finished testing at function\n");
        UArray2b_map(array, print, NULL);

        printf("Freeing the UArray2b\n");
        UArray2b_free(&array);
        
        printf("Finished testing the UArray2b\n");
        return EXIT_SUCCESS;
}

void print(int col, int row, UArray2b_T array2b, void *elem, void *cl) 
{
        (void) row;
        (void) cl;
        if (col == UArray2b_width(array2b) - 1) {
                printf("%d\n", *(int *)elem);
        } else {
                printf("%d ", *(int *)elem);
        }
}
