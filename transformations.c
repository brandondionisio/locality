/**************************************************************
 *
 *                     transformations.c
 *
 *     Assignment: HW 3: locality
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 02/22/24
 *
 *     Summary: This file implements the functions for the transformations
 *              that can be applied to a PPM image. The functions in this
 *              file are used to rotate, flip, and transpose a PPM image and
 *              are called from another program, called ppmtrans.c. Additional
 *              functionalities are included to time the transformations and 
 *              output the time to a file. Helper functions are also included
 *              to execute specific rotations, flips, and transpositions.
 *              
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "mem.h"
#include "pnm.h"
#include "cputiming.h"
#include "transformations.h"

typedef A2Methods_UArray2 A2; /* private abbreviation */

/********** trans_closure ********
 * 
 * Struct for the closure pointer that will be passed to the map function.
 * The closure struct contains the new array that will store the transformed
 * pixels and the methods object that will be used to access the array.
 *
 *******************/
typedef struct trans_closure {
        A2 new_array; /* New array to store transformed pixels */
        A2Methods_T methods; /* Methods object */
} *trans_closure;

/****************** rotation_driver *******************
 * 
 * Function to apply a rotation to a PPM image. The function will apply a
 * rotation of 90, 180, or 270 degrees to the image and return the modified
 * PPM. The function will also time the transformation and output the time to
 * a file if the time_file is not NULL.
 *
 * Parameters:
 *            int rotation: integer representing the rotation to be applied
 *     A2Methods_T methods: methods object to be used to access the array
 *   A2Methods_mapfun *map: map function to be used to apply the transformation
 *              Pnm_ppm p6: PPM image to be transformed
 *         FILE *time_file: file to output the time of the transformation
 * Returns:
 *    The modified PPM image after the appropriate rotation has been applied
 * Expects:
 *    The passed-in rotation values will be either 0, 90, 180, or 270. 
 *    The methods object will not be NULL (throws a CRE if NULL).
 *    The map function will not be NULL (throws a CRE if NULL).
 *    The PPM image will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern struct Pnm_ppm *rotation_driver(int rotation, A2Methods_T methods, 
                        A2Methods_mapfun *map, Pnm_ppm p6, FILE *time_file)
{
        assert(methods != NULL);
        assert(map != NULL);
        assert(p6 != NULL);

        /* Start the clock */
        CPUTime_T timer = start_timer();
                
        if (rotation == 90) {
                /* Create a new closure struct */
                trans_closure cl;
                NEW(cl);

                /* Fetch dimensions of original array */
                int width = methods->width(p6->pixels);
                int height = methods->height(p6->pixels);

                /* Declare a new swapped dimension array */
                A2 new_arr = methods->new(height, width, 
                                                       sizeof(struct Pnm_rgb));
                
                /* Populate the closure struct with new array and time file */
                cl->new_array = new_arr;
                cl->methods = methods;

                /* Map the original array onto the new array, 
                        applying the rotate 90 function */
                map(p6->pixels, rotate_90, cl);

                /* Deallocate the old pixel array */
                methods->free(&(p6->pixels));

                /* Set the new pixel array to the new array and dimensions */
                p6->pixels = new_arr;
                p6->width = height;
                p6->height = width;

                /* Stop the clock and calculate the CPU time */ 
                double time_used = CPUTime_Stop(timer);
                print_timer(time_used, time_file, width, height);

                /* Free the closure struct */
                FREE(cl);

        } else if (rotation == 180) {
                /* Create a new closure struct */
                trans_closure cl;
                NEW(cl);

                /* Fetch dimensions of original array */
                int width = methods->width(p6->pixels);
                int height = methods->height(p6->pixels);

                /* Declare a new array of same dimensions */
                A2 new_arr = methods->new(width, height, 
                                                       sizeof(struct Pnm_rgb));

                /* Populate the closure struct with new array and time file */
                cl->new_array = new_arr;
                cl->methods = methods;

                /* Map over the array, applying the rotate 180 function */
                map(p6->pixels, rotate_180, cl);

                /* Deallocate the old pixel array */
                methods->free(&(p6->pixels));

                /* Set the new pixel array to the new array */
                p6->pixels = new_arr;

                /* Stop the clock and calculate the CPU time */
                double time_used = CPUTime_Stop(timer);
                print_timer(time_used, time_file, width, height);

                /* Free the closure struct */
                FREE(cl);
        } else if (rotation == 270) {
                /* Create a new closure struct */
                trans_closure cl;
                NEW(cl);

                /* Fetch dimensions of original array */
                int width = methods->width(p6->pixels);
                int height = methods->height(p6->pixels);

                /* Declare a new swapped dimension array */
                A2 new_arr = methods->new(height, width, 
                                                       sizeof(struct Pnm_rgb));
                
                /* Populate the closure struct with new array and time file */
                cl->new_array = new_arr;
                cl->methods = methods;

                /* Map the original array onto the new array, 
                        applying the rotate 90 function */
                map(p6->pixels, rotate_270, cl);

                /* Deallocate the old pixel array */
                methods->free(&(p6->pixels));

                /* Set the new pixel array to the new array and dimensions */
                p6->pixels = new_arr;
                p6->width = height;
                p6->height = width;

                /* Stop the clock and calculate the CPU time */
                double time_used = CPUTime_Stop(timer);
                print_timer(time_used, time_file, width, height);

                /* Free the closure struct */
                FREE(cl);

        } else if (rotation == 0) {
                /* Fetch dimensions of original array */
                int width = methods->width(p6->pixels);
                int height = methods->height(p6->pixels);

                /* Stop the clock and calculate the CPU time */
                double time_used = CPUTime_Stop(timer);
                print_timer(time_used, time_file, width, height);
        }
        /* Free the timer */
        CPUTime_Free(&timer);

        /* Return the modified PPM */
        return p6;
}

