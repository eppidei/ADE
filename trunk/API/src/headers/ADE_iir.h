#ifndef _ADE_IIR_H
#define _ADE_IIR_H
#include "headers/ADE_typedefs.h"

struct ADE_IIR_S
{
    ADE_UINT32_T buff_len;
    ADE_FLOATING_T *p_in;//allocati fuori
    ADE_FLOATING_T *p_out;//allocati fuori
    ADE_UINT32_T n_SOS_sections;
    ADE_UINT32_T active_section;
    ADE_UINT32_T section_order;
    ADE_FLOATING_T **dp_nums; //allocati fuori
    ADE_FLOATING_T **dp_denoms; //allocati fuori
    ADE_FLOATING_T **dp_states; //allocati dentro
    ADE_FLOATING_T *p_gains; //allocati fuori
    ADE_FLOATING_T **dp_section_buffers; //allocati dentro
    ADE_FLOATING_T **dp_tempbuff; //allocati dentro tanti quante le sezione per rendere indipendenti le strutture (threading facilitato??)
    ADE_blas_level1_T **dp_Blas_L1;
    ADE_IIR_IMP_CHOICE_T filt_imp_type;
    ADE_FILTER_IMPLEMENTATION_T filter_func;
};
#ifdef __cplusplus
    extern "C" {
#endif
/******** Init Methods ***********/
ADE_API_RET_T ADE_Iir_Init(ADE_IIR_T** dp_this, ADE_UINT32_T n_SOS_sections,ADE_UINT32_T buff_len,ADE_IIR_IMP_CHOICE_T filt_imp_type);
ADE_VOID_T ADE_Iir_Release(ADE_IIR_T* p_iir);
/************ Set Methods **************/
ADE_API_RET_T ADE_Iir_setGains(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_gains);
ADE_API_RET_T ADE_Iir_setNums(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_nums);
ADE_API_RET_T ADE_Iir_setDenoms(ADE_IIR_T* p_iir, ADE_FLOATING_T** dp_denoms);
ADE_API_RET_T ADE_Iir_SetInBuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff);
ADE_API_RET_T ADE_Iir_SetOutBuff(ADE_IIR_T* p_iir, ADE_FLOATING_T* p_buff);


/******** Processing Methods **********/
ADE_API_RET_T ADE_Iir_Step(ADE_IIR_T* p_iir);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_IIR_H
