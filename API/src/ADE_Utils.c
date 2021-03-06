#include "headers/ADE_Utils.h"
#include "headers/ADE_errors.h"
#include <sys/ioctl.h>
#include <math.h>
#include "headers/ADE_complex.h"
#if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
#include "headers/ADE_accelerate_framework_wrapper.h"
#endif
#include "headers/ADE_nrutil.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_polyfit.h"
#include "headers/ADE_Error_Handler.h"
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif

static ADE_API_RET_T ADE_Utils_domemoryless_expander(ADE_FLOATING_T data,ADE_INT32_T n_pieces,ADE_INT32_T n_coeff_per_piece,ADE_FLOATING_T *p_x_breaks,ADE_FLOATING_T* p_coeffs,ADE_FLOATING_T* y_o);

/**************************** Private prototypes *********************************/

static ADE_API_RET_T ADE_Utils_PrintRowArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,ADE_UINT32_T n_var_per_printrow,ADE_UTILS_ROW_INFO_T row_info, FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintRowArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_UINT32_T n_var_per_printrow, ADE_UTILS_ROW_INFO_T row_info, FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintColArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx,   FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintColArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx,   FILE *p_stream);

static ADE_API_RET_T ADE_Utils_PrintColArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);
static ADE_API_RET_T ADE_Utils_PrintRowArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);

static ADE_API_RET_T ADE_Utils_PrintMatrixReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,  FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintMatrixCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,  FILE *p_stream);

static ADE_API_RET_T ADE_Utils_PrintMatrix(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);
static ADE_API_RET_T ADE_Utils_memset_blasconfig(ADE_blas_level1_T *p_blas_l1,ADE_FLOATING_T *vec,ADE_UINT32_T len,ADE_VOID_T* memset_val);
static ADE_API_RET_T ADE_Utils_memcpy_blasconfig(ADE_blas_level1_T *p_blas_l1,ADE_VOID_T *p_dst,ADE_VOID_T *p_src,ADE_UINT32_T len);
/********************* Functions ***********************************/

ADE_VOID_T ADE_Utils_Get_Terminal_size(ADE_INT32_T *lines ,ADE_INT32_T *columns  )
{
	#if (ADE_TARGET_TYPE!=ADE_ANDROID) &&  (ADE_TARGET_TYPE!=ADE_IOS)
    struct winsize sz;

    ioctl(0, TIOCGWINSZ, &sz);
    if (sz.ws_row==0 )
    {
        ADE_PRINT_ERRORS(ADE_WARNING,ADE_RETCHECKS,ADE_CLASS_UTILS,ADE_METHOD_Get_Terminal_size,sz.ws_row,"%d",(FILE*)ADE_STD_STREAM);
        ADE_LOG(ADE_STDERR_STREAM,"setting default value of 80\n");
        *lines=80;
    }
    if (sz.ws_col==0 )
    {
        ADE_PRINT_ERRORS(ADE_WARNING,ADE_RETCHECKS,ADE_CLASS_UTILS,ADE_METHOD_Get_Terminal_size,sz.ws_col,"%d",(FILE*)ADE_STD_STREAM);
        ADE_LOG(ADE_STDERR_STREAM,"setting default value of 24\n");
        *columns=24;
    }

    *lines=sz.ws_row;
    *columns=sz.ws_col;
	#endif
}

