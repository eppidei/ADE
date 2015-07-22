#include "headers/ADE_Bench_Utils.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_blas_level3.h"
#include "headers/ADE_fft.h"
#include "headers/ADE_Utils.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "headers/ADE_complex.h"
#if (ADE_TARGET_TYPE==ADE_IOS)
#include "headers/ADE_accelerate_framework_wrapper.h"
#endif
#include "headers/ADE_Error_Handler.h"
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif

void gemm_checker(ADE_blas_level3_T *p_blas_l3,ADE_VOID_T *p_C_custom,ADE_FLOATING_T tol,ADE_UINT32_T test_id,ADE_BENCH_MAT_T m_type,bench_times_T *bench_struct,FILE* p_fid)
{
    ADE_UINT32_T row_idx=0,col_idx=0,lin_idx=0;
    ADE_FLOATING_DP_T p_sum=0,result=0;
    ADE_CPLX_T p_csum=ADE_cset(0,0);
   // ADE_CPLX_T cresult=ADE_cset(0,0);
    ADE_MATH_ATTRIBUTE_T math_att=ADE_MATH_UNDEF;
    ADE_VOID_T *p_alpha=p_blas_l3->p_ALPHA;
    ADE_VOID_T *p_beta=p_blas_l3->p_BETA;
    ADE_VOID_T *p_A=p_blas_l3->p_A;
    ADE_VOID_T *p_B=p_blas_l3->p_B;
    ADE_VOID_T *p_C=p_blas_l3->p_C;
    ADE_UINT32_T n_row=p_blas_l3->M;
    ADE_UINT32_T n_col=p_blas_l3->N;
    ADE_UINT32_T n_row_col_int=p_blas_l3->K;
    ADE_FLOATING_DP_T time_sum1=0;
    ADE_FLOATING_DP_T time_sum2=0;

    if (m_type==ADE_BENCH_REAL)
    {
        for (row_idx=0;row_idx<n_row;row_idx++)
        {
            for (col_idx=0;col_idx<n_col;col_idx++)
            {
                lin_idx=row_idx*n_col+col_idx;
                p_sum+=((ADE_FLOATING_T*)p_C)[lin_idx]-((ADE_FLOATING_T*)p_C_custom)[lin_idx];
            }
        }
result=fabs(p_sum);

    }
    else if (m_type==ADE_BENCH_CPLX)
    {
        for (row_idx=0;row_idx<n_row;row_idx++)
        {
            for (col_idx=0;col_idx<n_col;col_idx++)
            {
                lin_idx=row_idx*n_col+col_idx;
                p_csum=ADE_csum(ADE_cdiff(((ADE_CPLX_T*)p_C)[lin_idx],((ADE_CPLX_T*)p_C_custom)[lin_idx]),p_csum);
            }
        }
        result=ADE_cabs(p_csum);

    }



time_sum1=(*(bench_struct->p_stop_1)).tv_nsec-(*(bench_struct->p_start_1)).tv_nsec;
time_sum2=(*(bench_struct->p_stop_2)).tv_nsec-(*(bench_struct->p_start_2)).tv_nsec;




    if (result<tol)
    {
            ADE_LOG(p_fid,"\n");
            ADE_LOG(p_fid,"TEST %d passed with residue %f and tolerance %f\n",test_id,result,tol);
            ADE_LOG(p_fid,"Blas Implementation took  %lf ns \n",time_sum1);
            ADE_LOG(p_fid,"Custom Implementation took  %lf ns \n",time_sum2);

    }
    else
    {
        ADE_LOG(p_fid,"Test %d NOT PASSED with residue %f and tolerance %f\n",test_id,result,tol);

        if (m_type==ADE_BENCH_REAL)
        {
            math_att=ADE_MATH_REAL;
        }
        else if (m_type==ADE_BENCH_CPLX)
        {
            math_att=ADE_MATH_CPLX;
        }
        ADE_Blas_level3_Print(p_blas_l3,p_fid);
        ADE_Utils_PrintArray(p_alpha,0,1-1, 0,1-1, "Alpha", p_fid,math_att);
        ADE_Utils_PrintArray(p_A,0,n_row-1, 0,n_row_col_int-1, "A", p_fid,math_att);
        ADE_Utils_PrintArray(p_B,0,n_row_col_int-1, 0,n_col-1, "B", p_fid,math_att);
        ADE_Utils_PrintArray(p_beta,0,1-1, 0,1-1, "Beta", p_fid,math_att);
        ADE_Utils_PrintArray(p_C,0,n_row-1, 0,n_col-1, "Matrice BLAS", p_fid,math_att);
        ADE_Utils_PrintArray(p_C_custom,0,n_row-1, 0,n_col-1, "Matrice Custom", p_fid,math_att);
    }





}

