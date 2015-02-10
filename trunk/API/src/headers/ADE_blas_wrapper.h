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
    void  *sa,
    void  *sx,
    int    *incx,
    void  *sy,
    int    *incy
);

#define caxpy FORTRAN_WRAPPER(caxpy)
extern void caxpy(
    int    *n,
    void  *sa,
    void  *sx,
    int    *incx,
    void  *sy,
    int    *incy
);


#define daxpy FORTRAN_WRAPPER(daxpy)
extern void daxpy(
    int    *n,
    void  *sa,
    void  *sx,
    int    *incx,
    void  *sy,
    int    *incy
);

#define zaxpy FORTRAN_WRAPPER(zaxpy)
extern void zaxpy(
    int    *n,
    void *sa,
    void *sx,
    int    *incx,
    void *sy,
    int    *incy
);

#define scopy FORTRAN_WRAPPER(scopy)
extern void scopy(
    int *n,
    void  *cx,
    int *incx,
    void *cy,
    int *incy
);
#define ccopy FORTRAN_WRAPPER(ccopy)
extern void ccopy(
    int *n,
    void *cx,
    int *incx,
    void *cy,
    int *incy
);
#define dcopy FORTRAN_WRAPPER(dcopy)
extern void dcopy(
    int *n,
    void  *cx,
    int *incx,
    void *cy,
    int *incy
);
#define zcopy FORTRAN_WRAPPER(zcopy)
extern void zcopy(
    int *n,
    void *cx,
    int *incx,
    void *cy,
    int *incy
);

/********************************* LEVEL 2 **************************************/
#define ssbmv FORTRAN_WRAPPER(ssbmv)
extern void ssbmv(
        char *uplo,
        int *n,
        int *k,
        void *alpha,
        void *a,
        int *lda,
        void *x,
        int *incx,
        void *beta,
        void *y,
        int* incy
                  );
#define dsbmv FORTRAN_WRAPPER(dsbmv)
extern void dsbmv(
        char *uplo,
        int *n,
        int *k,
        void *alpha,
        void *a,
        int *lda,
        void *x,
        int *incx,
        void *beta,
        void *y,
        int* incy
                  );
#define sger FORTRAN_WRAPPER(sger)
extern void sger(
        int *m,
        int *n,
        void *alpha,
        void *x,
        int *incx,
        void *y,
        int *incy,
        void *a,
        int *lda
        );
#define dger FORTRAN_WRAPPER(dger)
extern void dger(
        int *m,
        int *n,
        void *alpha,
        void *x,
        int *incx,
        void *y,
        int *incy,
        void *a,
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
        void *alpha,
        void *a,
        int *lda,
        void *b,
        int *ldb,
        void *beta,
        void *c,
        int *ldc
                  );
#define cgemm FORTRAN_WRAPPER(cgemm)
extern void cgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        void *alpha,
        void *a,
        int *lda,
        void *b,
        int *ldb,
        void *beta,
        void *c,
        int *ldc
                  );
#define dgemm FORTRAN_WRAPPER(dgemm)
extern void dgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        void *alpha,
        void *a,
        int *lda,
        void *b,
        int *ldb,
        void *beta,
        void *c,
        int *ldc
                  );
extern void zgemm(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        void*alpha,
        void*a,
        int *lda,
        void*b,
        int *ldb,
        void*beta,
        void*c,
        int *ldc
                  );
#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLAS_WRAPPER_H
