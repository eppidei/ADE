#ifndef _ADE_BLAS_WRAPPER_H
#define _ADE_BLAS_WRAPPER_H
#include "headers/ADE_defines.h"
#include "headers/ADE_complex.h"

#if  (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB )


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

#define sdot FORTRAN_WRAPPER(sdot)

extern float sdot(
int *n,
void* sx,
int *incx,
void *sy,
int *incy
);

#define cdotu FORTRAN_WRAPPER(cdotu)

extern ADE_CPLX_T cdotu(
int *n,
void* sx,
int *incx,
void *sy,
int *incy
);

#define cdotc FORTRAN_WRAPPER(cdotc)

extern ADE_CPLX_T cdotc(
int *n,
void* sx,
int *incx,
void *sy,
int *incy
);



#define zdotu FORTRAN_WRAPPER(zdotu)

extern ADE_CPLX_T zdotu(
int *n,
void* sx,
int *incx,
void *sy,
int *incy
);

#define zdotc FORTRAN_WRAPPER(zdotc)

extern ADE_CPLX_T zdotc(
int *n,
void* sx,
int *incx,
void *sy,
int *incy
);

#define ddot FORTRAN_WRAPPER(ddot)

extern double ddot(
int *n,
void* sx,
int *incx,
void *sy,
int *incy
);

#define snrm2 FORTRAN_WRAPPER(snrm2)

extern double snrm2(
int *n,
void* x,
int *incx
);

#define dnrm2 FORTRAN_WRAPPER(dnrm2)

extern double dnrm2(
int *n,
void* x,
int *incx
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
#define zgemm FORTRAN_WRAPPER(zgemm)
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



#endif


#endif //_ADE_BLAS_WRAPPER_H
