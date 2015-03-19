#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include "headers/ADE_Snap.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>


ADE_API_RET_T ADE_Init(ADE_T **dp_ADE_Handle, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T buff_size_i,ADE_FLOATING_T Fs_i)
{

    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_UINT32_T snap_flag = SNAP_FLAG;
    ADE_API_RET_T ret_blow = ADE_DEFAULT_RET;
    ADE_API_RET_T ret_snap = ADE_DEFAULT_RET;
    /***snap related****/
    ADE_UINT32_T n_pow_slots_i=2;
    ADE_UINT32_T n_max_indexes_i=1;
    ADE_FLOATING_T time_left_i=0.5e-3;
    ADE_FLOATING_T time_right_i=6e-3;
    ADE_UINT32_T fft_len_i=512;


    if (*dp_ADE_Handle==NULL)//if still not allocated to protected againt a new call for a different alg
    {
        *dp_ADE_Handle = calloc(1,sizeof(ADE_T));

    }


    if (*dp_ADE_Handle!=NULL)
    {
        if ( ( (Sel_Flag_i & blow_flag)==blow_flag ) &&  ( ( (*dp_ADE_Handle)->active_algs_flag & blow_flag ) == 0 ) )
        {
            (*dp_ADE_Handle)->active_algs_flag |= blow_flag; //accendo flag
            ret_blow = ADE_Blow_Init( &((*dp_ADE_Handle)->p_blow),buff_size_i,Fs_i);

            if (ret_blow<0)
            {
                 ADE_PRINT_ERRORS(ADE_MEM,ret_blow,"%d",ADE_Init);
                return ADE_E23;
            }
        }

        if ( ( (Sel_Flag_i & snap_flag)==snap_flag ) &&  ( ( (*dp_ADE_Handle)->active_algs_flag & snap_flag ) == 0 ) )
        {
            (*dp_ADE_Handle)->active_algs_flag |= snap_flag; //accendo flag
            ret_snap = ADE_Snap_Init( &((*dp_ADE_Handle)->p_snap),buff_size_i,Fs_i,n_pow_slots_i,n_max_indexes_i,time_left_i,time_right_i,fft_len_i);
            if (ret_snap<0)
            {
                 ADE_PRINT_ERRORS(ADE_MEM,ret_snap,"%d",ADE_Init);
                return ADE_E23;
            }
        }

    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,*dp_ADE_Handle,"%p",ADE_Init);
        return ADE_E23;
    }

    return ADE_DEFAULT_RET;



}

ADE_VOID_T ADE_Release(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
{
     ADE_UINT32_T blow_flag = BLOW_FLAG;
     ADE_UINT32_T snap_flag = SNAP_FLAG;

    if ( ( (Sel_Flag_i & blow_flag)==blow_flag ) &&  ( ( p_ADE->active_algs_flag & blow_flag ) == 1 ) )
    {
        p_ADE->active_algs_flag &= ~blow_flag; //spengo flag
        ADE_Blow_Release( p_ADE->p_blow);
    }

    if ( ( (Sel_Flag_i & snap_flag)==snap_flag ) &&  ( ( p_ADE->active_algs_flag & snap_flag ) == 1 ) )
    {
        p_ADE->active_algs_flag &= ~snap_flag; //spengo flag
        ADE_Snap_Release( p_ADE->p_snap);
    }

}

ADE_VOID_T ADE_SetInBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_buff)
{
    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_UINT32_T snap_flag = SNAP_FLAG;

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        ADE_Blow_SetInBuff(p_ADE->p_blow, p_buff);
     }

     if  ( (Sel_Flag_i & snap_flag)==snap_flag )
     {
        ADE_Snap_SetInbuff(p_ADE->p_snap, p_buff);
     }

}

ADE_VOID_T ADE_SetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_buff)
{
    ADE_UINT32_T blow_flag = BLOW_FLAG;
     ADE_UINT32_T snap_flag = SNAP_FLAG;

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        ADE_Blow_SetOutBuff(p_ADE->p_blow, p_buff);
     }

      if  ( (Sel_Flag_i & snap_flag)==snap_flag )
     {
        fprintf(stderr,"****Warning empty method*****\n");
     }

}

ADE_API_RET_T ADE_Step(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
{

    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_UINT32_T snap_flag = SNAP_FLAG;
    ADE_API_RET_T blow_ret = ADE_DEFAULT_RET;
    ADE_API_RET_T snap_ret = ADE_DEFAULT_RET;

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        blow_ret = ADE_Blow_Step(p_ADE->p_blow);

        #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)

        if (blow_ret<0)
        {
            ADE_PRINT_ERRORS(ADE_RETCHECKS,blow_ret,"%d",ADE_Step);
            return ADE_E24;
        }


        #endif
     }


      if  ( (Sel_Flag_i & snap_flag)==snap_flag )
     {
        snap_ret = ADE_Snap_Step(p_ADE->p_snap);

        #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)

        if (snap_ret<0)
        {
            ADE_PRINT_ERRORS(ADE_RETCHECKS,snap_ret,"%d",ADE_Step);
            return ADE_E24;
        }


        #endif
     }

      return ADE_DEFAULT_RET;

}
