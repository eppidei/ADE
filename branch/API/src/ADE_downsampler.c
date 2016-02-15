#include "headers/ADE_downsampler.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>
#include <string.h>

struct ADE_DOWNSAMPLER_S
{
    ADE_FLOATING_T* p_in;
    ADE_FLOATING_T* p_out;
    ADE_INT32_T out_buff_len;
  //  ADE_INT32_T max_in_buff_len;
    ADE_INT32_T down_fact;
    ADE_blas_level1_T *p_blas_l1_memcpy;
};

static ADE_API_RET_T ADE_Downsampler_doStep_blas(ADE_blas_level1_T *p_b1);
static ADE_API_RET_T ADE_Downsampler_doStep_custom (ADE_UINT32_T out_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T downfact);
static ADE_API_RET_T ADE_Downsampler_doStep(ADE_blas_level1_T *p_blas_l1_memcpy, ADE_INT32_T out_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_INT32_T downfact);
//static ADE_API_RET_T ADE_Downsampler_doStep(ADE_DOWNSAMPLER_T *p_downsampler);
//static ADE_API_RET_T ADE_Downsampler_Config_blas_memcpy(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff);
static ADE_API_RET_T ADE_Downsampler_SetInBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff);
static ADE_API_RET_T ADE_Downsampler_SetOutBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff);
static ADE_API_RET_T ADE_Downsampler_SetBufflength(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T out_buff_len);
static ADE_API_RET_T ADE_Downsampler_SetDownfact(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T downfact);

/*************** ADE_METHOD_Init Methods ****************/

ADE_API_RET_T ADE_Downsampler_Init(ADE_DOWNSAMPLER_T **dp_downsampler)//,ADE_UINT32_T out_buff_len,ADE_UINT32_T downfact)
{
    ADE_DOWNSAMPLER_T *p_this;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;
   // ADE_INT32_T max_buff_len=ADE_DOWNSAMPLER_MAX_BUFF_LEN;

    p_this=calloc(1,sizeof(ADE_DOWNSAMPLER_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Init,p_this);


        p_this->down_fact=1;
       //  p_this->max_out_buff_len=max_buff_len;
        p_this->out_buff_len=0;



        ret_blas=ADE_Blas_level1_Init(&p_this->p_blas_l1_memcpy,ADE_MATH_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Init,ret_blas);

        *dp_downsampler=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Downsampler_Release(ADE_DOWNSAMPLER_T *p_downsampler)
{
    ADE_Blas_level1_Release(p_downsampler->p_blas_l1_memcpy);
    ADE_CHECKNFREE(p_downsampler);
}
/*************** Set Methods ****************/

/***************** Config Methods **************/
ADE_API_RET_T ADE_Downsampler_Configure_bufflength(ADE_DOWNSAMPLER_T *p_downsampler, ADE_INT32_T out_buff_len)
{
      ADE_API_RET_T ret_bufflen=ADE_RET_ERROR;
      ADE_API_RET_T ret_blas=ADE_RET_ERROR;

     ret_bufflen=ADE_Downsampler_SetBufflength(p_downsampler, out_buff_len);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_bufflength,ret_bufflen);

     ret_blas=ADE_Blas_level1_configure_copy_bufflength(p_downsampler->p_blas_l1_memcpy,out_buff_len);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_params,ret_blas);

    return ADE_RET_SUCCESS;

}
ADE_API_RET_T ADE_Downsampler_Configure_params(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T downfact)
{

     ADE_API_RET_T ret_downfact=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas=ADE_RET_ERROR;



    ret_downfact=ADE_Downsampler_SetDownfact(p_downsampler, downfact);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_params,ret_downfact);

     ret_blas=ADE_Blas_level1_configure_copy_params(p_downsampler->p_blas_l1_memcpy,downfact,1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_params,ret_blas);


    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Downsampler_Configure_inout(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff)
 {

    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_out=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;

     ret_in=ADE_Downsampler_SetInBuff(p_downsampler,p_inbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_inout,ret_in);

     ret_out=ADE_Downsampler_SetOutBuff(p_downsampler,p_outbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_inout,ret_out);

    ret_blas=ADE_Blas_level1_configure_copy_inout(p_downsampler->p_blas_l1_memcpy,p_inbuff,p_outbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure_inout,ret_blas);

  return ADE_RET_SUCCESS;
 }

ADE_API_RET_T ADE_Downsampler_Configure(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T out_buff_len,
ADE_INT32_T downfact,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff)
{

    ADE_API_RET_T ret_par=ADE_RET_ERROR;
    ADE_API_RET_T ret_inout=ADE_RET_ERROR;
    ADE_API_RET_T ret_bufflen = ADE_RET_ERROR;

    ret_bufflen = ADE_Downsampler_Configure_bufflength(p_downsampler,  out_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure,ret_bufflen);

    ret_par=ADE_Downsampler_Configure_params(p_downsampler,   downfact);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure,ret_par);

    ret_inout=ADE_Downsampler_Configure_inout(p_downsampler,p_inbuff,p_outbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure,ret_inout);

     return ADE_RET_SUCCESS;
}
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Downsampler_Step(ADE_DOWNSAMPLER_T *p_downsampler)
{

    ADE_API_RET_T ret_pure=ADE_RET_ERROR;
    //ADE_API_RET_T ret_fir=ADE_RET_ERROR;



    ret_pure = ADE_Downsampler_doStep(p_downsampler->p_blas_l1_memcpy,p_downsampler->out_buff_len,p_downsampler->p_out,p_downsampler->p_in,p_downsampler->down_fact);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Step,ret_pure);


    return ADE_RET_SUCCESS;


}

