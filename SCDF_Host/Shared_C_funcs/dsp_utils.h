#ifndef _DSP_UTILS_H
#define _DSP_UTILS_H



#ifdef __cplusplus
extern "C"
{
#endif
s_int32 filter_DII_T (s_float *in, s_float *out, s_float *a, s_uint32 order, s_float *b, s_float *state,s_uint32 len_frame);
s_int32 filter_DII_T_b (dsp_float_t *in, dsp_float_t *out, dsp_float_t *a, s_uint32 order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,s_uint32 len_frame);
s_int32 filter_fir (dsp_float_t *in, dsp_float_t *out,s_uint32 order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,s_uint32 len_frame);
#ifdef __cplusplus
}
#endif


#endif