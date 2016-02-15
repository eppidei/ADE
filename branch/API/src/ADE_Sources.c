#include "headers/ADE_Sources.h"
#include <stdarg.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE_Matlab.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

struct ADE_SOURCES_S
{

    ADE_INT32_T n_rows;
    ADE_INT32_T n_cols;
    ADE_VOID_T   *p_source_data;
    ADE_FLOATING_T phase_offset_radiants;
    ADE_FLOATING_T last_phase;
    ADE_INT32_T step_call_idx;/* quante volte viene chiamato*/
    ADE_FLOATING_T Fc;/*frequenza segnale*/
    ADE_FLOATING_T Fs;/*frequenza sampling*/
    ADE_FLOATING_T Gain;/*frequenza sampling*/
    ADE_SOURCES_TYPE_T src_type;
    unsigned long mt[MT_N]; /* the array for the state vector  */
    int mti; /* mti==MT_N+1 means mt[MT_N] is not initialized */
    ADE_MATLAB_T *p_mat; /* NON VIENE INIZIALIZZATO MA SETTATO DALL' ESTERNO */
    ADE_CHAR_T  InBuffVarname[32];
    ADE_CHAR_T  InLenVarname[32];
    ADE_CHAR_T  FsamplingVarname[32];
    ADE_INT32_T MatCallIdx;

};

/*******************************************************************/
/************************* PRIVATE METHODS HEADERS***********************/
/*******************************************************************/

static ADE_API_RET_T ADE_Sources_SetGain(ADE_SOURCES_T *p_sources,ADE_FLOATING_T gain);
static ADE_API_RET_T ADE_Sources_SetFc(ADE_SOURCES_T *p_sources,ADE_FLOATING_T Fc);
static ADE_API_RET_T ADE_Sources_SetFs(ADE_SOURCES_T *p_sources,ADE_FLOATING_T Fs);
static ADE_API_RET_T ADE_Sources_SetNoRows(ADE_SOURCES_T *p_sources,ADE_INT32_T n_rows);
static ADE_API_RET_T ADE_Sources_SetNoCols(ADE_SOURCES_T * p_sources,ADE_INT32_T n_cols);
static void init_genrand(unsigned long * mt,unsigned long s);
static unsigned long genrand_int32(unsigned long * mt);
static double genrand_real3(unsigned long * mt);
static ADE_FLOATING_T genrand_real4(unsigned long * mt);
//static ADE_Sources_SetSourcePointer_float(ADE_SOURCES_T * p_sources, ADE_FLOATING_T **dp_buff);
//static ADE_Sources_SetSourcePointer_int(ADE_SOURCES_T * p_sources, ADE_INT32_T **dp_buff);
static ADE_API_RET_T ADE_Sources_SetSourceType(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T src);

/*******************************************************************/
/************************* PUBLIC METHODS ***********************/
/*******************************************************************/

