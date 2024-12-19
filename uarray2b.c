/**************************************************************
 *
 *                     uarray2b.c
 *
 *     Assignment: HW 3: locality
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 02/22/24
 *
 *     Summary: This file implements the UArray2b_T interface for the
 *              UArray2b_T data structure, which is a blocked 2D array of
 *              elements. This file is used to create a blocked 2D array of
 *              elements and contains apply functions to the elements in the
 *              array. This file include functions such as UArray2b_new, 
 *              UArray2b_new_64K_block, UArray2b_free, UArray2b_width,
 *              UArray2b_height, UArray2b_size, UArray2b_blocksize,
 *              UArray2b_at, and UArray2b_map. 
 *              
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "assert.h"
#include "mem.h"
#include "uarray.h"
#include "uarray2.h"
#include "uarray2b.h"

#define T UArray2b_T

/********** T ********
 * 
 * Struct for the blocked 2D bitmap array.
 * Contains the width and height for the 2D UArray.
 * Size is used for the size of each individual cell.
 * Blocks is the UArray2 that will hold all the UArray2s for the 2D array.
 *
 *******************/
struct T {
        int width, height; /* width and height of the array */
        int size; /* size of each cell */
        int blocksize; /* dimensions of each block in the array */
        int block_width; /* number of blocks in the width */
        int block_height; /* number of blocks in the height */
        UArray2_T blocks; /* 2D array of blocks that comprise the entire array,
                             with each block being a UArray itself */
};

/****************** UArray2b_new *******************
 * 
 * Creates a new instance of a UArray2b given its width, height, size, and
 * blocksize.
 *
 * Parameters:
 *      int width:     the width (number of cells along the x-axis) of the
 *                     UArray2b to be initialized
 *      int height:    the height (number of cells along the y-axis) of the
 *                     UArray2b to be initialized
 *      int size:      the size of each cell for the new UArray2b to be
 *                     initialized
 *      int blocksize: the length of the sides of each block in the UArray2b to
 *                     be initialized
 * Returns:
 *      Returns the struct holding the contents of the new UArray2b.
 * Expects:
 *      the passed-in width > 0, height > 0, size > 0, blocksize > 0 (throws a
 *              CRE otherwise)
 *      The new array struct is not NULL after setting its contents and its
*       block array dimensions are correct (throw CRE otherwise)
 *      The width attribute of the struct equals the integer returned from the
 *      UArray2b_width function. 
 *      The height attribute of the struct equals the integer returned from the
 *      UArray2b_height function.
 *
 ********************************************/
T UArray2b_new(int width, int height, int size, int blocksize)
{
        assert(width > 0 && height > 0 && size > 0 && blocksize > 0);
        T array; /* create a new instance of the UArray2b struct */
        NEW(array);
        array->width = width;
        array->height = height;
        array->size = size;
        array->blocksize = blocksize;

        /* calculate width and height in blocks and save in struct */
        array->block_width = (width / blocksize);
        array->block_height = (height / blocksize);

        /* if the width or height is not divisible by the blocksize, 
           add an extra block to the array so all cells can fit */
        if (width % blocksize != 0) {
                array->block_width++;
        }
        if (height % blocksize != 0) {
                array->block_height++;
        }

        /* create the 2D array of blocks */
        array->blocks = UArray2_new(array->block_width, array->block_height, 
                                                             sizeof(UArray_T));

        /* create the 2D array of blocks */
        for (int i = 0; i < array->block_width; i++) {
                for (int j = 0; j < array->block_height; j++) {
                        UArray_T *blockp = UArray2_at(array->blocks, i, j);
                        *blockp = UArray_new(blocksize * blocksize, size);
                }
        }

        /* Check that the array was created of correct dimensions and size */
        assert(array && (UArray2_width(array->blocks) == 
                (array->block_width)) && (UArray2_height(array->blocks) == 
                (array->block_height)) && (UArray2_size(array->blocks) == 
                sizeof(UArray2_T)));
        return array;
}

/************* UArray2b_new_64K_block ***************
 * 
 * Creates a new instance of a UArray2b given its width, height, and size.
 * This function defaults the blocksize to be as large as possible while
 * still allowing a block to fit in 64KB of RAM.
 *
 * Parameters:
 *      int width:     the width (number of cells along the x-axis) of the
 *                     UArray2b to be initialized
 *      int height:    the height (number of cells along the y-axis) of the
 *                     UArray2b to be initialized
 *      int size:      the size of each cell for the new UArray2b to be
 *                     initialized
 * Returns:
 *      Returns the struct holding the contents of the new UArray2b.
 * Expects:
 *      the passed-in width > 0, height > 0, size > 0 (throws a CRE otherwise)
 *
 ********************************************/
