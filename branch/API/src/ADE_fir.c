#include "headers/ADE_fir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_Error_Handler.h"
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif



static ADE_API_RET_T ADE_Fir_SetInBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff);
static ADE_API_RET_T ADE_Fir_SetOutBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff);
static ADE_API_RET_T ADE_Fir_setOrder(ADE_FIR_T* p_fir, ADE_INT32_T order);
static ADE_API_RET_T ADE_Fir_setNum(ADE_FIR_T* p_fir, ADE_FLOATING_T *p_num);
static ADE_API_RET_T ADE_Fir_SetBufflength(ADE_FIR_T* p_fir, ADE_INT32_T bufflen);
static ADE_API_RET_T ADE_Fir_filter_DII_T (ADE_FIR_T* p_fir);
static ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_fir,ADE_FIR_IMP_CHOICE_T filt_imp_type);
static ADE_API_RET_T ADE_Fir_dofilter_DII_T_blas (ADE_blas_level1_T *p_Blas_L1, ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_FLOATING_T *p_temp_buffer,ADE_UINT32_T temp_buff_size);
static ADE_API_RET_T ADE_Fir_dofilter_DII_T_custom(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order);

ADE_API_RET_T ADE_Fir_Init(ADE_FIR_T** dp_this)
{
    ADE_blas_level1_T *p_Blas_L1;
    ADE_FLOATING_T default_gain = 1.0;
    ADE_FLOATING_T *p_state=NULL;
    ADE_API_RET_T ret=ADE_RET_ERROR;
    ADE_INT32_T max_filt_ord = ADE_FIR_MAX_ORDER;




    ADE_FIR_T* pthis = calloc(1,sizeof(ADE_FIR_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,pthis);

        pthis->buff_len=0;
        pthis->gain=default_gain;
        pthis->filter_order = 0;
        pthis->max_filter_order=max_filt_ord;
        pthis->filt_imp_type=ADE_FIR_IMP_UNDEF;
      //  ret = ADE_Fir_setFilt_Implementation(pthis,pthis->filt_imp_type);
       // ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Init,ret);

        /******** state buffer allocation ***********/
        p_state=calloc(max_filt_ord+1,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,p_state);

        pthis->p_state=p_state;

        /********** numerator buffer allocation *************/
        pthis->p_num=calloc(max_filt_ord+1,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_FIR,ADE_METHOD_Init,pthis->p_num);

        /********* Blas1 Allocation ***************/
         #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
            ADE_Blas_level1_Init(&p_Blas_L1,ADE_MATH_REAL);
            pthis->p_Blas_L1=p_Blas_L1;
            #endif



        /*********** Temp buffer allocation ***********/

        pthis->p_tempbuff=calloc(max_filt_ord,sizeof(ADE_FLOATING_T));
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


ADE_API_RET_T ADE_Fir_ResetState(ADE_FIR_T* p_fir,ADE_FLOATING_T rst_val)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_ResetState,p_fir);

    ADE_Utils_memset_float(p_fir->p_state,(p_fir->filter_order+1),rst_val);
   // memset(p_fir->p_state,0,(p_fir->filter_order+1)*sizeof(ADE_FLOATING_T));

    return ADE_RET_SUCCESS;
}



