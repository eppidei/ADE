#include "headers/ADE_fir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_Error_Handler.h"



static ADE_API_RET_T ADE_Fir_filter_DII_T (ADE_FIR_T* p_fir);
static ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_fir,ADE_FIR_IMP_CHOICE_T filt_imp_type);
static ADE_API_RET_T ADE_Fir_dofilter_DII_T_blas (ADE_blas_level1_T *p_Blas_L1, ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_FLOATING_T *p_temp_buffer,ADE_UINT32_T temp_buff_size);
static ADE_API_RET_T ADE_Fir_dofilter_DII_T_custom(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order);

ADE_API_RET_T ADE_Fir_Init(ADE_FIR_T** dp_this, ADE_UINT32_T fir_order,ADE_UINT32_T buff_len,ADE_FIR_IMP_CHOICE_T filt_imp_type)
{
    ADE_blas_level1_T *p_Blas_L1;
    ADE_FLOATING_T default_gain = 1.0;
    ADE_FLOATING_T *p_state=NULL;
    ADE_API_RET_T ret=ADE_RET_ERROR;




    ADE_FIR_T* pthis = calloc(1,sizeof(ADE_FIR_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,pthis);

        pthis->buff_len=buff_len;
        pthis->gain=default_gain;
        pthis->filter_order = fir_order;
        pthis->filt_imp_type=filt_imp_type;
        ret = ADE_Fir_setFilt_Implementation(pthis,pthis->filt_imp_type);
        ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Init,ret);

        /******** state buffer allocation ***********/
        p_state=calloc(fir_order+1,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,p_state);

        pthis->p_state=p_state;

        /********** numerator buffer allocation *************/
        pthis->p_num=calloc(fir_order+1,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,pthis->p_num);

        /********* Blas1 Allocation ***************/

        if (pthis->filt_imp_type==ADE_FIR_TRASP_II)
        {
            #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
            ADE_Blas_level1_Init(&p_Blas_L1,ADE_REAL);
             ADE_Blas_level1_SetN(p_Blas_L1,fir_order);
            ADE_Blas_level1_SetINCX(p_Blas_L1,1);
            ADE_Blas_level1_SetINCY(p_Blas_L1,1);

            pthis->p_Blas_L1=p_Blas_L1;
            #endif

        }
        else
        {

             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,ADE_METHOD_Init,pthis->filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;

        }

        /*********** Temp buffer allocation ***********/

        pthis->p_tempbuff=calloc(fir_order,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,pthis->p_tempbuff);

         *dp_this=pthis;

    return ADE_RET_SUCCESS;
}
ADE_VOID_T ADE_Fir_Release(ADE_FIR_T* p_fir)
{
    ADE_CHECKNFREE(p_fir->p_state);
    ADE_CHECKNFREE(p_fir->p_num);
    ADE_CHECKNFREE(p_fir->p_tempbuff);
     #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
    ADE_Blas_level1_Release(p_fir->p_Blas_L1);
    #endif
    ADE_CHECKNFREE(p_fir);
}

/********** Set Methods ***************/

ADE_API_RET_T ADE_Fir_setNum(ADE_FIR_T* p_fir, ADE_FLOATING_T *p_num)
{

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_setNum,p_num);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_setNum,p_fir);

    ADE_Utils_memcpy_float(p_fir->p_num,p_num,(p_fir->filter_order+1));

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Fir_ResetState(ADE_FIR_T* p_fir,ADE_FLOATING_T rst_val)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_ResetState,p_fir);

    ADE_Utils_memset_float(p_fir->p_state,(p_fir->filter_order+1),rst_val);
   // memset(p_fir->p_state,0,(p_fir->filter_order+1)*sizeof(ADE_FLOATING_T));

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Fir_SetInBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetInBuff,p_buff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetInBuff,p_fir);

     p_fir->p_in=p_buff;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Fir_SetOutBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetOutBuff,p_buff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetOutBuff,p_fir);

    p_fir->p_out=p_buff;

    return ADE_RET_SUCCESS;
}