T UArray2b_new_64K_block(int width, int height, int size)
{
        /* check for valid (positive) parameters */
        assert(width >  0 && height > 0 && size > 0);

        /* the number of bytes in 64KB divided by the size of each cell */
        int blocksize = (64 * 1024) / (size * 1024);

        if (blocksize < 4) {
                blocksize = 1;
        } else {
                blocksize = sqrt(blocksize);
        }
        return UArray2b_new(width, height, size, blocksize);
}

/************* UArray2b_free ***************
 * 
 * Frees all the memory associated with the UArray2b passed in as a
 * pointer.
 *
 * Parameters:
 *      T *array2b: pointer to a UArray2b
 * Returns:
 *      None
 * Expects:
 *      The passed-in pointer does not point to nothing (throw CRE otherwise)
 *      The dereferenced UArray2b is not NULL (throw CRE otherwise)
 *
 ********************************************/
void UArray2b_free(T *array2b)
{
        assert(array2b != NULL && *array2b != NULL);
        UArray2_T p = (*array2b)->blocks;
        for (int i = 0; i < (*array2b)->block_width; i++) {
                for (int j = 0; j < (*array2b)->block_height; j++) {
                        UArray_free(UArray2_at(p, i, j));
                }
        }
        UArray2_free(&p);
        FREE(*array2b);
}

/************* UArray2b_width ***************
 * 
 * Returns the width of the given UArray2b
 *
 * Parameters:
 *      T array2b: a UArray2b
 * Returns:
 *      integer representing the width of the passed-in UArray2b
 * Expects:
 *      The passed-in UArray2b is not NULL (throw CRE if NULL)
 *
 ********************************************/
