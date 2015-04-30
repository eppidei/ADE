#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
#include "engine.h"
#include "matrix.h"
#endif
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include "headers/ADE_Snap.h"
#include "headers/ADE_SCDF_Interface.h"

static unsigned long mt[MT_N]; /* the array for the state vector  */
static int mti=MT_N+1; /* mti==MT_N+1 means mt[MT_N] is not initialized */

/* initializes mt[MT_N] with a seed */
void init_genrand(unsigned long s)
{
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
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MT_MATRIX_A};
    /* mag01[x] = x * MT_MATRIX_A  for x=0,1 */

    if (mti >= MT_N) { /* generate MT_N words at one time */
        int kk;

        if (mti == MT_N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

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
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on (-1,1)-real-interval */
double genrand_real4(void)
{
    return (genrand_real3()*2-1.0);
    /* divided by 2^32 */
}

ADE_VOID_T fill_buffer(ADE_FLOATING_T* p_buff,ADE_UINT32_T buff_len)
{
    ADE_UINT32_T i=0;

    for (i=0;i<buff_len;i++)
    {

    p_buff[i]=genrand_real4();

    }

}

int main()
{

ADE_T *p_ade_handle=NULL;
ADE_UINT32_T audio_buff_len = 0;
ADE_FLOATING_T audio_fs=0;
ADE_SCDF_Input_Int_T sensor_data;
ADE_FLOATING_T *p_data=NULL,*p_global_outbuff=NULL;
ADE_UINT32_T n_simul_cycles = 1024,cycle_idx=0;
ADE_SCDF_Output_Int_T *p_out=NULL;
ADE_API_RET_T ret_blow_step=ADE_RET_ERROR;
ADE_API_RET_T ret_blow_snap=ADE_RET_ERROR;

audio_buff_len=256;
audio_fs=44100;
n_simul_cycles=8192;
//ADE_Init(&p_ade_handle,BLOW_FLAG,audio_buff_len,audio_fs);
ADE_Init(&p_ade_handle,SNAP_FLAG,audio_buff_len,audio_fs);

#ifdef ADE_CONFIGURATION_INTERACTIVE
/*************** BLOW CASE *****************/
//audio_buff_len=ADE_Matlab_GetScalar(p_ade_handle->p_blow->p_mat,"Frame_len");
//audio_fs=ADE_Matlab_GetScalar(p_ade_handle->p_blow->p_mat,"Fs");
//n_simul_cycles=ADE_Matlab_GetScalar(p_ade_handle->p_blow->p_mat,"n_iterations");
//ADE_Release(p_ade_handle,BLOW_FLAG);
//p_ade_handle=NULL;
//ADE_Init(&p_ade_handle,BLOW_FLAG,audio_buff_len,audio_fs);
/*********** SNAP CASE **************/
    frame_len_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"frame_len");
    if (frame_len_mat!=frame_len)
    {
        printf("ERROR : adjust frame len with matlab for comparison matlab is %d\n",frame_len_mat);
        return -11;
    }
    time_left_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"time_left");
    if (time_left_mat!=time_left_i)
    {
        printf("ERROR : adjust time_left_i with matlab for comparison matlab is %f \n,time_left_mat");
        return -11;
    }
    time_right_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"time_right");
    if (time_right_mat!=time_right_i)
    {
        printf("ERROR : adjust time_right_i with matlab for comparison matlab is %f\n",time_right_mat);
        return -11;
    }
    fft_len_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"len_fft");
    if (fft_len_mat!=fft_len_i)
    {
        printf("ERROR : adjust fft_len with matlab for comparison matlab is %d \n",fft_len_mat);
        return -11;
    }
    n_pow_slots_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"n_pow_slots");
    if (n_pow_slots_mat!=n_pow_slots_i)
    {
        printf("ERROR : adjust fft_len with matlab for comparison matlab is %d \n",n_pow_slots_mat);
        return -11;
    }

    n_max_indexes_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"max_events_per_frame");
    if (n_max_indexes_mat!=n_max_indexes_i)
    {
        printf("ERROR : adjust n_max_indexes_i with matlab for comparison matlab is %d \n",n_max_indexes_mat);
        return -11;
    }

    n_simul_cycles=ADE_Matlab_GetScalar(p_snap->p_mat,"n_frames");

#endif



//ADE_Configure_params(p_ade_handle,BLOW_FLAG);
ADE_Configure_params(p_ade_handle,SNAP_FLAG);
#ifndef ADE_CONFIGURATION_INTERACTIVE
p_data=(ADE_FLOATING_T*)calloc(audio_buff_len,sizeof(ADE_FLOATING_T));
#endif
p_global_outbuff=(ADE_FLOATING_T*)calloc(audio_buff_len*n_simul_cycles,sizeof(ADE_FLOATING_T));