ADE_API_RET_T ADE_Utils_PrintArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{

    ADE_API_RET_T ret = ADE_RET_ERROR;
    ADE_UINT32_T n_row=stop_0based_row_idx-start_0based_row_idx+1;
    ADE_UINT32_T n_col=stop_0based_col_idx-start_0based_col_idx+1;

    if (n_row==1)
    {
        ret = ADE_Utils_PrintRowArray(p_var,start_0based_col_idx, stop_0based_col_idx,p_varname, p_stream,math_type);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_PrintArray,ret);
    }
    else if (n_col==1)
    {
        ret = ADE_Utils_PrintColArray(p_var,start_0based_row_idx,stop_0based_row_idx, p_varname, p_stream,math_type);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_PrintArray,ret);
    }
    else
    {
        ret = ADE_Utils_PrintMatrix(p_var,start_0based_row_idx,stop_0based_row_idx, start_0based_col_idx, stop_0based_col_idx, p_varname,p_stream,math_type);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_PrintArray,ret);
    }


    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Utils_FindIndexes(ADE_FLOATING_T *p_frame_i,ADE_UINT32_T frame_len ,ADE_FLOATING_T *p_indexes_o,ADE_UINT32_T *p_n_indexes_o, ADE_FLOATING_T *p_threshold,ADE_UTILS_CONDITION_T condition)
{
    ADE_UINT32_T i=0,idx=0;

    if (condition==ADE_UTILS_MAJOR)
    {
        for(i=0; i<frame_len; i++)
        {
            if (p_frame_i[i]>p_threshold[i])
            {
                p_indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else if (condition==ADE_UTILS_MINOR)
    {
        for(i=0; i<frame_len; i++)
        {
            if (p_frame_i[i]<p_threshold[i])
            {
                p_indexes_o[idx]=i;
                idx++;
            }
        }
    }
    else if (condition==ADE_UTILS_EQUAL)
    {
        for(i=0; i<frame_len; i++)
        {
            if (p_frame_i[i]==p_threshold[i])
            {
                p_indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else if (condition==ADE_UTILS_MAJEQUAL)
    {
        for(i=0; i<frame_len; i++)
        {
            if (p_frame_i[i]>=p_threshold[i])
            {
                p_indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else if (condition==ADE_UTILS_MINEQUAL)
    {
        for(i=0; i<frame_len; i++)
        {
            if (p_frame_i[i]<=p_threshold[i])
            {
                p_indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_FindIndexes,condition,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    *p_n_indexes_o=idx;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Utils_Complex2Split(ADE_CPLX_T *p_in,ADE_UINT32_T Stride_in,ADE_SplitComplex_T *p_out,ADE_UINT32_T Stride_out,ADE_UINT32_T cplx_len)
{

    ADE_UINT32_T idx=0;
    ADE_UINT32_T len=cplx_len*2;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_Complex2Split,p_in);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_Complex2Split,p_out);

#if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
        vDSP_ctoz (p_in,Stride_in,p_out,Stride_out,cplx_len);
    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        vDSP_ctozD (p_in,Stride_in,p_out,Stride_out,cplx_len);
    #else
    #error (ADE_FP_PRECISION)
    #endif
#else

    for (idx = 0; idx < len; ++idx)
    {
        p_out->realp[idx*Stride_out] = p_in[idx*Stride_in/2].realpart;
        p_out->imagp[idx*Stride_out] = p_in[idx*Stride_in/2].imagpart;
    }
#endif

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_Utils_Split2Complex( ADE_SplitComplex_T *p_in,ADE_UINT32_T Stride_in,ADE_CPLX_T *p_out,ADE_UINT32_T Stride_out,ADE_UINT32_T split_len)
{

    ADE_UINT32_T idx=0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_Split2Complex,p_in);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_Split2Complex,p_out);

#if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
        vDSP_ztoc (p_in,Stride_in,p_out,Stride_out,split_len);
    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        vDSP_ztocD (p_in,Stride_in,p_out,Stride_out,split_len);
    #else
    #error (ADE_FP_PRECISION)
    #endif
#else
    for (idx = 0; idx < split_len; ++idx)
    {
        p_out[idx*Stride_in/2].realpart = p_in->realp[idx*Stride_out];
        p_out[idx*Stride_in/2].imagpart = p_in->imagp[idx*Stride_out];
    }

#endif
    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_Utils_SetSplit(ADE_VOID_T *p_buff,ADE_UINT32_T buff_len,ADE_SplitComplex_T *p_split)
{

    /*This function needs that buffer length = n_cplx = n_floats/2 */

    ADE_FLOATING_T *p_int=(ADE_FLOATING_T *)p_buff;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_SetSplit,p_buff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_SetSplit,p_split);

    p_split->realp=p_int;
    p_split->imagp=&(p_int[buff_len]);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Utils_FillSplitReal(ADE_FLOATING_T real,ADE_UINT32_T idx,ADE_SplitComplex_T *p_split)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_FillSplitReal,p_split);

    p_split->realp[idx]=real;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Utils_FillSplitImag(ADE_FLOATING_T imag,ADE_UINT32_T idx,ADE_SplitComplex_T *p_split)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_FillSplitImag,p_split);

     p_split->imagp[idx]=imag;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Utils_FillSplitCplx(ADE_FLOATING_T real,ADE_FLOATING_T imag,ADE_UINT32_T idx,ADE_SplitComplex_T *p_split)
{

    ADE_API_RET_T ret=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_FillSplitCplx,p_split);

    ret=ADE_Utils_FillSplitReal(real, idx,p_split);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_FillSplitCplx,ret);

    ret=ADE_Utils_FillSplitImag(imag,idx,p_split);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_FillSplitCplx,ret);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Utils_memset_float(ADE_FLOATING_T *vec,ADE_UINT32_T len, ADE_FLOATING_T memset_val)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    ADE_UINT32_T vec_idx=0;
    ADE_blas_level1_T *p_blas_l1=NULL;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;
    ADE_API_RET_T ret_init=ADE_RET_ERROR;
    ADE_API_RET_T ret_copy=ADE_RET_ERROR;
    ADE_VOID_T *p_val=&memset_val;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memset_float,vec);

    #if (ADE_UTILS_IMP==ADE_UTILS_USE_BLAS)
    ret_init=ADE_Blas_level1_Init(&p_blas_l1,ADE_MATH_REAL);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_float,ret_init);

    ret_conf=ADE_Utils_memset_blasconfig(p_blas_l1,vec,len,p_val);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_float,ret_conf);

    ret_copy=ADE_Blas_level1_copy(p_blas_l1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_float,ret_copy);

    ADE_Blas_level1_Release(p_blas_l1);

    #elif (ADE_UTILS_IMP==ADE_UTILS_USE_CUSTOM)

    for (vec_idx=0; vec_idx<len; vec_idx++)
    {
        vec[vec_idx]=memset_val;
    }
    #else

        #error (ADE_UTILS_IMP in ADE_Utils_memset_float)

    #endif

    return ret;

}


ADE_API_RET_T ADE_Utils_memset_cplx(ADE_CPLX_T *vec,ADE_UINT32_T len, ADE_CPLX_T memset_val)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    ADE_UINT32_T vec_idx=0;
    ADE_blas_level1_T *p_blas_l1=NULL;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;
    ADE_API_RET_T ret_init=ADE_RET_ERROR;
    ADE_API_RET_T ret_copy=ADE_RET_ERROR;
    ADE_VOID_T *p_val=&memset_val;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memset_cplx,vec);

    #if (ADE_UTILS_IMP==ADE_UTILS_USE_BLAS)
    ret_init=ADE_Blas_level1_Init(&p_blas_l1,ADE_MATH_CPLX);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_cplx,ret_init);

    ret_conf=ADE_Utils_memset_blasconfig(p_blas_l1,(ADE_FLOATING_T*)vec,len,p_val);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_cplx,ret_conf);

    ret_copy=ADE_Blas_level1_copy(p_blas_l1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_cplx,ret_copy);

    ADE_Blas_level1_Release(p_blas_l1);

    #elif (ADE_UTILS_IMP==ADE_UTILS_USE_CUSTOM)

    for (vec_idx=0; vec_idx<len; vec_idx++)
    {
        vec[vec_idx]=ADE_cset(memset_val.realpart,memset_val.imagpart);
    }
    #else

        #error (ADE_UTILS_IMP in ADE_Utils_memset_cplx)

    #endif

    return ret;

}


