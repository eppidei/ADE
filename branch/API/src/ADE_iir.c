#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE_Utils.h"
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif



static ADE_API_RET_T ADE_Iir_filter_DII_T_b (ADE_IIR_T* p_iir);
static ADE_API_RET_T ADE_Iir_setFilt_Implementation(ADE_IIR_T* p_iir,ADE_IIR_IMP_CHOICE_T filt_imp_type);
static ADE_API_RET_T ADE_Iir_dofilter_DII_T_b_blas ( ADE_blas_level1_T *p_Blas_L1,ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_a ,ADE_FLOATING_T *p_b,
ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order,ADE_FLOATING_T *p_temp_buffer,ADE_SIZE_T temp_buff_size, ADE_UINT32_T len_frame);
static ADE_API_RET_T ADE_Iir_dofilter_DII_T_b_custom ( ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_a ,ADE_FLOATING_T *p_b,
ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order,ADE_UINT32_T len_frame);


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
    ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init,pthis);


        pthis->buff_len=buff_len;

        pthis->n_SOS_sections=n_SOS_sections;
        pthis->filt_imp_type=filt_imp_type;

        ADE_Iir_setFilt_Implementation(pthis,pthis->filt_imp_type);


        /************ GAIN ALLOC ***************/
        (pthis->p_gains) = calloc(n_SOS_sections,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init,pthis->p_gains);

        /************ NUMS ALLOC ***************/
