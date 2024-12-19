/**************************************************************
 *
 *                     ppmtrans.c
 *
 *     Assignment: HW 3: locality
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 02/22/24
 *
 *     Summary: Interface for ppmtrans.c, a program that reads a PPM image
 *              and applies transformations to it, such as rotation, flip, and
 *              transpose. The program can also time the transformations and
 *              output the time to a file. This program is modelled on
 *              jpegtran and offers a subset of its functionality.
 *              
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "transformations.h"
#include "cputiming.h"

/* declaration for open_or_die function */
static FILE *open_or_die(char *fname, char *mode);

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] "
                       "[-time time_file] "
                        "[filename]\n",
                        progname);
        exit(1);
}

/****************** main *******************
 * 
 * Handles the querying and function calling for the ppmtrans program
 *
 * Parameters:
 *         int argc:   number of arguments passed into the program
 *      char *argv[]:  pointer to a char array that holds the arguments passed
 *                     into the function
 * Returns:
 *      an integer deciding the success of the completion of the program
 * Expects:
 *      uarray2_methods_plain from a2methods is not NULL (throw CRE if NULL)
 *      map_default from a2_methods_plain is not NULL (throw CRE if NULL)
 *      p6 the Pnm_ppm reader is not NULL when initialized (throw CRE if NULL)
 *
 ********************************************/
int main(int argc, char *argv[])
{
        FILE *fp              = NULL;
        char *time_file_name  = NULL;
        FILE *time_file       = NULL;
        int rotation          = 0;
        char flip             = ' ';
        bool transpose        = false;
        int i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        } else if (rotation == 90) {
                                rotation = 90;
                        } else if (rotation == 180) {
                                rotation = 180;
                        } else if (rotation == 270) {
                                rotation = 270;
                        }
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no flip specified */
                                usage(argv[0]);
                        }
                        char *flip_in = argv[++i];
                        if (strcmp(flip_in, "horizontal") != 0 && 
                                            strcmp(flip_in, "vertical") != 0) {
                                fprintf(stderr, 
                                      "Flip must be horizontal or vertical\n");
                                usage(argv[0]);
                        }
                        if (strcmp(flip_in, "horizontal") == 0) {
                                flip = 'h';
                        } else if (strcmp(flip_in, "vertical") == 0) {
                                flip = 'v';
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        transpose = true;
                } else if (strcmp(argv[i], "-time") == 0) {
                        if (!(i + 1 < argc)) {      /* no time file */
                                usage(argv[0]);
                        }
                        /* Save time file name */
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        /* The last argument is the file name */
                        fp = open_or_die(argv[i], "r");
                }
        }

        /* If no file has been provided, read from standard input */
        if (fp == NULL) {
                fp = stdin;
        }

        /* Check and open time file, if already provided above */
        if (time_file_name != NULL) {
                time_file = open_or_die(time_file_name, "w");
        }

        /* Read the file and store the image */
        Pnm_ppm p6 = Pnm_ppmread(fp, methods);
        assert(p6 != NULL);

        /* Time to start rotating */
        if (!transpose && flip == ' ') {
                p6 = rotation_driver(rotation, methods, map, p6, time_file);
        }
        
        /* Time to start flipping */
        p6 = flip_driver(flip, methods, map, p6, time_file);

        /* Time to transpose */
        if (transpose) {
                p6 = transpose_driver(methods, map, p6, time_file);
        }

        /* Write pixelmap to standard output */
        Pnm_ppmwrite(stdout, p6);

        /* Close the input file, if provided */
        if (fp != stdin) {
                fclose(fp);
        }

        /* Close the time file, if provided */
        if (time_file != NULL) {
                fclose(time_file);
        }

        /* Free the ppm map */
        Pnm_ppmfree(&p6);

        return EXIT_SUCCESS;
}

/************** FILE *open_or_die *************
 * 
 * Opens a file or exits with an error message if the file cannot be opened.
 *
 * Parameters:
 *      char *fname: name of the file to open
 *      char *mode:  mode to open the file in
 * Returns:
 *      FILE *: file pointer to the opened file
 * Expects:
 *      Valid file name and mode are passed in. If the file cannot be opened,
 *     the program will exit with an error message and an exit faulure status.
 *
 ********************************************/
static FILE *open_or_die(char *fname, char *mode)
{
        FILE *fp = fopen(fname, mode);
        if (fp == NULL) {
                fprintf(stderr, "Error: Could not open file %s\n", fname);
                exit(EXIT_FAILURE);
        }
        return fp;
}
