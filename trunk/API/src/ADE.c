#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include "headers/ADE_downsampler.h"
#include "headers/ADE_Snap.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"


/******** static headers **********/
static ADE_BOOL_T ADE_IsInactiveAlg(ADE_UINT32_T active_algs_flag, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T defined_Flag_i);
static ADE_BOOL_T ADE_IsActiveAlg(ADE_UINT32_T active_algs_flag, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T defined_Flag_i);
static ADE_BOOL_T ADE_IsOneFlag(ADE_UINT32_T Sel_Flag_i);
static ADE_BOOL_T ADE_IsEmptyFlag(ADE_UINT32_T Sel_Flag_i);
static ADE_API_RET_T ADE_Configure_inout(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_inbuff);
static ADE_API_RET_T ADE_toggle_logic(ADE_SCDF_Output_Int_T* p_out);

ADE_API_RET_T ADE_Init(ADE_T **dp_ADE_Handle, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T in_buff_len,ADE_FLOATING_T input_rate)
{

    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_UINT32_T snap_flag = SNAP_FLAG;
    ADE_API_RET_T ret_blow = ADE_RET_ERROR;
    ADE_API_RET_T ret_snap = ADE_RET_ERROR;
    /***snap related****/
    ADE_UINT32_T n_pow_slots_i=2;
    ADE_UINT32_T n_max_indexes_i=1;
    ADE_FLOATING_T time_left_i=0.5e-3;
    ADE_FLOATING_T time_right_i=6e-3;
    ADE_UINT32_T fft_len_i=512;
    ADE_UINT32_T downfact = 100;


    if (*dp_ADE_Handle==NULL)//if still not allocated to protected againt a new call for a different alg
    {
        *dp_ADE_Handle = calloc(1,sizeof(ADE_T));
         ADE_CHECK_MEMALLOC(ADE_CLASS_ADE,ADE_METHOD_Init,(*dp_ADE_Handle));

    }

   /** Only One alg at time check **/
   if (!ADE_IsOneFlag(Sel_Flag_i))
   {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_ADE,ADE_METHOD_Init,Sel_Flag_i,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
   }
//   if (p_in_struct==NULL)
//   {
//        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_ADE,ADE_METHOD_Init,p_in_struct,"%p",(FILE*)ADE_STD_STREAM);
//        return ADE_RET_ERROR;
//
//   }
//   if (p_in_struct->num_channels>1) /* now only mono*/
//   {
//        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_ADE,ADE_METHOD_Init,p_in_struct->num_channels,"%d",(FILE*)ADE_STD_STREAM);
//        return ADE_RET_ERROR;
//
//   }
//


        if (  ADE_IsInactiveAlg( (*dp_ADE_Handle)->active_algs_flag,Sel_Flag_i,blow_flag ))
        {
            (*dp_ADE_Handle)->active_algs_flag |= blow_flag; //accendo flag
            ret_blow = ADE_Blow_Init( &((*dp_ADE_Handle)->p_blow),in_buff_len,input_rate,input_rate);
            ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Init,ret_blow);

           //ADE_Downsampler_Init(&((*dp_ADE_Handle)->p_blow_downsampler),in_buff_len,downfact);


            (*dp_ADE_Handle)->p_blow_out_struct=calloc(1,sizeof(ADE_SCDF_Output_Int_T));
            ADE_CHECK_MEMALLOC(ADE_CLASS_ADE,ADE_METHOD_Init,(*dp_ADE_Handle)->p_blow_out_struct);

        }

        else if  (  ADE_IsInactiveAlg( (*dp_ADE_Handle)->active_algs_flag,Sel_Flag_i,snap_flag ))
        {
            (*dp_ADE_Handle)->active_algs_flag |= snap_flag; //accendo flag
            ret_snap = ADE_Snap_Init( &((*dp_ADE_Handle)->p_snap),in_buff_len,input_rate,n_pow_slots_i,n_max_indexes_i,time_left_i,time_right_i,fft_len_i);
           ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Init,ret_snap);

//           ret_snap_conf = ADE_Snap_SetInBuff((*dp_ADE_Handle)->p_snap);
//            ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Init,ret_snap);



            (*dp_ADE_Handle)->p_snap_out_struct=calloc(1,sizeof(ADE_SCDF_Output_Int_T));
            ADE_CHECK_MEMALLOC(ADE_CLASS_ADE,ADE_METHOD_Init,(*dp_ADE_Handle)->p_snap_out_struct);
        }

    return ADE_RET_SUCCESS;



}