ADE_API_RET_T ADE_Utils_memcpy_float(ADE_FLOATING_T *p_dst,ADE_FLOATING_T *p_src,ADE_UINT32_T len)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    ADE_UINT32_T vec_idx=0;
    ADE_blas_level1_T *p_blas_l1=NULL;
    ADE_API_RET_T ret_init=ADE_RET_ERROR;
    ADE_API_RET_T ret_copy=ADE_RET_ERROR;
     ADE_API_RET_T ret_conf=ADE_RET_ERROR;


     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_float,p_dst);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_float,p_src);

     #if (ADE_UTILS_IMP==ADE_UTILS_USE_BLAS)
    ret_init=ADE_Blas_level1_Init(&p_blas_l1,ADE_MATH_REAL);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_float,ret_init);

    ret_conf=ADE_Utils_memcpy_blasconfig(p_blas_l1,p_dst,p_src,len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_float,ret_conf);

    ret_copy=ADE_Blas_level1_copy(p_blas_l1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_float,ret_copy);

    ADE_Blas_level1_Release(p_blas_l1);


    #elif (ADE_UTILS_IMP==ADE_UTILS_USE_CUSTOM)

    for (vec_idx=0; vec_idx<len; vec_idx++)
    {
        p_dst[vec_idx]=p_src[vec_idx];
    }
    #else

        #error (ADE_UTILS_IMP in ADE_Utils_memcpy_float)

    #endif

    return ret;

}

ADE_API_RET_T ADE_Utils_memcpy_cplx(ADE_CPLX_T *p_dst,ADE_CPLX_T *p_src,ADE_UINT32_T len)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    ADE_UINT32_T vec_idx=0;
    ADE_blas_level1_T *p_blas_l1=NULL;
    ADE_API_RET_T ret_init=ADE_RET_ERROR;
    ADE_API_RET_T ret_copy=ADE_RET_ERROR;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_cplx,p_dst);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_cplx,p_src);


     #if (ADE_UTILS_IMP==ADE_UTILS_USE_BLAS)
    ret_init=ADE_Blas_level1_Init(&p_blas_l1,ADE_MATH_CPLX);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_cplx,ret_init);

    ret_conf=ADE_Utils_memcpy_blasconfig(p_blas_l1,p_dst,p_src,len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_cplx,ret_conf);

    ret_copy=ADE_Blas_level1_copy(p_blas_l1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_cplx,ret_copy);

    ADE_Blas_level1_Release(p_blas_l1);

    #elif (ADE_UTILS_IMP==ADE_UTILS_USE_CUSTOM)

    for (vec_idx=0; vec_idx<len; vec_idx++)
    {
        p_dst[vec_idx]=p_src[vec_idx];
    }
    #else

        #error (ADE_UTILS_IMP in ADE_Utils_memcpy_cplx)

    #endif

    return ret;

}



