#ifndef _ADE_SOURCES_H
#define _ADE_SOURCES_H
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"

struct ADE_SOURCES_S
{

    ADE_INT32_T n_rows;
    ADE_INT32_T n_cols;
    ADE_VOID_T   *p_source_data;
    ADE_FLOATING_T phase_offset_radiants;
    ADE_FLOATING_T last_phase;
    ADE_FLOATING_T Fc;/*frequenza segnale*/
    ADE_FLOATING_T Fs;/*frequenza sampling*/
    ADE_FLOATING_T Gain;/*frequenza sampling*/
    ADE_SOURCES_TYPE_T src_type;
    unsigned long mt[MT_N]; /* the array for the state vector  */
    int mti; /* mti==MT_N+1 means mt[MT_N] is not initialized */

};
#ifdef __cplusplus
    extern "C" {
#endif
/*************** Init Methods ****************/
ADE_API_RET_T ADE_Sources_Init( ADE_SOURCES_T ** dp_sources);
ADE_VOID_T ADE_Sources_Release(ADE_SOURCES_T * p_sources);
/*************** Configure Methods ****************/
ADE_API_RET_T ADE_Sources_Configure_params(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T src_type,ADE_INT32_T n_cols,ADE_FLOATING_T gain, ... );
/*************** Processing Methods ****************/
ADE_API_RET_T Ade_Sources_Step(ADE_SOURCES_T * p_sources);
#ifdef __cplusplus
    }   /* extern "C" */
#endif
#endif _ADE_SOURCES_H


