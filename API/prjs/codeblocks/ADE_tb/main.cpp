#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
#include "engine.h"
#include "matrix.h"
#endif
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE.h"
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
ADE_UINT32_T blow_buff_len = 512;
ADE_FLOATING_T audio_fs=44100;
ADE_SCDF_Input_Int_T sensor_data;
ADE_FLOATING_T *p_data=NULL;
ADE_UINT32_T n_simul_cycles = 1024,cycle_idx=0;

sensor_data.num_channels=1;
sensor_data.num_frames=blow_buff_len;
sensor_data.rate=audio_fs;
sensor_data.type=ADE_AudioInput;

p_data=(ADE_FLOATING_T*)calloc(blow_buff_len,sizeof(ADE_FLOATING_T));
if (p_data==NULL)
{
    return -11;
}

sensor_data.data=p_data;

ADE_Init(&p_ade_handle,BLOW_FLAG,blow_buff_len,audio_fs);
ADE_Init(&p_ade_handle,SNAP_FLAG,blow_buff_len,audio_fs);

for (cycle_idx=0;cycle_idx<n_simul_cycles;cycle_idx++)
{
    fill_buffer(p_data,blow_buff_len);
    ADE_Step(p_ade_handle,BLOW_FLAG,&sensor_data);
    ADE_Step(p_ade_handle,SNAP_FLAG,&sensor_data);
    //printf("%u\n",cycle_idx);
}


ADE_Release(p_ade_handle,BLOW_FLAG);
ADE_Release(p_ade_handle,SNAP_FLAG);
free(p_data);



return 0;
}