#define ADE_ISWAP(a,b) itemp=(a);(a)=(b);(b)=itemp;
ADE_VOID_T ADE_Utils_indexx(ADE_ULONG_T n, ADE_FLOATING_T arr[], ADE_ULONG_T indx[])
/*Indexes an array arr[1..n], i.e., outputs the array indx[1..n] such that arr[indx[j]] is
in ascending order for j = 1, 2, . . . , N . The input quantities n and arr are not changed.*/
{
ADE_ULONG_T i,indxt,ir=n,itemp,j,k,l=1;
ADE_INT32_T jstack=0,*istack;
ADE_FLOATING_T a;
istack=ivector(1,NR_STACK);
    for (j=1; j<=n; j++) indx[j]=j;
    for (;;)
    {
        if (ir-l < NR_M)
        {
            for (j=l+1; j<=ir; j++)
            {
                indxt=indx[j];
                a=arr[indxt];
                for (i=j-1; i>=l; i--)
                {
                    if (arr[indx[i]] <= a) break;
                    indx[i+1]=indx[i];
                }
                indx[i+1]=indxt;
            }
            if (jstack == 0) break;
            ir=istack[jstack--];
            l=istack[jstack--];
        }
        else
        {
            k=(l+ir) >> 1;
            ADE_ISWAP(indx[k],indx[l+1]);
            if (arr[indx[l]] > arr[indx[ir]])
            {
                ADE_ISWAP(indx[l],indx[ir])
            }
            if (arr[indx[l+1]] > arr[indx[ir]])
            {
                ADE_ISWAP(indx[l+1],indx[ir])
            }
            if (arr[indx[l]] > arr[indx[l+1]])
            {
                ADE_ISWAP(indx[l],indx[l+1])
            }
            i=l+1;
            j=ir;
            indxt=indx[l+1];
            a=arr[indxt];
            for (;;)
            {
                do i++;
                while (arr[indx[i]] < a);
                do j--;
                while (arr[indx[j]] > a);
                if (j < i) break;
                ADE_ISWAP(indx[i],indx[j])
            }
            indx[l+1]=indx[j];
            indx[j]=indxt;
            jstack += 2;
            if (jstack > NR_STACK) nrerror("NR_STACK too small in ADE_Utils_indexx.");
            if (ir-i+1 >= j-l)
            {
                istack[jstack]=ir;
                istack[jstack-1]=i;
                ir=j-1;
            }
            else
            {
                istack[jstack]=j-1;
                istack[jstack-1]=l;
                l=i;
            }
        }
    }
    free_ivector(istack,1,NR_STACK);
}

ADE_VOID_T ADE_Utils_indexx_descend(ADE_ULONG_T n, ADE_FLOATING_T arr[], ADE_ULONG_T indx[])
/*Indexes an array arr[1..n], i.e., outputs the array indx[1..n] such that arr[indx[j]] is
in ascending order for j = 1, 2, . . . , N . The input quantities n and arr are not changed.*/
{
ADE_ULONG_T i,indxt,ir=n,itemp,j,k,l=1;
ADE_INT32_T jstack=0,*istack;
ADE_FLOATING_T a;
istack=ivector(1,NR_STACK);
    for (j=1; j<=n; j++) indx[j]=j;
    for (;;)
    {
        if (ir-l < NR_M)
        {
            for (j=l+1; j<=ir; j++)
            {
                indxt=indx[j];
                a=arr[indxt];
                for (i=j-1; i>=l; i--)
                {
                    if (arr[indx[i]] > a) break;
                    indx[i+1]=indx[i];
                }
                indx[i+1]=indxt;
            }
            if (jstack == 0) break;
            ir=istack[jstack--];
            l=istack[jstack--];
        }
        else
        {
            k=(l+ir) >> 1;
            ADE_ISWAP(indx[k],indx[l+1]);
            if (arr[indx[l]] <= arr[indx[ir]])
            {
                ADE_ISWAP(indx[l],indx[ir])
            }
            if (arr[indx[l+1]] <= arr[indx[ir]])
            {
                ADE_ISWAP(indx[l+1],indx[ir])
            }
            if (arr[indx[l]] <= arr[indx[l+1]])
            {
                ADE_ISWAP(indx[l],indx[l+1])
            }
            i=l+1;
            j=ir;
            indxt=indx[l+1];
            a=arr[indxt];
            for (;;)
            {
                do i++;
                while (arr[indx[i]] > a);
                do j--;
                while (arr[indx[j]] < a);
                if (j < i) break;
                ADE_ISWAP(indx[i],indx[j])
            }
            indx[l+1]=indx[j];
            indx[j]=indxt;
            jstack += 2;
            if (jstack > NR_STACK) nrerror("NR_STACK too small in ADE_Utils_indexx.");
            if (ir-i+1 >= j-l)
            {
                istack[jstack]=ir;
                istack[jstack-1]=i;
                ir=j-1;
            }
            else
            {
                istack[jstack]=j-1;
                istack[jstack-1]=l;
                l=i;
            }
        }
    }
    free_ivector(istack,1,NR_STACK);
}