void load_fmatrix(ADE_FLOATING_T *p_mat,ADE_UINT32_T n_row,ADE_UINT32_T n_col)
{
    ADE_UINT32_T row_idx=0,col_idx=0,lin_idx=0;

    for (row_idx=0;row_idx<n_row;row_idx++)
    {
        for (col_idx=0;col_idx<n_col;col_idx++)
        {
            lin_idx=row_idx*n_col+col_idx;
            p_mat[lin_idx]=(ADE_FLOATING_T)(rand()%10);
        }
    }

}

void load_cmatrix(ADE_CPLX_T *p_mat,ADE_UINT32_T n_row,ADE_UINT32_T n_col)
{
    ADE_UINT32_T row_idx=0,col_idx=0,lin_idx=0;

    for (row_idx=0;row_idx<n_row;row_idx++)
    {
        for (col_idx=0;col_idx<n_col;col_idx++)
        {
            lin_idx=row_idx*n_col+col_idx;
            p_mat[lin_idx]=ADE_cset((rand()%10),(rand()%10));
        }
    }

}

void custom_faABpbC (ADE_blas_level3_T *p_blas_l3,ADE_FLOATING_T *p_C)
{
    ADE_FLOATING_T *p_alpha=p_blas_l3->p_ALPHA;
    ADE_FLOATING_T *p_beta=p_blas_l3->p_BETA;
    ADE_FLOATING_T *p_A=p_blas_l3->p_A;
    ADE_FLOATING_T *p_B=p_blas_l3->p_B;
    ADE_UINT32_T n_row_A=p_blas_l3->M;
    ADE_UINT32_T n_col_A=p_blas_l3->K;
    ADE_UINT32_T n_col_B=p_blas_l3->N;
    ADE_UINT32_T A_row_idx=0,A_col_idx=0,B_col_idx=0,B_row_idx=0,C_col_idx=0,C_row_idx=0;
    ADE_FLOATING_T parz_sum=0;
    ADE_UINT32_T A_lin_idx=0,B_lin_idx=0,C_lin_idx=0;
    ADE_FLOATING_T alpha=*p_alpha;
    ADE_FLOATING_T beta=*p_beta;
    ADE_UINT32_T n_col_C=0;

    for (A_row_idx=0;A_row_idx<n_row_A;A_row_idx++)
    {
        for (B_col_idx=0;B_col_idx<n_col_B;B_col_idx++)
        {
            for (A_col_idx=0;A_col_idx<n_col_A;A_col_idx++)
            {
                A_lin_idx=A_row_idx*n_col_A+A_col_idx;
                B_row_idx=A_col_idx;
                B_lin_idx=B_row_idx*n_col_B+B_col_idx;
                parz_sum+=p_A[A_lin_idx]*p_B[B_lin_idx];
            }
            C_row_idx=A_row_idx;
            C_col_idx=B_col_idx;
            n_col_C=n_col_B;
            C_lin_idx=C_row_idx*n_col_C+C_col_idx;
            //Out_lin_idx=A_row_idx*n_col_A+B_col_idx;
            p_C[C_lin_idx]=alpha*parz_sum+beta*p_C[C_lin_idx];
            parz_sum=0;

        }
    }

}