/********** Processing Methods ***********/

ADE_API_RET_T ADE_Fir_Step(ADE_FIR_T* p_fir)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Step,p_fir);

    (p_fir->filter_func)(p_fir);

	return ADE_RET_SUCCESS;

}


/************** static methods *********************/

static ADE_API_RET_T ADE_Fir_filter_DII_T (ADE_FIR_T* p_fir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{

    ADE_FLOATING_T *p_in =NULL;
    ADE_FLOATING_T *p_out = NULL;
    ADE_UINT32_T order = 0;
    ADE_FLOATING_T *p_b = NULL;
    ADE_FLOATING_T gain = 0;
    ADE_FLOATING_T *p_state = NULL;
    ADE_UINT32_T len_frame = 0;
   #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
    ADE_blas_level1_T *p_Blas_L1 = NULL;
    #endif
    ADE_UINT32_T temp_buff_size = order*sizeof(ADE_FLOATING_T);
    ADE_FLOATING_T *p_temp_buffer=NULL;
    ADE_API_RET_T ret=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_filter_DII_T,p_fir);

    p_in = p_fir->p_in;
    p_out = p_fir->p_out;
    order = p_fir->filter_order;
    p_b = (p_fir-> p_num);
    gain = (p_fir-> gain);
    p_state = (p_fir-> p_state);
    len_frame = p_fir->buff_len;
    p_temp_buffer=p_fir->p_tempbuff;
     #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
    p_Blas_L1 = p_fir->p_Blas_L1;
    #endif



#if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)

ret=ADE_Fir_dofilter_DII_T_blas (p_Blas_L1, p_in,p_b,len_frame,p_out, p_state,gain,p_temp_buffer,temp_buff_size);
ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_filter_DII_T,ret);

    #elif (ADE_FIR_IMP==ADE_FIR_USE_CUSTOM)

   ret=ADE_Fir_dofilter_DII_T_custom(p_in,p_b, len_frame,p_out, p_state, gain,order) ;
   ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_filter_DII_T,ret);
    #else

        #error ADE_FIR_IMP in filter_DII_T
    #endif

    return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Fir_dofilter_DII_T_blas (ADE_blas_level1_T *p_Blas_L1, ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_FLOATING_T *p_temp_buffer,ADE_UINT32_T temp_buff_size)
{

ADE_UINT32_T k=0;
ADE_FLOATING_T ALPHA=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_Blas_L1);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_in);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_out);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_state);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_temp_buffer);

 for (k=0;k<len_frame;k++)
    {

        p_out[k] = gain*p_b[0]*(p_in[k])+p_state[0];
        memcpy(p_temp_buffer,&p_state[0+1],temp_buff_size);
        /*************/
        ADE_Blas_level1_SetY(p_Blas_L1,p_temp_buffer);
        ADE_Blas_level1_SetX(p_Blas_L1,&p_b[0+1]);
        ALPHA=gain*p_in[k];
        ADE_Blas_level1_SetALPHA(p_Blas_L1,&ALPHA);
        ADE_Blas_level1_axpy(p_Blas_L1);
        /*****************/
        memcpy(&p_state[0],p_temp_buffer,temp_buff_size);
        memset(p_temp_buffer,0,temp_buff_size);

    }
     return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Fir_dofilter_DII_T_custom(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order)

{
ADE_UINT32_T k=0,i=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_in);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_b);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_out);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_state);

 for (k=0;k<len_frame;k++)
    {


        p_out[k] = gain*p_b[0]*(p_in[k])+p_state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            p_state[i]=gain*p_b[i+1]*(p_in[k])+p_state[i+1];
        }

    }

 return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_fir,ADE_FIR_IMP_CHOICE_T filt_imp_type)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_setFilt_Implementation,p_fir);

    if (filt_imp_type==ADE_FIR_TRASP_II)
   {
       p_fir->filter_func=ADE_Fir_filter_DII_T;
   }
   else
   {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,ADE_METHOD_setFilt_Implementation,filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
   }

    return ADE_RET_SUCCESS;
}