//
        dp_nums = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

         ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init,dp_nums);

        (pthis->dp_nums)=dp_nums;

        for (i=0;i<(n_SOS_sections);i++)
        {
            (pthis->dp_nums)[i] = calloc(1,(order+1)*sizeof(ADE_FLOATING_T));

            ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, (pthis->dp_nums)[i]);

        }

           /************ DENOMS ALLOC ***************/

        dp_denoms = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init,dp_denoms);

        (pthis->dp_denoms)=dp_denoms;

        for (i=0;i<(n_SOS_sections);i++)
        {
            (pthis->dp_denoms)[i] = calloc(1,(order+1)*sizeof(ADE_FLOATING_T));

             ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, (pthis->dp_denoms)[i]);

        }

        /************ SEC BUFF ALLOC ***************/

        dp_sec_buff = (ADE_FLOATING_T**)calloc(n_SOS_sections+1,sizeof(ADE_FLOATING_T*));

         ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, dp_sec_buff);

        (pthis->dp_section_buffers)=dp_sec_buff;


        for (i=0;i<(n_SOS_sections+1);i++)
        {

            p_sec_buff = calloc(1,buff_len*sizeof(ADE_FLOATING_T));

            ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, p_sec_buff);

            (pthis->dp_section_buffers)[i]=p_sec_buff;



        }

        /************ STATES ALLOC ***************/

        dp_states = (ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));

        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, dp_states);

        (pthis->dp_states)=dp_states;


        for (i=0;i<(n_SOS_sections);i++)
        {
            p_states = calloc(n_SOS_sections,(order+n_padd_zeros)*sizeof(ADE_FLOATING_T));

            ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, p_states);

            (pthis->dp_states)[i]=p_states;

        }
           /*********** Temp buffer allocation ***********/

        pthis->dp_tempbuff=(ADE_FLOATING_T**)calloc(n_SOS_sections,sizeof(ADE_FLOATING_T*));
         ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, pthis->dp_tempbuff);
        for (i=0;i<(n_SOS_sections);i++)
        {
            pthis->dp_tempbuff[i] = calloc(order,sizeof(ADE_FLOATING_T));

             ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init, pthis->dp_tempbuff[i]);


        }

        /************ BLAS ALLOC ***************/
        #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)

        pthis->dp_Blas_L1=(ADE_blas_level1_T**)calloc(n_SOS_sections,sizeof(ADE_blas_level1_T*));

        ADE_CHECK_MEMALLOC(ADE_CLASS_IIR,ADE_METHOD_Init,  pthis->dp_Blas_L1);

         for (i=0;i<(n_SOS_sections);i++)
        {
            ADE_Blas_level1_Init(&(pthis->dp_Blas_L1[i]),ADE_MATH_REAL);
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

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setGains,p_gains);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setGains,p_iir);

    memcpy((p_iir->p_gains),p_gains,p_iir->n_SOS_sections*sizeof(ADE_FLOATING_T));


    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_Iir_setNums(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_nums)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setNums,dp_nums);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setNums,p_iir);

    for (i=0;i<p_iir->n_SOS_sections;i++)
    {
        memcpy((p_iir->dp_nums)[i],dp_nums[i],(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_RET_SUCCESS;
}



ADE_API_RET_T ADE_Iir_setDenoms(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_denoms)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setDenoms,dp_denoms);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setDenoms,p_iir);

    for (i=0;i<p_iir->n_SOS_sections;i++)
    {

        memcpy((p_iir->dp_denoms)[i], dp_denoms[i],(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_SetInBuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_SetInBuff,p_buff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_SetInBuff,p_iir);

    p_iir->p_in=p_buff;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_SetOutBuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_SetOutBuff,p_buff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_SetOutBuff,p_iir);

    p_iir->p_out=p_buff;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_ResetState(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_ResetState,p_iir);

    for(i=0;i<p_iir->n_SOS_sections;i++)
    {
        memset( (p_iir->dp_states)[i],0,(p_iir->section_order+1)*sizeof(ADE_FLOATING_T));
    }

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_Configure_params(ADE_IIR_T* p_iir,ADE_FLOATING_T** dp_nums,ADE_FLOATING_T** dp_denoms)
{
     ADE_API_RET_T ret_num=ADE_RET_ERROR;
     ADE_API_RET_T ret_denoms=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure_params,p_iir);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure_params,dp_nums);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure_params,dp_denoms);

     ret_num=ADE_Iir_setNums(p_iir,dp_nums);
     ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_Configure_params,ret_num);

     ret_denoms=ADE_Iir_setDenoms(p_iir,dp_denoms);
     ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_Configure_params,ret_denoms);

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_Configure_inout(ADE_IIR_T* p_iir,ADE_FLOATING_T* p_inbuff,ADE_FLOATING_T* p_outbuff)
{

    ADE_API_RET_T ret_setin=ADE_RET_ERROR;
    ADE_API_RET_T ret_setout=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure_inout,p_iir);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure_inout,p_inbuff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure_inout,p_outbuff);


     ret_setin=ADE_Iir_SetInBuff(p_iir,p_inbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_Configure_inout,ret_setin);

     ret_setout=ADE_Iir_SetOutBuff(p_iir,p_outbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_Configure_inout,ret_setout);


     return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_Iir_Configure(ADE_IIR_T* p_iir,ADE_FLOATING_T** dp_nums,ADE_FLOATING_T** dp_denoms,ADE_FLOATING_T* p_inbuff,ADE_FLOATING_T* p_outbuff)
{
     ADE_API_RET_T ret_params=ADE_RET_ERROR;
     ADE_API_RET_T ret_inout=ADE_RET_ERROR;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure,p_iir);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure,p_inbuff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure,p_outbuff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure,dp_nums);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Configure,dp_denoms);

    ret_params=ADE_Iir_Configure_params(p_iir,dp_nums,dp_denoms);
     ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_Configure,ret_params);

    ret_inout=ADE_Iir_Configure_inout(p_iir,p_inbuff,p_outbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_Configure,ret_inout);

     return ADE_RET_SUCCESS;
}

/************** Utils methods *********************/

