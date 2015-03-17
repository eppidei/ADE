#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE_Snap.h"
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
ADE_API_RET_T ret=ADE_DEFAULT_RET;
//ADE_UINT32_T buff_len=512;
ADE_UINT32_T Fs_i=22050,cycles_idx=0,j=0;
ADE_UINT32_T n_pow_slots_i = 2;
ADE_UINT32_T n_max_indexes_i = 2;
ADE_FLOATING_T time_left_i =0.5e-3;
ADE_FLOATING_T time_right_i = 4e-3;
ADE_UINT32_T fft_len_i = 256;
ADE_FLOATING_T *p_buff=NULL;
ADE_FLOATING_T frame_time_len = 300e-3;
ADE_UINT32_T frame_len = floor(frame_time_len*Fs_i+0.5)-1;
ADE_UINT32_T frame_len_mat = 0;
unsigned long seed= 5489;
char *input_type="matlab";
double *p_buf_mat=NULL;

init_genrand(seed);



p_buff=(ADE_FLOATING_T *)calloc(frame_len,sizeof(ADE_FLOATING_T));


ret =  ADE_Snap_Init(&p_snap, frame_len, Fs_i, n_pow_slots_i, n_max_indexes_i, time_left_i, time_right_i, fft_len_i);

if (!strcmp(input_type,"matlab"))
 {
    frame_len_mat=ADE_Matlab_GetScalar(p_snap->p_mat,"frame_len");
    if (frame_len_mat!=frame_len)
    {
        printf("adjust frame len with matlab for comparison\n");
        return -11;
    }
 }

ret = ADE_Snap_SetInbuff(p_snap, p_buff);

ret = ADE_Snap_Configure(p_snap);

for (cycles_idx=0;cycles_idx<100;cycles_idx++)
{
    if (!strcmp(input_type,"rand"))
    {
        for (j=0;j< frame_len;j++)
        {
            p_buff[j]=genrand_real4();
        }
    }
    else if (!strcmp(input_type,"matlab"))
    {

        p_buf_mat=ADE_Matlab_GetDataPointer(p_snap->p_mat,"actual_samples")+cycles_idx*p_snap->buff_len;

        for (j=0;j< frame_len;j++)
        {
            p_buff[j]=p_buf_mat[j];
        }

    }



ret = ADE_Snap_Step(p_snap);

}

ADE_Snap_Release(p_snap);
free(p_buff);

    return 0;
}