ADE_API_RET_T ADE_Sources_Init( ADE_SOURCES_T ** dp_sources)
{
    ADE_SOURCES_T *p_this = NULL;
    ADE_INT32_T max_dim_buff =ADE_SOURCES_MAX_BUFF_LEN;


    p_this=calloc(1,sizeof(ADE_SOURCES_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_SOURCES,ADE_METHOD_Init,p_this);

    p_this->p_source_data=calloc(max_dim_buff,sizeof(ADE_FLOATING_DP_T));
    p_this->n_rows=0;
    p_this->n_cols=0;
    p_this->step_call_idx=0;
    p_this->phase_offset_radiants=0;
    p_this->src_type=ADE_SOURCES_NOTYPE;
    p_this->Gain=0.0;
    p_this->Fc=0.0;
    p_this->Fs=0.0;
    p_this->MatCallIdx=0;
    p_this->mti=MT_N+1;

    *dp_sources=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Sources_Release(ADE_SOURCES_T * p_sources)
{
    ADE_CHECKNFREE(p_sources->p_source_data);
    ADE_CHECKNFREE(p_sources);

}
/* variable parameters Fc */
ADE_API_RET_T ADE_Sources_ConfigureParams(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T src_type,ADE_INT32_T n_cols,ADE_FLOATING_T fs,ADE_FLOATING_DP_T gain, ... )
{
    va_list arguments;
    ADE_API_RET_T src_ret=ADE_RET_ERROR;
    ADE_API_RET_T col_ret=ADE_RET_ERROR;
    ADE_API_RET_T gain_ret=ADE_RET_ERROR;
    ADE_API_RET_T fc_ret=ADE_RET_ERROR;
    ADE_API_RET_T fs_ret=ADE_RET_ERROR;
    ADE_INT32_T i = 0;
    ADE_FLOATING_DP_T fc;
    ADE_FLOATING_T fc2=0;
    unsigned long seed=5555;/* tirarlo fuori */

    /* at the moment we consider only one dimensional case */
    p_sources->n_rows=1;
    src_ret=ADE_Sources_SetSourceType(p_sources,src_type);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,src_ret);
    col_ret=ADE_Sources_SetNoCols(p_sources,n_cols);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,col_ret);
    gain_ret=ADE_Sources_SetGain(p_sources,gain);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,gain_ret);
    fs_ret=ADE_Sources_SetFs(p_sources,fs);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,fs_ret);

    if ( (src_type==ADE_SOURCES_SINE) || (src_type==ADE_SOURCES_COSINE) )
    {
        va_start ( arguments, gain );
        fc=va_arg(arguments ,ADE_FLOATING_DP_T);
        fc2=(ADE_FLOATING_T)fc;
        fc_ret=ADE_Sources_SetFc(p_sources,fc2);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,fc_ret);
//        fs=va_arg(arguments ,ADE_FLOATING_T);

    }
    else if (src_type==ADE_SOURCES_RAND)
    {

       init_genrand(p_sources ->mt,seed);
    }

    va_end(arguments);
    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Sources_ConfigureFromMatlab(ADE_SOURCES_T * p_sources,ADE_MATLAB_T *p_mat,ADE_SOURCES_TYPE_T src_type,ADE_CHAR_T* InBuffVarname,ADE_CHAR_T* n_cols_varname,ADE_CHAR_T* fs_varname,ADE_CHAR_T* gain_varname )
{

    double n_cols=0;
    double fs=0,fc=0;
    double gain=0;
    ADE_INT32_T val0=0;
    ADE_FLOATING_T val0F=0;
    ADE_API_RET_T src_ret=ADE_RET_ERROR;
    ADE_API_RET_T col_ret=ADE_RET_ERROR;
    ADE_API_RET_T ret=ADE_RET_ERROR;
    ADE_API_RET_T gain_ret=ADE_RET_ERROR;
    ADE_API_RET_T fs_ret=ADE_RET_ERROR;
    ADE_API_RET_T fc_ret=ADE_RET_ERROR;
    ADE_CHAR_T* fc_varname;
    ADE_INT32_T dimallocated =0;
    ADE_INT32_T maxdim =ADE_TB_MAX_OUT_TOTAL_BUFFER;

/* at the moment we consider only one dimensional case */
    p_sources->n_rows=1;
    src_ret=ADE_Sources_SetSourceType(p_sources,src_type);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,src_ret);

    ret= ADE_Sources_SetMatlab(p_sources,p_mat);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);

    ret=ADE_Matlab_GetScalar(p_mat,n_cols_varname,&n_cols);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);
    dimallocated =n_cols*sizeof(ADE_FLOATING_T);
    ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,dimallocated,"%d",val0,maxdim);
    col_ret=ADE_Sources_SetNoCols(p_sources,(ADE_INT32_T)n_cols);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,col_ret);

    if (gain_varname==NULL)
    {
        gain=1.0;
    }
    else
    {
    ret=ADE_Matlab_GetScalar(p_mat,gain_varname,&gain);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);
    }
    gain_ret=ADE_Sources_SetGain(p_sources,(ADE_FLOATING_T)gain);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,gain_ret);

     ret=ADE_Matlab_GetScalar(p_mat,fs_varname,&fs);
     ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);
    fs_ret=ADE_Sources_SetFs(p_sources,(ADE_FLOATING_T)fs);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,fs_ret);


    ret= ADE_Sources_SetMatlabInBuffName(p_sources,InBuffVarname);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);

    ret= ADE_Sources_SetMatlabInLenVarname( p_sources,n_cols_varname);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);

    ret= ADE_Sources_SetMatlabFsamplingVarName( p_sources,fs_varname);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_SetConfigFromMatlab,ret);

    if (gain_varname!=NULL)
    {
        ADE_LOG(stderr, " variable %s not handled\n",gain_varname);
        return ADE_RET_WARNING;
    }



    return ADE_RET_SUCCESS;

}