ADE_API_RET_T ADE_Iir_Print(ADE_IIR_T* p_iir, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{
    ADE_UINT32_T i=0;
    ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T temp_str[64];
    ADE_CHAR_T temp_str2[16];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Print,p_iir);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Print,p_fid);

            memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);
 memset(temp_str,'\0',sizeof(temp_str));
 memset(temp_str2,'\0',sizeof(temp_str2));
    if (p_fid!=NULL)
    {
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"buff_len = %u\n"),p_iir->buff_len);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_in = %p(%f)\n"),p_iir->p_in,p_iir->p_in[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_out = %p(%f)\n"),p_iir->p_out,p_iir->p_out[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"n_SOS_sections = %u\n"),p_iir->n_SOS_sections);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"active_section = %u\n"),p_iir->active_section);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"section_order = %u\n"),p_iir->section_order);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_gains = %p(%f)\n"),p_iir->p_gains,p_iir->p_gains[0]);

        for (i=0;i<p_iir->n_SOS_sections;i++)
        {
             strcpy(pri_str,fixed_str);
            ADE_LOG(p_fid,strcat(pri_str,"dp_nums = %p([%d]%p(%f))\n"),p_iir->dp_nums,i,p_iir->dp_nums[i],*(p_iir->dp_nums[i]));
             strcpy(pri_str,fixed_str);
            ADE_LOG(p_fid,strcat(pri_str,"dp_denoms = %p([%d]%p(%f))\n"),p_iir->dp_denoms,i,p_iir->dp_denoms[i],*(p_iir->dp_denoms[i]));
             strcpy(pri_str,fixed_str);
            ADE_LOG(p_fid,strcat(pri_str,"dp_states = %p([%d]%p(%f))\n"),p_iir->dp_states,i,p_iir->dp_states[i],*(p_iir->dp_states[i]));
             strcpy(pri_str,fixed_str);
            ADE_LOG(p_fid,strcat(pri_str,"dp_section_buffers = %p([%d]%p(%f))\n"),p_iir->dp_section_buffers,i,p_iir->dp_section_buffers[i],*(p_iir->dp_section_buffers[i]));
             strcpy(pri_str,fixed_str);
            ADE_LOG(p_fid,strcat(pri_str,"dp_tempbuff = %p([%d]%p(%f))\n"),p_iir->dp_tempbuff,i,p_iir->dp_tempbuff[i],*(p_iir->dp_tempbuff[i]));
             strcpy(pri_str,fixed_str);
            ADE_LOG(p_fid,strcat(pri_str,"dp_Blas_L1 = %p"),p_iir->dp_Blas_L1);
            strncpy(temp_str,fixed_str,len_str-2);
            sprintf(temp_str2,"dp_Blas_L1[%d]",i);
            ADE_Blas_level1_Print(p_iir->dp_Blas_L1[i],p_fid,temp_str2,temp_str);
        }
        ADE_LOG(p_fid,strcat(pri_str,"filt_imp_type = %d\n"),p_iir->filt_imp_type);
        ADE_LOG(p_fid,strcat(pri_str,"filter_func = %p\n"),p_iir->filter_func);

    }

        return ADE_RET_SUCCESS;
}

/********* static methods ********/

