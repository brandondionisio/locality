/**************************************************************
 *
 *                     transformations.h
 *
 *     Assignment: HW 3: locality
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 02/22/24
 *
 *     Summary: Function declarations for the transformations
 *              that can be applied to a PPM image. The functions in this
 *              file are used to rotate, flip, and transpose a PPM image and
 *              are called from another program, called ppmtrans.c
 *              
 **************************************************************/

#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "cputiming.h"

/*****************************************************************
 *                  Rotation Function Declarations
 *****************************************************************/
extern struct Pnm_ppm *rotation_driver(int rotation, A2Methods_T methods, 
                           A2Methods_mapfun *map, Pnm_ppm p6, FILE *time_file);
extern void rotate_90(int col, int row, A2Methods_UArray2 array, void *elem, 
                                                                     void *cl);
extern void rotate_180(int col, int row, A2Methods_UArray2 array, void *elem, 
                                                                     void *cl);
extern void rotate_270(int col, int row, A2Methods_UArray2 array, void *elem, 
                                                                     void *cl);

/*****************************************************************
 *                  Flip Functions Declarations
 *****************************************************************/
extern struct Pnm_ppm *flip_driver(char flip, A2Methods_T methods, 
                           A2Methods_mapfun *map, Pnm_ppm p6, FILE *time_file);

extern void flip_horizontal(int col, int row, A2Methods_UArray2 array, 
                                                         void *elem, void *cl);

extern void flip_vertical(int col, int row, A2Methods_UArray2 array, 
                                                         void *elem, void *cl);

/*****************************************************************
 *                  Transpose Function Declarations
 *****************************************************************/
extern struct Pnm_ppm *transpose_driver(A2Methods_T methods,
                           A2Methods_mapfun *map, Pnm_ppm p6, FILE *time_file);

extern void take_transpose(int col, int row, A2Methods_UArray2 array,
                                                         void *elem, void *cl);


/*****************************************************************
 *                  Helper Function Declarations
 *****************************************************************/
extern CPUTime_T start_timer();

extern void print_timer(double time, FILE *time_file, int width,
                       int height);
#endif