#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE_Snap.h"
#include "headers/ADE_Utils.h"
#include <math.h>

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

int main()
{

ADE_SNAP_T * p_snap;
ADE_API_RET_T ret=ADE_RET_ERROR;
//ADE_UINT32_T buff_len=512;
ADE_UINT32_T Fs_i=22050,cycles_idx=0,j=0;
ADE_UINT32_T n_pow_slots_i = 2,n_pow_slots_mat;
ADE_UINT32_T n_max_indexes_i = 1,n_max_indexes_mat;
ADE_FLOATING_T time_left_i =0.5e-3,time_left_mat;
ADE_FLOATING_T time_right_i = 6e-3,time_right_mat;
ADE_UINT32_T fft_len_i = 256,fft_len_mat;
ADE_FLOATING_T *p_buff=NULL;
ADE_FLOATING_T frame_time_len = 300e-3;
ADE_UINT32_T frame_len = floor(frame_time_len*Fs_i+0.5)-1;
ADE_UINT32_T frame_len_mat = 0,n_frames=0;
unsigned long seed= 5489;
double *p_buf_mat=NULL;

ADE_FLOATING_T *p_pos_event_idx=NULL,*p_neg_event_idx=NULL;
ADE_UINT32_T pos_cnt=0,neg_cnt=0,i=0;

//ADE_FLOATING_T p_vals[10]={1,2,3,4,5,6,7,8,9,10};
//ADE_ULONG_T p_vals_idx[10]={0,0,0,0,0,0,0,0,0,0};
//
//ADE_Utils_indexx_descend(10, p_vals-1, p_vals_idx-1);



init_genrand(seed);



p_buff=(ADE_FLOATING_T *)calloc(frame_len,sizeof(ADE_FLOATING_T));


ret =  ADE_Snap_Init(&p_snap);
#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)

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

#endif
//ret = ADE_Snap_SetInBuff(p_snap, p_buff);

ret = ADE_Snap_Configure_params(p_snap, frame_len, Fs_i, n_pow_slots_i, n_max_indexes_i, time_left_i, time_right_i, fft_len_i);

//ADE_Snap_Print(p_snap,stdout,"p_snap","");

#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
n_frames=ADE_Matlab_GetScalar(p_snap->p_mat,"n_frames");
#else
n_frames=100;

#endif

p_pos_event_idx=(ADE_FLOATING_T *)calloc(frame_len*n_max_indexes_i,sizeof(ADE_FLOATING_T));
p_neg_event_idx=(ADE_FLOATING_T *)calloc(frame_len*n_max_indexes_i,sizeof(ADE_FLOATING_T));
for (cycles_idx=0;cycles_idx<n_frames;cycles_idx++)
{

#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
        p_buf_mat=ADE_Matlab_GetDataPointer(p_snap->p_mat,"actual_samples")+cycles_idx*p_snap->buff_len;
        for (j=0;j< frame_len;j++)
        {
            p_buff[j]=p_buf_mat[j];
        }
    #else
    for (j=0;j< frame_len;j++)
        {
            p_buff[j]=genrand_real4();
        }
#endif





ret = ADE_Snap_Configure_inout(p_snap, p_buff);

ret = ADE_Snap_Step(p_snap);

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
#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
ADE_Matlab_PutVarintoWorkspace(p_snap->p_mat, p_pos_event_idx, "pos_event_idx_C", 1, n_max_indexes_i*n_frames, ADE_MATH_REAL);
ADE_Matlab_PutVarintoWorkspace(p_snap->p_mat, p_neg_event_idx, "neg_event_idx_C", 1, n_max_indexes_i*n_frames, ADE_MATH_REAL);
/*** Run Matlab algorithm****/
ADE_Matlab_launch_script_segment(p_snap->p_mat,"Snap");
/* Compare Results */
ADE_Matlab_Evaluate_StringnWait(p_snap->p_mat, "figure;hold on;plot(pos_event_idx_C,'or');plot(positive_events_idx,'+b');hold off;");
ADE_Matlab_Evaluate_StringnWait(p_snap->p_mat, "figure;hold on;plot(neg_event_idx_C,'or');plot(negative_events_idx,'+b');hold off;");
#endif

/*** Free Mem ***/

ADE_Snap_Release(p_snap);
free(p_buff);
free(p_pos_event_idx);
free(p_neg_event_idx);

    return 0;
}