ADE_API_RET_T ADE_Utils_memoryless_expander(ADE_POLYFIT_T* p_poly,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o)
{

  //  ADE_UINT32_T frame_size = p_blow->buff_size;

    ADE_FLOATING_T data=0.0;
    ADE_FLOATING_T* p_coeffs=NULL;
    ADE_INT32_T n_pieces = 0;
    ADE_FLOATING_T *p_x_breaks = NULL;
    ADE_API_RET_T ret = ADE_RET_ERROR;
       ADE_INT32_T n_coeff_per_piece = 0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memoryless_expander,p_poly);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memoryless_expander,frame_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memoryless_expander,y_o);

    p_coeffs=p_poly->p_poly_coeffs;
    n_pieces = p_poly->n_breaks-1;
    p_x_breaks = p_poly->p_breaks;
    n_coeff_per_piece = p_poly->poly_order+1;
//for (i=0;i<frame_size;i++)
//{
   *y_o=0;
    data = *frame_i;

    ret = ADE_Utils_domemoryless_expander(data,n_pieces,n_coeff_per_piece,p_x_breaks,p_coeffs,y_o);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memoryless_expander,ret);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Utils_domemoryless_expander(ADE_FLOATING_T data,ADE_INT32_T n_pieces,ADE_INT32_T n_coeff_per_piece,ADE_FLOATING_T *p_x_breaks,ADE_FLOATING_T* p_coeffs,ADE_FLOATING_T* y_o)
{

 ADE_UINT32_T zz=0;
 ADE_UINT32_T i=0, k=0;
 ADE_UINT32_T coeffs_row_idx=0, coeffs_col_idx=0;
 ADE_INT32_T piece_min_val=0;


  ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_domemoryless_expander,p_x_breaks);
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_domemoryless_expander,y_o);
  ADE_CHECK_VALUE_MAJOR(ADE_CLASS_UTILS,ADE_METHOD_domemoryless_expander,n_pieces,"%d",piece_min_val);


    if ( data>=0 )
    {
        for (k=0;k<n_pieces;k++)
        {
             if (k==0)
             {
                  if ( (data>=p_x_breaks[k]) && (data<p_x_breaks[k+1]) )
                  {
                      zz=k;
                    break;
                  }

             }

        else if (k==(n_pieces-1))
        {
             if ( (data>=p_x_breaks[k]) && (data<=p_x_breaks[k+1]) )
             {
                 zz=k;
                 break;
             }
        }
        else
        {
             if ( (data>=p_x_breaks[k]) && (data<p_x_breaks[k+1]) )
             {
                   zz=k;
                    break;
             }

        }



        }


    }


    else
    {
        zz=0;
        data = 0;
    }

    coeffs_row_idx=zz*n_coeff_per_piece;

    for(i=0;i<n_coeff_per_piece;i++)
    {
        coeffs_col_idx = n_coeff_per_piece-1-i;
       *y_o +=p_coeffs[coeffs_row_idx+coeffs_col_idx]*pow((data-p_x_breaks[zz]),i);
    }


      return ADE_RET_SUCCESS;

}


/*********************** Private Functions ******************************/

