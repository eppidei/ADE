#include "headers/ADE_TestBench.h"
#include "headers/ADE_errors.h"
#include <strings.h>



ADE_API_RET_T ADE_TestBench_Init( ADE_TESTBENCH_T ** dp_tb)
{
    ADE_TESTBENCH_T * p_this=NULL;

    p_this=calloc(1,sizeof(ADE_TESTBENCH_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,p_this);


    *dp_tb=p_this;

    return ADE_RET_SUCCESS;

}

ADE_VOID_T ADE_TestBench_Release(ADE_TESTBENCH_T * p_tb)
{

     ADE_CHECKNFREE(p_tb);
}
//Varargs ADE_INT32_T num_frames,ADE_SensorType_T,ADE_INT32_T rate;
ADE_API_RET_T ADE_TestBench_LoadParams(ADE_TESTBENCH_T * p_tb,ADE_UINT32_T flag,ADE_T* p_ade_handle,...)
{
    ADE_MATLAB_T *p_mat =NULL;
    va_list arguments;


    p_tb->flag=flag;
    p_tb->p_ade_handle=p_ade_handle;

    #if defined(ADE_CONFIGURATION_INTERACTIVE)
    if (flag==BLOW_FLAG)
    {
        p_mat=p_ade_handle->p_blow->p_mat;
        p_tb->sensor_data.num_channels=1;
        p_tb->sensor_data.num_frames=ADE_Matlab_GetScalar(p_mat,"Frame_len");
        p_tb->sensor_data.rate=ADE_Matlab_GetScalar(p_ade_handle->p_blow->p_mat,"Fs");
        p_tb->sensor_data.type=ADE_AudioInput;

    }
    else if (flag==SNAP_FLAG)
    {
        p_mat=p_ade_handle->p_snap->p_mat;
        p_tb->sensor_data.num_channels=1;
        p_tb->sensor_data.num_frames=ADE_Matlab_GetScalar(p_mat,"frame_len");
        p_tb->sensor_data.rate=ADE_Matlab_GetScalar(p_mat,"Fs");
        p_tb->sensor_data.type=ADE_AudioInput;

    }
    else
    {


    }
    #else
      va_start ( arguments,p_ade_handle);
      num_frames=va_arg(arguments ,ADE_INT32_T);
      sens_type=va_arg(arguments ,ADE_SensorType_T);
      Fsampling=va_arg(arguments ,ADE_INT32_T);
        p_tb->sensor_data.num_channels=1;
        p_tb->sensor_data.num_frames=num_frames;
        p_tb->sensor_data.rate=Fsampling;
        p_tb->sensor_data.type=sens_type;


    #endif






}
