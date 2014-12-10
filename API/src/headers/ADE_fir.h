#ifndef _ADE_FIR_H
#define _ADE_FIR_H
#include "headers/ADE_typedefs.h"

struct ADE_FIR_S
{
    ADE_UINT32_T buff_size;
    ADE_FLOATING_T *p_in;//allocati fuori
    ADE_FLOATING_T *p_out;//allocati fuori
    //ADE_UINT32_T n_SOS_sections;
    //ADE_UINT32_T active_section;
    ADE_UINT32_T filter_order;
    ADE_FLOATING_T *p_num; //allocati fuori
    //ADE_FLOATING_T **dp_denoms; //allocati fuori
    ADE_FLOATING_T *p_state; //allocati dentro
    ADE_FLOATING_T gain; //allocati fuori
    //ADE_FLOATING_T **dp_section_buffers; //allocati dentro
    ADE_blas_level1_T *p_Blas_L1;
    ADE_FIR_FILTER_IMPLEMENTATION_T filter_func;
};

ADE_API_RET_T ADE_Fir_Init(ADE_FIR_T** dp_this, ADE_UINT32_T fir_order,ADE_UINT32_T buff_size);
ADE_VOID_T ADE_Fir_Release(ADE_FIR_T* p_fir);
ADE_API_RET_T ADE_Fir_setNum(ADE_FIR_T* p_fir, ADE_FLOATING_T *p_num);
ADE_API_RET_T ADE_Fir_ResetState(ADE_FIR_T* p_fir);
ADE_API_RET_T ADE_Fir_setInbuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff);
ADE_API_RET_T ADE_Fir_setOutbuff(ADE_FIR_T* p_fir, ADE_FLOATING_T* p_buff);
ADE_API_RET_T ADE_Fir_setFilt_Implementation(ADE_FIR_T* p_iir,ADE_FILTER_IMP_CHOICE_T filt_imp_type);
ADE_API_RET_T ADE_Fir_Step(ADE_FIR_T* p_fir);




#endif //_ADE_FIR_H