void custom_caABpbC (ADE_blas_level3_T *p_blas_l3,ADE_CPLX_T *p_C)
{

    ADE_CPLX_T *p_alpha=(ADE_CPLX_T *)p_blas_l3->p_ALPHA;
   // ADE_CPLX_T *p_beta=(ADE_CPLX_T *)p_blas_l3->p_BETA;
    ADE_CPLX_T *p_A=(ADE_CPLX_T *)p_blas_l3->p_A;
    ADE_CPLX_T *p_B=(ADE_CPLX_T *)p_blas_l3->p_B;
    ADE_UINT32_T n_row_A=p_blas_l3->M;
    ADE_UINT32_T n_col_A=p_blas_l3->K;
    ADE_UINT32_T n_col_B=p_blas_l3->N;
    ADE_UINT32_T A_row_idx=0,A_col_idx=0,B_col_idx=0,B_row_idx=0,C_col_idx=0,C_row_idx=0;
    ADE_CPLX_T parz_sum=ADE_cset(0,0);
    ADE_UINT32_T A_lin_idx=0,B_lin_idx=0,C_lin_idx=0;
    ADE_CPLX_T alpha=*p_alpha;
    ADE_CPLX_T beta=*p_alpha;
    ADE_UINT32_T n_col_C=0;

    for (A_row_idx=0;A_row_idx<n_row_A;A_row_idx++)
    {
        for (B_col_idx=0;B_col_idx<n_col_B;B_col_idx++)
        {
            for (A_col_idx=0;A_col_idx<n_col_A;A_col_idx++)
            {
                A_lin_idx=A_row_idx*n_col_A+A_col_idx;
                B_row_idx=A_col_idx;
                B_lin_idx=B_row_idx*n_col_B+B_col_idx;
                parz_sum=ADE_csum(ADE_cmult(p_A[A_lin_idx],p_B[B_lin_idx]),parz_sum);
            }
            C_row_idx=A_row_idx;
            C_col_idx=B_col_idx;
            n_col_C=n_col_B;
            C_lin_idx=C_row_idx*n_col_C+C_col_idx;
            //Out_lin_idx=A_row_idx*n_col_A+B_col_idx;
            p_C[C_lin_idx]=ADE_csum(ADE_cmult(alpha,parz_sum),ADE_cmult(beta,p_C[C_lin_idx]));
            parz_sum=ADE_cset(0,0);

        }
    }

}

int blas3_configuration(ADE_blas_level3_T* p_Blas_l3,ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,ADE_INT32_T M,
                         ADE_INT32_T N,ADE_INT32_T K,ADE_INT32_T LDA,ADE_INT32_T LDB,ADE_INT32_T LDC,
                         ADE_FLOATING_T *p_alpha,ADE_FLOATING_T *p_beta,ADE_FLOATING_T *p_A,ADE_FLOATING_T *p_B,ADE_FLOATING_T *p_C)
{
    ADE_API_RET_T ret = ADE_RET_SUCCESS;

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
ADE_Blas_Level3_SetALPHA( p_Blas_l3, p_alpha);
ADE_Blas_Level3_SetBeta( p_Blas_l3, p_beta);

return ret;
}

ADE_INT32_T blas3_test_procedure(ADE_BENCH_T *test_cases,ADE_UINT32_T n_tests,ADE_BENCH_MAT_T *mat_type,ADE_UINT32_T n_types,ADE_INT32_T *n_rows_A,ADE_INT32_T *n_cols_A,ADE_INT32_T *n_cols_B,ADE_INT32_T n_dim_cases,FILE* p_fid)
{
    ADE_blas_level3_T *p_blas_l3=NULL;
    ADE_UINT32_T test_idx=0,type_idx=0,dim_cases_idx=0;
    ADE_VOID_T *p_A=NULL,*p_B=NULL,*p_C=NULL,*p_C_custom=NULL;
    ADE_CHAR_T TRANSA='N';
    ADE_CHAR_T TRANSB='N';
    ADE_FLOATING_T ALPHA[2]={1.0,1.0};
    ADE_INT32_T LDA=0;
    ADE_INT32_T LDB=0;
    ADE_FLOATING_T BETA[2]={1.0,1.0};
    ADE_INT32_T LDC=0;
    ADE_FLOATING_T tolerance=1e-1;
    ADE_UINT32_T test_id=0;
    struct timespec start_blas, stop_blas,start_cust, stop_cust,res;
    int ret_time=0;
    int err_code=0;
    FILE *p_stream=ADE_STDERR_STREAM;
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )
    clockid_t clock_id=CLOCK_MONOTONIC;//CLOCK_REALTIME,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
#endif
   bench_times_T bench_times_int;

    bench_times_int.p_start_1=&start_blas;
    bench_times_int.p_stop_1=&stop_blas;
    bench_times_int.p_start_2=&start_cust;
    bench_times_int.p_stop_2=&stop_cust;
    bench_times_int.p_res=&res;

#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )

    ret_time=clock_getres(clock_id,&res);
    if (ret_time==-1)
    {
        err_code=errno;
        ADE_LOG(p_fid,"Failed setting clock %d : error %d\n",clock_id,err_code);
        return -1;
    }