/********** Configure Methods ***************/
ADE_API_RET_T ADE_Fir_Configure_params(ADE_FIR_T* p_fir,ADE_FLOATING_T *p_num,ADE_UINT32_T num_len,ADE_FIR_IMP_CHOICE_T filt_imp_type)
{

    ADE_API_RET_T ret_num=ADE_RET_ERROR;
    ADE_API_RET_T ret_order=ADE_RET_ERROR;
    ADE_API_RET_T ret_imp=ADE_RET_ERROR;
    ADE_INT32_T order = num_len-1;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Configure_params,p_fir);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Configure_params,p_num);

    if (order>p_fir->max_filter_order || order<=0)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_CHECK_INPUTS,ADE_CLASS_FIR,ADE_METHOD_Configure_params,order,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    ret_order=ADE_Fir_setOrder(p_fir,order);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure_params,ret_order);


       if (filt_imp_type==ADE_FIR_TRASP_II)
        {
            ret_imp = ADE_Fir_setFilt_Implementation(p_fir,filt_imp_type);
            ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure_params,ret_imp);

        }
        else
        {

             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,ADE_METHOD_Configure_params,filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;

        }





    #if (ADE_FIR_IMP==ADE_FIR_USE_BLAS)
    ADE_Blas_level1_SetN(p_fir->p_Blas_L1,order);
    ADE_Blas_level1_SetINCX(p_fir->p_Blas_L1,1);
    ADE_Blas_level1_SetINCY(p_fir->p_Blas_L1,1);
    #endif

    ret_num=ADE_Fir_setNum(p_fir,p_num);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure_params,ret_num);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Fir_Configure_inout(ADE_FIR_T* p_fir,ADE_FLOATING_T* p_inbuff,ADE_FLOATING_T* p_outbuff)
{

    ADE_API_RET_T ret_setin=ADE_RET_ERROR;
    ADE_API_RET_T ret_setout=ADE_RET_ERROR;

    ret_setin=ADE_Fir_SetInBuff(p_fir,p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure_inout,ret_setin);

    ret_setout=ADE_Fir_SetOutBuff(p_fir,p_outbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure_inout,ret_setout);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Fir_Configure_bufflength(ADE_FIR_T* p_fir,ADE_INT32_T buff_len)
{
    ADE_API_RET_T ret_bufflen = ADE_RET_ERROR;

    ret_bufflen = ADE_Fir_SetBufflength(p_fir,  buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure_bufflength,ret_bufflen);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Fir_Configure(ADE_FIR_T* p_fir,ADE_FLOATING_T *p_num,ADE_UINT32_T num_len,ADE_FLOATING_T* p_inbuff,ADE_FLOATING_T* p_outbuff,ADE_FIR_IMP_CHOICE_T filt_imp_type,ADE_UINT32_T buff_len)
{

    ADE_API_RET_T ret_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_inout=ADE_RET_ERROR;
    ADE_API_RET_T ret_len=ADE_RET_ERROR;

    ret_params = ADE_Fir_Configure_params(p_fir,p_num,num_len,filt_imp_type);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure,ret_params);

    ret_inout = ADE_Fir_Configure_inout(p_fir,p_inbuff,p_outbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure,ret_inout);

    ret_len = ADE_Fir_Configure_bufflength(p_fir,buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Configure,ret_len);


    return ADE_RET_SUCCESS;

}
/********** Processing Methods ***********/

ADE_API_RET_T ADE_Fir_Step(ADE_FIR_T* p_fir)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Step,p_fir);

    (p_fir->filter_func)(p_fir);

	return ADE_RET_SUCCESS;

}

/************** Utils methods *********************/

