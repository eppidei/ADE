#ifndef _ADE_TESTBENCH_H
#define _ADE_TESTBENCH_H
#include "headers/ADE_typedefs.h"
#include "headers/ADE_SCDF_Interface.h"

struct ADE_TESTBENCH_S
{
    ADE_SCDF_Input_Int_T scdf_params;
    ADE_UINT32_T algo_flag;
    ADE_SCDF_Output_Int_T * p_outbuff;
    ADE_T *p_ade_handle;

};



#endif //_ADE_TESTBENCH_H