/****************** rotate_90 *******************
 * 
 * Function to apply a 90 degree rotation to a PPM image. The function will
 * be called by the map function and will apply the rotation to the pixel, 
 * saving the pixel to the rotated spot in the new array.
 *
 * Parameters:
 *            int col:      column index of the pixel
 *            int row:      row index of the pixel
 * A2Methods_UArray2 array: array to be transformed
 *     void *elem:          pointer to the pixel to be transformed
 *       void *cl:          closure pointer to struct of new array and 
 *                          methods object
 * Returns:
 *    Nothing
 * Expects:
 *    The array will not be NULL (throws a CRE if NULL).
 *    The elem pointer will not be NULL (throws a CRE if NULL).
 *    The closure pointer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern void rotate_90(int col, int row, A2 array, void *elem, void *cl) 
{
        assert(array != NULL);
        assert(elem != NULL);
        assert(cl != NULL);

        /* From the closure struct, dereference to obtain the A2 object */
        trans_closure closure = (trans_closure)cl;
        A2 new_arr = closure->new_array;

        /* Dereference the methods object */
        A2Methods_T methods = closure->methods;

        /* Fetch the height of the original array */
        int org_height = methods->height(array);

        /* Save the pixel to the rotated spot in the new array */
        struct Pnm_rgb *new_elem = methods->at(new_arr, 
                                                    org_height - row - 1, col);
        *new_elem = *(struct Pnm_rgb *)elem;
}

/****************** rotate_180 *******************
 * 
 * Function to apply a 180 degree rotation to a PPM image. The function will
 * be called by the map function and will apply the rotation to the pixel, 
 * saving the pixel to the rotated spot in the new array.
 *
 * Parameters:
 *            int col:      column index of the pixel
 *            int row:      row index of the pixel
 * A2Methods_UArray2 array: array to be transformed
 *     void *elem:          pointer to the pixel to be transformed
 *       void *cl:          closure pointer to struct of new array and 
 *                          methods object
 * Returns:
 *    Nothing
 * Expects:
 *    The array will not be NULL (throws a CRE if NULL).
 *    The elem pointer will not be NULL (throws a CRE if NULL).
 *    The closure pointer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern void rotate_180(int col, int row, A2 array, void *elem, void *cl) 
{
        /* Check for NULL pointers */
        assert(array != NULL);
        assert(elem != NULL);
        assert(cl != NULL);
        
        /* From the closure struct, dereference to obtain the A2 object */
        trans_closure closure = (trans_closure)cl;
        A2 new_arr = closure->new_array;

        /* Dereference the methods object */
        A2Methods_T methods = closure->methods;

        /* Fetch the height and width of the original array */
        int width = methods->width(array);
        int height = methods->height(array);
        
        /* Save the pixel to the rotated spot in the new array */
        struct Pnm_rgb *new_elem = methods->at(new_arr, width - col - 1, 
                                                             height - row - 1);
        *new_elem = *(struct Pnm_rgb *)elem;
}

