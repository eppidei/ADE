#ifndef _ADE_FIR_H
#define _ADE_FIR_H
#include "headers/ADE_typedefs.h"

struct ADE_FIR_S
{
    ADE_INT32_T buff_len;
    ADE_FLOATING_T *p_in;//allocati fuori
    ADE_FLOATING_T *p_out;//allocati fuori
    ADE_INT32_T filter_order;
    ADE_INT32_T max_filter_order;
    ADE_FLOATING_T *p_num; //allocati fuori
    ADE_FLOATING_T *p_state; //allocati dentro
    ADE_FLOATING_T *p_tempbuff; //allocati dentro
    ADE_FLOATING_T gain; //allocati fuori
    ADE_blas_level1_T *p_Blas_L1;
    ADE_FIR_IMP_CHOICE_T filt_imp_type;
    ADE_FIR_FILTER_IMPLEMENTATION_T filter_func;
};

#ifdef __cplusplus
    extern "C" {
#endif
/******** Init Methods ************/
ADE_API_RET_T ADE_Fir_Init(ADE_FIR_T** dp_this);
ADE_VOID_T ADE_Fir_Release(ADE_FIR_T* p_fir);
/************* Set Methods ****************/
ADE_API_RET_T ADE_Fir_ResetState(ADE_FIR_T* p_fir,ADE_FLOATING_T rst_val);
/************* Configure Methods ****************/
ADE_API_RET_T ADE_Fir_Configure_params(ADE_FIR_T* p_fir,ADE_FLOATING_T *p_num,ADE_UINT32_T num_len,ADE_FIR_IMP_CHOICE_T filt_imp_type);
ADE_API_RET_T ADE_Fir_Configure_inout(ADE_FIR_T* p_fir,ADE_FLOATING_T* p_inbuff,ADE_FLOATING_T* p_outbuff);
ADE_API_RET_T ADE_Fir_Configure_bufflength(ADE_FIR_T* p_fir,ADE_INT32_T buff_len);
ADE_API_RET_T ADE_Fir_Configure(ADE_FIR_T* p_fir,ADE_FLOATING_T *p_num,ADE_UINT32_T num_len,ADE_FLOATING_T* p_inbuff,ADE_FLOATING_T* p_outbuff,ADE_FIR_IMP_CHOICE_T filt_imp_type, ADE_UINT32_T buff_len);
/********** Processing Methods *****************/
ADE_API_RET_T ADE_Fir_Step(ADE_FIR_T* p_fir);
/************** Utils methods *********************/

ADE_API_RET_T ADE_Fir_Print(ADE_FIR_T* p_fir, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);

#ifdef __cplusplus
    }   /* extern "C" */
#endif



#endif //_ADE_FIR_H