ADE_API_RET_T Ade_Sources_FillSensorData(ADE_SOURCES_T * p_sources,ADE_SCDF_Input_Int_T *p_SensorData)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_FillSensorData,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_FillSensorData,p_SensorData);

    p_SensorData->data=p_sources->p_source_data;
    p_SensorData->num_channels=p_sources->n_rows;
    p_SensorData->num_frames=p_sources->n_cols;
    p_SensorData->rate=p_sources->Fs;
    p_SensorData->timeid=0;
    p_SensorData->timestamp=0;
    p_SensorData->type=ADE_NoType;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Sources_SetMatlab(ADE_SOURCES_T * p_sources, ADE_MATLAB_T *p_mat)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlab,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlab,p_sources);


    p_sources->p_mat=p_mat;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T Ade_Sources_GetData(ADE_SOURCES_T * p_sources,ADE_FLOATING_T **dp_data)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetData,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetData,dp_data);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetData,p_sources->p_source_data);


    *dp_data=(ADE_FLOATING_T*) p_sources->p_source_data;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T Ade_Sources_GetNoCols(ADE_SOURCES_T * p_sources,ADE_INT32_T *p_NumCols)
{

    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetNoCols,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetNoCols,p_NumCols);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_SOURCES,ADE_METHOD_GetNoCols,p_sources->n_cols,"%d",val0);


    *p_NumCols=p_sources->n_cols;

    return ADE_RET_SUCCESS;

}
ADE_API_RET_T Ade_Sources_GetNoRows(ADE_SOURCES_T * p_sources,ADE_INT32_T *p_NumRows)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetNoRows,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetNoRows,p_NumRows);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_SOURCES,ADE_METHOD_GetNoRows,p_sources->n_rows,"%d",val0);


    *p_NumRows=p_sources->n_rows;


    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetOffsetRadians(ADE_SOURCES_T * p_sources,ADE_FLOATING_T *p_off)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetOffsetRadians,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetOffsetRadians,p_off);

    *p_off=p_sources->phase_offset_radiants;

    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetLastPhase(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_phase)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetLastPhase,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetLastPhase,p_phase);

    *p_phase=p_sources->last_phase;
    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetFc(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_Fc)
{
    ADE_FLOATING_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetFc,p_sources);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetFc,p_Fc);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_SOURCES,ADE_METHOD_GetFc,p_sources->Fc,"%f",val0);

    *p_Fc=p_sources->Fc;

    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetFs(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_Fs)
{
    ADE_FLOATING_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetFs,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetFs,p_Fs);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_SOURCES,ADE_METHOD_GetFs,p_sources->Fs,"%f",val0);

    *p_Fs=p_sources->Fs;
    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetGain(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_gain)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetGain,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetGain,p_gain);

    *p_gain=p_sources->Gain;
    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetSrcType(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T  *p_type)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetSrcType,p_sources);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetSrcType,p_type);

     *p_type=p_sources->src_type;
    return ADE_RET_SUCCESS;
}
ADE_API_RET_T Ade_Sources_GetMt(ADE_SOURCES_T * p_sources,unsigned long **dp_mt)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetMt,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_GetMt,dp_mt);

    *dp_mt=p_sources->mt;
     return ADE_RET_SUCCESS;

}

/***************************** SET METHODS **********************************/
ADE_API_RET_T Ade_Sources_SetLastPhase(ADE_SOURCES_T * p_sources,ADE_FLOATING_T phase)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetLastPhase,p_sources);

    p_sources->last_phase=phase;
     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Sources_SetMatlabFsamplingVarName(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlabFsamplingVarName,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlabFsamplingVarName,varname);

    strncpy(p_sources->FsamplingVarname,varname,sizeof(p_sources->FsamplingVarname));

    return ADE_RET_SUCCESS;


}
ADE_API_RET_T ADE_Sources_SetMatlabInBuffName(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlabInBuffName,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlabInBuffName,varname);

    strncpy(p_sources->InBuffVarname,varname,sizeof(p_sources->InBuffVarname));

    return ADE_RET_SUCCESS;


}
ADE_API_RET_T ADE_Sources_SetMatlabInLenVarname(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlabInLenVarname,p_sources);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetMatlabInLenVarname,varname);

    strncpy(p_sources->InLenVarname,varname,sizeof(p_sources->InLenVarname));

    return ADE_RET_SUCCESS;


}