/****************** rotate_270 *******************
 * 
 * Function to apply a 270 degree rotation to a PPM image. The function will
 * be called by the map function and will apply the rotation to the pixel, 
 * saving the pixel to the rotated spot in the new array.
 *
 * Parameters:
 *            int col:      column index of the pixel
 *            int row:      row index of the pixel
 * A2Methods_UArray2 array: array to be transformed
 *     void *elem:          pointer to the pixel to be transformed
 *       void *cl:          closure pointer to struct of new array and 
 *                          methods object
 * Returns:
 *    Nothing
 * Expects:
 *    The array will not be NULL (throws a CRE if NULL).
 *    The elem pointer will not be NULL (throws a CRE if NULL).
 *    The closure pointer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern void rotate_270(int col, int row, A2 array, void *elem, void *cl) 
{
        /* Check for NULL pointers */
        assert(array != NULL);
        assert(elem != NULL);
        assert(cl != NULL);

        /* From the closure struct, dereference to obtain the A2 object */
        trans_closure closure = (trans_closure)cl;
        A2 new_arr = closure->new_array;

        /* Dereference the methods object */
        A2Methods_T methods = closure->methods;

        /* Fetch the height of the original array */
        int org_width = methods->width(array);

        /* Save the pixel to the rotated spot in the new array */
        struct Pnm_rgb *new_elem = methods->at(new_arr, row, 
                                                          org_width - col - 1);
        *new_elem = *(struct Pnm_rgb *)elem;
}

/****************** flip_driver *******************
 * 
 * Function to apply a flip to a PPM image. The function will apply a
 * horizontal or vertical flip to the image and return the modified PPM. The
 * function will also time the transformation and output the time to a file if
 * the time_file is not NULL.
 *
 * Parameters:
 *               char flip: character representing the flip to be applied
 *     A2Methods_T methods: methods object to be used to access the array
 *   A2Methods_mapfun *map: map function to be used to apply the transformation
 *              Pnm_ppm p6: PPM image to be transformed
 *         FILE *time_file: file to output the time of the transformation
 * Returns:
 *    The modified PPM image after the appropriate flip has been applied
 * Expects:
 *    The passed-in flip values will be either 'h' or 'v'. 
 *    The methods object will not be NULL (throws a CRE if NULL).
 *    The map function will not be NULL (throws a CRE if NULL).
 *    The PPM image will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern struct Pnm_ppm *flip_driver(char flip, A2Methods_T methods, 
                            A2Methods_mapfun *map, Pnm_ppm p6, FILE *time_file) 
{
        /* Check for NULL pointers */
        assert(methods != NULL);
        assert(map != NULL);
        assert(p6 != NULL);
       
        /* Start the clock */
        CPUTime_T timer = start_timer();

        /* Apply the horizontal flip */
        if (flip == 'h') {
                /* Create a new closure struct */
                trans_closure cl;
                NEW(cl);

                /* Fetch dimensions of original array */
                int width = methods->width(p6->pixels);
                int height = methods->height(p6->pixels);

                /* Declare a new swapped dimension array */
                A2 new_arr = methods->new(width, height,
                                                       sizeof(struct Pnm_rgb));
                
                /* Populate the closure struct with new array and time file */
                cl->new_array = new_arr;
                cl->methods = methods;

                /* Map the original array onto the new array, 
                        applying the flip horizontal function */
                map(p6->pixels, flip_horizontal, cl);

                /* Deallocate the old pixel array */
                methods->free(&(p6->pixels));

                /* Set the new pixel array to the new array */
                p6->pixels = new_arr;

                /* Free the closure struct */
                FREE(cl);

                /* Stop the clock and calculate the CPU time */
                double time_used = CPUTime_Stop(timer);
                print_timer(time_used, time_file, width, height);

        /* Apply the vertical flip */
        } else if (flip == 'v') {
                /* Create a new closure struct */
                trans_closure cl;
                NEW(cl);

                /* Fetch dimensions of original array */
                int width = methods->width(p6->pixels);
                int height = methods->height(p6->pixels);

                /* Declare a new array of same size as original */
                A2 new_arr = methods->new(width, height,
                                                       sizeof(struct Pnm_rgb));

                /* Populate the closure struct with new array and time file */
                cl->new_array = new_arr;
                cl->methods = methods;

                /* Map the original array onto the new array, 
                        applying the flip vertical */
                map(p6->pixels, flip_vertical, cl);

                /* Deallocate the old pixel array */
                methods->free(&(p6->pixels));

                /* Set the new pixel array to the new array */
                p6->pixels = new_arr;

                /* Free the closure struct */
                FREE(cl);

                /* Stop the clock and calculate the CPU time */
                double time_used = CPUTime_Stop(timer);
                print_timer(time_used, time_file, width, height);
        }
        /* Free the timer */
        CPUTime_Free(&timer);

        /* Return the modified PPM */
        return p6;
}

