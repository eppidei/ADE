#include "headers/ADE_Sources.h"
#include <stdarg.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_Error_Handler.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>



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
    p_this->phase_offset_radiants=0;
    p_this->src_type=ADE_SOURCES_NOTYPE;
    p_this->Gain=0.0;
    p_this->Fc=0.0;
    p_this->Fs=0.0;
    p_this->mti=MT_N+1;

    *dp_sources=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Sources_Release(ADE_SOURCES_T * p_sources)
{
    ADE_CHECKNFREE(p_sources->p_source_data);
    ADE_CHECKNFREE(p_sources);

}
/* variable parameters Fc, Fs */
ADE_API_RET_T ADE_Sources_Configure_params(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T src_type,ADE_INT32_T n_cols,ADE_FLOATING_T gain, ... )
{
    va_list arguments;
    ADE_API_RET_T src_ret=ADE_RET_ERROR;
    ADE_API_RET_T col_ret=ADE_RET_ERROR;
    ADE_API_RET_T gain_ret=ADE_RET_ERROR;
    ADE_API_RET_T fc_ret=ADE_RET_ERROR;
    ADE_API_RET_T fs_ret=ADE_RET_ERROR;
    ADE_INT32_T i = 0;
    ADE_FLOATING_T fc,fs;


    src_ret=ADE_Sources_SetSourceType(p_sources,src_type);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,src_ret);
    col_ret=ADE_Sources_SetNoCols(p_sources,n_cols);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,col_ret);
    gain_ret=ADE_Sources_SetGain(p_sources,gain);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,gain_ret);
    if ( (src_type==ADE_SOURCES_SINE) || (src_type==ADE_SOURCES_COSINE) )
    {
        va_start ( arguments, gain );
        fc=va_arg(arguments ,ADE_FLOATING_T);
        fc_ret=ADE_Sources_SetFc(p_sources,fc);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,fc_ret);
        fs=va_arg(arguments ,ADE_FLOATING_T);
        fs_ret=ADE_Sources_SetFs(p_sources,fs);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SOURCES,ADE_METHOD_Configure_params,fs_ret);

    }

    va_end(arguments);
    return ADE_RET_SUCCESS;
}

ADE_API_RET_T Ade_Sources_Step(ADE_SOURCES_T * p_sources)
{

     ADE_FLOATING_T normalized_frequency=0.0;
     ADE_INT32_T i=0;
     ADE_INT32_T n_row = 0;
     ADE_INT32_T n_col = 0;
     ADE_FLOATING_T fase = 0.0;
     ADE_FLOATING_T gain = 0.0;
     ADE_FLOATING_T norm_phase = 0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_Step,p_sources);


     normalized_frequency=p_sources->Fc/p_sources->Fs;
     n_row = p_sources->n_rows;
     n_col = p_sources->n_cols;
     fase = p_sources->last_phase;
     gain = p_sources->Gain;

    if (p_sources==ADE_SOURCES_SINE)
    {
        for (i=0;i<=n_row*n_col;i++)
        {
            norm_phase = fmod(fase+(i+1)*normalized_frequency,1);
            ((ADE_FLOATING_T*)p_sources->p_source_data)[i]=gain*sin(2*M_PI*norm_phase);

        }
        p_sources->last_phase=norm_phase;

    }
    else if (p_sources==ADE_SOURCES_COSINE)
    {
        for (i=0;i<=n_row*n_col;i++)
        {
            norm_phase = fmod(fase+(i+1)*normalized_frequency,1);
            ((ADE_FLOATING_T*)p_sources->p_source_data)[i]=gain*cos(2*M_PI*norm_phase);

        }
        p_sources->last_phase =norm_phase;
    }
    else if (p_sources==ADE_SOURCES_RAND)
    {
        ((ADE_FLOATING_T*) p_sources->p_source_data)[i]=gain*genrand_real4(p_sources->mt);
    }
    else
    {
        ADE_LOG(stderr,"ADE -> Unhandled type %d\n",p_sources->src_type);

    }
}

/*******************************************************************/
/************************* PRIVATE METHODS ***********************/
/*******************************************************************/

static ADE_API_RET_T  ADE_Sources_SetGain(ADE_SOURCES_T *p_sources,ADE_FLOATING_T gain)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetGain,p_sources);

    p_sources->Gain=gain;

    return ADE_RET_SUCCESS;
}
static ADE_API_RET_T ADE_Sources_SetFc(ADE_SOURCES_T *p_sources,ADE_FLOATING_T Fc)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SOURCES,ADE_METHOD_SetFc,p_sources);

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