#endif




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
                     ADE_Blas_level3_Init(&p_blas_l3,ADE_MATH_REAL );
                     /*** cicla dimensioni***/
                        for (dim_cases_idx=0;dim_cases_idx<n_dim_cases;dim_cases_idx++)
                        {

                            /**** matrix alloc********/
                            p_A=calloc(1,n_rows_A[dim_cases_idx]*n_cols_A[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_A);

                            p_B=calloc(1,n_cols_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_B);

                            p_C=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_C);

                            p_C_custom=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                             ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_C_custom);

                            LDA=n_rows_A[dim_cases_idx];
                            LDB=n_cols_A[dim_cases_idx];
                            LDC=n_rows_A[dim_cases_idx];
                            blas3_configuration(p_blas_l3,TRANSA,TRANSB,n_rows_A[dim_cases_idx],
                         n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx],LDA, LDB,LDC,
                         ALPHA,BETA,(ADE_FLOATING_T*)p_A,(ADE_FLOATING_T*)p_B,(ADE_FLOATING_T*)p_C);
                         /**********load data *********/
                         load_fmatrix((ADE_FLOATING_T*)p_A,n_rows_A[dim_cases_idx],n_cols_A[dim_cases_idx]);
                         load_fmatrix((ADE_FLOATING_T*)p_B,n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx]);
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )
                         clock_gettime(clock_id,&start_blas);
//#elif (ADE_TARGET_TYPE==ADE_IOS)
//                            TICK;
#endif
                         ADE_Blas_level3_gemm (p_blas_l3);
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )
                          clock_gettime(clock_id,&stop_blas);

                          clock_gettime(clock_id,&start_cust);
//#elif (ADE_TARGET_TYPE==ADE_IOS)
//                            TOCK;
//                            TICK;
#endif
                          custom_faABpbC (p_blas_l3,(ADE_FLOATING_T*)p_C_custom);
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )
                        clock_gettime(clock_id,&stop_cust);
//#elif (ADE_TARGET_TYPE==ADE_IOS)
//                            TOCK;
#endif

                        test_id=test_idx*n_types*n_dim_cases+type_idx*n_dim_cases+dim_cases_idx;
                        gemm_checker(p_blas_l3,p_C_custom,tolerance,test_id,mat_type[type_idx],&bench_times_int,p_fid);

                            /**** freematrices****/
                            free(p_A);
                            free(p_B);
                            free(p_C);
                            free(p_C_custom);


                        }
                    ADE_Blas_level3_Release(p_blas_l3);
                }

                else if (mat_type[type_idx]==ADE_BENCH_CPLX)
                {

                    for (dim_cases_idx=0;dim_cases_idx<n_dim_cases;dim_cases_idx++)
                    {
                        ADE_Blas_level3_Init(&p_blas_l3,ADE_MATH_CPLX );

                        /**** matrix alloc********/
                            p_A=calloc(1,n_rows_A[dim_cases_idx]*n_cols_A[dim_cases_idx]*sizeof(ADE_CPLX_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_A);

                            p_B=calloc(1,n_cols_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_CPLX_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_B);

                            p_C=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_CPLX_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_C);

                            p_C_custom=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_CPLX_T));
                            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS,ADE_METHOD_blas3_test_procedure,p_C_custom);

                            LDA=n_rows_A[dim_cases_idx];
                            LDB=n_cols_A[dim_cases_idx];
                            LDC=n_rows_A[dim_cases_idx];
                                blas3_configuration(p_blas_l3,TRANSA,TRANSB,n_rows_A[dim_cases_idx],
                         n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx],LDA, LDB,LDC,
                         ALPHA,BETA,(ADE_FLOATING_T*)p_A,(ADE_FLOATING_T*)p_B,(ADE_FLOATING_T*)p_C);

                         /**********load data *********/
                         load_cmatrix((ADE_CPLX_T*)p_A,n_rows_A[dim_cases_idx],n_cols_A[dim_cases_idx]);
                         load_cmatrix((ADE_CPLX_T*)p_B,n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx]);



                         ADE_Blas_level3_gemm (p_blas_l3);

                         custom_caABpbC (p_blas_l3,(ADE_CPLX_T*)p_C_custom);
                             test_id=test_idx*n_types*n_dim_cases+type_idx*n_dim_cases+dim_cases_idx;
                            gemm_checker(p_blas_l3,p_C_custom,tolerance,test_id,mat_type[type_idx],&bench_times_int,p_fid);


                            /**** freematrices****/
                            free(p_A);
                            free(p_B);
                            free(p_C);
                            free(p_C_custom);

                        ADE_Blas_level3_Release(p_blas_l3);
                    }
                }
                else
                {
                    ADE_LOG(p_stream,"ADE_PRINT_ERRORS(ADE_INCHECKS,mat_type[type_idx],main);\n");
                    return -1;
                }

            }

        }
        else
        {
            ADE_LOG(p_stream,"ADE_PRINT_ERRORS(ADE_INCHECKS,test_cases[test_idx],main);\n");
            return -1;
        }
    }

    return 0;
}