ADE_VOID_T ADE_Release(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
{
     ADE_UINT32_T blow_flag = BLOW_FLAG;
     ADE_UINT32_T snap_flag = SNAP_FLAG;

    if (  ADE_IsActiveAlg( p_ADE->active_algs_flag,Sel_Flag_i,blow_flag ))
    {
        p_ADE->active_algs_flag &= ~blow_flag; //spengo flag
        ADE_Blow_Release( p_ADE->p_blow);
        //ADE_Downsampler_Release( p_ADE->p_blow_downsampler);
    }

    if(  ADE_IsActiveAlg( p_ADE->active_algs_flag,Sel_Flag_i,snap_flag ))
    {
        p_ADE->active_algs_flag &= ~snap_flag; //spengo flag
        ADE_Snap_Release( p_ADE->p_snap);
    }

    if (ADE_IsEmptyFlag(p_ADE->active_algs_flag))
    {
        ADE_CHECKNFREE(p_ADE);
    }

}


ADE_API_RET_T ADE_Configure_params(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
{
    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_API_RET_T ret_blow=ADE_RET_ERROR;

    if (  ADE_IsActiveAlg( p_ADE->active_algs_flag,Sel_Flag_i,blow_flag ))
    {

        ret_blow = ADE_Blow_Configure_params( p_ADE->p_blow);
        ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Configure_params,ret_blow);
        //ADE_API_RET_T ADE_Downsampler_Configure(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff, ADE_SIZE_T out_buff_size);

    }

    return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_Configure_inout(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_inbuff)
{
    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_API_RET_T ret_blow=ADE_RET_ERROR;

    if (  ADE_IsActiveAlg( p_ADE->active_algs_flag,Sel_Flag_i,blow_flag ))
    {

        ret_blow = ADE_Blow_Configure_inout( p_ADE->p_blow,p_inbuff);
        ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Configure_inout,ret_blow);

    }

    return ADE_RET_SUCCESS;

}


//
//ADE_VOID_T ADE_GetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
//{
//    ADE_UINT32_T blow_flag = BLOW_FLAG;
//     ADE_UINT32_T snap_flag = SNAP_FLAG;
//
//     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
//     {
//        ADE_Blow_SetOutBuff(p_ADE->p_blow, p_buff);
//     }
//
//      if  ( (Sel_Flag_i & snap_flag)==snap_flag )
//     {
//        fprintf(stderr,"****Warning empty method*****\n");
//     }
//
//}

ADE_API_RET_T ADE_Step(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_SCDF_Input_Int_T *p_in_struct)
{

    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_UINT32_T snap_flag = SNAP_FLAG;
    ADE_API_RET_T blow_ret = ADE_RET_ERROR;
    ADE_API_RET_T snap_ret = ADE_RET_ERROR,ret_snap_conf=ADE_RET_ERROR;

    if (p_in_struct!=NULL)
    {

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        blow_ret = ADE_Blow_Configure_inout(p_ADE->p_blow, p_in_struct->data);
        ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Step,blow_ret);

        blow_ret = ADE_Blow_Step(p_ADE->p_blow);
        ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Step,blow_ret);

     } else if  ( (Sel_Flag_i & snap_flag)==snap_flag )
        {
            snap_ret = ADE_Snap_SetInBuff(p_ADE->p_snap, p_in_struct->data);
            ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Step,snap_ret);

            ret_snap_conf = ADE_Snap_Configure(p_ADE->p_snap);
            ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Step,ret_snap_conf);

            snap_ret = ADE_Snap_Step(p_ADE->p_snap);
            ADE_CHECK_ADERETVAL(ADE_CLASS_ADE,ADE_METHOD_Step,snap_ret);
        }
    }

      return ADE_RET_SUCCESS;

}

ADE_SCDF_Output_Int_T* ADE_GetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
{



ADE_SCDF_Output_Int_T* p_out=NULL;

#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

/** Only One alg at time check **/
   if (!ADE_IsOneFlag(Sel_Flag_i))
   {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_ADE,ADE_METHOD_GetOutBuff,Sel_Flag_i,"%d",(FILE*)ADE_STD_STREAM);
   }

#endif

