#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>


ADE_API_RET_T ADE_Init(ADE_T **dp_ADE_Handle, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T buff_size_i,ADE_FLOATING_T Fs_i)
{

    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_API_RET_T ret_blow = ADE_DEFAULT_RET;


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

    if ( ( (Sel_Flag_i & blow_flag)==blow_flag ) &&  ( ( p_ADE->active_algs_flag & blow_flag ) == 1 ) )
    {
        p_ADE->active_algs_flag &= ~blow_flag; //spengo flag
        ADE_Blow_Release( p_ADE->p_blow);
    }

}

ADE_VOID_T ADE_SetInBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_buff)
{
    ADE_UINT32_T blow_flag = BLOW_FLAG;

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        ADE_Blow_SetInBuff(p_ADE->p_blow, p_buff);
     }

}

ADE_VOID_T ADE_SetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_buff)
{
    ADE_UINT32_T blow_flag = BLOW_FLAG;

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        ADE_Blow_SetOutBuff(p_ADE->p_blow, p_buff);
     }

}

ADE_API_RET_T ADE_Step(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i)
{

    ADE_UINT32_T blow_flag = BLOW_FLAG;
    ADE_API_RET_T blow_ret = ADE_DEFAULT_RET;

     if  ( (Sel_Flag_i & blow_flag)==blow_flag )
     {
        blow_ret = ADE_Blow_Step(p_ADE->p_blow);

        #if ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE

        if (blow_ret<0)
        {
            ADE_PRINT_ERRORS(ADE_RETCHECKS,blow_ret,"%d",ADE_Step);
            return ADE_E24;
        }


        #endif
     }

      return ADE_DEFAULT_RET;

}
