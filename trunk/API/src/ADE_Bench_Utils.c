#include "headers/ADE_Bench_Utils.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_blas_level3.h"
#include "headers/ADE_Utils.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "headers/ADE_complex.h"

void checker(ADE_blas_level3_T *p_blas_l3,ADE_VOID_T *p_C_custom,ADE_FLOATING_T tol,ADE_UINT32_T test_id,ADE_BENCH_MAT_T m_type,bench_times_T *bench_struct,FILE* p_fid)
{
    ADE_UINT32_T row_idx=0,col_idx=0,lin_idx=0;
    double p_sum=0,result=0;
    ADE_CPLX_T p_csum=ADE_cset(0,0);
    ADE_CPLX_T cresult=ADE_cset(0,0);
    ADE_MATH_ATTRIBUTE_T math_att;
    ADE_VOID_T *p_alpha=p_blas_l3->p_ALPHA;
    ADE_VOID_T *p_beta=p_blas_l3->p_BETA;
    ADE_VOID_T *p_A=p_blas_l3->p_A;
    ADE_VOID_T *p_B=p_blas_l3->p_B;
    ADE_VOID_T *p_C=p_blas_l3->p_C;
    ADE_UINT32_T n_row=p_blas_l3->M;
    ADE_UINT32_T n_col=p_blas_l3->N;
    ADE_UINT32_T n_row_col_int=p_blas_l3->K;
    double time_sum1=0;
    double time_sum2=0;

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
            fprintf(p_fid,"\n");
            fprintf(p_fid,"TEST %d passed with residue %f and tolerance %f\n",test_id,result,tol);
            fprintf(p_fid,"Blas Implementation took  %lf ns \n",time_sum1);
            fprintf(p_fid,"Custom Implementation took  %lf ns \n",time_sum1);

    }
    else
    {
        fprintf(p_fid,"Test %d NOT PASSED with residue %f and tolerance %f\n",test_id,result,tol);

        if (m_type==ADE_BENCH_REAL)
        {
            math_att=ADE_REAL;
        }
        else if (m_type==ADE_BENCH_CPLX)
        {
            math_att=ADE_CPLX;
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
    ADE_UINT32_T A_lin_idx=0,B_lin_idx=0,C_lin_idx=0,Out_lin_idx;
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
    ADE_CPLX_T *p_beta=(ADE_CPLX_T *)p_blas_l3->p_BETA;
    ADE_CPLX_T *p_A=(ADE_CPLX_T *)p_blas_l3->p_A;
    ADE_CPLX_T *p_B=(ADE_CPLX_T *)p_blas_l3->p_B;
    ADE_UINT32_T n_row_A=p_blas_l3->M;
    ADE_UINT32_T n_col_A=p_blas_l3->K;
    ADE_UINT32_T n_col_B=p_blas_l3->N;
    ADE_UINT32_T A_row_idx=0,A_col_idx=0,B_col_idx=0,B_row_idx=0,C_col_idx=0,C_row_idx=0;
    ADE_CPLX_T parz_sum=ADE_cset(0,0);
    ADE_UINT32_T A_lin_idx=0,B_lin_idx=0,C_lin_idx=0,Out_lin_idx;
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
    ADE_FLOATING_T tolerance=1e-4;
    ADE_UINT32_T test_id=0;
    struct timespec start_blas, stop_blas,start_cust, stop_cust,res;
    int ret_time=0;
    int err_code=0;
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
    clockid_t clock_id=CLOCK_MONOTONIC;//CLOCK_REALTIME,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
#endif
   bench_times_T bench_times_int;

    bench_times_int.p_start_1=&start_blas;
    bench_times_int.p_stop_1=&stop_blas;
    bench_times_int.p_start_2=&start_cust;
    bench_times_int.p_stop_2=&stop_cust;
    bench_times_int.p_res=&res;

#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )

    ret_time=clock_getres(clock_id,&res);
    if (ret_time==-1)
    {
        err_code=errno;
        fprintf(p_fid,"Failed setting clock %d : error %d\n",clock_id,err_code);
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
                            p_C_custom=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_FLOATING_T));
                             if (p_C_custom==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_C,"%p",blas3_test_procedure);
                                return -1;
                            }
                            LDA=n_rows_A[dim_cases_idx];
                            LDB=n_cols_A[dim_cases_idx];
                            LDC=n_rows_A[dim_cases_idx];
                            blas3_configuration(p_blas_l3,TRANSA,TRANSB,n_rows_A[dim_cases_idx],
                         n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx],LDA, LDB,LDC,
                         ALPHA,BETA,(ADE_FLOATING_T*)p_A,(ADE_FLOATING_T*)p_B,(ADE_FLOATING_T*)p_C);
                         /**********load data *********/
                         load_fmatrix((ADE_FLOATING_T*)p_A,n_rows_A[dim_cases_idx],n_cols_A[dim_cases_idx]);
                         load_fmatrix((ADE_FLOATING_T*)p_B,n_cols_A[dim_cases_idx],n_cols_B[dim_cases_idx]);
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
                         clock_gettime(clock_id,&start_blas);
//#elif (ADE_TARGET==ADE_IOS)
//                            TICK;
#endif
                         ADE_Blas_level3_gemm (p_blas_l3);
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
                          clock_gettime(clock_id,&stop_blas);

                          clock_gettime(clock_id,&start_cust);
//#elif (ADE_TARGET==ADE_IOS)
//                            TOCK;
//                            TICK;
#endif
                          custom_faABpbC (p_blas_l3,(ADE_FLOATING_T*)p_C_custom);
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
                        clock_gettime(clock_id,&stop_cust);
//#elif (ADE_TARGET==ADE_IOS)
//                            TOCK;
#endif

                        test_id=test_idx*n_types*n_dim_cases+type_idx*n_dim_cases+dim_cases_idx;
                        checker(p_blas_l3,p_C_custom,tolerance,test_id,mat_type[type_idx],&bench_times_int,p_fid);

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
                            p_C_custom=calloc(1,n_rows_A[dim_cases_idx]*n_cols_B[dim_cases_idx]*sizeof(ADE_CPLX_T));
                             if (p_C_custom==NULL)
                            {
                                ADE_PRINT_ERRORS(ADE_INCHECKS,p_C,"%p",blas3_test_procedure);
                                return -1;
                            }
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
                            checker(p_blas_l3,p_C_custom,tolerance,test_id,mat_type[type_idx],&bench_times_int,p_fid);


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
