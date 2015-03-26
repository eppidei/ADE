#include "headers/ADE_fir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_Error_Handler.h"



static ADE_API_RET_T filter_DII_T (ADE_FIR_T* p_fir);
static ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_fir,ADE_FIR_IMP_CHOICE_T filt_imp_type);
static ADE_VOID_T dofilter_DII_T_blas (ADE_blas_level1_T *p_Blas_L1, ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_FLOATING_T *p_temp_buffer,ADE_UINT32_T temp_buff_size);
static ADE_VOID_T dofilter_DII_T_custom(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order);

ADE_API_RET_T ADE_Fir_Init(ADE_FIR_T** dp_this, ADE_UINT32_T fir_order,ADE_UINT32_T buff_len,ADE_FIR_IMP_CHOICE_T filt_imp_type)
{
    ADE_blas_level1_T *p_Blas_L1;
    ADE_FLOATING_T default_gain = 1.0;
    ADE_FLOATING_T *p_state=NULL;
    ADE_API_RET_T ret=ADE_RET_SUCCESS;




    ADE_FIR_T* pthis = calloc(1,sizeof(ADE_FIR_T));

    if (pthis!=NULL)
    {
        pthis->buff_len=buff_len;
        pthis->gain=default_gain;
        pthis->filter_order = fir_order;
        pthis->filt_imp_type=filt_imp_type;
        ret = ADE_Fir_setFilt_Implementation(pthis,pthis->filt_imp_type);

        /******** state buffer allocation ***********/
        p_state=calloc(fir_order+1,sizeof(ADE_FLOATING_T));
        if (p_state==NULL)
        {
            //ADE_PRINT_ERRORS(ADE_MEM,p_state,"%d",ADE_Fir_Init);
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_FIR,Init,p_state,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//17;
        }
        pthis->p_state=p_state;

        /********** numerator buffer allocation *************/
        pthis->p_num=calloc(fir_order+1,sizeof(ADE_FLOATING_T));
        if (pthis->p_num==NULL)
        {
            //ADE_PRINT_ERRORS(ADE_MEM,pthis->p_num,"%d",ADE_Fir_Init);
             ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_FIR,Init,pthis->p_num,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//17;
        }

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

             //ADE_PRINT_ERRORS(ADE_INCHECKS,pthis->filt_imp_type,"%d",ADE_Fir_Init);
             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,Init,pthis->filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;//13;

        }

        /*********** Temp buffer allocation ***********/

        pthis->p_tempbuff=calloc(fir_order,sizeof(ADE_FLOATING_T));
        if (pthis->p_tempbuff==NULL)
        {
            //ADE_PRINT_ERRORS(ADE_MEM,pthis->p_tempbuff,"%d",ADE_Fir_Init);
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_FIR,Init,pthis->p_tempbuff,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//17;
        }

         *dp_this=pthis;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_FIR,Init,pthis,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//17;
    }




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
    if (p_num==NULL)
    {
     ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,setNum,p_num,"%p",(FILE*)ADE_STD_STREAM);
     return ADE_RET_ERROR;//13;
    }

    if (p_fir==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,setNum,p_fir,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//13;

    }

    //memcpy(p_fir->p_num,p_num,(p_fir->filter_order+1)*sizeof(ADE_FLOATING_T));
    ADE_Utils_memcpy_float(p_fir->p_num,p_num,(p_fir->filter_order+1));

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Fir_ResetState(ADE_FIR_T* p_fir,ADE_FLOATING_T rst_val)
{
    if (p_fir==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,ResetState,p_fir,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//13;

    }
    ADE_Utils_memset_float(p_fir->p_state,(p_fir->filter_order+1),rst_val);
   // memset(p_fir->p_state,0,(p_fir->filter_order+1)*sizeof(ADE_FLOATING_T));

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Fir_SetInBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
    if (p_buff==NULL)
    {
     ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,SetInBuff,p_buff,"%p",(FILE*)ADE_STD_STREAM);
     return ADE_RET_ERROR;//13;
    }

    if (p_fir==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,SetInBuff,p_fir,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//13;

    }
    p_fir->p_in=p_buff;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Fir_SetOutBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
     if (p_buff==NULL)
    {
     ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,SetOutBuff,p_buff,"%p",(FILE*)ADE_STD_STREAM);
     return ADE_RET_ERROR;//13;
    }

    if (p_fir==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,SetOutBuff,p_fir,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//13;

    }
    p_fir->p_out=p_buff;

    return ADE_RET_SUCCESS;
}