/****************** flip_horizontal *******************
 * 
 * Function to apply a horizontal flip to a PPM image. The function will be
 * called by the map function and will apply the flip to the pixel, saving the
 * pixel to the flipped spot in the new array.
 *
 * Parameters:
 *            int col:      column index of the pixel
 *            int row:      row index of the pixel
 * A2Methods_UArray2 array: array to be transformed
 *     void *elem:          pointer to the pixel to be transformed
 *       void *cl:          closure pointer to struct of
 *                          new array and methods object
 * Returns:
 *    Nothing
 * Expects:
 *    The array will not be NULL (throws a CRE if NULL).
 *    The elem pointer will not be NULL (throws a CRE if NULL).
 *    The closure pointer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern void flip_horizontal(int col, int row, A2 array, void *elem, void *cl) 
{
        /* Check for NULL pointers */
        assert(array != NULL);
        assert(elem != NULL);
        assert(cl != NULL);

        /* From the closure struct, dereference to obtain the A2 object */
        trans_closure closure = (trans_closure)cl;
        A2 new_arr = closure->new_array;

        /* Dereference the methods object */
        A2Methods_T methods = closure->methods;

        /* Fetch the height of the original array */
        int org_width = methods->width(array);

        /* Save the pixel to the flipped spot in the new array */
        struct Pnm_rgb *new_elem = methods->at(new_arr, 
                                                     org_width - col - 1, row);
        *new_elem = *(struct Pnm_rgb *)elem;
}

/****************** flip_vertical *******************
 * 
 * Function to apply a vertical flip to a PPM image. The function will be
 * called by the map function and will apply the flip to the pixel, saving the
 * pixel to the flipped spot in the new array.
 *
 * Parameters:
 *            int col:      column index of the pixel
 *            int row:      row index of the pixel
 * A2Methods_UArray2 array: array to be transformed
 *     void *elem:          pointer to the pixel to be transformed
 *       void *cl:          closure pointer to struct of new array and
 *                          methods object
 * Returns:
 *    Nothing
 * Expects:
 *    The array will not be NULL (throws a CRE if NULL).
 *    The elem pointer will not be NULL (throws a CRE if NULL).
 *    The closure pointer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern void flip_vertical(int col, int row, A2 array, void *elem, void *cl) 
{
        /* Check for NULL pointers */
        assert(array != NULL);
        assert(elem != NULL);
        assert(cl != NULL);

        /* From the closure struct, dereference to obtain the A2 object */
        trans_closure closure = (trans_closure)cl;
        A2 new_arr = closure->new_array;

        /* Dereference the methods object */
        A2Methods_T methods = closure->methods;

        /* Fetch the height of the original array */
        int org_height = methods->height(array);

        /* Save the pixel to the flipped spot in the new array */
        struct Pnm_rgb *new_elem = methods->at(new_arr, col, 
                                                         org_height - row - 1);
        *new_elem = *(struct Pnm_rgb *)elem;
}

