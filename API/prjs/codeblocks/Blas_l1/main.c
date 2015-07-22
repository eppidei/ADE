#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Utils.h"


/*
#define ADE_FP_PRECISION ADE_USE_SINGLE_PREC
#define ADE_TARGET_MODE ADE_RELEASE
#define ADE_BLAS_IMPLEMENTATION ADE_USE_BLAS_CUSTOM
*/

int main()
{

ADE_blas_level1_T *p_bl1=NULL;
int array_len =0;
int array_maxlen_test =100;
int lenstep_test =10;
float *p_in_buff=NULL,*p_out_buff=NULL,*p_result_buff=NULL;
float memset_val=1.0;
int k=0,i=0,vec_idx=0;
int n_tests = ceil(array_maxlen_test/lenstep_test);


 p_result_buff=(float*)calloc(n_tests,sizeof(float));
ADE_Blas_level1_Init(&p_bl1,ADE_MATH_REAL);

for (array_len=1;array_len<array_maxlen_test;array_len+=lenstep_test)
{
 /*** In-Out allocation ****/
    p_in_buff=(float*)calloc(array_len,sizeof(float));
    p_out_buff=(float*)calloc(array_len,sizeof(float));

/** Blas object configuration ***/
    ADE_Blas_level1_configure_dot(p_bl1,1,1,array_len,(ADE_FLOATING_T*)p_in_buff,(ADE_FLOATING_T*)p_in_buff);
    for (vec_idx=0; vec_idx<array_len; vec_idx++)
    {
        p_in_buff[vec_idx]=memset_val;
    }
/*** Dot product *******/
    p_result_buff[k]=ADE_Blas_level1_dot(p_bl1);

/*** Free buffers ****/
    free(p_in_buff);
    free(p_out_buff);

    k++;

}

ADE_Blas_level1_Release(p_bl1);

PRINT_ARRAY(p_result_buff,i,n_tests,"%f");



 free(p_result_buff);

return 0;
}