/********** Processing Methods ***********/

ADE_API_RET_T ADE_Fir_Step(ADE_FIR_T* p_fir)
{
//    ADE_UINT32_T i=0;

#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

if ((p_fir->p_state)==NULL)
{
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,Step,p_fir->p_state,"%p",(FILE*)ADE_STD_STREAM);
    return ADE_RET_ERROR;//13;
}

if ((p_fir->p_in)==NULL)
{
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,Step,p_fir->p_in,"%p",(FILE*)ADE_STD_STREAM);
    return ADE_RET_ERROR;//13;
}
if ((p_fir->p_out)==NULL)
{
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,Step,p_fir->p_out,"%p",(FILE*)ADE_STD_STREAM);
    return ADE_RET_ERROR;//13;
}

if ((p_fir->filter_func)==NULL)
{
     ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,Step,p_fir->filter_func,"%p",(FILE*)ADE_STD_STREAM);
    return ADE_RET_ERROR;//13;

}

#endif


       (p_fir->filter_func)(p_fir);//(p_iir-> dp_section_buffers)[i], (p_iir-> dp_section_buffers)[i+1], (p_iir-> dp_denoms)[i], order, (p_iir-> dp_nums)[i], (p_iir-> p_gains)[i],(p_iir-> dp_states)[i],p_iir->buff_len,p_iir->p_Blas_L1);

	//memcpy(p_fir->p_out ,(p_fir-> dp_section_buffers)[p_fir->n_SOS_sections],p_fir->buff_len*sizeof(ADE_FLOATING_T));

	return ADE_RET_SUCCESS;

}


/************** static methods *********************/

static ADE_API_RET_T filter_DII_T (ADE_FIR_T* p_fir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{
    ADE_UINT32_T i=0,k=0;
    //ADE_UINT32_T active_section = p_iir->active_section;
    ADE_FLOATING_T *p_in = p_fir->p_in;
    ADE_FLOATING_T *p_out = p_fir->p_out;
    //ADE_FLOATING_T *a = (p_iir-> dp_denoms)[active_section];
    ADE_UINT32_T order = p_fir->filter_order;
    ADE_FLOATING_T *p_b = (p_fir-> p_num);
    ADE_FLOATING_T gain = (p_fir-> gain);
    ADE_FLOATING_T *p_state = (p_fir-> p_state);
    ADE_UINT32_T len_frame = p_fir->buff_len;
   #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
    ADE_blas_level1_T *p_Blas_L1 = p_fir->p_Blas_L1;
    #endif
    //ADE_FLOATING_T *temp_buffer = calloc(order,sizeof(ADE_FLOATING_T));
    ADE_UINT32_T temp_buff_size = order*sizeof(ADE_FLOATING_T);

    ADE_FLOATING_T *p_temp_buffer=p_fir->p_tempbuff;

#if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)

dofilter_DII_T_blas (p_Blas_L1, p_in,p_b,len_frame,p_out, p_state,gain,p_temp_buffer,temp_buff_size);

    #elif (ADE_FIR_IMP==ADE_FIR_USE_CUSTOM)

   dofilter_DII_T_custom(p_in,p_b, len_frame,p_out, p_state, gain,order) ;
    #else

        #error ADE_FIR_IMP in filter_DII_T
    #endif

    //ADE_CHECKNFREE(temp_buffer);

    return ADE_RET_SUCCESS;

}
static ADE_VOID_T dofilter_DII_T_blas (ADE_blas_level1_T *p_Blas_L1, ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_FLOATING_T *p_temp_buffer,ADE_UINT32_T temp_buff_size)
{

ADE_UINT32_T k=0;
ADE_FLOATING_T ALPHA=0;

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
}

static ADE_VOID_T dofilter_DII_T_custom(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order)

{
ADE_UINT32_T k=0,i=0;

 for (k=0;k<len_frame;k++)
    {


        p_out[k] = gain*p_b[0]*(p_in[k])+p_state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            p_state[i]=gain*p_b[i+1]*(p_in[k])+p_state[i+1];
        }

    }


}

static ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_fir,ADE_FIR_IMP_CHOICE_T filt_imp_type)
{

    if (filt_imp_type==ADE_FIR_TRASP_II)
   {
       p_fir->filter_func=filter_DII_T;
   }
   else
   {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,setFilt_Implementation,filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);

            return ADE_RET_ERROR;//13;
   }

    return ADE_RET_SUCCESS;
}