int UArray2b_width(T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/************* UArray2b_height ***************
 * 
 * Returns the height of the given UArray2b
 *
 * Parameters:
 *      T array2b: a UArray2b
 * Returns:
 *      integer representing the height of the passed-in UArray2b
 * Expects:
 *      The passed-in UArray2b is not NULL (throw CRE if NULL)
 *
 ********************************************/
int UArray2b_height(T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/************* UArray2b_size ***************
 * 
 * Returns an integer representing the size of each cell of the given UArray2b
 *
 * Parameters:
 *      T array2b: a UArray2b
 * Returns:
 *      integer of the size of each cell of the UArray2b
 * Expects:
 *      The passed-in UArray2b is not NULL (throw CRE if NULL)
 *
 ********************************************/
int UArray2b_size(T array2b)
{
        assert(array2b != NULL);
        return array2b->size;
}

/************* UArray2b_blocksize ***************
 * 
 * Returns the blocksize of the given UArray2b
 *
 * Parameters:
 *      T array2b: a UArray2b
 * Returns:
 *      integer representing the blocksize of the passed-in UArray2b -- the
 *      blocksize being the dimensions of each block in the blocked array.
 * Expects:
 *      The passed-in UArray2b is not NULL (throw CRE if NULL)
 *
 ********************************************/
int UArray2b_blocksize(T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/************* UArray2b_at ***************
 * 
 * Returns a void pointer to the element at the given indices for the given
 * UArray2b
 *
 * Parameters:
 *      T array2b:  a UArray2b from which we are retrieving the element
 *      int column: an integer representing the column index of the element we
 *                  are trying to retrieve
 *      int row:    an integer representing the row index of the element we are
 *                  trying to retrieve
 * Returns:
 *      a void pointer to the element in the UArray2b
 * Expects:
 *      The passed-in UArray2b is not NULL (throw CRE if NULL)
 *      [0 < column < width of array2b), throws CRE if not
 *      [0 < row < width of array2b), throws CRE if not
 *
 ********************************************/
void *UArray2b_at(T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert((column >= 0) && (column < UArray2b_width(array2b)));
        assert((row >= 0) && (row < UArray2b_height(array2b)));
        int blocksize = UArray2b_blocksize(array2b);
        int block_col = column / blocksize;
        int block_row = row / blocksize;
        
        UArray_T block = *(UArray_T *)UArray2_at(array2b->blocks, 
                          block_col, block_row);
        
        return UArray_at(block,
                         blocksize * (row % blocksize) + (column % blocksize));
}

/************* corner_check ***************
 * 
 * Returns whether the element at the indices of the given UArray2b is part
 * of the array (it exists). We know this element is in the bottom right
 * corner block, so we must verify that it is actually part of the used array.
 *
 * Parameters:
 *      int c: integer representing the index of the column of the current
 *             element
 *      int r: integer representing the index of the row of the current
 *             element
 *      T array2b: a UArray2b of which we are checking the corner
 * Returns:
 *      boolean deciding whether the element at the given indices is valid to
 *      execute the apply function on
 * Expects:
 *      None
 *
 ********************************************/
bool corner_check(int c, int r, T array2b)
{
        /* check if we are at the bottom right of 2D array */
        if (r / array2b->blocksize == array2b->block_height - 1 &&
                c / array2b->blocksize == array2b->block_width - 1) {
                /* check if the element at the indices exists
                and then call apply func if so */
                if ((array2b->height % array2b->blocksize == 0 &&
                     array2b->width % array2b->blocksize == 0) ||
                    (array2b->height % array2b->blocksize >= (r + 1) %
                                                array2b->blocksize ||
                     array2b->width % array2b->blocksize >= (c + 1) %
                                                array2b->blocksize)) {
                        return true;
                }
        }
        return false;
}

/************* right_edge_check ***************
 * 
 * Returns whether the element at the indices of the given UArray2b is part
 * of the array (it exists). We know this element is in a block on the right
 * edge, so we must verify that it is actually part of the used array.
 *
 * Parameters:
 *      int c: integer representing the index of the column of the current
 *             element
 *      int r: integer representing the index of the row of the current
 *             element
 *      T array2b: a UArray2b of which we are checking the right edge
 * Returns:
 *      boolean deciding whether the element at the given indices is valid to
 *      execute the apply function on
 * Expects:
 *      None
 *
 ********************************************/
bool right_edge_check(int r, T array2b)
{
        if (r / array2b->blocksize == array2b->block_height - 1) {
                /* check if the element at the indices exists
                and then call apply func if so */
                if ((array2b->height % array2b->blocksize == 0) ||
                     array2b->height % array2b->blocksize >= (r + 1) %
                     array2b->blocksize) {
                        return true;
                }
        }
        return false;
}

/************* bottom_edge_check ***************
 * 
 * Returns whether the element at the indices of the given UArray2b is part
 * of the array (it exists). We know this element is in a block on the bottom
 * edge, so we must verify that it is actually part of the used array.
 *
 * Parameters:
 *      int c: integer representing the index of the column of the current
 *             element
 *      int r: integer representing the index of the row of the current
 *             element
 *      T array2b: a UArray2b of which we are checking the bottom edge
 * Returns:
 *      boolean deciding whether the element at the given indices is valid to
 *      execute the apply function on
 * Expects:
 *      None
 *
 ********************************************/
bool bottom_edge_check(int c, T array2b)
{
        if (c / array2b->blocksize == array2b->block_width - 1) {
                /* check if the element at the indices exists
                and then call apply func if so */
                if ((array2b->width % array2b->blocksize == 0) ||
                        array2b->width % array2b->blocksize >= (c + 1) %
                        array2b->blocksize) {
                        return true;
                }
        }
        return false;
}

/************* UArray2b_map ***************
 * 
 * Mapping function which parses through the given array2b in block-major
 * order, executing the apply function on each element, and storing the closure
 * throughout the iterations.
 *
 * Parameters:
 *      T array2b:  a UArray2b that is being mapped through
 *      void apply: the apply function that will be executed upon parsing
 *                  through every element
 *      void *cl:   void pointer that is a closure to store data throughout
 *                  the entirety of the mapping
 * Returns:
 *      None
 * Expects:
 *      The passed-in UArray2b is not NULL (throw CRE if NULL)
 *
 ********************************************/
void UArray2b_map(T array2b, void apply(int col, int row, T array2b,void *elem,
                                                          void *cl),void *cl) {
        assert(array2b != NULL);
        int h = array2b->height;  /* keeping height and width in registers */
        int w = array2b->width;   /* avoids extra memory traffic */
        /* parse through height and then width of 2D array <- (row-major) */
        for (int r = 0; r < h; r++) {
                for (int c = 0; c < w; c++) {
                        if (corner_check(c, r, array2b)) {
                                apply(c, r, array2b, 
                                               UArray2b_at(array2b, c, r), cl);
                        /* check if we are at a bottom edge of 2D array */
                        } else if (right_edge_check(r, array2b)) {
                                apply(c, r, array2b, 
                                               UArray2b_at(array2b, c, r), cl);
                        /* check if we are at a right edge of 2D array */
                        } else if (bottom_edge_check(c, array2b)) {
                                apply(c, r, array2b,
                                               UArray2b_at(array2b, c, r), cl);
                        /* else we're not at an edge and don't have to worry */
                        } else {
                                apply(c, r, array2b, 
                                               UArray2b_at(array2b, c, r), cl);
                        }
                }
        }
}
