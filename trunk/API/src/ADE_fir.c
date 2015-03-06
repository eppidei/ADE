#include "headers/ADE_fir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static ADE_API_RET_T filter_DII_T (ADE_FIR_T* p_fir);
static ADE_API_RET_T filter_DII_T_blas (ADE_FIR_T* p_fir);
static ADE_API_RET_T filter_DII_T_matrix(ADE_FIR_T* p_fir);
static ADE_API_RET_T filter_DII_T_matrix_blas (ADE_FIR_T* p_fir);

ADE_API_RET_T ADE_Fir_Init(ADE_FIR_T** dp_this, ADE_UINT32_T fir_order,ADE_UINT32_T buff_len)
{
    ADE_blas_level1_T *p_Blas_L1;
    ADE_FLOATING_T default_gain = 1.0;
    ADE_FLOATING_T *p_state=NULL;

    ADE_FIR_T* pthis = calloc(1,sizeof(ADE_FIR_T));

    if (pthis!=NULL)
    {
        pthis->buff_len=buff_len;
        pthis->gain=default_gain;
        pthis->filter_order = fir_order;
        p_state=calloc(fir_order+1,sizeof(ADE_FLOATING_T));
//        if (p_state==NULL)
//        {
//            ADE_PRINT_ERRORS(ADE_MEM,p_state,"%d",ADE_Fir_Init);
//            return ADE_E17;
//        }
        pthis->p_state=p_state;
        pthis->p_num=calloc(fir_order+1,sizeof(ADE_FLOATING_T));
//        if (pthis->p_num==NULL)
//        {
//            ADE_PRINT_ERRORS(ADE_MEM,pthis->p_num,"%d",ADE_Fir_Init);
//            return ADE_E17;
//        }

        ADE_Blas_level1_Init(&p_Blas_L1,ADE_REAL);
        ADE_Blas_level1_setN(p_Blas_L1,fir_order);
        ADE_Blas_level1_setINCX(p_Blas_L1,1);
        ADE_Blas_level1_setINCY(p_Blas_L1,1);

        pthis->p_Blas_L1=p_Blas_L1;

         *dp_this=pthis;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,pthis,"%p",ADE_Fir_Init);
        return ADE_E17;
    }




    return ADE_DEFAULT_RET;
}
ADE_VOID_T ADE_Fir_Release(ADE_FIR_T* p_fir)
{
    ADE_CHECKNFREE(p_fir->p_state);
    ADE_CHECKNFREE(p_fir->p_num);
    ADE_CHECKNFREE(p_fir);

}

ADE_API_RET_T ADE_Fir_setNum(ADE_FIR_T* p_fir, ADE_FLOATING_T *p_num)
{

    memcpy(p_fir->p_num,p_num,(p_fir->filter_order+1)*sizeof(ADE_FLOATING_T));

    return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Fir_ResetState(ADE_FIR_T* p_fir)
{

    memset(p_fir->p_state,0,(p_fir->filter_order+1)*sizeof(ADE_FLOATING_T));

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Fir_setInbuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
    p_fir->p_in=p_buff;

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Fir_setOutbuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
    p_fir->p_out=p_buff;

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_fir,ADE_FILTER_IMP_CHOICE_T filt_imp_type)
{

    if (filt_imp_type==trasp_II)
   {
       p_fir->filter_func=filter_DII_T;
   }
   else if (filt_imp_type==trasp_II_blas)
   {
     p_fir->filter_func=filter_DII_T_blas;
   }
   else
   {
       #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        ADE_PRINT_ERRORS(ADE_INCHECKS,filt_imp_type,"%d",ADE_Fir_setFilt_Implementation);
        #endif

            return ADE_E9;
   }

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Fir_Step(ADE_FIR_T* p_fir)
{
//    ADE_UINT32_T i=0;

#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

if ((p_fir->p_state)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_fir->p_state,"%p",ADE_Fir_Step);
    return ADE_E14;
}

if ((p_fir->p_in)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_fir->p_in),"%p",ADE_Fir_Step);
    return ADE_E12;
}
if ((p_fir->p_out)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_fir->p_out),"%p",ADE_Fir_Step);
    return ADE_E13;
}

