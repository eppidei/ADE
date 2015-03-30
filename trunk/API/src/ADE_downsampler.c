#include "headers/ADE_downsampler.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>

static ADE_API_RET_T ADE_Downsampler_doPure_blas(ADE_blas_level1_T *p_b1);
static ADE_API_RET_T ADE_Downsampler_doPure_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T downfact);
static ADE_API_RET_T ADE_Downsampler_Pure(ADE_DOWNSAMPLER_T *p_downsampler);
static ADE_API_RET_T ADE_Downsampler_Fir(ADE_DOWNSAMPLER_T *p_downsampler);

/*************** Init Methods ****************/

ADE_API_RET_T ADE_Downsampler_Init(ADE_DOWNSAMPLER_T **dp_downsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T downfact,ADE_UINT32_T filter_order,
ADE_FIR_IMP_CHOICE_T fir_type, ADE_IIR_IMP_CHOICE_T iir_type,ADE_UINT32_T n_iir_sections,ADE_DOWNSAMP_CHOICE_T down_type)
{
    ADE_DOWNSAMPLER_T *p_this;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;



    p_this=calloc(1,sizeof(ADE_DOWNSAMPLER_T));

    if (p_this!=NULL)
    {
        p_this->down_fact=downfact;
         p_this->down_samp_type=down_type;
         p_this->filter_order=0;
         p_this->in_buff_len=in_buff_len;
         p_this->n_iir_sections=0;

        if (down_type==ADE_DOWNSAMP_FIR)
        {
            p_this->filter_order=filter_order;
            ret_fir=ADE_Fir_Init(&p_this->p_fir,filter_order,in_buff_len,fir_type);
            if (ret_fir==ADE_RET_ERROR)
            {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Init,ret_fir,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
            }

            p_this->p_temp_buff=calloc(in_buff_len,sizeof(ADE_FLOATING_T));
            if (p_this->p_temp_buff==NULL)
            {

                ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_DOWNSAMPLER,Init,p_this->p_temp_buff,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
            }
        }
        else if (down_type==ADE_DOWNSAMP_IIR)
        {
            p_this->filter_order=filter_order;
            p_this->n_iir_sections=n_iir_sections;
            ret_iir=ADE_Iir_Init(&p_this->p_iir,filter_order,in_buff_len,iir_type);
            if (ret_iir==ADE_RET_ERROR)
            {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Init,ret_iir,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
            }
        }

        ret_blas=ADE_Blas_level1_Init(&p_this->p_blas_l1_memcpy,ADE_REAL);
        if (ret_blas==ADE_RET_ERROR)
        {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Init,ret_blas,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
        }




        *dp_downsampler=p_this;
    }
    else
    {
       ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_DOWNSAMPLER,Init,p_this,"%p",(FILE*)ADE_STD_STREAM);
       return ADE_RET_ERROR;//47;
    }



    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Downsampler_Release(ADE_DOWNSAMPLER_T *p_downsampler)
{
    if (p_downsampler->down_samp_type==ADE_DOWNSAMP_FIR)
    {
        ADE_Fir_Release(p_downsampler->p_fir);
        ADE_CHECKNFREE(p_downsampler->p_temp_buff);
    }
    else if (p_downsampler->down_samp_type==ADE_DOWNSAMP_IIR)
    {

        ADE_Iir_Release(p_downsampler->p_iir);
    }

    ADE_Blas_level1_Release(p_downsampler->p_blas_l1_memcpy);
    ADE_CHECKNFREE(p_downsampler);
}
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Downsampler_SetInBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
     ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    /* Input Check */
    if (p_downsampler==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,SetInBuff,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,SetInBuff,p_buff,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    /* Do */

    p_downsampler->p_in=p_buff;

    if (p_downsampler->down_samp_type==ADE_DOWNSAMP_PURE)
     {
         ret_blas1=ADE_Blas_level1_SetX(p_downsampler->p_blas_l1_memcpy,p_buff);

            if (ret_blas1==ADE_RET_ERROR)
            {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,SetInBuff,ret_blas1,"%d",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;//49;
            }


     } else if (p_downsampler->down_samp_type==ADE_DOWNSAMP_FIR)
     {

        ret_fir = ADE_Fir_SetInBuff(p_downsampler->p_fir, p_buff);
        if (ret_fir==ADE_RET_ERROR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,SetOutBuff,ret_fir,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//49;
        }
      }




    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Downsampler_SetOutBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    if (p_downsampler==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,SetOutBuff,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (p_buff==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,SetOutBuff,p_buff,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (buff_size!=(p_downsampler->in_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    p_downsampler->p_out=p_buff;



    ret_blas1=ADE_Blas_level1_SetY(p_downsampler->p_blas_l1_memcpy,p_buff);


    if (ret_blas1==ADE_RET_ERROR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,SetOutBuff,ret_blas1,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }





    return ADE_RET_SUCCESS;

}

/***************** Config Methods ***************************/

ADE_API_RET_T ADE_Downsampler_Config_blas_memcpy(ADE_DOWNSAMPLER_T *p_downsampler)
{

    ADE_API_RET_T ret_blas_N=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCX=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCY=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_X=ADE_RET_ERROR;

      /* Input Check */

    if (p_downsampler==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Config_blas_memcpy,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    /* Do */

    ret_blas_INCY=ADE_Blas_level1_SetINCY(p_downsampler->p_blas_l1_memcpy,1);
    if (ret_blas_INCY==ADE_RET_ERROR)
    {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Config_blas_memcpy,ret_blas_INCY,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
    }
    ret_blas_INCX=ADE_Blas_level1_SetINCX(p_downsampler->p_blas_l1_memcpy,p_downsampler->down_fact);
    if (ret_blas_INCX==ADE_RET_ERROR)
    {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Config_blas_memcpy,ret_blas_INCX,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
    }
    ret_blas_N=ADE_Blas_level1_SetN(p_downsampler->p_blas_l1_memcpy,p_downsampler->in_buff_len);
    if (ret_blas_N==ADE_RET_ERROR)
    {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Config_blas_memcpy,ret_blas_N,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
    }

     if (p_downsampler->down_samp_type==ADE_DOWNSAMP_FIR)
     {

        ret_blas_X=ADE_Blas_level1_SetX(p_downsampler->p_blas_l1_memcpy,p_downsampler->p_temp_buff);


        if (ret_blas_X==ADE_RET_ERROR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Config_blas_memcpy,ret_blas_X,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//49;
        }


     }




        return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Downsampler_Config_fir(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_num,ADE_FLOATING_T *p_out,ADE_SIZE_T out_size)
{
    ADE_API_RET_T ret_firnum=ADE_RET_ERROR;
    ADE_API_RET_T ret_firbuf=ADE_RET_ERROR;

    /* Input Check */
    if (p_downsampler==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Config_fir,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (p_num==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Config_fir,p_num,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    if (p_out==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Config_fir,p_out,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    if (p_downsampler->down_samp_type!=ADE_DOWNSAMP_FIR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Config_fir,p_downsampler->down_samp_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    /* Do */

    ret_firnum = ADE_Fir_setNum(p_downsampler->p_fir, p_num);
    if (ret_firnum==ADE_RET_ERROR)
         {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Config_fir,ret_firnum,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;
         }
    ret_firbuf = ADE_Fir_SetOutBuff(p_downsampler->p_fir, p_downsampler->p_temp_buff);
     if (ret_firbuf==ADE_RET_ERROR)
         {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Config_fir,ret_firbuf,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;
         }

      return ADE_RET_SUCCESS;
}
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Downsampler_Step(ADE_DOWNSAMPLER_T *p_downsampler)
{

    ADE_API_RET_T ret_pure=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

     #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        if (p_downsampler==NULL)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Step,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }
    #endif

    if (p_downsampler->down_samp_type==ADE_DOWNSAMP_PURE)
    {

         ret_pure = ADE_Downsampler_Pure(p_downsampler);
          #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
         if (ret_pure==ADE_RET_ERROR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Step,ret_pure,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }
        #endif
    }
    else if (p_downsampler->down_samp_type==ADE_DOWNSAMP_FIR)
    {
        ret_fir = ADE_Downsampler_Fir(p_downsampler);
        #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
         if (ret_fir==ADE_RET_ERROR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Step,ret_fir,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }
        #endif

    }

    return ADE_RET_SUCCESS;


}

/********************* Static Methods *******************************/

static ADE_API_RET_T ADE_Downsampler_Pure(ADE_DOWNSAMPLER_T *p_downsampler)
{
    ADE_API_RET_T ret_down=ADE_RET_ERROR;

    #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        if (p_downsampler==NULL)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Pure,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }
    #endif

    #if (ADE_DOWNSAMPLER_IMP==ADE_DOWNSAMPLER_USE_BLAS)

         ret_down=ADE_Downsampler_doPure_blas(p_downsampler->p_blas_l1_memcpy);

    #elif (ADE_DOWNSAMPLER_IMP==ADE_DOWNSAMPLER_USE_CUSTOM)

        ret_down= ADE_Downsampler_doPure_custom (p_downsampler->in_buff_len,p_downsampler->p_out,p_downsampler->p_in,p_downsampler->down_fact);

    #else

        #error ADE_DOWNSAMPLER_IMP in ADE_Downsampler_Pure
    #endif

 #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
         if (ret_down==ADE_RET_ERROR)
         {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Pure,ret_down,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;
         }
        #endif


    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Downsampler_doPure_blas(ADE_blas_level1_T *p_b1)
{

ADE_API_RET_T ret=ADE_RET_ERROR;
ADE_UINT32_T i=0;

     #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
    if (p_b1==NULL)
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,doPure_blas,p_b1,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
    }
     #endif
     ret=ADE_Blas_level1_copy(p_b1);
     #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
     if (ret==ADE_RET_ERROR)
     {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,doPure_blas,p_b1,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
     }
    #endif

     return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Downsampler_doPure_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T downfact)

{

    ADE_UINT32_T i=0;

#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
    if (p_out==NULL)
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,doPure_custom,p_out,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
    }
     if (p_in==NULL)
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,doPure_custom,p_in,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
    }
     #endif


        for (i=0;i<in_buff_len;i++)
        {
            p_out[i*downfact]=p_in[i];

        }

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Downsampler_Fir(ADE_DOWNSAMPLER_T *p_downsampler)
{

    ADE_API_RET_T ret_down=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        if (p_downsampler==NULL)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,Fir,p_downsampler,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }
    #endif

    ret_down=ADE_Downsampler_Pure(p_downsampler);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_down==ADE_RET_ERROR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Fir,ret_down,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    #endif
    ret_fir=ADE_Fir_Step(p_downsampler->p_fir);
     #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
     if (ret_fir==ADE_RET_ERROR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_DOWNSAMPLER,Fir,ret_fir,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    #endif

    return ADE_RET_SUCCESS;
}

