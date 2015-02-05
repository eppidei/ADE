#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_blas_level3.h"

int blas3_configuration(ADE_blas_level3_T* p_Blas_l3,ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,ADE_INT32_T M,
                         ADE_INT32_T N,ADE_INT32_T K,ADE_INT32_T LDA,ADE_INT32_T LDB,ADE_INT32_T LDC,
                         ADE_FLOATING_T *p_alpha,ADE_FLOATING_T *p_beta,ADE_FLOATING_T *p_A,ADE_FLOATING_T *p_B,ADE_FLOATING_T *p_C)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    ret= ADE_Blas_Level3_SetTransA(p_Blas_l3, TRANSA);
ret= ADE_Blas_Level3_SetTransB(p_Blas_l3, TRANSB);
ADE_Blas_Level3_SetM(p_Blas_l3, M);
ADE_Blas_Level3_SetN( p_Blas_l3, N);
ADE_Blas_Level3_SetK( p_Blas_l3, K);
ADE_Blas_Level3_SetLda(p_Blas_l3, LDA);
ADE_Blas_Level3_SetLdb(p_Blas_l3, LDB);
ADE_Blas_Level3_SetLdc(p_Blas_l3, LDC);
ret= ADE_Blas_Level3_SetA(p_Blas_l3, p_A);
ret= ADE_Blas_Level3_SetB( p_Blas_l3, p_B);
ret= ADE_Blas_Level3_SetC( p_Blas_l3, p_C);
ADE_Blas_Level3_SetAlpha( p_Blas_l3, p_alpha);
ADE_Blas_Level3_SetBeta( p_Blas_l3, p_beta);

return 0;
}

ADE_INT32_T blas3_test_procedure(ADE_BENCH_T *test_cases,ADE_UINT32_T n_tests,ADE_BENCH_MAT_T *mat_type,ADE_UINT32_T n_types,ADE_INT32_T *n_rows_A,ADE_INT32_T *n_cols_A,ADE_INT32_T *n_cols_B,ADE_INT32_T n_dim_cases)
{
    ADE_blas_level3_T *p_blas_l3=NULL;
    unsigned int test_idx=0,type_idx=0,dim_cases_idx=0;
    ADE_FLOATING_T *p_A=NULL,*p_B=NULL,*p_C=NULL;
    ADE_CHAR_T TRANSA='N';
    ADE_CHAR_T TRANSB='N';
    ADE_FLOATING_T ALPHA[2]={1.0,1.0};
    ADE_INT32_T LDA=0;
    ADE_INT32_T LDB=0;
    ADE_FLOATING_T BETA[2]={1.0,1.0};
    ADE_INT32_T LDC=0;


/*** cicla tests***/
    for (test_idx=0;test_idx<n_tests;test_idx++)
    {
        if (test_cases[test_idx]==ADE_BLAS_L3_BM)
        {
            /*** cicla real/cplx***/
            for (type_idx=0;type_idx<n_types;type_idx++)
            {
                if (mat_type[type_idx]==ADE_BENCH_REAL)
                {
                     ADE_Blas_level3_Init(&p_blas_l3,ADE_REAL );
                     /*** cicla dimensioni***/
                        for (dim_cases_idx=0;dim_cases_idx<n_dim_cases;dim_cases_idx++)
                        {

                            /**** matrix alloc********/
                            p_A=calloc(1,n_rows_A[dim_cases_idx]*n_cols_A[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                            if (p_A==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_A,"%p",blas3_test_procedure);
                                return -1;
                            }
                            p_B=calloc(1,n_cols_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                             if (p_A==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_B,"%p",blas3_test_procedure);
                                return -1;
                            }
                            p_C=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                             if (p_C==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_C,"%p",blas3_test_procedure);
                                return -1;
                            }
                            LDA=n_rows_A[dim_cases_idx];
                            LDB=n_cols_A[dim_cases_idx];
                            LDC=n_rows_A[dim_cases_idx];
                            blas3_configuration(p_blas_l3,TRANSA,TRANSB,n_rows_A[dim_cases_idx],
                         n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx],LDA, LDB,LDC,
                         ALPHA,BETA,p_A,p_B,p_C);

                         //  ADE_Blas_level3_Print(p_blas_l3);
                         ADE_Blas_level3_gemm (p_blas_l3);



                            /**** freematrices****/
                            free(p_A);
                            free(p_B);
                            free(p_C);


                        }
                    ADE_Blas_level3_Release(p_blas_l3);
                }

                else if (mat_type[type_idx]==ADE_BENCH_CPLX)
                {

                    for (dim_cases_idx=0;dim_cases_idx<n_dim_cases;dim_cases_idx++)
                    {
                        ADE_Blas_level3_Init(&p_blas_l3,ADE_CPLX );

                        /**** matrix alloc********/
                            p_A=calloc(1,n_rows_A[dim_cases_idx]*n_cols_A[dim_cases_idx]*sizeof(ADE_CPLX_T));
                            if (p_A==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_A,"%p",blas3_test_procedure);
                                return -1;
                            }
                            p_B=calloc(1,n_cols_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_CPLX_T));
                             if (p_A==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_B,"%p",blas3_test_procedure);
                                return -1;
                            }
                            p_C=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_CPLX_T));
                             if (p_C==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_C,"%p",blas3_test_procedure);
                                return -1;
                            }
                            LDA=n_rows_A[dim_cases_idx];
                            LDB=n_cols_A[dim_cases_idx];
                            LDC=n_rows_A[dim_cases_idx];
                                blas3_configuration(p_blas_l3,TRANSA,TRANSB,n_rows_A[dim_cases_idx],
                         n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx],LDA, LDB,LDC,
                         ALPHA,BETA,p_A,p_B,p_C);

                        // ADE_Blas_level3_Print(p_blas_l3);

                         ADE_Blas_level3_gemm (p_blas_l3);

                            /**** freematrices****/
                            free(p_A);
                            free(p_B);
                            free(p_C);

                        ADE_Blas_level3_Release(p_blas_l3);
                    }
                }
                else
                {
                    ADE_PRINT_ERRORS(ADE_INCHECKS,mat_type[type_idx],"%d",main);
                    return -1;
                }

            }

        }
        else
        {
            ADE_PRINT_ERRORS(ADE_INCHECKS,test_cases[test_idx],"%d",main);
            return -1;
        }
    }

    return 0;
}

//int configure_blas_l3 ()
// gemm=alpha*A*B+Beta*C

int main()
{

    ADE_BENCH_T test_cases[1]={ADE_BLAS_L3_BM};
    ADE_BENCH_MAT_T mat_type[2]={ADE_BENCH_REAL,ADE_BENCH_CPLX};
    ADE_INT32_T ret=0;

    ADE_INT32_T n_rows_A[10]={2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_A[10]={2,10,50,100,500,1000,2000,3000,5000,10000};
    ADE_INT32_T n_cols_B[10]={2,10,50,100,500,1000,2000,3000,5000,10000};


ret=blas3_test_procedure(test_cases,1,mat_type,2,n_rows_A,n_cols_A,n_cols_B,10);



    return ret;
}