/****************** transpose_driver *******************
 * 
 * Function to apply a transpose to a PPM image. The function will transpose
 * the image and return the modified PPM. The function will also time the
 * transformation and output the time to a file if the time_file is not NULL.
 *
 * Parameters:
 *     A2Methods_T methods: methods object to be used to access the array
 *   A2Methods_mapfun *map: map function to be used to apply the transformation
 *              Pnm_ppm p6: PPM image to be transformed
 *         FILE *time_file: file to output the time of the transformation
 * Returns:
 *    The modified PPM image after the transpose has been applied
 * Expects:
 *    The methods object will not be NULL (throws a CRE if NULL).
 *    The map function will not be NULL (throws a CRE if NULL).
 *    The PPM image will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern struct Pnm_ppm *transpose_driver(A2Methods_T methods, 
                            A2Methods_mapfun *map, Pnm_ppm p6, FILE *time_file)
{
        /* Check for NULL pointers */
        assert(methods != NULL);
        assert(map != NULL);
        assert(p6 != NULL);

        /* Start the clock */
        CPUTime_T timer = start_timer();

        /* Create a new closure struct */
        trans_closure cl;
        NEW(cl);

        /* Fetch dimensions of original array */
        int width = methods->width(p6->pixels);
        int height = methods->height(p6->pixels);

        /* Declare a new swapped dimension array */
        A2 new_arr = methods->new(height, width, sizeof(struct Pnm_rgb));

        /* Populate the closure struct with new array and time file */
        cl->new_array = new_arr;
        cl->methods = methods;

        /* Map the original array onto the new array, 
                applying the rotate 90 function */
        map(p6->pixels, take_transpose, cl);

        /* Deallocate the old pixel array */
        methods->free(&(p6->pixels));

        /* Set the new pixel array to the new array and dimensions */
        p6->pixels = new_arr;
        p6->width = height;
        p6->height = width;

        /* Free the closure struct */
        FREE(cl);

        /* Stop the clock and calculate the CPU time */
        double time_used = CPUTime_Stop(timer);
        print_timer(time_used, time_file, width, height);

        /* Free the timer */
        CPUTime_Free(&timer);

        /* Return the modified PPM */
        return p6;
}

/****************** take_transpose *******************
 * 
 * Function to apply a transpose to a PPM image. The function will be called
 * by the map function and will apply the transpose to the pixel, saving the
 * pixel to the transposed spot in the new array.
 *
 * Parameters:
 *            int col:      column index of the pixel
 *            int row:      row index of the pixel
 * A2Methods_UArray2 array: array to be transformed
 *     void *elem:          pointer to the pixel to be transformed
 *       void *cl:          closure pointer to struct of new array 
 *                          and methods object
 * Returns:
 *    Nothing
 * Expects:
 *    The elem pointer will not be NULL (throws a CRE if NULL).
 *    The closure pointer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern void take_transpose(int col, int row, A2Methods_UArray2 array,
                                                          void *elem, void *cl)
{
        (void) array; /* Array is not needed for this transformation */

        /* Check for NULL pointers */
        assert(elem != NULL);
        assert(cl != NULL);

        /* From the closure struct, dereference to obtain the A2 object */
        trans_closure closure = (trans_closure)cl;
        A2 new_arr = closure->new_array;

        /* Dereference the methods object */
        A2Methods_T methods = closure->methods;

        /* Save the pixel to the transposed spot in the new array */
        struct Pnm_rgb *new_elem = methods->at(new_arr, row, col);
        *new_elem = *(struct Pnm_rgb *)elem;
}

/****************** start_timer *******************
 * 
 * Function to start the clock and return the timer.
 *
 * Parameters:
 *    Nothing
 * Returns:
 *    The timer that has been started
 * Expects:
 *    The timer will not be NULL (throws a CRE if NULL).
 *
 ********************************************/
extern CPUTime_T start_timer()
{
        /* Start the clock */
        CPUTime_T timer;
        timer = CPUTime_New();
        CPUTime_Start(timer);

        /* Check for NULL pointer */
        assert(timer != NULL);

        return timer;
}

/****************** print_timer *******************
 * 
 * Function to print the time to a file and output the time per pixel to the
 * file.
 *
 * Parameters:
 *             double time:  time to be printed
 *         FILE *time_file:  file to output the time
 *               int width:  width of the image
 *              int height:  height of the image
 * Returns:
 *    Nothing
 * Expects:
 *    The time_file will not be NULL. If not, function will not do anything.
 *
 ********************************************/
extern void print_timer(double time, FILE *time_file, int width, int height)
{
        /* Stop the clock and calculate the CPU time */
        if (time_file != NULL) {
                fprintf(time_file, 
                "CPU time for transformation: %f nanoseconds\n", time);
                fprintf(time_file, "Time per pixel: %fs\n",
                        time / (width * height));
        }
}
