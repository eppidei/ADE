#ifndef _ADE_TYPEDEFS_H
#define _ADE_TYPEDEFS_H
#include <stdint.h>
#include "headers/ADE_defines.h"


typedef int64_t ADE_INT64_T ;
typedef int32_t ADE_INT32_T ;
typedef int16_t ADE_INT16_T ;
typedef int8_t  ADE_CHAR_T;
typedef uint64_t ADE_UINT64_T ;
typedef uint32_t ADE_UINT32_T ;
typedef uint16_t ADE_UINT16_T ;
typedef uint8_t  ADE_UCHAR_T ;
typedef double ADE_FLOATING_DP_T;
typedef float ADE_FLOATING_SP_T;
#if (ADE_FP_PRECISION==1)
typedef ADE_FLOATING_DP_T ADE_FLOATING_T;
#elif (ADE_FP_PRECISION==0)
typedef ADE_FLOATING_SP_T ADE_FLOATING_T;
#endif
typedef ADE_INT32_T ADE_API_RET_T ;
typedef void ADE_VOID_T;

/*********************************** ADE ENVIRONMENT ***********************************/
typedef struct ADE_S ADE_T;
typedef struct ADE_S* ADE_HANDLE;

/**************************** BLAS *********************/
typedef struct ADE_blas_level1_S ADE_blas_level1_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL1_FCN_TYPE1_T)(ADE_blas_level1_T*);
typedef ADE_FLOATING_T* (*ADE_BLAS_LEVEL1_FCN_TYPE2_T)(ADE_blas_level1_T*);

/******************************* IIR **************************/
typedef struct ADE_IIR_S ADE_IIR_T;
typedef ADE_API_RET_T (*ADE_FILTER_IMPLEMENTATION_T)(ADE_IIR_T*);
typedef enum  {trasp_II,trasp_II_blas} ADE_FILTER_IMP_CHOICE_T;
/******************************* FIR **************************/
typedef struct ADE_FIR_S ADE_FIR_T;
typedef ADE_API_RET_T (*ADE_FIR_FILTER_IMPLEMENTATION_T)(ADE_FIR_T*);

/**************************** POLYFIT ****************************/
typedef struct ADE_POLYFIT_S ADE_POLYFIT_T;

/******************************* BLOW ***************************/
typedef struct ADE_BLOW_S ADE_BLOW_T;
typedef ADE_API_RET_T (*ADE_BLOW_FUNCS_T)(ADE_BLOW_T*);
typedef enum  {waiting,evaluation,tracking} BLOW_SM_STATES_T;


#endif //_ADE_TYPEDEFS_H
