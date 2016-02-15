#include "headers/ADE_proximity.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>

struct ADE_PROXIMITY_S
{
    ADE_FLOATING_T Fs_i;
    ADE_UINT32_T buff_len_i;
    ADE_FLOATING_T Fs_o;
    ADE_UINT32_T buff_len_o;
    ADE_FLOATING_T * p_in;
    ADE_BOOL_T state;
};


ADE_API_RET_T ADE_Proximity_Init(ADE_PROXIMITY_T **dp_proxy)
{

    ADE_PROXIMITY_T *p_this=NULL;

     p_this=calloc(1,sizeof(ADE_PROXIMITY_T));
     ADE_CHECK_MEMALLOC(ADE_CLASS_PROXIMITY,ADE_METHOD_Init,p_this);

    // p_this->p_state=calloc(ADE_PROXIMITY_MAX_BUFF_LEN,sizeof(ADE_BOOL_T));
     //ADE_CHECK_MEMALLOC(ADE_CLASS_PROXIMITY,ADE_METHOD_Init,p_this->p_state);

     *dp_proxy=p_this;


    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Proximity_Release(ADE_PROXIMITY_T *p_proxy)
{
   // ADE_CHECKNFREE(p_proxy->p_state);
    ADE_CHECKNFREE(p_proxy);
}

ADE_API_RET_T ADE_Proximity_Configure_params(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T Fs_i)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure_params,p_proxy);

    p_proxy->Fs_i=Fs_i;
    p_proxy->Fs_o=Fs_i;// step method shall overwrite this if different

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Proximity_Configure_inout(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T * p_in)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure_inout,p_proxy);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure_inout,p_in);

    p_proxy->p_in=p_in;

    return ADE_RET_SUCCESS;

}
/**************************** Get Methods ********************************************/


ADE_API_RET_T ADE_Proximity_GetFsOut(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T *p_Fs)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_GetFsOut,p_proxy);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_GetFsOut,p_Fs);

    *p_Fs=p_proxy->Fs_o;

return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_Proximity_GetState(ADE_PROXIMITY_T *p_proxy,ADE_BOOL_T *p_state)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_GetState,p_proxy);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_GetState,p_state);

     *p_state=p_proxy->state;

return ADE_RET_SUCCESS;
}












/****************************end Get Methods******************************/

ADE_API_RET_T ADE_Proximity_Configure_bufflength(ADE_PROXIMITY_T *p_proxy,ADE_UINT32_T buff_len_i)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure_params,p_proxy);

    if (buff_len_i>ADE_PROXIMITY_MAX_BUFF_LEN)
    {
        fprintf((FILE*)ADE_STD_STREAM," buff len in %d greater than max allowed %d\n",buff_len_i,ADE_PROXIMITY_MAX_BUFF_LEN);
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_CHECK_INPUTS,ADE_CLASS_PROXIMITY,ADE_METHOD_Configure_bufflength,buff_len_i,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    p_proxy->buff_len_i=buff_len_i;

//     if (buff_len_o>ADE_PROXIMITY_MAX_BUFF_LEN)
//    {
//        fprintf((FILE*)ADE_STD_STREAM," buff len out %d greater than max allowed %d\n",buff_len_i,ADE_PROXIMITY_MAX_BUFF_LEN);
//        ADE_PRINT_ERRORS(ADE_ERROR,ADE_CHECK_INPUTS,ADE_CLASS_PROXIMITY,ADE_METHOD_Configure_bufflength,buff_len_o,"%d",(FILE*)ADE_STD_STREAM);
//        return ADE_RET_ERROR;
//    }


      p_proxy->buff_len_o=buff_len_i;// step method shall overwrite this if different

      return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_Proximity_Configure(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T Fs_i,ADE_UINT32_T buff_len_i,ADE_FLOATING_T * p_in)
{

    ADE_API_RET_T ret_par=ADE_RET_ERROR;
    ADE_API_RET_T ret_inout=ADE_RET_ERROR;
    ADE_API_RET_T ret_buff=ADE_RET_ERROR;

    ret_par=ADE_Proximity_Configure_params(p_proxy, Fs_i);
    ADE_CHECK_ADERETVAL(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure,ret_par);

    ret_inout=ADE_Proximity_Configure_inout(p_proxy,p_in);
    ADE_CHECK_ADERETVAL(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure,ret_inout);

    ret_buff=ADE_Proximity_Configure_bufflength(p_proxy,buff_len_i);
    ADE_CHECK_ADERETVAL(ADE_CLASS_PROXIMITY,ADE_METHOD_Configure,ret_buff);

    return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_Proximity_Step(ADE_PROXIMITY_T *p_proxy)
{
    ADE_INT32_T i=0;

     p_proxy->state=ADE_FALSE;

    for (i=0;i<p_proxy->buff_len_i;i++)
    {
        if (p_proxy->p_in[i]>0)
        {
            p_proxy->state=ADE_TRUE;
            break;
        }
    }

     return ADE_RET_SUCCESS;
}



