#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static ADE_API_RET_T filter_DII_T_b (ADE_IIR_T* p_iir);
static ADE_API_RET_T filter_DII_T_b_blas (ADE_IIR_T* p_iir);


ADE_API_RET_T ADE_Iir_Init(ADE_IIR_T** dp_this, ADE_UINT32_T n_SOS_sections,ADE_UINT32_T buff_len)
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


    if (pthis!=NULL)
    {
        pthis->buff_len=buff_len;

        pthis->n_SOS_sections=n_SOS_sections;


        /************ GAIN ALLOC ***************/
        (pthis->p_gains) = calloc(n_SOS_sections,sizeof(ADE_FLOATING_T));

        if (pthis->p_gains==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,pthis->p_gains,"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
        }

        /************ NUMS ALLOC ***************/
//
        dp_nums = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        (pthis->dp_nums)=dp_nums;

        if (pthis->dp_nums==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,pthis->dp_nums,"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
        }


        for (i=0;i<(n_SOS_sections);i++)
        {
            (pthis->dp_nums)[i] = calloc(1,(order+1)*sizeof(ADE_FLOATING_T));

            if ( (pthis->dp_nums)[i]==NULL)
            {
            ADE_PRINT_ERRORS(ADE_MEM,(pthis->dp_nums)[i],"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
            }

        }

           /************ DENOMS ALLOC ***************/

        dp_denoms = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        (pthis->dp_denoms)=dp_denoms;

        if (pthis->dp_denoms==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,pthis->dp_denoms,"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
        }

        for (i=0;i<(n_SOS_sections);i++)
        {
            (pthis->dp_denoms)[i] = calloc(1,(order+1)*sizeof(ADE_FLOATING_T));

             if ( (pthis->dp_denoms)[i]==NULL)
            {
            ADE_PRINT_ERRORS(ADE_MEM,(pthis->dp_denoms)[i],"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
            }

        }

        /************ SEC BUFF ALLOC ***************/

        dp_sec_buff = (ADE_FLOATING_T**)calloc(n_SOS_sections+1,sizeof(ADE_FLOATING_T*));

        (pthis->dp_section_buffers)=dp_sec_buff;

        for (i=0;i<(n_SOS_sections+1);i++)
        {

            p_sec_buff = calloc(1,buff_len*sizeof(ADE_FLOATING_T));

            if (p_sec_buff!=NULL)
            {
                (pthis->dp_section_buffers)[i]=p_sec_buff;
            }
            else
            {
               // #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
                ADE_PRINT_ERRORS(ADE_MEM,p_sec_buff,"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
            }


        }

        /************ STATES ALLOC ***************/

        dp_states = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        (pthis->dp_states)=dp_states;

        for (i=0;i<(n_SOS_sections);i++)
        {
            p_states = calloc(n_SOS_sections,(order+n_padd_zeros)*sizeof(ADE_FLOATING_T));

            if (p_states!=NULL)
            {
                (pthis->dp_states)[i]=p_states;
            }
            else
            {
               // #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
                    ADE_PRINT_ERRORS(ADE_MEM,p_states,"%p",ADE_Iir_Init);
               // #endif
                return ADE_E3;
            }

        }

        /************ BLAS ALLOC ***************/

        ADE_Blas_level1_Init(&p_Blas_L1,ADE_REAL);
        ADE_Blas_level1_setN(p_Blas_L1,order);
        ADE_Blas_level1_setINCX(p_Blas_L1,1);
        ADE_Blas_level1_setINCY(p_Blas_L1,1);

        pthis->p_Blas_L1=p_Blas_L1;

        pthis->section_order=order;

        *dp_this=pthis;

    }
    else
    {

        ADE_PRINT_ERRORS(ADE_MEM,pthis,"%p",ADE_Iir_Init);
        return  ADE_E3;

    }



    return ADE_DEFAULT_RET;
}


ADE_VOID_T ADE_Iir_Release(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;
    //ADE_FLOATING_T *dummy_address = 1;

    ADE_Blas_level1_Release(p_iir->p_Blas_L1);
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
    ADE_CHECKNFREE(p_iir->dp_states);
    ADE_CHECKNFREE(p_iir->dp_section_buffers);
    ADE_CHECKNFREE(p_iir->dp_nums);
    ADE_CHECKNFREE(p_iir->dp_denoms);
    ADE_CHECKNFREE(p_iir->p_gains);
    ADE_CHECKNFREE(p_iir);
}

ADE_API_RET_T ADE_Iir_setGains(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_gains)
{

        memcpy((p_iir->p_gains),p_gains,p_iir->n_SOS_sections*sizeof(ADE_FLOATING_T));


    return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Iir_setNums(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_nums)
{
    ADE_UINT32_T i=0;

    for (i=0;i<p_iir->n_SOS_sections;i++)
    {
        memcpy((p_iir->dp_nums)[i],dp_nums[i],(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }


    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Iir_setDenoms(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_denoms)
{
    ADE_UINT32_T i=0;

    for (i=0;i<p_iir->n_SOS_sections;i++)
    {
        memcpy((p_iir->dp_denoms)[i], dp_denoms[i],(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Iir_setInbuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff)
{
    p_iir->p_in=p_buff;

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Iir_setOutbuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff)
{
    p_iir->p_out=p_buff;

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Iir_ResetState(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;

    for(i=0;i<p_iir->n_SOS_sections;i++)
    {
        memset( (p_iir->dp_states)[i],0,(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Iir_setFilt_Implementation(ADE_IIR_T* p_iir,ADE_FILTER_IMP_CHOICE_T filt_imp_type)
{

    if (filt_imp_type==trasp_II)
   {
       p_iir->filter_func=filter_DII_T_b;
   }
   else if (filt_imp_type==trasp_II_blas)
   {
     p_iir->filter_func=filter_DII_T_b_blas;
   }
   else
   {
       #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        ADE_PRINT_ERRORS(ADE_INCHECKS,filt_imp_type,"%d",ADE_Iir_setFilt_Implementation);
        #endif

            return ADE_E4;
   }

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Iir_Step(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;
   // ADE_UINT32_T order=2;
    //ADE_FLOATING_T* p_in_buff;
   // ADE_FLOATING_T* p_out_buff;


    /**** set static params blas ****/
#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

if ((p_iir->filter_func)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_iir->filter_func),"%p",ADE_Iir_Step);
    return ADE_E5;
}

if (*(p_iir->dp_denoms)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,*(p_iir->dp_denoms),"%p",ADE_Iir_Step);
    return ADE_E6;
}
if (*(p_iir->dp_nums)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,*(p_iir->dp_nums),"%p",ADE_Iir_Step);
    return ADE_E7;
}
if ((p_iir->p_gains)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_iir->p_gains),"%p",ADE_Iir_Step);
    return ADE_E8;
}

if ((p_iir->p_in)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_iir->p_in),"%p",ADE_Iir_Step);
    return ADE_E10;
}

if ((p_iir->p_out)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_iir->p_out),"%p",ADE_Iir_Step);
    return ADE_E11;
}

if (*(p_iir->dp_states)==NULL)
{
        ADE_PRINT_ERRORS(ADE_INCHECKS,*(p_iir->dp_states),"%p",ADE_Iir_Step);
    return ADE_E15;
}

#endif

 memcpy( ((p_iir-> dp_section_buffers)[0]),p_iir->p_in,(p_iir->buff_len)*sizeof(ADE_FLOATING_T));

    for (i=0;i<p_iir->n_SOS_sections;i++)
	{
	    /*** set dynamic params blas ***/
       p_iir->active_section = i;
       (p_iir->filter_func)(p_iir);//(p_iir-> dp_section_buffers)[i], (p_iir-> dp_section_buffers)[i+1], (p_iir-> dp_denoms)[i], order, (p_iir-> dp_nums)[i], (p_iir-> p_gains)[i],(p_iir-> dp_states)[i],p_iir->buff_len,p_iir->p_Blas_L1);


	}

	memcpy(p_iir->p_out ,(p_iir-> dp_section_buffers)[p_iir->n_SOS_sections],p_iir->buff_len*sizeof(ADE_FLOATING_T));

	return ADE_DEFAULT_RET;

}
//
//
 static ADE_API_RET_T filter_DII_T_b_blas (ADE_IIR_T* p_iir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{
    ADE_UINT32_T k=0;
    ADE_UINT32_T active_section = p_iir->active_section;
    ADE_FLOATING_T *in = (p_iir-> dp_section_buffers)[active_section];
    ADE_FLOATING_T *out = (p_iir-> dp_section_buffers)[active_section+1];
    ADE_FLOATING_T *a = (p_iir-> dp_denoms)[active_section];
    ADE_UINT32_T order = p_iir->section_order;
    ADE_FLOATING_T *b = (p_iir-> dp_nums)[active_section];
    ADE_FLOATING_T gain = (p_iir-> p_gains)[active_section];
    ADE_FLOATING_T *state = (p_iir-> dp_states)[active_section];
    ADE_UINT32_T len_frame = p_iir->buff_len;
    ADE_blas_level1_T *p_Blas_L1 = p_iir->p_Blas_L1;
    ADE_FLOATING_T *temp_buffer = calloc(order,sizeof(ADE_FLOATING_T));
    ADE_UINT32_T temp_buff_size = order*sizeof(ADE_FLOATING_T);
    ADE_FLOATING_T ALPHA=0.0;


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
        ADE_Blas_level1_setX(p_Blas_L1,&a[0+1]);
        ALPHA=-out[k];
        ADE_Blas_level1_setALPHA(p_Blas_L1,&ALPHA);
        ADE_Blas_level1_axpy(p_Blas_L1);
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

static ADE_API_RET_T filter_DII_T_b (ADE_IIR_T* p_iir)//(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame,ADE_blas_level1_T *p_Blas_L1;)
{
    ADE_UINT32_T i=0,k=0;
    ADE_UINT32_T active_section = p_iir->active_section;
    ADE_FLOATING_T *in = (p_iir-> dp_section_buffers)[active_section];
    ADE_FLOATING_T *out = (p_iir-> dp_section_buffers)[active_section+1];
    ADE_FLOATING_T *a = (p_iir-> dp_denoms)[active_section];
    ADE_UINT32_T order = p_iir->section_order;
    ADE_FLOATING_T *b = (p_iir-> dp_nums)[active_section];
    ADE_FLOATING_T gain = (p_iir-> p_gains)[active_section];
    ADE_FLOATING_T *state = (p_iir-> dp_states)[active_section];
    ADE_UINT32_T len_frame = p_iir->buff_len;




    for (k=0;k<len_frame;k++)
    {


        out[k] = gain*b[0]*(in[k])+state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=gain*b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    return ADE_DEFAULT_RET;

}