//int configure_blas_l3 ()
// gemm=alpha*A*B+Beta*C

#define ADE_SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void custom_FFT(ADE_FLOATING_T data[], unsigned long nn, ADE_CUSTOM_FFT_DIRECTION_T isign)
/*Replaces data[1..2*nn] by its discrete Fourier transform, if isign is input as 1; or replaces
data[1..2*nn] by nn times its inverse discrete Fourier transform, if isign is input as −1.
data is a complex array of length nn or, equivalently, a real array of length 2*nn. nn MUST
be an integer power of 2 (this is not checked for!).*/
{
    unsigned long n,mmax,m,j,istep,i;
    ADE_FLOATING_DP_T wtemp,wr,wpr,wpi,wi,theta; /*Double precision for the trigonomet ric recurrences.*/
    ADE_FLOATING_T tempr,tempi;
    n=nn << 1;
    j=1;
    for (i=1; i<n; i+=2) /*This is the bit-reversal section of the  routine.*/
    {
        if (j > i)
        {

            ADE_SWAP(data[j],data[i]); /*Exchange the two complex numbers.*/
            ADE_SWAP(data[j+1],data[i+1]);
        }
        m=n >> 1;
        while (m >= 2 && j > m)
        {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    /*Here begins the Danielson-Lanczos section of the routine. Outer loop executed log2 nn times.*/
    mmax=2;
    while (n > mmax)
    {

        istep=mmax << 1;
        theta=isign*(6.28318530717959/mmax);
        /*Initialize the trigonometric recurrence.*/
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0;
        wi=0.0;
        for (m=1; m<mmax; m+=2)
        {
            for (i=m; i<=n; i+=istep)
            {
                j=i+mmax;
                tempr=wr*data[j]-wi*data[j+1];
                tempi=wr*data[j+1]+wi*data[j];
                data[j]=data[i]-tempr;
                data[j+1]=data[i+1]-tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }
        mmax=istep;
    }
}


void fft_checker(ADE_FFT_T *p_fft,ADE_VOID_T *p_fft_custom,ADE_FLOATING_T tol,ADE_UINT32_T test_id,ADE_FFT_TYPE_T m_type,bench_times_T *bench_struct,FILE* p_fid)
{
    ADE_UINT32_T row_idx=0;
    ADE_FLOATING_DP_T result=0;
    ADE_CPLX_T p_csum=ADE_cset(0,0);
    ADE_MATH_ATTRIBUTE_T math_att=ADE_MATH_UNDEF;
    ADE_VOID_T * p_desplit=NULL;
    ADE_UINT32_T n_row=0;

    ADE_FLOATING_DP_T time_sum1=0;
    ADE_FLOATING_DP_T time_sum2=0;

    p_desplit=p_fft->p_out;

//    #if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
//        if (p_fft->type==ADE_FFT_C2C)
//        {
//        p_desplit=malloc(p_fft->buff_len*sizeof(ADE_FFTCPLX_T));
//        ADE_Utils_Split2Complex( &(p_fft->split_out),1,(ADE_CPLX_T*)p_desplit,2,p_fft->buff_len);
//        }
//    else if (p_fft->type==ADE_FFT_R2C)
//    {
//        p_desplit=malloc(p_fft->buff_len/2*sizeof(ADE_FFTCPLX_T));
//        ADE_Utils_Split2Complex( &(p_fft->split_out),1,(ADE_CPLX_T*)p_desplit,2,p_fft->buff_len/2);
//    }
//
//    #endif
    if (p_fft->type==ADE_FFT_C2C)
    {
       n_row=p_fft->buff_len;
    }
    else if (p_fft->type==ADE_FFT_R2C)
    {
        n_row=p_fft->buff_len/2;
    }


    if (m_type==ADE_FFT_C2C || m_type==ADE_FFT_R2C)
    {

        for (row_idx=0;row_idx<n_row;row_idx++)
        {
            #if (ADE_FFT_IMP==ADE_USE_FFTW)
                p_csum=ADE_csum(ADE_cdiff(((ADE_CPLX_T*)p_fft->p_out)[row_idx],((ADE_CPLX_T*)p_fft_custom)[row_idx]),p_csum);
             #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
                p_csum=ADE_csum(ADE_cdiff(((ADE_CPLX_T*)p_desplit)[row_idx],((ADE_CPLX_T*)p_fft_custom)[row_idx]),p_csum);
             #endif
        }

        result=ADE_cabs(p_csum);

   }
    else
    {
        printf ("CASE Not handled!!! in fft_checker\n");
    }



time_sum1=(*(bench_struct->p_stop_1)).tv_nsec-(*(bench_struct->p_start_1)).tv_nsec;
time_sum2=(*(bench_struct->p_stop_2)).tv_nsec-(*(bench_struct->p_start_2)).tv_nsec;




    if (result<tol)
    {
            ADE_LOG(p_fid,"\n");
            ADE_LOG(p_fid,"TEST %d passed with residue %f and tolerance %f\n",test_id,result,tol);
            ADE_LOG(p_fid,"FFT LIBRARY took  %lf ns \n",time_sum1);
            ADE_LOG(p_fid,"Custom FFT took  %lf ns \n",time_sum2);

    }
    else
    {
        ADE_LOG(p_fid,"Test %d NOT PASSED with residue %f and tolerance %f\n",test_id,result,tol);

        if (m_type==ADE_FFT_C2C || m_type==ADE_FFT_R2C)
        {
            math_att=ADE_MATH_CPLX;
        }
        else
        {
            printf ("CASE Not handled!!! in fft_checker math_att\n");
        }
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
        ADE_Utils_PrintArray(p_fft->p_out,0,n_row-1, 0,0, "FFT LIBRARY ", p_fid,math_att);
        #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
         ADE_Utils_PrintArray(p_desplit,0,n_row-1, 0,0, "FFT LIBRARY ", p_fid,math_att);
         #endif
        ADE_Utils_PrintArray(p_fft_custom,0,n_row-1, 0,0, "FFT Custom", p_fid,math_att);

    }
 #if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
   // free(p_desplit);
#endif

}




ADE_INT32_T fft_iosreal_test_procedure(ADE_UINT32_T *p_dim,ADE_INT32_T n_dim_cases,FILE*p_fid)
{
    ADE_FFT_T *p_fft;
    ADE_UINT32_T buff_len;
    ADE_SIZE_T size_fft_in,size_fft_out;
    ADE_VOID_T *p_in=NULL;
    ADE_VOID_T *p_out=NULL;
    ADE_UINT32_T i=0,buff_len_idx=0;
    ADE_API_RET_T ret=ADE_RET_ERROR;
    int randa;

    for (buff_len_idx=0; buff_len_idx<n_dim_cases; buff_len_idx++)
    {

        buff_len=p_dim[buff_len_idx];

        size_fft_in=buff_len*sizeof(ADE_FLOATING_T);
        p_in=malloc(size_fft_in);
        ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS, ADE_METHOD_fft_test_procedure,p_in);
        size_fft_out=(buff_len/2)*sizeof(ADE_FFTCPLX_T);
        p_out=malloc(size_fft_out);
        memset(p_out,0,size_fft_out);
        ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS, ADE_METHOD_fft_test_procedure,p_out);
        ret=ADE_Fft_Init(&p_fft, buff_len);
        ADE_CHECK_ADERETVAL(ADE_CLASS_BENCH_UTILS,ADE_METHOD_fft_test_procedure,ret);
        for(i=0; i<buff_len; i++)
        {

            randa=i;//(rand()%10);
            ((ADE_FLOATING_T*)p_in)[i]=(ADE_FLOATING_T)(randa);
        }
        ret=ADE_Fft_Configure(p_fft,ADE_FFT_R2C, ADE_FFT_FORWARD,p_in,p_out);

        ADE_CHECK_ADERETVAL(ADE_CLASS_BENCH_UTILS,ADE_METHOD_fft_test_procedure,ret);
    ret=ADE_Fft_Step(p_fft);
    free(p_in);
    free(p_out);
    ADE_Fft_Release(p_fft);
    }

    return 0;
}

ADE_INT32_T fft_test_procedure(ADE_FFT_TYPE_T fft_type,ADE_UINT32_T *p_dim,ADE_INT32_T n_dim_cases,FILE* p_fid)
{

    ADE_FFT_T *p_fft;
   // ADE_MATLAB_T *p_mat;
    ADE_UINT32_T buff_len;
    ADE_SIZE_T size_fft_in,size_fft_out;
    ADE_VOID_T *p_in=NULL;
    ADE_VOID_T *p_out=NULL;

    ADE_UINT32_T i=0,buff_len_idx=0;
    //ADE_SplitComplex_T split_in,split_out;
   // ADE_INT32_T lin,col;
    ADE_API_RET_T ret=ADE_RET_ERROR;

     ADE_FLOATING_T tolerance=1e-1;
    ADE_VOID_T *p_fft_custom=NULL;
     struct timespec start_fft, stop_fft,start_cust, stop_cust,res;
    int randa=0;
    int err_code=0;
    int ret_time=0;
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )
    clockid_t clock_id=CLOCK_MONOTONIC;//CLOCK_REALTIME,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
#endif
   bench_times_T bench_times_int;

    bench_times_int.p_start_1=&start_fft;
    bench_times_int.p_stop_1=&stop_fft;
    bench_times_int.p_start_2=&start_cust;
    bench_times_int.p_stop_2=&stop_cust;
    bench_times_int.p_res=&res;

#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_PC_RELEASE) || (ADE_TARGET_TYPE==ADE_ANDROID) )

    ret_time=clock_getres(clock_id,&res);
    if (ret_time==-1)
    {
        err_code=errno;
        ADE_LOG(p_fid,"Failed setting clock %d : error %d\n",clock_id,err_code);
        return -1;
    }
#endif

/******** Cycle on dimension*************/

    for (buff_len_idx=0; buff_len_idx<n_dim_cases; buff_len_idx++)
    {

        buff_len=p_dim[buff_len_idx];

        /*********************** IN-OUT BUFF MEM ALLOC ********************************/


        if (fft_type==ADE_FFT_C2C)
        {
            size_fft_in=buff_len*sizeof(ADE_FFTCPLX_T);
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
				#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
            		p_in=fftw_malloc(size_fft_in);
				#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
					p_in=fftwf_malloc(size_fft_in);
				#else
					#error ADE_FP_PRECISION in ADE_Bench_Utils
				#endif
            #else
            p_in=malloc(size_fft_in);
            #endif
            if (p_in==NULL)
            {
                ADE_LOG(p_fid,"ADE_PRINT_ERRORS(ADE_MEM,p_in,fft_test_procedure);\n");
                return -1;
            }


            size_fft_out=buff_len*sizeof(ADE_FFTCPLX_T);
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
				#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
             p_out=fftw_malloc(size_fft_out);
				#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
			p_out=fftwf_malloc(size_fft_out);
				#else
					#error ADE_FP_PRECISION in ADE_Bench_Utils
				#endif
            #else
             p_out=malloc(size_fft_out);
            #endif
            if (p_out==NULL)
            {
                ADE_LOG(p_fid,"ADE_PRINT_ERRORS(ADE_MEM,p_out,fft_test_procedure);\n");
                return -1;
            }
            p_fft_custom=malloc(size_fft_in);
            if (p_fft_custom==NULL)
            {
                ADE_LOG(p_fid,"ADE_PRINT_ERRORS(ADE_MEM,p_fft_custom,fft_test_procedure);\n");
                return -1;
            }



        }
        else if (fft_type==ADE_FFT_R2C)
        {
            size_fft_in=buff_len*sizeof(ADE_FLOATING_T);
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
            p_in=fftw_malloc(size_fft_in);
            #else
            p_in=malloc(size_fft_in);
            #endif
            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS, ADE_METHOD_fft_test_procedure,p_in);
            size_fft_out=(buff_len/2)*sizeof(ADE_FFTCPLX_T);
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
            p_out=fftw_malloc(size_fft_out);
            #else
            p_out=malloc(size_fft_out);
            memset(p_out,0,size_fft_out);
            #endif
            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS, ADE_METHOD_fft_test_procedure,p_out);
            /*2check*/
            p_fft_custom=calloc(buff_len*2,sizeof(ADE_FLOATING_T));
            ADE_CHECK_MEMALLOC(ADE_CLASS_BENCH_UTILS, ADE_METHOD_fft_test_procedure,p_fft_custom);
            /**/


        }

        /******************* FFT SETUP ****************************/

        ret=ADE_Fft_Init(&p_fft, buff_len);
        ADE_CHECK_ADERETVAL(ADE_CLASS_BENCH_UTILS,ADE_METHOD_fft_test_procedure,ret);


        /******************* INPUT FILLING ********************************/

        if (fft_type==ADE_FFT_C2C)
        {
            for(i=0; i<buff_len; i++)
            {
                //#if ( ADE_FFT_IMP==ADE_USE_FFTW )
                    ((ADE_CPLX_T*)p_in)[i]=ADE_cset((rand()%10),(rand()%10));
                //#elif  ( ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
                   // ADE_Fft_FillSplitIn(p_fft,(rand()%10),(rand()%10),i);
                //#endif

            }
//#if ( ADE_FFT_IMP==ADE_USE_FFTW )
            memcpy(p_fft_custom,p_in,size_fft_in);
//#elif  ( ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
  //          ADE_Utils_Split2Complex( &(p_fft->split_in),1,(ADE_CPLX_T *)p_fft_custom,2,buff_len);
//#endif


        }
        else if (fft_type==ADE_FFT_R2C)
        {
         for(i=0; i<buff_len; i++)
            {
               // #if ( ADE_FFT_IMP==ADE_USE_FFTW )
                randa=(rand()%10);
                   ((ADE_FLOATING_T*)p_in)[i]=(ADE_FLOATING_T)(randa);
               // #elif  ( ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)

                     //ADE_Fft_FillSplitIn(p_fft,randa,0,i);
                ((ADE_FLOATING_T*)p_fft_custom)[2*i]=randa;
                ((ADE_FLOATING_T*)p_fft_custom)[2*i+1]=0.0;
               // #endif
                }


        }
