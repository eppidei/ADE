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
    float complex  *sa,
    float complex  *sx,
    int    *incx,
    float complex  *sy,
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

#define scopy FORTRAN_WRAPPER(scopy)
extern void scopy(
    int *n,
    float  *cx,
    int *incx,
    float *cy,
    int *incy
);
#define ccopy FORTRAN_WRAPPER(ccopy)
extern void ccopy(
    int *n,
    float complex *cx,
    int *incx,
    float complex *cy,
    int *incy
);
#define dcopy FORTRAN_WRAPPER(dcopy)
extern void dcopy(
    int *n,
    double  *cx,
    int *incx,
    double *cy,
    int *incy
);
#define zcopy FORTRAN_WRAPPER(zcopy)
extern void zcopy(
    int *n,
    double complex *cx,
    int *incx,
    double complex *cy,
    int *incy
);

/********************************* LEVEL 2 **************************************/
#define ssbmv FORTRAN_WRAPPER(ssbmv)
extern void ssbmv(
        char *uplo,
        int *n,
        int *k,
        float *alpha,
        float *a,
        int *lda,
        float *x,
        int *incx,
        float *beta,
        float *y,
        int* incy
                  );
#define dsbmv FORTRAN_WRAPPER(dsbmv)
extern void dsbmv(
        char *uplo,
        int *n,
        int *k,
        double *alpha,
        double *a,
        int *lda,
        double *x,
        int *incx,
        double *beta,
        double *y,
        int* incy
                  );
#define sger FORTRAN_WRAPPER(sger)
extern void sger(
        int *m,
        int *n,
        float *alpha,
        float *x,
        int *incx,
        float *y,
        int *incy,
        float *a,
        int *lda
        );
#define dger FORTRAN_WRAPPER(dger)
extern void dger(
        int *m,
        int *n,
        double *alpha,
        double *x,
        int *incx,
        double *y,
        int *incy,
        double *a,
        int *lda
        );

/********************************* LEVEL 3 **************************************/
#define sgemm FORTRAN_WRAPPER(sgemm)
extern void sgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        float *alpha,
        float *a,
        int *lda,
        float *b,
        int *ldb,
        float *beta,
        float *c,
        int *ldc
                  );
#define cgemm FORTRAN_WRAPPER(cgemm)
extern void cgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        float complex *alpha,
        float complex *a,
        int *lda,
        float complex *b,
        int *ldb,
        float complex *beta,
        float complex *c,
        int *ldc
                  );
#define dgemm FORTRAN_WRAPPER(dgemm)
extern void dgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        double *alpha,
        double *a,
        int *lda,
        double *b,
        int *ldb,
        double *beta,
        double *c,
        int *ldc
                  );
extern void zgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        double complex*alpha,
        double complex*a,
        int *lda,
        double complex*b,
        int *ldb,
        double complex*beta,
        double complex*c,
        int *ldc
                  );
#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLAS_WRAPPER_H
