/*=========================================================
Common types and utilties. 
=========================================================*/

#pragma once

/*=========================================================
INCLUDES
=========================================================*/

#include <memory>

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*
Smart pointers - using STL smart pointers
*/

/** Shared pointer */
template <class T>
using sptr = std::shared_ptr<T>;

/** Weak pointer */
template <class T>
using wptr = std::weak_ptr<T>;

/** Unique pointer */
template <class T>
using uptr = std::unique_ptr<T>;

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