static ADE_API_RET_T ADE_Utils_PrintRowArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,ADE_UINT32_T n_var_per_printrow, ADE_UTILS_ROW_INFO_T row_info,FILE *p_stream)
{
    ADE_UINT32_T i=0,k=0,incremental_idx=0;
    ADE_UINT32_T len = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArrayReal,p_var);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArrayReal,p_stream);


    if (stop_0based_col_idx<start_0based_col_idx)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_PrintRowArrayReal,stop_0based_col_idx,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//34;
    }

    len=stop_0based_col_idx-start_0based_col_idx+1;


    for (i=start_0based_col_idx; i<=stop_0based_col_idx; i++)
    {
        if ( (incremental_idx%n_var_per_printrow)==(0) )
        {
            k++;
            if (len>=k*n_var_per_printrow)
            {
                if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                {
                    if (len>1 && start_0based_col_idx==0)
                    {
                        ADE_LOG(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,start_0based_col_idx+k*n_var_per_printrow);
                    }

                }
                ADE_LOG(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
            }
            else
            {
                if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                {
                    if (len>1 && start_0based_col_idx==0)
                    {
                        ADE_LOG(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,stop_0based_col_idx+1);
                    }
                    if (i==stop_0based_col_idx)
                    {
                        ADE_LOG(p_stream,"%*.*lf\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                    }
                    else
                    {
                        ADE_LOG(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                    }

                }
                else
                {
                    if (i==stop_0based_col_idx)
                    {
                        ADE_LOG(p_stream,"%*.*lf\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                    }
                    else
                    {
                        ADE_LOG(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                    }
                }



            }

        }
        else
        {
            if (i==stop_0based_col_idx)
            {
                ADE_LOG(p_stream,"%*.*lf\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
            }
            else
            {
                ADE_LOG(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
            }


        }
        incremental_idx++;
    }
    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Utils_PrintRowArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_UINT32_T n_var_per_printrow,ADE_UTILS_ROW_INFO_T row_info, FILE *p_stream)
{

    ADE_UINT32_T i=0,k=0,incremental_idx=0;
    ADE_UINT32_T len = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArrayCplx,p_var);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArrayCplx,p_stream);

    if (stop_0based_col_idx<start_0based_col_idx)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_PrintRowArrayCplx,stop_0based_col_idx,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//34;
    }

    len=stop_0based_col_idx-start_0based_col_idx+1;


    for (i=start_0based_col_idx; i<=stop_0based_col_idx; i++)
    {
        if ( (incremental_idx%n_var_per_printrow)==(0) )
        {
            k++;
            if (len>=k*n_var_per_printrow)
            {
                if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                {
                    if (len>1 && start_0based_col_idx==0)
                    {
                        ADE_LOG(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,start_0based_col_idx+k*n_var_per_printrow);
                    }
                }
                ADE_LOG(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
            }
            else
            {
                if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                {
                    if (len>1 && start_0based_col_idx==0)
                    {
                        ADE_LOG(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,stop_0based_col_idx+1);
                    }
                    if (i==stop_0based_col_idx)
                    {
                        ADE_LOG(p_stream,"%*.*lf%+*.*lfi\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                    }
                    else
                    {
                        ADE_LOG(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                    }

                }
                else
                {
                    if (i==stop_0based_col_idx)
                    {
                        ADE_LOG(p_stream,"%*.*lf%+*.*lfi\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                    }
                    else
                    {
                        ADE_LOG(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                    }
                }



            }

        }
        else
        {
            if (i==stop_0based_col_idx)
            {
                ADE_LOG(p_stream,"%*.*lf%+*.*lfi\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
            }
            else
            {
                ADE_LOG(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
            }


        }
        incremental_idx++;
    }
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Utils_PrintColArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx,   FILE *p_stream)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArrayReal,p_var);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArrayReal,p_stream);

    if (stop_0based_row_idx<start_0based_row_idx)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_PrintColArrayReal,stop_0based_row_idx,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//34;
    }

    for (i=start_0based_row_idx; i<=stop_0based_row_idx; i++)
    {
        ADE_LOG(p_stream,"(%u) %*.*lf\n",i+1,ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,p_var[i]);
    }

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Utils_PrintColArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, FILE *p_stream)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArrayCplx,p_var);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArrayCplx,p_stream);

    if (stop_0based_row_idx<start_0based_row_idx)
    {
        return ADE_RET_ERROR;
    }

    for (i=start_0based_row_idx; i<=stop_0based_row_idx; i++)
    {
        ADE_LOG(p_stream,"(%u) %*.*lf%+*.*lfi\n",i+1,ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));

    }

    return ADE_RET_SUCCESS;
}




static ADE_API_RET_T ADE_Utils_PrintRowArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{
    ADE_INT32_T lines=0,columns=0;
    ADE_UINT32_T n_var_per_printrow=0;
    ADE_UTILS_ROW_INFO_T row_info=ADE_UTILS_FIRST_PRINT_ROW;
    ADE_UINT32_T len = stop_0based_col_idx-start_0based_col_idx+1;


     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArray,p_var);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArray,p_varname);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintRowArray,p_stream);

    ADE_Utils_Get_Terminal_size(&lines ,&columns);

    if (math_type==ADE_MATH_REAL)
    {
        n_var_per_printrow=floor(columns/ADE_UTILS_PRINTF_FLOAT_WIDTH);
        if (len==1)
        {
            ADE_LOG(p_stream,"\n%s =\n",p_varname);
        }
        else
        {
            ADE_LOG(p_stream,"\n%s(1,%u:%u) =\n",p_varname,start_0based_col_idx+1,stop_0based_col_idx+1);
        }

        ADE_Utils_PrintRowArrayReal((ADE_FLOATING_T*)p_var, start_0based_col_idx,stop_0based_col_idx,n_var_per_printrow,row_info,p_stream);
    }
    else if(math_type==ADE_MATH_CPLX)
    {
        n_var_per_printrow=floor(columns/(ADE_UTILS_PRINTF_FLOAT_WIDTH+ADE_UTILS_PRINTF_FLOAT_WIDTH+2));
        if (len==1)
        {
            ADE_LOG(p_stream,"\n%s =\n",p_varname);
        }
        else
        {
            ADE_LOG(p_stream,"\n%s(1,%u:%u) =\n",p_varname,start_0based_col_idx+1,stop_0based_col_idx+1);
        }

        ADE_Utils_PrintRowArrayCplx((ADE_CPLX_T*)p_var,start_0based_col_idx, stop_0based_col_idx,n_var_per_printrow,row_info,p_stream);
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_PrintRowArray,math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Utils_PrintColArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{
    ADE_UINT32_T len = stop_0based_row_idx-start_0based_row_idx+1;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArray,p_var);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArray,p_varname);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintColArray,p_stream);

    if (math_type==ADE_MATH_REAL)
    {
        if (len==1)
        {
            ADE_LOG(p_stream,"\n%s =\n\n",p_varname);
        }
        else
        {
            ADE_LOG(p_stream,"\n%s(%u:%u,1) =\n\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1);
        }

        ADE_Utils_PrintColArrayReal((ADE_FLOATING_T*)p_var, start_0based_row_idx,stop_0based_row_idx,p_stream);
    }
    else if(math_type==ADE_MATH_CPLX)
    {
        if (len==1)
        {
            ADE_LOG(p_stream,"\n%s =\n\n",p_varname);
        }
        else
        {
            ADE_LOG(p_stream,"\n%s(%u:%u,1) =\n\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1);
        }

        ADE_Utils_PrintColArrayCplx((ADE_CPLX_T*)p_var, start_0based_row_idx,stop_0based_row_idx,p_stream);
    }
    else
    {
        //ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Utils_PrintArray);
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_PrintColArray,math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//34;
    }

    return ADE_RET_SUCCESS;

}




static ADE_API_RET_T ADE_Utils_PrintMatrixReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, FILE *p_stream)
{
    ADE_UINT32_T row_idx=0,k=0;
    ADE_INT32_T lines=0,columns=0;
    ADE_UINT32_T n_var_per_printrow=0;
    ADE_UINT32_T n_block_cols=0,n_col=0;
    ADE_UINT32_T start_col_idx_int=0,stop_col_idx_int=0;
    ADE_INT32_T check_col=0;
    ADE_UTILS_ROW_INFO_T row_info;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrixReal,p_var);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrixReal,p_stream);

    ADE_Utils_Get_Terminal_size(&lines ,&columns  );

    n_col=stop_0based_col_idx-start_0based_col_idx+1;
    n_var_per_printrow=floor(columns/ADE_UTILS_PRINTF_FLOAT_WIDTH);
    n_block_cols=ceil((double)(n_col)/n_var_per_printrow); //colonne totali/colonne printabili

    for (k=0; k<n_block_cols; k++)
    {

        start_col_idx_int=k*n_var_per_printrow; // forzo lunghezza array a n_var stampabili così fa tutte le righe e poi passa ad altre colonne
        check_col = (k+1)*n_var_per_printrow-n_col;
        if (check_col>0)
        {
            stop_col_idx_int=start_col_idx_int+n_var_per_printrow-check_col-1;
        }
        else
        {
            stop_col_idx_int=(k+1)*n_var_per_printrow-1;
        }
        for (row_idx=start_0based_row_idx; row_idx<=stop_0based_row_idx; row_idx++)
        {
            if (row_idx==start_0based_row_idx)
            {
                row_info=ADE_UTILS_FIRST_PRINT_ROW;
            }
            else
            {
                row_info=ADE_UTILS_NOTFIRST_PRINT_ROW;
            }


            ADE_Utils_PrintRowArrayReal(&(p_var[row_idx*n_col]),start_col_idx_int,stop_col_idx_int,n_var_per_printrow,row_info,p_stream);
        }

    }

    return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Utils_PrintMatrixCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, FILE *p_stream)
{
    ADE_UINT32_T row_idx=0,k=0;
    ADE_INT32_T lines=0,columns=0;
    ADE_UINT32_T n_var_per_printrow=0;
    ADE_UINT32_T n_block_cols=0,n_col=0;
    ADE_UINT32_T start_col_idx_int=0,stop_col_idx_int=0;
    ADE_INT32_T check_col=0;
    ADE_UTILS_ROW_INFO_T row_info;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrixCplx,p_var);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrixCplx,p_stream);



    ADE_Utils_Get_Terminal_size(&lines ,&columns  );

    n_col=stop_0based_col_idx-start_0based_col_idx+1;
    n_var_per_printrow=floor(columns/(ADE_UTILS_PRINTF_FLOAT_WIDTH+ADE_UTILS_PRINTF_FLOAT_WIDTH+2));
    n_block_cols=ceil((double)(n_col)/n_var_per_printrow); //colonne totali/colonne printabili

    for (k=0; k<n_block_cols; k++)
    {

        start_col_idx_int=k*n_var_per_printrow; // forzo lunghezza array a n_var stampabili così fa tutte le righe e poi passa ad altre colonne
        check_col = (k+1)*n_var_per_printrow-n_col;
        if (check_col>0)
        {
            stop_col_idx_int=start_col_idx_int+n_var_per_printrow-check_col-1;
        }
        else
        {
            stop_col_idx_int=(k+1)*n_var_per_printrow-1;
        }

        for (row_idx=start_0based_row_idx; row_idx<=stop_0based_row_idx; row_idx++)
        {
            if (row_idx==start_0based_row_idx)
            {
                row_info=ADE_UTILS_FIRST_PRINT_ROW;
            }
            else
            {
                row_info=ADE_UTILS_NOTFIRST_PRINT_ROW;
            }
            ADE_Utils_PrintRowArrayCplx(&(p_var[row_idx*n_col]),start_col_idx_int,stop_col_idx_int,n_var_per_printrow,row_info,p_stream);
        }

    }

    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Utils_PrintMatrix(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrix,p_var);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrix,p_varname);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_PrintMatrix,p_stream);

    if (math_type==ADE_MATH_REAL)
    {
        ADE_LOG(p_stream,"\n%s(%u:%u,%u:%u) =\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1,start_0based_col_idx+1,stop_0based_col_idx+1);
        ADE_Utils_PrintMatrixReal( (ADE_FLOATING_T*)p_var,start_0based_row_idx,stop_0based_row_idx,start_0based_col_idx,stop_0based_col_idx,p_stream);
    }
    else if (math_type==ADE_MATH_CPLX)
    {
        ADE_LOG(p_stream,"\n%s(%u:%u,%u:%u) =\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1,start_0based_col_idx+1,stop_0based_col_idx+1);
        ADE_Utils_PrintMatrixCplx( (ADE_CPLX_T*)p_var,start_0based_row_idx,stop_0based_row_idx,start_0based_col_idx,stop_0based_col_idx,p_stream);
    }
    else
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UTILS,ADE_METHOD_PrintMatrix,math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Utils_memset_blasconfig(ADE_blas_level1_T *p_blas_l1,ADE_FLOATING_T *vec,ADE_UINT32_T len,ADE_VOID_T *memset_val)
{

ADE_API_RET_T ret_N=ADE_RET_ERROR;
ADE_API_RET_T ret_INCX=ADE_RET_ERROR;
ADE_API_RET_T ret_INCY=ADE_RET_ERROR;
ADE_API_RET_T ret_X=ADE_RET_ERROR;
ADE_API_RET_T ret_Y=ADE_RET_ERROR;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,p_blas_l1);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,vec);

    ret_N=ADE_Blas_level1_SetN(p_blas_l1,len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,ret_N);

    ret_INCX=ADE_Blas_level1_SetINCX(p_blas_l1,0);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,ret_INCX);

    ret_INCY=ADE_Blas_level1_SetINCY(p_blas_l1,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,ret_INCY);

    ret_X=ADE_Blas_level1_SetX(p_blas_l1,memset_val);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,ret_X);

    ret_Y=ADE_Blas_level1_SetY(p_blas_l1,vec);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memset_blasconfig,ret_Y);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Utils_memcpy_blasconfig(ADE_blas_level1_T *p_blas_l1,ADE_VOID_T *p_dst,ADE_VOID_T *p_src,ADE_UINT32_T len)
{

ADE_API_RET_T ret_N=ADE_RET_ERROR;
ADE_API_RET_T ret_INCX=ADE_RET_ERROR;
ADE_API_RET_T ret_INCY=ADE_RET_ERROR;
ADE_API_RET_T ret_X=ADE_RET_ERROR;
ADE_API_RET_T ret_Y=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,p_blas_l1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,p_dst);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,p_src);

    ret_N=ADE_Blas_level1_SetN(p_blas_l1,len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,ret_N);

    ret_INCX=ADE_Blas_level1_SetINCX(p_blas_l1,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,ret_INCX);

    ret_INCY=ADE_Blas_level1_SetINCY(p_blas_l1,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,ret_INCY);

    ret_X=ADE_Blas_level1_SetX(p_blas_l1,p_src);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,ret_X);

    ret_Y=ADE_Blas_level1_SetY(p_blas_l1,p_dst);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UTILS,ADE_METHOD_memcpy_blasconfig,ret_Y);


   return ADE_RET_SUCCESS;

}