//        p_fft->split_in.realp=p_fft->p_split_buff_in;
//        p_fft->split_in.imagp=&(p_fft->p_split_buff_in[buff_len]);
//        p_fft->split_out.realp=p_fft->p_split_buff_out;
//        p_fft->split_out.imagp=&(p_fft->p_split_buff_out[buff_len]);
//        p_fft->p_out=p_out;
        ret=ADE_Fft_Configure(p_fft,fft_type, ADE_FFT_FORWARD,p_in,p_out);

        ADE_CHECK_ADERETVAL(ADE_CLASS_BENCH_UTILS,ADE_METHOD_fft_test_procedure,ret);

//
//        for(i=0;i<32;i++)
//        {
//            fprintf(stdout, "p_fft_custom[%d]=%f\n",i,((ADE_FLOATING_T*)p_fft_custom)[i]);
//        }
//







        /******************** FFT CALCULATION ******************************/
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) ||  (ADE_TARGET_TYPE==ADE_ANDROID) )
                         clock_gettime(clock_id,&start_fft);
#endif
         ret=ADE_Fft_Step(p_fft);
#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_ANDROID) )
                          clock_gettime(clock_id,&stop_fft);

                          clock_gettime(clock_id,&start_cust);
#endif

            custom_FFT((ADE_FLOATING_T*)p_fft_custom-1,buff_len, ADE_CUSTOM_FFT_FORWARD);