/**************************** Proc Methods *********************************/

ADE_API_RET_T Ade_Sources_Step(ADE_SOURCES_T * p_sources)
{

     ADE_FLOATING_T normalized_frequency=0.0;
     ADE_INT32_T i=0;
     ADE_INT32_T n_row = 0;
     ADE_INT32_T n_col = 0;
     ADE_INT32_T size_samples=0;
     ADE_FLOATING_T Fc,Fs=0;
     ADE_FLOATING_T fase = 0.0;
     ADE_FLOATING_T gain = 0.0;
     ADE_FLOATING_T norm_phase = 0.0;
     ADE_API_RET_T ret=ADE_RET_ERROR;
     ADE_SOURCES_TYPE_T src_type;
     ADE_FLOATING_T *p_data=NULL;
     double *p_data_matlab=NULL;
     unsigned long *p_mt=NULL;
     ADE_UINT32_T algo_flags=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_Step,p_sources);



    ret= Ade_Sources_GetNoCols(p_sources,&n_col);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
    ret= Ade_Sources_GetNoRows(p_sources,&n_row);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
    size_samples=n_row*n_col;
    ret= Ade_Sources_GetSrcType(p_sources,&src_type);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
    ret= Ade_Sources_GetData(p_sources,&p_data);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
    if (src_type==ADE_SOURCES_MATLAB)
    {


       ret=  ADE_Matlab_GetDataPointer(p_sources->p_mat, p_sources->InBuffVarname,&p_data_matlab);
       ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
       memcpy(p_data,&p_data_matlab[p_sources->MatCallIdx*size_samples],size_samples*sizeof(double));
       p_sources->MatCallIdx++;
    }
    else
    {
     ret= Ade_Sources_GetLastPhase(p_sources,&fase);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
    ret= Ade_Sources_GetGain(p_sources,&gain);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);





    if (src_type==ADE_SOURCES_SINE)
    {
         ret= Ade_Sources_GetFc(p_sources,&Fc);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);

        ret= Ade_Sources_GetFs(p_sources,&Fs);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
          normalized_frequency=Fc/Fs;
        for (i=0;i<=size_samples;i++)
        {
            norm_phase = fmod(fase+(i+1)*normalized_frequency,1);
            ((ADE_FLOATING_T*)p_data)[i]=gain*sin(2*M_PI*norm_phase);

        }

        ret= Ade_Sources_SetLastPhase(p_sources,norm_phase);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);


    }
    else if (src_type==ADE_SOURCES_COSINE)
    {
         ret= Ade_Sources_GetFc(p_sources,&Fc);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);

        ret= Ade_Sources_GetFs(p_sources,&Fs);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
          normalized_frequency=Fc/Fs;
        for (i=0;i<=size_samples;i++)
        {
            norm_phase = fmod(fase+(i+1)*normalized_frequency,1);
            ((ADE_FLOATING_T*)p_data)[i]=gain*cos(2*M_PI*norm_phase);

        }
        ret= Ade_Sources_SetLastPhase(p_sources,norm_phase);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
    }
    else if (src_type==ADE_SOURCES_RAND)
    {
        ret= Ade_Sources_GetMt(p_sources,&p_mt);
         ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Step,ret);
         for (i=0;i<=n_row*n_col;i++)
        {
            ((ADE_FLOATING_T*) p_data)[i]=gain*genrand_real4(p_mt);
        }
    }
    else
    {
        ADE_LOG(stderr,"ADE -> Unhandled type %d\n",src_type);
        return ADE_RET_ERROR;
    }

   }

    p_sources->step_call_idx+=1;

    return ADE_RET_SUCCESS;
}

/*******************************************************************/
/************************* PRIVATE METHODS ***********************/
/*******************************************************************/