if ((p_fir->filter_func)==NULL)
{
     ADE_PRINT_ERRORS(ADE_INCHECKS,(p_fir->filter_func),"%p",ADE_Fir_Step);
    return ADE_E13;

}

#endif


       (p_fir->filter_func)(p_fir);//(p_iir-> dp_section_buffers)[i], (p_iir-> dp_section_buffers)[i+1], (p_iir-> dp_denoms)[i], order, (p_iir-> dp_nums)[i], (p_iir-> p_gains)[i],(p_iir-> dp_states)[i],p_iir->buff_len,p_iir->p_Blas_L1);

	//memcpy(p_fir->p_out ,(p_fir-> dp_section_buffers)[p_fir->n_SOS_sections],p_fir->buff_len*sizeof(ADE_FLOATING_T));

	return ADE_DEFAULT_RET;

}

static ADE_API_RET_T filter_DII_T_matrix(ADE_FIR_T* p_fir)
{
    printf("TO BE IMPLEMENTED\n");
    //matrice order*len_frame di stati
    return -1;

}

static ADE_API_RET_T filter_DII_T_matrix_blas (ADE_FIR_T* p_fir)
{
    printf("TO BE IMPLEMENTED\n");
    //matrice order*len_frame di stati
    return -1;
}

static ADE_API_RET_T filter_DII_T_blas (ADE_FIR_T* p_fir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{
    ADE_UINT32_T i=0,k=0;
    //ADE_UINT32_T active_section = p_iir->active_section;
    ADE_FLOATING_T *in = p_fir->p_in;
    ADE_FLOATING_T *out = p_fir->p_out;
    //ADE_FLOATING_T *a = (p_iir-> dp_denoms)[active_section];
    ADE_UINT32_T order = p_fir->filter_order;
    ADE_FLOATING_T *b = (p_fir-> p_num);
    ADE_FLOATING_T gain = (p_fir-> gain);
    ADE_FLOATING_T *state = (p_fir-> p_state);
    ADE_UINT32_T len_frame = p_fir->buff_len;

    ADE_blas_level1_T *p_Blas_L1 = p_fir->p_Blas_L1;
    ADE_FLOATING_T *temp_buffer = calloc(order,sizeof(ADE_FLOATING_T));
    ADE_UINT32_T temp_buff_size = order*sizeof(ADE_FLOATING_T);
    ADE_FLOATING_T ALPHA=0;



    for (k=0;k<len_frame;k++)
    {

        out[k] = gain*b[0]*(in[k])+state[0];
        memcpy(temp_buffer,&state[0+1],temp_buff_size);
        /*************/
        ADE_Blas_level1_setY(p_Blas_L1,temp_buffer);
        ADE_Blas_level1_setX(p_Blas_L1,&b[0+1]);
        ALPHA=gain*in[k];
        ADE_Blas_level1_setALPHA(p_Blas_L1,&ALPHA);
        ADE_Blas_level1_axpy(p_Blas_L1);
        /*****************/
        memcpy(&state[0],temp_buffer,temp_buff_size);
        memset(temp_buffer,0,temp_buff_size);

//        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
//        {
//            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
//        }

    }

    ADE_CHECKNFREE(temp_buffer);

    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T filter_DII_T (ADE_FIR_T* p_fir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{
    ADE_UINT32_T i=0,k=0;
    //ADE_UINT32_T active_section = p_iir->active_section;
    ADE_FLOATING_T *in = p_fir->p_in;
    ADE_FLOATING_T *out = p_fir->p_out;
    //ADE_FLOATING_T *a = (p_iir-> dp_denoms)[active_section];
    ADE_UINT32_T order = p_fir->filter_order;
    ADE_FLOATING_T *b = (p_fir-> p_num);
    ADE_FLOATING_T gain = (p_fir-> gain);
    ADE_FLOATING_T *state = (p_fir-> p_state);
    ADE_UINT32_T len_frame = p_fir->buff_len;




    for (k=0;k<len_frame;k++)
    {


        out[k] = gain*b[0]*(in[k])+state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=gain*b[i+1]*(in[k])+state[i+1];
        }

    }

    return ADE_DEFAULT_RET;

}

