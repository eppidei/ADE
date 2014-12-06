#include <math.h>
#include "my_defines.h"
#include "my_typedefs.h"

s_int32 filter_DII_T (s_float *in, s_float *out, s_float *a, s_uint32 order, s_float *b, s_float *state,s_uint32 len_frame)
{
    s_uint32 i=0,k=0;

    for (k=0;k<len_frame;k++)
    {


    out[k] = b[0]*(in[k])+state[0];

        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    return SCDF_OK_RETURN;

}

s_int32 filter_DII_T_b (dsp_float_t *in, dsp_float_t *out, dsp_float_t *a, s_uint32 order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,s_uint32 len_frame)
{
    s_uint32 i=0,k=0;

    for (k=0;k<len_frame;k++)
    {


    out[k] = gain*b[0]*(in[k])+state[0];

        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    return SCDF_OK_RETURN;

}

s_int32 filter_fir (dsp_float_t *in, dsp_float_t *out, s_uint32 order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,s_uint32 len_frame)
{
    s_uint32 i=0,k=0;

    for (k=0;k<len_frame;k++)
    {


    out[k] = gain*b[0]*(in[k])+state[0];

        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=b[i+1]*(in[k])+state[i+1];
        }

    }

    return SCDF_OK_RETURN;

}

