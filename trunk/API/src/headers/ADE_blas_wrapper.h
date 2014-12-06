#ifndef _ADE_BLAS_WRAPPER_H
#define _ADE_BLAS_WRAPPER_H
#include "headers/ADE_defines.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define saxpy FORTRAN_WRAPPER(saxpy)
extern void saxpy(
    int    *n,
    float  *sa,
    float  *sx,
    int    *incx,
    float  *sy,
    int    *incy
);


#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLAS_WRAPPER_H