static ADE_API_RET_T ADE_Iir_setFilt_Implementation(ADE_IIR_T* p_iir,ADE_IIR_IMP_CHOICE_T filt_imp_type)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_setFilt_Implementation,p_iir);

    if (filt_imp_type==ADE_IIR_TRASP_II_B)
   {
       p_iir->filter_func=ADE_Iir_filter_DII_T_b;
   }
   else
   {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_IIR,ADE_METHOD_setFilt_Implementation,filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);

        return ADE_RET_ERROR;
   }

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Iir_Step(ADE_IIR_T* p_iir)
{
    ADE_UINT32_T i=0;


 ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_Step,p_iir);


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

    ADE_UINT32_T active_section = 0;
    ADE_FLOATING_T *p_in = NULL;
    ADE_FLOATING_T *p_out = NULL;
    ADE_FLOATING_T *p_a =NULL;
    ADE_UINT32_T order =0;
    ADE_FLOATING_T *p_b = NULL;
    ADE_FLOATING_T gain = 0;
    ADE_FLOATING_T *p_state = NULL;
    ADE_UINT32_T len_frame = 0;
    #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)
    ADE_blas_level1_T *p_Blas_L1 = NULL;
    #endif
    ADE_FLOATING_T *p_temp_buffer = NULL;//calloc(order,sizeof(ADE_FLOATING_T));
    ADE_SIZE_T temp_buff_size = 0;
    ADE_API_RET_T ret=ADE_RET_ERROR;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_filter_DII_T_b,p_iir);


    active_section = p_iir->active_section;
    p_in = (p_iir-> dp_section_buffers)[active_section];
    p_out = (p_iir-> dp_section_buffers)[active_section+1];
    p_a = (p_iir-> dp_denoms)[active_section];
    order = p_iir->section_order;
    p_b = (p_iir-> dp_nums)[active_section];
    gain = (p_iir-> p_gains)[active_section];
    p_state = (p_iir-> dp_states)[active_section];
    len_frame = p_iir->buff_len;
    #if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)
    p_Blas_L1 = p_iir->dp_Blas_L1[active_section];
    #endif
    p_temp_buffer = p_iir->dp_tempbuff[active_section];//calloc(order,sizeof(ADE_FLOATING_T));
    temp_buff_size = order*sizeof(ADE_FLOATING_T);


#if (ADE_IIR_IMP==ADE_IIR_USE_BLAS)

    ret=ADE_Iir_dofilter_DII_T_b_blas (p_Blas_L1,p_in,p_out,p_a ,p_b,p_state,gain,order,p_temp_buffer,temp_buff_size,len_frame);
    ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_filter_DII_T_b,ret);

    #elif (ADE_IIR_IMP==ADE_IIR_USE_CUSTOM)


    ret=ADE_Iir_dofilter_DII_T_b_custom ( p_in,p_out,p_a ,p_b,p_state,gain,order,len_frame);
    ADE_CHECK_ADERETVAL(ADE_CLASS_IIR,ADE_METHOD_filter_DII_T_b,ret);

    #else
        #error ADE_IIR_IMP in filter_DII_T_b

    #endif

  //  ADE_CHECKNFREE(temp_buffer);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Iir_dofilter_DII_T_b_blas ( ADE_blas_level1_T *p_Blas_L1,ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_a ,ADE_FLOATING_T *p_b,
ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order,ADE_FLOATING_T *p_temp_buffer,ADE_SIZE_T temp_buff_size, ADE_UINT32_T len_frame)
{

    ADE_UINT32_T k=0;
    ADE_FLOATING_T ALPHA=0.0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_Blas_L1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_in);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_out);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_a);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_b);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_state);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_blas,p_temp_buffer);


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
        ADE_Blas_level1_SetX(p_Blas_L1,&p_a[0+1]);
        ALPHA=-p_out[k];
        ADE_Blas_level1_SetALPHA(p_Blas_L1,&ALPHA);
        ADE_Blas_level1_axpy(p_Blas_L1);
        memcpy(&p_state[0],p_temp_buffer,temp_buff_size);
        memset(p_temp_buffer,0,temp_buff_size);

    }

return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Iir_dofilter_DII_T_b_custom ( ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_a ,ADE_FLOATING_T *p_b,
ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order,ADE_UINT32_T len_frame)
{

    ADE_UINT32_T k=0,i=0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_custom,p_in);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_custom,p_out);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_custom,p_a);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_custom,p_b);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_IIR,ADE_METHOD_dofilter_DII_T_b_custom,p_state);


   for (k=0;k<len_frame;k++)
    {


        p_out[k] = gain*p_b[0]*(p_in[k])+p_state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            p_state[i]=gain*p_b[i+1]*(p_in[k])+p_state[i+1]-p_a[i+1]*(p_out[k]);
        }

    }

return ADE_RET_SUCCESS;
}