if (Sel_Flag_i==BLOW_FLAG) /*vale se i flag sono esclusivi*/
{
   // ADE_Downsampler_Configure(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff, ADE_SIZE_T out_buff_size);
/************** Processing Methods ***************/

//ADE_API_RET_T ADE_Downsampler_Step(ADE_DOWNSAMPLER_T *p_downsampler);
    p_out=(p_ADE->p_blow_out_struct);
    p_out->Fs_data=p_ADE->p_blow->Fs_o;
    p_out->p_data=p_ADE->p_blow->p_out;
    p_out->n_data=p_ADE->p_blow->buff_len_o;
    p_out->state=p_ADE->p_blow->state;
    ADE_toggle_logic(p_out);


}
else if (Sel_Flag_i==SNAP_FLAG)
{
    p_out=(p_ADE->p_snap_out_struct);
    p_out->Fs_data=p_ADE->p_snap->Fs;
    //p_out->p_data=p_ADE->p_snap->p_out;
    //p_out->n_data=p_ADE->p_snap->buff_len_o;
    p_out->state=p_ADE->p_snap->state;
    ADE_toggle_logic(p_out);

}



return p_out;


}


/************************ Static Methods **********************/


static ADE_BOOL_T ADE_IsInactiveAlg(ADE_UINT32_T active_algs_flag, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T defined_Flag_i)
{
    if ( ( (Sel_Flag_i & defined_Flag_i)==defined_Flag_i ) &&  ( ( active_algs_flag & defined_Flag_i ) == 0 ) )
    {

        return ADE_TRUE;
    }
    else
    {
        return ADE_FALSE;
    }
}

static ADE_BOOL_T ADE_IsActiveAlg(ADE_UINT32_T active_algs_flag, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T defined_Flag_i)
{
    if ( ( (Sel_Flag_i & defined_Flag_i)==defined_Flag_i ) &&  ( ( active_algs_flag & defined_Flag_i ) == 1 ) )
    {

        return ADE_TRUE;
    }
    else
    {
        return ADE_FALSE;
    }
}

static ADE_BOOL_T ADE_IsOneFlag(ADE_UINT32_T Sel_Flag_i)
{
    ADE_UINT32_T i=0;
    ADE_UINT32_T bit_len = sizeof(Sel_Flag_i)*8;
    ADE_UINT32_T count=0;
    ADE_UINT32_T tmp_val=0;

    for (i=0;i<bit_len;i++)
    {
        tmp_val=(Sel_Flag_i>>i) & (0x0001);
        count = count + tmp_val;
    }

    if (count==1)
    {
        return ADE_TRUE;
    }
    else
    {
        return ADE_FALSE;
    }

}

static ADE_BOOL_T ADE_IsEmptyFlag(ADE_UINT32_T Sel_Flag_i)
{
    ADE_UINT32_T i=0;
    ADE_UINT32_T bit_len = sizeof(Sel_Flag_i)*8;
    ADE_UINT32_T count=0;
    ADE_UINT32_T tmp_val=0;

    for (i=0;i<bit_len;i++)
    {
        tmp_val=(Sel_Flag_i>>i) & (0x0001);
        count = count + tmp_val;
    }

    if (count==0)
    {
        return ADE_TRUE;
    }
    else
    {
        return ADE_FALSE;
    }

}


static ADE_API_RET_T ADE_toggle_logic(ADE_SCDF_Output_Int_T* p_out)
{
    ADE_UINT32_T antibouncing_counter = 100;


        if  ( (p_out->n_calls==0) && (p_out->state==ADE_TRUE) )
        {
            p_out->toggle=!p_out->toggle;
            p_out->n_calls=p_out->n_calls+1;

        }
        else if (p_out->n_calls>0)
        {
            if (p_out->n_calls<antibouncing_counter)
            {
                p_out->n_calls=p_out->n_calls+1;
            }
            else
            {

                p_out->n_calls=0;
            }
        }


    return ADE_RET_SUCCESS;

}
//static ADE_API_RET_T ADE_AllocateOutBuff(ADE_SCDF_Output_Int_T* p_out_struct)
//{
//
//     p_out_struct=calloc(1,sizeof(ADE_SCDF_Output_Int_T));
//
//     if (p_out_struct==NULL)
//    {
//        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_ADE,AllocateOutBuff,p_out_struct,"%p",(FILE*)ADE_STD_STREAM);
//        return ADE_RET_ERROR;
//
//    }
//
//    return ADE_RET_SUCCESS;
//
//}
