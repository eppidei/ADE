#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE_Utils.h"



static ADE_API_RET_T filter_DII_T_b (ADE_IIR_T* p_iir);
static ADE_API_RET_T ADE_Iir_setFilt_Implementation(ADE_IIR_T* p_iir,ADE_IIR_IMP_CHOICE_T filt_imp_type);


ADE_API_RET_T ADE_Iir_Init(ADE_IIR_T** dp_this, ADE_UINT32_T n_SOS_sections,ADE_UINT32_T buff_len,ADE_IIR_IMP_CHOICE_T filt_imp_type)
{


    ADE_FLOATING_T *p_sec_buff=NULL;
    ADE_FLOATING_T *p_states=NULL;

    ADE_FLOATING_T **dp_nums=NULL;
    ADE_FLOATING_T **dp_denoms=NULL;
    ADE_FLOATING_T **dp_sec_buff=NULL;
    ADE_FLOATING_T **dp_states=NULL;
    ADE_UINT32_T i=0;
    ADE_UINT32_T order = 2;
    ADE_UINT32_T n_padd_zeros = 1;
    ADE_blas_level1_T *p_Blas_L1;

    ADE_IIR_T* pthis = calloc(1,sizeof(ADE_IIR_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init,pthis);


        pthis->buff_len=buff_len;

        pthis->n_SOS_sections=n_SOS_sections;
        pthis->filt_imp_type=filt_imp_type;

        ADE_Iir_setFilt_Implementation(pthis,pthis->filt_imp_type);


        /************ GAIN ALLOC ***************/
        (pthis->p_gains) = calloc(n_SOS_sections,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init,pthis->p_gains);

        /************ NUMS ALLOC ***************/
//
        dp_nums = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

         ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init,dp_nums);

        (pthis->dp_nums)=dp_nums;

        for (i=0;i<(n_SOS_sections);i++)
        {
            (pthis->dp_nums)[i] = calloc(1,(order+1)*sizeof(ADE_FLOATING_T));

            ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, (pthis->dp_nums)[i]);

        }

           /************ DENOMS ALLOC ***************/

        dp_denoms = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init,dp_denoms);

        (pthis->dp_denoms)=dp_denoms;

        for (i=0;i<(n_SOS_sections);i++)
        {
            (pthis->dp_denoms)[i] = calloc(1,(order+1)*sizeof(ADE_FLOATING_T));

             ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, (pthis->dp_denoms)[i]);

        }

        /************ SEC BUFF ALLOC ***************/

        dp_sec_buff = (ADE_FLOATING_T**)calloc(n_SOS_sections+1,sizeof(ADE_FLOATING_T*));

         ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, dp_sec_buff);

        (pthis->dp_section_buffers)=dp_sec_buff;


        for (i=0;i<(n_SOS_sections+1);i++)
        {

            p_sec_buff = calloc(1,buff_len*sizeof(ADE_FLOATING_T));

            ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, p_sec_buff);

            (pthis->dp_section_buffers)[i]=p_sec_buff;



        }

        /************ STATES ALLOC ***************/

        dp_states = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, dp_states);

        (pthis->dp_states)=dp_states;


        for (i=0;i<(n_SOS_sections);i++)
        {
            p_states = calloc(n_SOS_sections,(order+n_padd_zeros)*sizeof(ADE_FLOATING_T));

            ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, p_states);

            (pthis->dp_states)[i]=p_states;

        }
           /*********** Temp buffer allocation ***********/

        pthis->dp_tempbuff=(ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));
         ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, pthis->dp_tempbuff);
        for (i=0;i<(n_SOS_sections);i++)
        {
            pthis->dp_tempbuff[i] = calloc(order,sizeof(ADE_FLOATING_T));

             ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init, pthis->dp_tempbuff[i]);


        }

        /************ BLAS ALLOC ***************/
        #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)

        pthis->dp_Blas_L1=(ADE_blas_level1_T**)calloc(n_SOS_sections,sizeof(ADE_blas_level1_T*));

        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,Init,  pthis->dp_Blas_L1);

         for (i=0;i<(n_SOS_sections);i++)
        {
            ADE_Blas_level1_Init(&(pthis->dp_Blas_L1[i]),ADE_REAL);
            ADE_Blas_level1_SetN(pthis->dp_Blas_L1[i],order);
            ADE_Blas_level1_SetINCX(pthis->dp_Blas_L1[i],1);
            ADE_Blas_level1_SetINCY(pthis->dp_Blas_L1[i],1);
            //ADE_Blas_level1_SetY(pthis->dp_Blas_L1[i],pthis->p_tempbuff);
            //ADE_Blas_level1_SetX(pthis->dp_Blas_L1[i],&((pthis-> dp_nums)[i])[0+1]);
        }



        #endif








        pthis->section_order=order;

        *dp_this=pthis;


    return ADE_RET_SUCCESS;
}