/********************* Static Methods *******************************/
static ADE_API_RET_T ADE_Downsampler_SetInBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_conf=ADE_RET_ERROR;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetInBuff,p_downsampler);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetInBuff,p_buff);

    p_downsampler->p_in=p_buff;

    return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Downsampler_SetBufflength(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T out_buff_len)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_conf=ADE_RET_ERROR;
    ADE_INT32_T val0=0;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetBufflength,p_downsampler);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetBufflength,out_buff_len,"%d",val0);
    /* Do */

    p_downsampler->out_buff_len=out_buff_len;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Downsampler_SetDownfact(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T downfact)
{

    ADE_INT32_T val0=0;
    ADE_INT32_T int_mult;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetDownfact,p_downsampler);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetDownfact,downfact,"%d",val0);
    /* wrong check it should be done againt input buff len : decide if 2 add in strcture*/
   // ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetDownfact,downfact,"%d",val0,p_downsampler->out_buff_len);

    /*Condition buffer multiple*/
    //int_mult=p_downsampler->out_buff_len%downfact;
    //ADE_CHECK_VALUE_EQUAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetDownfact,int_mult,"%d",val0);

    p_downsampler->down_fact=downfact;


    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Downsampler_SetOutBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff)
{

    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetOutBuff,p_downsampler);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetOutBuff,p_buff);


    p_downsampler->p_out=p_buff;



    return ADE_RET_SUCCESS;

}




static ADE_API_RET_T ADE_Downsampler_doStep(ADE_blas_level1_T *p_blas_l1_memcpy, ADE_INT32_T out_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_INT32_T downfact)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;


    #if (ADE_DOWNSAMPLER_IMP==ADE_DOWNSAMPLER_USE_BLAS)

         ret_up=ADE_Downsampler_doStep_blas(p_blas_l1_memcpy);

    #elif (ADE_DOWNSAMPLER_IMP==ADE_DOWNSAMPLER_USE_CUSTOM)

        ret_up= ADE_Downsampler_doStep_custom (out_buff_len,p_out,p_in,downfact);

    #else

        #error ADE_DOWNSAMPLER_IMP in ADE_Downsampler_Pure
    #endif

    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep,ret_up);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Downsampler_doStep_blas(ADE_blas_level1_T *p_b1)
{

ADE_API_RET_T ret=ADE_RET_ERROR;
ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep_blas,p_b1);

     ret=ADE_Blas_level1_copy(p_b1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep_blas,ret);

     return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Downsampler_doStep_custom (ADE_UINT32_T out_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T downfact)

{

    ADE_INT32_T i=0;
    //ADE_INT32_T out_buff_len=out_buff_len/downfact;
    //ADE_INT32_T int_mult;
    ADE_INT32_T val0=0;
    //ADE_INT32_T max_len = ADE_DOWNSAMPLER_MAX_BUFF_LEN;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep_custom,p_out);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep_custom,p_in);
    //ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep,p_blas_l1_memcpy);
    //ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep_custom,out_buff_len,"%d",val0,max_len);
    ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep_custom,downfact,"%d",val0,out_buff_len);
   // int_mult=out_buff_len%downfact;
   // ADE_CHECK_VALUE_EQUAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetDownfact,int_mult,"%d",val0);

        for (i=0;i<out_buff_len;i++)
        {
            p_out[i]=p_in[i*downfact];

        }

    return ADE_RET_SUCCESS;
}

/************** Utils methods *********************/

ADE_API_RET_T ADE_Downsampler_Print(ADE_DOWNSAMPLER_T* p_downsampler, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{

    ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;
     ADE_CHAR_T temp_str[64];
     ADE_API_RET_T ret=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Print,p_downsampler);
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
        ADE_LOG(p_fid,strcat(pri_str,"out_buff_len = %u\n"),p_downsampler->out_buff_len);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"down_fact = %u\n"),p_downsampler->down_fact);
        strcpy(pri_str,fixed_str);
        //ADE_LOG(p_fid,strcat(pri_str,"out_buff_len = %u\n"),p_downsampler->out_buff_len);
        //strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_in = %p(%*.*f)\n"),p_downsampler->p_in,ADE_DOWNSAMPLER_PRINT_FLOAT_WIDTH,ADE_DOWNSAMPLER_PRINT_FLOAT_PRECISION,p_downsampler->p_in[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_out = %p(%*.*f)\n"),p_downsampler->p_out,ADE_DOWNSAMPLER_PRINT_FLOAT_WIDTH,ADE_DOWNSAMPLER_PRINT_FLOAT_PRECISION,p_downsampler->p_out[0]);
        strncpy(temp_str,fixed_str,len_str-2);
        ret=ADE_Blas_level1_Print(p_downsampler->p_blas_l1_memcpy,p_fid,"p_blas_l1_memcpy",temp_str);
        ADE_CHECK_ADERETVAL(ADE_CLASS_FIR,ADE_METHOD_Print,ret);
        strcpy(pri_str,fixed_str);
    }

    return ADE_RET_SUCCESS;


}


