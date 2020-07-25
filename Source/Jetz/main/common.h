/*=========================================================
Common types and utilties. 
=========================================================*/

#pragma once

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Determines the number of elements in an array. Array size must be
known at compile time.
@param arr The array to get the number of elements in.
*/
#define cnt_of_array(arr)	( sizeof(arr) / sizeof((arr)[0]) )

#ifndef max
#define max( a, b ) ( ((a)>(b))?(a):(b) )
#endif

#ifndef min
#define min( a, b ) ( ((a)<(b))?(a):(b) )
#endif