p_pos_event_idx=(ADE_FLOATING_T *)calloc(frame_len*n_max_indexes_i,sizeof(ADE_FLOATING_T));
p_neg_event_idx=(ADE_FLOATING_T *)calloc(frame_len*n_max_indexes_i,sizeof(ADE_FLOATING_T));


sensor_data.num_channels=1;
sensor_data.num_frames=audio_buff_len;
sensor_data.rate=audio_fs;
sensor_data.type=ADE_AudioInput;

for (cycle_idx=0;cycle_idx<n_simul_cycles;cycle_idx++)
{
    #ifdef ADE_CONFIGURATION_INTERACTIVE
    //p_data=ADE_Matlab_GetDataPointer(p_ade_handle->p_blow->p_mat,"audio_left")+cycle_idx*audio_buff_len;
    p_data=ADE_Matlab_GetDataPointer(p_ade_handle->p_snap->p_mat,"actual_samples")+cycle_idx*audio_buff_len;
    #else
    fill_buffer(p_data,audio_buff_len);
    #endif

    sensor_data.data=p_data;
   // ret_blow_step=ADE_Step(p_ade_handle,BLOW_FLAG,&sensor_data);
    ret_blow_snap=ADE_Step(p_ade_handle,SNAP_FLAG,&sensor_data);
  //  ADE_Blow_Print(p_ade_handle->p_blow,stdout,"p_blow","p_ade_handle");
   // if (ret_blow_step==ADE_RET_ERROR) return -2;
   if (ret_blow_snap==ADE_RET_ERROR) return -2;

   // p_out=ADE_GetOutBuff(p_ade_handle,BLOW_FLAG);
   p_out=ADE_GetOutBuff(p_ade_handle,SNAP_FLAG);

    memcpy(p_global_outbuff+cycle_idx*audio_buff_len,p_out->p_data,p_out->n_data*sizeof(ADE_FLOATING_T));
    /*********SNAP CASE *************/
    for (i=0;i<n_max_indexes_i;i++)
    {
        if (p_snap->n_found_indexes>0)
        {
            if (p_snap->p_snaps[i]==true)
            {
                p_pos_event_idx[pos_cnt]=p_snap->p_indexes[i]+cycles_idx*p_snap->buff_len;
                pos_cnt++;
            }
            else if (p_snap->p_snaps[i]==false)
            {
                p_neg_event_idx[neg_cnt]=p_snap->p_indexes[i]+cycles_idx*p_snap->buff_len;
                neg_cnt++;
            }
        }
    }

}

#ifdef ADE_CONFIGURATION_INTERACTIVE
// ADE_Matlab_PutVarintoWorkspace(p_ade_handle->p_blow->p_mat, p_global_outbuff, "outt", 1, audio_buff_len*n_simul_cycles, ADE_MATH_REAL);
//ADE_Matlab_launch_script_segment(p_ade_handle->p_blow->p_mat,"Blow");
//ADE_Matlab_Evaluate_StringnWait(p_ade_handle->p_blow->p_mat, "figure;hold on;plot(outt,'or');plot(expanded_pow_iir,'+b');hold off;");
ADE_Matlab_PutVarintoWorkspace(p_ade_handle->p_snap->p_mat, p_pos_event_idx, "pos_event_idx_C", 1, n_max_indexes_i*n_frames, ADE_MATH_REAL);
ADE_Matlab_PutVarintoWorkspace(p_ade_handle->p_snap->p_mat, p_neg_event_idx, "neg_event_idx_C", 1, n_max_indexes_i*n_frames, ADE_MATH_REAL);
/*** Run Matlab algorithm****/
ADE_Matlab_launch_script_segment(p_ade_handle->p_snap->p_mat,"Snap");
/* Compare Results */
ADE_Matlab_Evaluate_StringnWait(p_ade_handle->p_snap->p_mat, "figure;hold on;plot(pos_event_idx_C,'or');plot(positive_events_idx,'+b');hold off;");
ADE_Matlab_Evaluate_StringnWait(p_ade_handle->p_snap->p_mat, "figure;hold on;plot(neg_event_idx_C,'or');plot(negative_events_idx,'+b');hold off;");
#endif // ADE_CONFIGURATION_INTERACTIVE

//ADE_Release(p_ade_handle,BLOW_FLAG);
ADE_Release(p_ade_handle,SNAP_FLAG);
free(p_global_outbuff);
#ifndef ADE_CONFIGURATION_INTERACTIVE
free(p_data);
#endif



return 0;
}
