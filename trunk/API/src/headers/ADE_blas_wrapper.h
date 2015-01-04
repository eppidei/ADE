#ifndef _ADE_BLAS_WRAPPER_H
#define _ADE_BLAS_WRAPPER_H
#include "headers/ADE_defines.h"

#ifdef __cplusplus
    extern "C" {
#endif

/************************************* LEVEL 1 ****************************************/

#define saxpy FORTRAN_WRAPPER(saxpy)
extern void saxpy(
    int    *n,
    float  *sa,
    float  *sx,
    int    *incx,
    float  *sy,
    int    *incy
);

#define caxpy FORTRAN_WRAPPER(caxpy)
extern void caxpy(
    int    *n,
    complex  *sa,
    float  *sx,
    int    *incx,
    complex  *sy,
    int    *incy
);


#define daxpy FORTRAN_WRAPPER(daxpy)
extern void daxpy(
    int    *n,
    double  *sa,
    double  *sx,
    int    *incx,
    double  *sy,
    int    *incy
);

#define zaxpy FORTRAN_WRAPPER(zaxpy)
extern void zaxpy(
    int    *n,
    double complex *sa,
    double complex *sx,
    int    *incx,
    double complex *sy,
    int    *incy
);

/********************************* LEVEL 2 **************************************/
#define sger FORTRAN_WRAPPER(sger)
extern void sger(
        int *M,
        int *N,
        float *alpha,
        float *X,
        int *incX,
        float *Y,
        int *incY,
        float *A,
        int *lda
        );
#define dger FORTRAN_WRAPPER(dger)
extern void dger(
        int *M,
        int *N,
        double *alpha,
        double *X,
        int *incX,
        double *Y,
        int *incY,
        double *A,
        int *lda
        );

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLAS_WRAPPER_H