ADE_VOID_T ADE_Iir_Release(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;


    for (i=0;i<( p_iir->n_SOS_sections+1);i++)
    {
      ADE_CHECKNFREE( (p_iir->dp_section_buffers)[i] );
    }
    for (i=0;i<( p_iir->n_SOS_sections);i++)
    {
      ADE_CHECKNFREE( (p_iir->dp_states)[i] );
    }
    for (i=0;i<( p_iir->n_SOS_sections);i++)
    {
      ADE_CHECKNFREE( (p_iir->dp_nums)[i] );
    }
    for (i=0;i<( p_iir->n_SOS_sections);i++)
    {
      ADE_CHECKNFREE( (p_iir->dp_denoms)[i] );
    }
    for (i=0;i<( p_iir->n_SOS_sections);i++)
    {
      ADE_CHECKNFREE( (p_iir->dp_tempbuff)[i] );
    }
      #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)
    for (i=0;i<( p_iir->n_SOS_sections);i++)
    {
      ADE_Blas_level1_Release( (p_iir->dp_Blas_L1)[i] );
    }
    ADE_CHECKNFREE(p_iir->dp_Blas_L1);
    #endif
    ADE_CHECKNFREE(p_iir->dp_states);
    ADE_CHECKNFREE(p_iir->dp_section_buffers);
    ADE_CHECKNFREE(p_iir->dp_nums);
    ADE_CHECKNFREE(p_iir->dp_denoms);
    ADE_CHECKNFREE(p_iir->p_gains);
    ADE_CHECKNFREE(p_iir->dp_tempbuff);
    ADE_CHECKNFREE(p_iir);
}

