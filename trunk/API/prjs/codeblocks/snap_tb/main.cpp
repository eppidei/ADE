#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE_Snap.h"
#include <math.h>

int main()
{

ADE_SNAP_T * p_snap;
ADE_API_RET_T ret=ADE_DEFAULT_RET;
//ADE_UINT32_T buff_len=512;
ADE_UINT32_T Fs_i=22050;
ADE_UINT32_T n_pow_slots_i = 2;
ADE_UINT32_T n_max_indexes_i = 2;
ADE_FLOATING_T time_left_i =0.5e-3;
ADE_FLOATING_T time_right_i = 4e-3;
ADE_UINT32_T fft_len_i = 256;
ADE_FLOATING_T *p_buff=NULL;
ADE_FLOATING_T frame_time_len = 300e-3;
ADE_UINT32_T frame_len = floor(frame_time_len*Fs_i+0.5)-1;


if (frame_len%2==1)
{
    frame_len++;
}

p_buff=(ADE_FLOATING_T *)calloc(frame_len,sizeof(ADE_FLOATING_T));


ret =  ADE_Snap_Init(&p_snap, frame_len, Fs_i, n_pow_slots_i, n_max_indexes_i, time_left_i, time_right_i, fft_len_i);

ret = ADE_Snap_SetInbuff(p_snap, p_buff);

ret = ADE_Snap_Configure(p_snap);

ret = ADE_Snap_Step(p_snap);

ADE_Snap_Release(p_snap);
free(p_buff);

    return 0;
}