if (fft_type==ADE_FFT_R2C)
{
    ((ADE_FLOATING_T*)p_fft_custom)[1]=((ADE_FLOATING_T*)p_fft_custom)[buff_len]; //impacchetta reale nel punto simmetria nell'immaginario del primo valore (come si usa di solito) per poter fare il check
}



#if ( (ADE_TARGET_TYPE==ADE_PC_MATLAB) || (ADE_TARGET_TYPE==ADE_PC_NORMAL) || (ADE_TARGET_TYPE==ADE_ANDROID) )
                        clock_gettime(clock_id,&stop_cust);

#endif

       fft_checker(p_fft,p_fft_custom,tolerance,buff_len_idx,fft_type,&bench_times_int,p_fid);

        /************ FFT RELEASE***************/
#if  ( ADE_FFT_IMP==ADE_USE_FFTW )
	#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        fftw_free(p_in);
        fftw_free(p_out);
	#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
 			fftwf_free(p_in);
        fftwf_free(p_out);
	#else
					#error ADE_FP_PRECISION in ADE_Bench_Utils
	#endif
    #else
      //  fprintf(stdout,"%p\n",p_in);
       //  fprintf(stdout,"%p\n",p_out);
        free(p_in);
        free(p_out);
    #endif
        free(p_fft_custom);
        ADE_Fft_Release(p_fft);

}
    return ADE_RET_SUCCESS;
}