ADE_API_RET_T ADE_Fir_Print(ADE_FIR_T* p_fir, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{

    ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;
     ADE_CHAR_T temp_str[64];
     ADE_API_RET_T ret;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Print,p_fir);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Print,p_fid);

        memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);
 memset(temp_str,'\0',sizeof(temp_str));

    if (p_fid!=NULL)
    {
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"buff_len = %u\n"),p_fir->buff_len);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_in = %p(%*.*f)\n"),p_fir->p_in,ADE_PRINT_FLOAT_WIDTH,ADE_PRINT_FLOAT_PRECISION,p_fir->p_in[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_out = %p(%*.*f)\n"),p_fir->p_out,ADE_PRINT_FLOAT_WIDTH,ADE_PRINT_FLOAT_PRECISION,p_fir->p_out[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"filter_order = %u\n"),p_fir->filter_order);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"max_filter_order = %u\n"),p_fir->max_filter_order);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_num = %p(%*.*f)\n"),p_fir->p_num,ADE_PRINT_FLOAT_WIDTH,ADE_PRINT_FLOAT_PRECISION,p_fir->p_num[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_state = %p(%*.*f)\n"),p_fir->p_state,ADE_PRINT_FLOAT_WIDTH,ADE_PRINT_FLOAT_PRECISION,p_fir->p_state[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_tempbuff = %p(%*.*f)\n"),p_fir->p_num,ADE_PRINT_FLOAT_WIDTH,ADE_PRINT_FLOAT_PRECISION,p_fir->p_tempbuff[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"gain = %f\n"),ADE_PRINT_FLOAT_WIDTH,ADE_PRINT_FLOAT_PRECISION,p_fir->gain);
         strncpy(temp_str,fixed_str,len_str-2);
        ret=ADE_Blas_level1_Print(p_fir->p_Blas_L1,p_fid,"p_Blas_L1",temp_str);
        ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Print,ret);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"filt_imp_type = %d\n"),p_fir->filt_imp_type);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"filter_func = %p\n"),p_fir->filter_func);
        strcpy(pri_str,fixed_str);
    }

    return ADE_RET_SUCCESS;


}

/************** static methods *********************/

static ADE_API_RET_T ADE_Fir_SetInBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetInBuff,p_buff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetInBuff,p_fir);

     p_fir->p_in=p_buff;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Fir_SetOutBuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetOutBuff,p_buff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetOutBuff,p_fir);

    p_fir->p_out=p_buff;

    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Fir_setNum(ADE_FIR_T* p_fir, ADE_FLOATING_T *p_num)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_setNum,p_num);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_setNum,p_fir);

    ADE_Utils_memcpy_float(p_fir->p_num,p_num,(p_fir->filter_order+1));

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Fir_setOrder(ADE_FIR_T* p_fir, ADE_INT32_T order)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_setOrder,p_fir);
    ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_FIR,ADE_METHOD_setOrder,order,"%d",val0,p_fir->max_filter_order);

   p_fir->filter_order=order;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Fir_SetBufflength(ADE_FIR_T* p_fir, ADE_INT32_T bufflen)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_SetBufflength,p_fir);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_FIR,ADE_METHOD_SetBufflength,bufflen,"%d",val0);

   p_fir->buff_len=bufflen;

    return ADE_RET_SUCCESS;

}

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
    ADE_UINT32_T temp_buff_size = 0;
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
    temp_buff_size = order*sizeof(ADE_FLOATING_T);



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
static ADE_API_RET_T ADE_Fir_dofilter_DII_T_blas (ADE_blas_level1_T *p_Blas_L1, ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,ADE_FLOATING_T *p_out,
ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_FLOATING_T *p_temp_buffer,ADE_UINT32_T temp_buff_size)
{

ADE_UINT32_T k=0;
ADE_FLOATING_T ALPHA=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_Blas_L1);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_in);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_out);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_state);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,p_temp_buffer);
#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
if (temp_buff_size==0)
{
    ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,ADE_METHOD_dofilter_DII_T_blas,temp_buff_size,"%u",(FILE*)ADE_STD_STREAM);
    ADE_LOG((FILE*)ADE_STD_STREAM," ERROR : temp_buff_size is zero \n");
}


#endif

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

static ADE_API_RET_T ADE_Fir_dofilter_DII_T_custom(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_b,ADE_UINT32_T len_frame,
ADE_FLOATING_T *p_out, ADE_FLOATING_T *p_state,ADE_FLOATING_T gain,ADE_UINT32_T order)

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
       p_fir->filt_imp_type=filt_imp_type;
       p_fir->filter_func=ADE_Fir_filter_DII_T;
   }
   else
   {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FIR,ADE_METHOD_setFilt_Implementation,filt_imp_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
   }

    return ADE_RET_SUCCESS;
}
