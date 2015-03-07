#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "headers/ADE_nrutil.h"
#define NR_END 1 //safety extra storage at the beginning of the array
#define FREE_ARG char*
ADE_VOID_T nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{

    fprintf(stderr,"Numerical Recipes run-time error...\n");
    fprintf(stderr,"%s\n",error_text);
    fprintf(stderr,"...now exiting to system...\n");
    exit(1);

}

ADE_INT32_T *ivector(ADE_LONG_T nl, ADE_LONG_T nh)
/* allocate an ADE_INT32_T vector with subscript range v[nl..nh] */
{
    ADE_INT32_T *v;
    v=(ADE_INT32_T *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(ADE_INT32_T)));
    if (!v) nrerror("allocation failure in ivector()");
    return v-nl+NR_END;
}




ADE_VOID_T free_ivector(ADE_INT32_T *v, ADE_LONG_T nl, ADE_LONG_T nh)
/* free an ADE_INT32_T vector allocated with ivector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}