static ADE_API_RET_T  ADE_Sources_SetGain(ADE_SOURCES_T *p_sources,ADE_FLOATING_T gain)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetGain,p_sources);
    /*ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_SOURCES,ADE_METHOD_SetGain,gain,"%d",val0); gain can be any pos or neg or zero value*/

    p_sources->Gain=gain;

    return ADE_RET_SUCCESS;
}
static ADE_API_RET_T ADE_Sources_SetFc(ADE_SOURCES_T *p_sources,ADE_FLOATING_T Fc)
{
     ADE_INT32_T val0=0;
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetFc,p_sources);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_SOURCES,ADE_METHOD_SetFc,Fc,"%d",val0);


    p_sources->Fc=Fc;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Sources_SetFs(ADE_SOURCES_T *p_sources,ADE_FLOATING_T Fs)
{
    ADE_FLOATING_T val0 = 0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetFs,p_sources);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_SOURCES,ADE_METHOD_SetFs,Fs,"%f",val0);

    p_sources->Fs=Fs;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Sources_SetNoRows(ADE_SOURCES_T *p_sources,ADE_INT32_T n_rows)
{
    ADE_INT32_T val0 = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetNoRows,p_sources);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_SOURCES,ADE_METHOD_SetNoRows,n_rows,"%d",val0);

    p_sources->n_rows=n_rows;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Sources_SetNoCols(ADE_SOURCES_T * p_sources,ADE_INT32_T n_cols)
{
    ADE_INT32_T val0 = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetNoCols,p_sources);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_SOURCES,ADE_METHOD_SetNoCols,n_cols,"%d",val0);

    p_sources->n_cols=n_cols;

    return ADE_RET_SUCCESS;
}

//static ADE_Sources_SetSourcePointer_float(ADE_SOURCES_T * p_sources, ADE_FLOATING_T **dp_buff)
//{
//    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetSourcePointer_float,p_sources);
//    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetSourcePointer_float,dp_buff);
//
//     *dp_buff=(ADE_FLOATING_T *)p_sources->p_source_data;
//    return ADE_RET_SUCCESS;
//}
//
//static ADE_Sources_SetSourcePointer_int(ADE_SOURCES_T * p_sources, ADE_INT32_T **dp_buff)
//{
//    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetSourcePointer_int,p_sources);
//    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetSourcePointer_int,dp_buff);
//
//     *dp_buff=(ADE_INT32_T *)p_sources->p_source_data;
//    return ADE_RET_SUCCESS;
//}

static ADE_API_RET_T ADE_Sources_SetSourceType(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T src)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetSourceType,p_sources);

     p_sources->src_type=src;

    return ADE_RET_SUCCESS;
}





/* initializes mt[MT_N] with a seed */
static void init_genrand(unsigned long * mt,unsigned long s)
{

    int mti;

    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<MT_N; mti++) {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* generates a random number on [0,0xffffffff]-interval */
static unsigned long genrand_int32(unsigned long * mt)
{

    unsigned long y;
    int mti;
    static unsigned long mag01[2]={0x0UL, MT_MATRIX_A};
    /* mag01[x] = x * MT_MATRIX_A  for x=0,1 */

    if (mti >= MT_N) { /* generate MT_N words at one time */
        int kk;

        if (mti == MT_N+1)   /* if init_genrand() has not been called, */
            init_genrand(mt,5489UL); /* a default initial seed is used */

        for (kk=0;kk<MT_N-MT_M;kk++) {
            y = (mt[kk]&MT_UPPER_MASK)|(mt[kk+1]&MT_LOWER_MASK);
            mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<MT_N-1;kk++) {
            y = (mt[kk]&MT_UPPER_MASK)|(mt[kk+1]&MT_LOWER_MASK);
            mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N-1]&MT_UPPER_MASK)|(mt[0]&MT_LOWER_MASK);
        mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on (0,1)-real-interval */
static double genrand_real3(unsigned long * mt)
{
    return (((double)genrand_int32(mt)) + 0.5)*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on (-1,1)-real-interval */
static ADE_FLOATING_T genrand_real4(unsigned long * mt)
{
    return (ADE_FLOATING_T)(genrand_real3(mt)*2-1.0);
    /* divided by 2^32 */
}