ADE_API_RET_T ADE_Iir_setGains(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_gains)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setGains,p_gains);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setGains,p_iir);

    memcpy((p_iir->p_gains),p_gains,p_iir->n_SOS_sections*sizeof(ADE_FLOATING_T));


    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_Iir_setNums(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_nums)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setNums,dp_nums);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setNums,p_iir);

    for (i=0;i<p_iir->n_SOS_sections;i++)
    {
        memcpy((p_iir->dp_nums)[i],dp_nums[i],(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_RET_SUCCESS;
}



ADE_API_RET_T ADE_Iir_setDenoms(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_denoms)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setDenoms,dp_denoms);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setDenoms,p_iir);

    for (i=0;i<p_iir->n_SOS_sections;i++)
    {

        memcpy((p_iir->dp_denoms)[i], dp_denoms[i],(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_SetInBuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,SetInBuff,p_buff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,SetInBuff,p_iir);

    p_iir->p_in=p_buff;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_SetOutBuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,SetOutBuff,p_buff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,SetOutBuff,p_iir);

    p_iir->p_out=p_buff;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_ResetState(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ResetState,p_iir);

    for(i=0;i<p_iir->n_SOS_sections;i++)
    {
        memset( (p_iir->dp_states)[i],0,(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_RET_SUCCESS;
}


/********* static methods ********/

static ADE_API_RET_T ADE_Iir_setFilt_Implementation(ADE_IIR_T* p_iir,ADE_IIR_IMP_CHOICE_T filt_imp_type)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,setFilt_Implementation,p_iir);

    if (filt_imp_type==ADE_IIR_TRASP_II_B)
   {
       p_iir->filter_func=filter_DII_T_b;
   }
   else
   {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_IIR,setFilt_Implementation,filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);

        return ADE_RET_ERROR;
   }

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_Step(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;


 ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,Step,p_iir);


 ADE_Utils_memcpy_float(((p_iir-> dp_section_buffers)[0]),p_iir->p_in,p_iir->buff_len);

    for (i=0;i<p_iir->n_SOS_sections;i++)
	{
	    /*** set dynamic params blas ***/
       p_iir->active_section = i;
       (p_iir->filter_func)(p_iir);//(p_iir-> dp_section_buffers)[i], (p_iir-> dp_section_buffers)[i+1], (p_iir-> dp_denoms)[i], order, (p_iir-> dp_nums)[i], (p_iir-> p_gains)[i],(p_iir-> dp_states)[i],p_iir->buff_len,p_iir->p_Blas_L1);

	}

	//memcpy(p_iir->p_out ,(p_iir-> dp_section_buffers)[p_iir->n_SOS_sections],p_iir->buff_len*sizeof(ADE_FLOATING_T));
ADE_Utils_memcpy_float(p_iir->p_out ,(p_iir-> dp_section_buffers)[p_iir->n_SOS_sections],p_iir->buff_len);
	return ADE_RET_SUCCESS;

}
//
//
 static ADE_API_RET_T ADE_Iir_filter_DII_T_b (ADE_IIR_T* p_iir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{
    ADE_UINT32_T k=0,i=0;
    ADE_UINT32_T active_section = 0;
    ADE_FLOATING_T *in = NULL;
    ADE_FLOATING_T *out = NULL;
    ADE_FLOATING_T *a =NULL;
    ADE_UINT32_T order =0;
    ADE_FLOATING_T *b = NULL;
    ADE_FLOATING_T gain = 0;
    ADE_FLOATING_T *state = NULL;
    ADE_UINT32_T len_frame = NULL;
    #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)
    ADE_blas_level1_T *p_Blas_L1 = NULL;
    #endif
    ADE_FLOATING_T *temp_buffer = NULL;//calloc(order,sizeof(ADE_FLOATING_T));
    ADE_UINT32_T temp_buff_size = 0;
    ADE_FLOATING_T ALPHA=0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,filter_DII_T_b,p_iir);


    active_section = p_iir->active_section;
    in = (p_iir-> dp_section_buffers)[active_section];
    out = (p_iir-> dp_section_buffers)[active_section+1];
    a = (p_iir-> dp_denoms)[active_section];
    order = p_iir->section_order;
    b = (p_iir-> dp_nums)[active_section];
    gain = (p_iir-> p_gains)[active_section];
    state = (p_iir-> dp_states)[active_section];
    len_frame = p_iir->buff_len;
    #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)
    p_Blas_L1 = p_iir->dp_Blas_L1[active_section];
    #endif
    temp_buffer = p_iir->dp_tempbuff[active_section];//calloc(order,sizeof(ADE_FLOATING_T));
    temp_buff_size = order*sizeof(ADE_FLOATING_T);


#if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)
    for (k=0;k<len_frame;k++)
    {

        out[k] = gain*b[0]*(in[k])+state[0];
        memcpy(temp_buffer,&state[0+1],temp_buff_size);
        /*************/
        ADE_Blas_level1_SetY(p_Blas_L1,temp_buffer);
        ADE_Blas_level1_SetX(p_Blas_L1,&b[0+1]);
        ALPHA=gain*in[k];
        ADE_Blas_level1_SetALPHA(p_Blas_L1,&ALPHA);
        ADE_Blas_level1_axpy(p_Blas_L1);
        /*****************/
        ADE_Blas_level1_SetX(p_Blas_L1,&a[0+1]);
        ALPHA=-out[k];
        ADE_Blas_level1_SetALPHA(p_Blas_L1,&ALPHA);
        ADE_Blas_level1_axpy(p_Blas_L1);
        memcpy(&state[0],temp_buffer,temp_buff_size);
        memset(temp_buffer,0,temp_buff_size);

//        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
//        {
//            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
//        }

    }

    #elif (ADE_IIR_IMP==ADE_IIR_USE_CUSTOM)


      for (k=0;k<len_frame;k++)
    {


        out[k] = gain*b[0]*(in[k])+state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=gain*b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    #else
        #error ADE_IIR_IMP in filter_DII_T_b

    #endif

  //  ADE_CHECKNFREE(temp_buffer);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Iir_dofilter_DII_T_b_blas (ADE_IIR_T* p_iir)
