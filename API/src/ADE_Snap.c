#include "headers/ADE_Snap.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_level2.h"
#include "headers/ADE_fft.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_complex.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"


/******* Private methods prototypes ***********************/
static ADE_API_RET_T ADE_Snap_TeagerKaiser(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_ThresholdDetection(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_Xrms2(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_find_local_max(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_extract_events(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_snap_recognition(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_snap_detector(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_doXrms2(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_FLOATING_T attack_time, ADE_FLOATING_T release_time,ADE_INT32_T len);
static ADE_API_RET_T ADE_Snap_dofind_local_max(ADE_INT32_T samples_range,ADE_INT32_T search_step,ADE_INT32_T look_ahead_step,
                                                ADE_FLOATING_T *p_min_thresh,ADE_FLOATING_T *p_data,ADE_UINT32_T *p_indexes,ADE_FLOATING_T *p_index_vals,ADE_UINT32_T *p_n_found_idxs,
                                                ADE_FLOATING_T min_data_sensed,ADE_UINT32_T len);
static ADE_API_RET_T ADE_Snap_SetInBuff(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff);
/******* ADE_METHOD_Init methods  ***********************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap,ADE_UINT32_T buff_len,ADE_UINT32_T Fs_i,ADE_UINT32_T n_pow_slots_i,ADE_UINT32_T n_max_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,ADE_UINT32_T fft_len_i)
{
    ADE_SNAP_T *p_this=NULL;
    ADE_API_RET_T iir_ret=ADE_RET_ERROR;
    ADE_API_RET_T blas2_ret_tgk1=ADE_RET_ERROR;
    ADE_API_RET_T blas2_ret_tgk2=ADE_RET_ERROR;
    ADE_API_RET_T blas1_ret_threshold=ADE_RET_ERROR;
    ADE_API_RET_T blas1_ret_pow=ADE_RET_ERROR;
    ADE_API_RET_T blas1_ret_specw=ADE_RET_ERROR;
    ADE_API_RET_T blas1_ret_specb=ADE_RET_ERROR;
    ADE_API_RET_T ret_fft=ADE_RET_ERROR;
    ADE_UINT32_T iir_n_sos=1;
    ADE_UINT32_T i=0;
    ADE_UINT32_T out_buff_fft_len=0;
    FILE *p_stream=ADE_STDERR_STREAM;

    #ifdef ADE_CONFIGURATION_INTERACTIVE
     char *p_matpath = ADE_MATLAB_EXE ;
	 char *p_scriptpath=ADE_SNAP_SCRIPT;
	 char *p_matfilepath=ADE_SNAP_WS;
	 ADE_API_RET_T mat_ret=0;
	// ADE_MATLAB_T* p_mat=NULL;
     Engine *p_mateng=NULL;
	#endif

    p_this=calloc(1,sizeof(ADE_SNAP_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this);

        p_this->buff_len=buff_len;
        p_this->Fs=Fs_i;
        p_this->frame_time_len=((ADE_FLOATING_T)(buff_len-1))/Fs_i;
        p_this->n_pow_est_slots=n_pow_slots_i;
        p_this->n_max_indexes=n_max_indexes_i;
        p_this->time_left=time_left_i;
        p_this->time_right=time_right_i;
        p_this->extract_len=fft_len_i;//ceil((time_left_i+time_right_i)*Fs_i);
        p_this->fft_len=fft_len_i;
        if (fft_len_i<p_this->extract_len)
        {
             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_SNAP,ADE_METHOD_Init,fft_len_i,"%d",(FILE*)ADE_STD_STREAM);
             fprintf(p_stream,"FFT length too short\n");
             return ADE_RET_ERROR;
        }

         /******** MATLAB ALLOC ********/

            #ifdef ADE_CONFIGURATION_INTERACTIVE

            mat_ret = ADE_Matlab_Init(&(p_this->p_mat),p_mateng,p_scriptpath, p_matfilepath,p_matpath);
            ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,mat_ret);

            #endif

        /****** ALLOC OUT BUFF TGK ******/
        p_this->p_tgk=calloc(buff_len,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_tgk);

        p_this->p_indexes=calloc( p_this->n_max_indexes,sizeof(ADE_UINT32_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_indexes);

        p_this->p_index_vals=calloc( p_this->n_max_indexes,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_index_vals);

        p_this->p_sort_indexes=calloc( p_this->n_max_indexes,sizeof(ADE_UINT32_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_sort_indexes);

        p_this->p_pow_est=calloc(buff_len,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_pow_est);

        p_this->p_thresh=calloc(buff_len,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_thresh);

        /*********** FFT allocations ****************/

        /**** segment allocation *****/
        p_this->dp_segments=(ADE_FLOATING_T**)calloc(p_this->n_max_indexes,sizeof(ADE_FLOATING_T*));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_segments);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
				#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                p_this->dp_segments[i]=(ADE_FLOATING_T*)fftw_malloc(p_this->extract_len*sizeof(ADE_FLOATING_T));
				#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
				p_this->dp_segments[i]=(ADE_FLOATING_T*)fftwf_malloc(p_this->extract_len*sizeof(ADE_FLOATING_T));
				#else
					#error ADE_FP_PRECISION in ADE_Bench_Utils
				#endif

ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_segments[i]);
                memset(p_this->dp_segments[i],0,p_this->extract_len*sizeof(ADE_FLOATING_T));
            #else

            p_this->dp_segments[i]=(ADE_FLOATING_T*)calloc(p_this->extract_len,sizeof(ADE_FLOATING_T));

             ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_segments[i]);


            #endif
        }


        /****** spectrum allocation ********/

           p_this->dp_spectrum=(ADE_VOID_T**)calloc(p_this->n_max_indexes,sizeof(ADE_VOID_T*));
            ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_spectrum);

        out_buff_fft_len=(p_this->fft_len/2+1);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
        /* size (p_this->fft_len/2+1) perchè la parte simmetrica non viene calcola */
        /* To improve bisogna rendere coerente questo con il type FFT_R2C  */
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )

			#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
            p_this->dp_spectrum[i]=(ADE_VOID_T*)fftw_malloc(out_buff_fft_len*sizeof(ADE_CPLX_T));
        	#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
        	 p_this->dp_spectrum[i]=(ADE_VOID_T*)fftwf_malloc(out_buff_fft_len*sizeof(ADE_CPLX_T));
        	#else
			 #error (ADE_FP_PRECISION in ADE_Fft_Release)
			#endif

            ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_spectrum[i]);
            memset(p_this->dp_spectrum[i],0,out_buff_fft_len*sizeof(ADE_CPLX_T));

            #else

             p_this->dp_spectrum[i]=(ADE_VOID_T*)calloc(out_buff_fft_len,sizeof(ADE_CPLX_T));
            ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_spectrum[i]);


            #endif

        }

        /** percent pow allocation **/

         p_this->p_percent_pow=calloc( p_this->n_max_indexes,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_percent_pow);

        /** percent bool allocation **/

        p_this->p_snaps=malloc( p_this->n_max_indexes*sizeof(ADE_BOOL_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_snaps);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
            p_this->p_snaps[i]=ADE_FALSE;
        }

        /* FFt objects allocation */

        p_this->dp_fft=(ADE_FFT_T**)calloc(p_this->n_max_indexes,sizeof(ADE_FFT_T*));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_fft);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
           ret_fft=ADE_Fft_Init(&(p_this->dp_fft[i]),p_this->fft_len);
           ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,ret_fft);
        }

         /************** ALLOC Blas1********************/

        p_this->dp_blas_l1_pow_est = (ADE_blas_level1_T**)calloc(p_this->n_pow_est_slots,sizeof(ADE_blas_level1_T*));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_blas_l1_pow_est);

        for (i=0;i<p_this->n_pow_est_slots;i++)
        {
                blas1_ret_pow=ADE_Blas_level1_Init(&(p_this->dp_blas_l1_pow_est[i]),ADE_MATH_REAL);
                ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,blas1_ret_pow);
        }

        p_this->p_dot_vals=calloc(p_this->n_pow_est_slots,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->p_dot_vals);

        blas1_ret_threshold=ADE_Blas_level1_Init(&(p_this->p_blas_l1_threshold),ADE_MATH_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,blas1_ret_threshold);

        /* whole spectrum allocation */
        p_this->dp_blas_l1_pow_spect_whole = (ADE_blas_level1_T**)calloc(p_this->n_max_indexes,sizeof(ADE_blas_level1_T*));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_blas_l1_pow_spect_whole);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
                blas1_ret_specw=ADE_Blas_level1_Init(&(p_this->dp_blas_l1_pow_spect_whole[i]),ADE_MATH_CPLX);
                ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,blas1_ret_threshold);
        }

        /*spectrum band allocation */

        p_this->dp_blas_l1_pow_spect_band = (ADE_blas_level1_T**)calloc(p_this->n_max_indexes,sizeof(ADE_blas_level1_T*));
        ADE_CHECK_MEMALLOC(ADE_CLASS_SNAP,ADE_METHOD_Init,p_this->dp_blas_l1_pow_spect_band);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
                blas1_ret_specb=ADE_Blas_level1_Init(&(p_this->dp_blas_l1_pow_spect_band[i]),ADE_MATH_CPLX);
                ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,blas1_ret_specb);
        }

        /************** ALLOC Blas2********************/

//        p_this->p_blas_l2_threshold = (ADE_blas_level2_T**)calloc(n_thresh_slots_i,sizeof(ADE_blas_level2_T*));
//
//        if (p_this->p_blas_l2_threshold==NULL)
//        {
//             ADE_PRINT_ERRORS(ADE_MEM,p_this->p_blas_l2_threshold,"%p",ADE_Snap_Init);
//             return ADE_RET_ERROR;
//        }
//
//        for (i=0;i<n_thresh_slots_i;i++)
//        {

       // }

        blas2_ret_tgk1=ADE_Blas_level2_Init(&(p_this->p_blas_l2_tgk1),ADE_MATH_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,blas2_ret_tgk1);

        blas2_ret_tgk2=ADE_Blas_level2_Init(&(p_this->p_blas_l2_tgk2),ADE_MATH_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,blas2_ret_tgk2);


        /***************** ALLOC CC IIR *********************/
        //iir_ret=ADE_Iir_Init(&(p_this->p_iir),iir_n_sos,buff_len,ADE_IIR_TRASP_II_B);
        //ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Init,iir_ret);

        *p_snap=p_this;


return ADE_RET_SUCCESS;

}

ADE_VOID_T ADE_Snap_Release(ADE_SNAP_T *p_snap)
{
    ADE_UINT32_T i=0;


   // ADE_Iir_Release(p_snap->p_iir);

    for (i=0;i<p_snap->n_pow_est_slots;i++)
    {
        ADE_Blas_level1_Release(p_snap->dp_blas_l1_pow_est[i]);
    }
    ADE_CHECKNFREE(p_snap->dp_blas_l1_pow_est);

    for (i=0;i<p_snap->n_max_indexes;i++)
    {
        ADE_Blas_level1_Release(p_snap->dp_blas_l1_pow_spect_whole[i]);
    }
    ADE_CHECKNFREE(p_snap->dp_blas_l1_pow_spect_whole);

    for (i=0;i<p_snap->n_max_indexes;i++)
    {
        ADE_Blas_level1_Release(p_snap->dp_blas_l1_pow_spect_band[i]);
    }
    ADE_CHECKNFREE(p_snap->dp_blas_l1_pow_spect_band);
    ADE_CHECKNFREE(p_snap->p_percent_pow);
     ADE_CHECKNFREE(p_snap->p_snaps);
     ADE_CHECKNFREE(p_snap->p_dot_vals);
    ADE_Blas_level1_Release(p_snap->p_blas_l1_threshold);
    ADE_Blas_level2_Release(p_snap->p_blas_l2_tgk1);
    ADE_Blas_level2_Release(p_snap->p_blas_l2_tgk2);
    ADE_CHECKNFREE(p_snap->p_pow_est);
    ADE_CHECKNFREE(p_snap->p_thresh);
    ADE_CHECKNFREE(p_snap->p_tgk);
    ADE_CHECKNFREE(p_snap->p_indexes);
    ADE_CHECKNFREE(p_snap->p_sort_indexes);
    ADE_CHECKNFREE(p_snap->p_index_vals);
    /* segment release */
    for (i=0;i<p_snap->n_max_indexes;i++)
    {
     #if ( ADE_FFT_IMP==ADE_USE_FFTW )
		#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        fftw_free(p_snap->dp_segments[i]);
		#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
		fftwf_free(p_snap->dp_segments[i]);
		#else
			#error ADE_FP_PRECISION in ADE_Snap_Release
		#endif
     #else
        ADE_CHECKNFREE(p_snap->dp_segments[i]);
     #endif
    }
    ADE_CHECKNFREE(p_snap->dp_segments);
    /* spectrum release */
    for (i=0;i<p_snap->n_max_indexes;i++)
    {
     #if ( ADE_FFT_IMP==ADE_USE_FFTW )
        #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        fftw_free(p_snap->dp_spectrum[i]);
		#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
		fftwf_free(p_snap->dp_spectrum[i]);
		#else
			#error ADE_FP_PRECISION in ADE_Snap_Release
		#endif
     #else
        ADE_CHECKNFREE(p_snap->dp_spectrum[i]);
     #endif
    }
    ADE_CHECKNFREE(p_snap->dp_spectrum);
     /* fft release */
     for (i=0;i<p_snap->n_max_indexes;i++)
    {

        ADE_Fft_Release(p_snap->dp_fft[i]);

    }
    ADE_CHECKNFREE(p_snap->dp_fft);

     #ifdef ADE_CONFIGURATION_INTERACTIVE
     ADE_Matlab_Release(p_snap->p_mat);
     #endif
    //
    ADE_CHECKNFREE(p_snap);



}
/******* Configure methods  ***********************/
ADE_API_RET_T ADE_Snap_Configure_inout(ADE_SNAP_T *p_snap,ADE_FLOATING_T *p_buff)
{

ADE_API_RET_T ret_set = ADE_RET_ERROR;
ADE_API_RET_T ret_elew_cfg = ADE_RET_ERROR;
ADE_API_RET_T ret_elew_cfg2 = ADE_RET_ERROR;
ADE_API_RET_T ret_b1a = ADE_RET_ERROR;
ADE_UINT32_T b1_idx=0;
ADE_FLOATING_SP_T slot_len=0, mod_res=0;
ADE_UINT32_T uslot_len=0;

ret_set =  ADE_Snap_SetInBuff(p_snap,p_buff);

ret_elew_cfg=ADE_Blas_level2_configure_elewise_inout(p_snap->p_blas_l2_tgk1,&((p_snap->p_in)[1]),&((p_snap->p_in)[1]) ,&((p_snap->p_tgk)[1]),(p_snap->buff_len)-2);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_inout,ret_elew_cfg);



ret_elew_cfg2=ADE_Blas_level2_configure_elewise_inout(p_snap->p_blas_l2_tgk2,&((p_snap->p_in)[0]),&((p_snap->p_in)[2]) ,&((p_snap->p_tgk)[1]),(p_snap->buff_len)-2);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_inout,ret_elew_cfg2);

/***2 improve first time params must be called before inout***/
if ( mod_res== 0.0F )
{
   slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
}
else
{
    fprintf(ADE_STDERR_STREAM,"!!!!!!!!ERROR:Buff len divided by pow_slots is not an integer value\n");
    return ADE_RET_ERROR;
}

uslot_len=(ADE_UINT32_T)slot_len;

for (b1_idx=0;b1_idx<p_snap->n_pow_est_slots;b1_idx++)
{
    ret_b1a = ADE_Blas_level1_configure_dot_inout(p_snap->dp_blas_l1_pow_est[b1_idx],&(p_snap->p_in[b1_idx*uslot_len]),&(p_snap->p_in[b1_idx*uslot_len]));
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_inout,ret_b1a);
}

}


ADE_API_RET_T ADE_Snap_Configure_params(ADE_SNAP_T *p_snap)
{

ADE_FLOATING_T freq_left=0;
ADE_FLOATING_T freq_right=0;
ADE_FLOATING_T spectral_threshold_schiocco  = 0;
ADE_FLOATING_T thresh_gain = 0;
ADE_FLOATING_T thresh_bias = 0;
ADE_FLOATING_T attack_time=0;
ADE_FLOATING_T release_time=0;
ADE_FLOATING_T at = 0;
ADE_FLOATING_T rt = 0;
ADE_FLOATING_T freq_step=0;
ADE_UINT32_T sx_bin=0;
ADE_UINT32_T dx_bin=0;
ADE_UINT32_T band_len=0;
ADE_FLOATING_T samp_range_search_time = 0;
ADE_UINT32_T samp_range_search = 0;
ADE_FLOATING_T max_range[2]  = {0,0};
ADE_UINT32_T search_step = 0;
ADE_UINT32_T look_ahead_step = 0;
ADE_FLOATING_T time_left=0;
ADE_FLOATING_T time_right=0;

ADE_UINT32_T b1_idx=0,thresh_idx=0,fft_idx=0;
ADE_API_RET_T ret_b1a=ADE_RET_ERROR;
ADE_API_RET_T ret_b1b=ADE_RET_ERROR;
ADE_API_RET_T ret_elew_cfg=ADE_RET_ERROR;
ADE_API_RET_T ret_fft=ADE_RET_ERROR;
ADE_API_RET_T ret_specw=ADE_RET_ERROR;
ADE_API_RET_T ret_specb=ADE_RET_ERROR;
ADE_FLOATING_SP_T slot_len=0, mod_res=0;
ADE_UINT32_T uslot_len=0;
FILE *p_stream=ADE_STDERR_STREAM;
ADE_CPLX_T *p_init_bin=NULL;


ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,p_snap);


 #ifdef ADE_CONFIGURATION_INTERACTIVE
 double  *p_max_array=NULL;

    freq_left=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_left");
    freq_right=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_right");
    spectral_threshold_schiocco=ADE_Matlab_GetScalar(p_snap->p_mat,"spectral_threshold_schiocco");
    thresh_gain=ADE_Matlab_GetScalar(p_snap->p_mat,"thresh_gain");
    thresh_bias=ADE_Matlab_GetScalar(p_snap->p_mat,"thresh_bias");
    attack_time=ADE_Matlab_GetScalar(p_snap->p_mat,"at");
    release_time=ADE_Matlab_GetScalar(p_snap->p_mat,"rt");
    samp_range_search_time=ADE_Matlab_GetScalar(p_snap->p_mat,"samp_range_search_time");
    samp_range_search=ADE_Matlab_GetScalar(p_snap->p_mat,"samp_range_search");
    p_max_array= ADE_Matlab_GetDataPointer(p_snap->p_mat, "max_range");
    if (p_max_array!=NULL)
    {
        max_range[0]  = p_max_array[0];
        max_range[1]  =  p_max_array[1];
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_SNAP,ADE_METHOD_Configure_params,p_max_array,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    search_step=ADE_Matlab_GetScalar(p_snap->p_mat,"search_step");
    look_ahead_step=ADE_Matlab_GetScalar(p_snap->p_mat,"look_ahead_step");
    time_left=ADE_Matlab_GetScalar(p_snap->p_mat,"time_left");
    time_right=ADE_Matlab_GetScalar(p_snap->p_mat,"time_right");
#else

freq_left=1800;
freq_right=3200;
if (freq_right>p_snap->Fs)
{
    ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_SNAP,ADE_METHOD_Configure_params,freq_right,"%d",(FILE*)ADE_STD_STREAM);
    fprintf(p_stream,"freq_right right %f greater than Fs/2 \n",freq_right);
    return ADE_RET_ERROR;
}
spectral_threshold_schiocco  = 0.2;
 thresh_gain = 7;
 thresh_bias = 2e-2;
 attack_time=1e-4;
 release_time=50e-3;
samp_range_search_time = p_snap->frame_time_len/4;//80e-3;
 samp_range_search = ceil(samp_range_search_time*p_snap->Fs)-1;
 max_range[0]  = 2000;
  max_range[1]  = 3000;
 search_step = 3;
 look_ahead_step = 3;
 time_left=0.5e-3;
 time_right=6e-3;

#endif
at = 1-exp(-2.2/(p_snap->Fs*attack_time));
rt = 1-exp(-2.2/(p_snap->Fs*release_time));
freq_step=p_snap->Fs/(p_snap->fft_len-1);
sx_bin=floor(freq_left/freq_step+0.5);
 dx_bin=floor(freq_right/freq_step+0.5);
band_len=dx_bin-sx_bin+1;


/*** to put into set methods***/
//p_snap->frame_time_len;
//p_snap->buff_len=frame_len;
p_snap->freq_left=freq_left;
p_snap->freq_right=freq_right;
p_snap->spectral_threshold_schiocco=spectral_threshold_schiocco;
p_snap->thresh_gain=thresh_gain;
p_snap->thresh_bias=thresh_bias;
p_snap->at=at;
p_snap->rt=rt;
p_snap->time_left=time_left;
p_snap->time_right=time_right;
p_snap->samp_range_search_time=samp_range_search_time;
p_snap->samp_range_search=samp_range_search;
p_snap->max_range[0]=max_range[0];
p_snap->max_range[1]=max_range[1];
p_snap->search_step=search_step;
p_snap->look_ahead_step=look_ahead_step;


/**** Configuration for tg ************/
/*y = x(2:L-1).^2-x(1:L-2).*x(3:L);*/

ret_elew_cfg=ADE_Blas_level2_configure_elewise_params(p_snap->p_blas_l2_tgk1,1.0,0.0);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_elew_cfg);

ret_elew_cfg=ADE_Blas_level2_configure_elewise_params(p_snap->p_blas_l2_tgk2,-1.0,1.0);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_elew_cfg);

/*** Configure Blas1 for pow estimate ********/
mod_res=fmodf ((ADE_FLOATING_SP_T)p_snap->buff_len ,(ADE_FLOATING_SP_T) p_snap->n_pow_est_slots);

if ( mod_res== 0.0F )
{
   slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
}
else
{
    fprintf(ADE_STDERR_STREAM,"!!!!!!!!ERROR:Buff len divided by pow_slots is not an integer value\n");
    return ADE_RET_ERROR;
}

uslot_len=(ADE_UINT32_T)slot_len;
for (b1_idx=0;b1_idx<p_snap->n_pow_est_slots;b1_idx++)
{
    ret_b1a = ADE_Blas_level1_configure_dot_params(p_snap->dp_blas_l1_pow_est[b1_idx],1,1,uslot_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_b1a);
}

    /*** configure threshold blas2***/

ret_b1b = ADE_Blas_level1_configure_axpy(p_snap->p_blas_l1_threshold,&(p_snap->thresh_gain),1,1,p_snap->buff_len,p_snap->p_pow_est,p_snap->p_thresh);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_b1b);

/*** fft config ****/
for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
{
    ret_fft=ADE_Fft_Configure(p_snap->dp_fft[fft_idx],ADE_FFT_R2C, ADE_FFT_FORWARD,p_snap->dp_segments[fft_idx],p_snap->dp_spectrum[fft_idx]);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_fft);
}

     /*** blas1 spectrum whole config ***/

for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
{
 ret_specw = ADE_Blas_level1_configure_dotc(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],1,1,(p_snap->fft_len/2),(p_snap->dp_spectrum[fft_idx]),(p_snap->dp_spectrum[fft_idx]));
   //per essere come matlab forse meglio len/2+1
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_specw);
}

 /*** blas1 spectrum band config ***/

 for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
{
    p_init_bin = (ADE_CPLX_T*)p_snap->dp_spectrum[fft_idx];
    ret_specb = ADE_Blas_level1_configure_dotc(p_snap->dp_blas_l1_pow_spect_band[fft_idx],1,1,band_len,(p_init_bin+sx_bin),(p_init_bin+sx_bin));
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure_params,ret_specb);

}
 return ADE_RET_SUCCESS;



}

ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff)
{

    ADE_API_RET_T ret_conf = ADE_RET_ERROR;
    ADE_API_RET_T ret_inout = ADE_RET_ERROR;

ret_conf = ADE_Snap_Configure_params(p_snap);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_conf);
ret_inout = ADE_Snap_Configure_inout(p_snap,p_buff);
ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_inout);

return ADE_RET_SUCCESS;

}


//ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap)
//{
//
//ADE_FLOATING_T freq_left=0;
//ADE_FLOATING_T freq_right=0;
//ADE_FLOATING_T spectral_threshold_schiocco  = 0;
//ADE_FLOATING_T thresh_gain = 0;
//ADE_FLOATING_T thresh_bias = 0;
//ADE_FLOATING_T attack_time=0;
//ADE_FLOATING_T release_time=0;
//ADE_FLOATING_T at = 0;
//ADE_FLOATING_T rt = 0;
//ADE_FLOATING_T freq_step=0;
//ADE_UINT32_T sx_bin=0;
//ADE_UINT32_T dx_bin=0;
//ADE_UINT32_T band_len=0;
//ADE_FLOATING_T samp_range_search_time = 0;
//ADE_UINT32_T samp_range_search = 0;
//ADE_FLOATING_T max_range[2]  = {0,0};
//ADE_UINT32_T search_step = 0;
//ADE_UINT32_T look_ahead_step = 0;
//ADE_FLOATING_T time_left=0;
//ADE_FLOATING_T time_right=0;
//
//ADE_UINT32_T b1_idx=0,thresh_idx=0,fft_idx=0;
//ADE_API_RET_T ret_b1a=ADE_RET_ERROR;
//ADE_API_RET_T ret_b1b=ADE_RET_ERROR;
//ADE_API_RET_T ret_elew_cfg=ADE_RET_ERROR;
//ADE_API_RET_T ret_fft=ADE_RET_ERROR;
//ADE_API_RET_T ret_specw=ADE_RET_ERROR;
//ADE_API_RET_T ret_specb=ADE_RET_ERROR;
//ADE_FLOATING_SP_T slot_len=0, mod_res=0;
//ADE_UINT32_T uslot_len=0;
//FILE *p_stream=ADE_STDERR_STREAM;
//ADE_CPLX_T *p_init_bin=NULL;
//
//
//ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_Configure,p_snap);
//
//
// #ifdef ADE_CONFIGURATION_INTERACTIVE
// double  *p_max_array=NULL;
//
//    freq_left=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_left");
//    freq_right=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_right");
//    spectral_threshold_schiocco=ADE_Matlab_GetScalar(p_snap->p_mat,"spectral_threshold_schiocco");
//    thresh_gain=ADE_Matlab_GetScalar(p_snap->p_mat,"thresh_gain");
//    thresh_bias=ADE_Matlab_GetScalar(p_snap->p_mat,"thresh_bias");
//    attack_time=ADE_Matlab_GetScalar(p_snap->p_mat,"at");
//    release_time=ADE_Matlab_GetScalar(p_snap->p_mat,"rt");
//    samp_range_search_time=ADE_Matlab_GetScalar(p_snap->p_mat,"samp_range_search_time");
//    samp_range_search=ADE_Matlab_GetScalar(p_snap->p_mat,"samp_range_search");
//    p_max_array= ADE_Matlab_GetDataPointer(p_snap->p_mat, "max_range");
//    if (p_max_array!=NULL)
//    {
//        max_range[0]  = p_max_array[0];
//        max_range[1]  =  p_max_array[1];
//    }
//    else
//    {
//        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_SNAP,ADE_METHOD_Configure,p_max_array,"%p",(FILE*)ADE_STD_STREAM);
//        return ADE_RET_ERROR;
//    }
//    search_step=ADE_Matlab_GetScalar(p_snap->p_mat,"search_step");
//    look_ahead_step=ADE_Matlab_GetScalar(p_snap->p_mat,"look_ahead_step");
//    time_left=ADE_Matlab_GetScalar(p_snap->p_mat,"time_left");
//    time_right=ADE_Matlab_GetScalar(p_snap->p_mat,"time_right");
//#else
//
//freq_left=1800;
//freq_right=3200;
//if (freq_right>p_snap->Fs)
//{
//    ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_SNAP,ADE_METHOD_Configure,freq_right,"%d",(FILE*)ADE_STD_STREAM);
//    fprintf(p_stream,"freq_right right %f greater than Fs/2 \n",freq_right);
//    return ADE_RET_ERROR;
//}
//spectral_threshold_schiocco  = 0.2;
// thresh_gain = 7;
// thresh_bias = 2e-2;
// attack_time=1e-4;
// release_time=50e-3;
//samp_range_search_time = p_snap->frame_time_len/4;//80e-3;
// samp_range_search = ceil(samp_range_search_time*p_snap->Fs)-1;
// max_range[0]  = 2000;
//  max_range[1]  = 3000;
// search_step = 3;
// look_ahead_step = 3;
// time_left=0.5e-3;
// time_right=6e-3;
//
//#endif
//at = 1-exp(-2.2/(p_snap->Fs*attack_time));
//rt = 1-exp(-2.2/(p_snap->Fs*release_time));
//freq_step=p_snap->Fs/(p_snap->fft_len-1);
//sx_bin=floor(freq_left/freq_step+0.5);
// dx_bin=floor(freq_right/freq_step+0.5);
//band_len=dx_bin-sx_bin+1;
//
//
///*** to put into set methods***/
////p_snap->frame_time_len;
////p_snap->buff_len=frame_len;
//p_snap->freq_left=freq_left;
//p_snap->freq_right=freq_right;
//p_snap->spectral_threshold_schiocco=spectral_threshold_schiocco;
//p_snap->thresh_gain=thresh_gain;
//p_snap->thresh_bias=thresh_bias;
//p_snap->at=at;
//p_snap->rt=rt;
//p_snap->time_left=time_left;
//p_snap->time_right=time_right;
//p_snap->samp_range_search_time=samp_range_search_time;
//p_snap->samp_range_search=samp_range_search;
//p_snap->max_range[0]=max_range[0];
//p_snap->max_range[1]=max_range[1];
//p_snap->search_step=search_step;
//p_snap->look_ahead_step=look_ahead_step;
//
///**** Configuration fro tg ************/
///*y = x(2:L-1).^2-x(1:L-2).*x(3:L);*/
//
//    ret_elew_cfg=ADE_Blas_level2_configure_elewise(p_snap->p_blas_l2_tgk1,&((p_snap->p_in)[1]),&((p_snap->p_in)[1]) ,&((p_snap->p_tgk)[1]),1.0,0.0,(p_snap->buff_len)-2);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_elew_cfg);
//
//    ret_elew_cfg=ADE_Blas_level2_configure_elewise(p_snap->p_blas_l2_tgk2,&((p_snap->p_in)[0]),&((p_snap->p_in)[2]) ,&((p_snap->p_tgk)[1]),-1.0,1.0,(p_snap->buff_len)-2);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_elew_cfg);
///*** Configure Blas1 for pow estimate ********/
//mod_res=fmodf ((ADE_FLOATING_SP_T)p_snap->buff_len ,(ADE_FLOATING_SP_T) p_snap->n_pow_est_slots);
//
//if ( mod_res== 0.0F )
//{
//   slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
//}
//else
//{
//    fprintf(ADE_STDERR_STREAM,"!!!!!!!!ERROR:Buff len divided by pow_slots is not an integer value\n");
//    return ADE_RET_ERROR;
//}
//
//uslot_len=(ADE_UINT32_T)slot_len;
//for (b1_idx=0;b1_idx<p_snap->n_pow_est_slots;b1_idx++)
//{
//    ret_b1a = ADE_Blas_level1_configure_dot(p_snap->dp_blas_l1_pow_est[b1_idx],1,1,uslot_len,&(p_snap->p_in[b1_idx*uslot_len]),&(p_snap->p_in[b1_idx*uslot_len]));
//    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_b1a);
//}
//
//    /*** configure threshold blas2***/
//
//ret_b1b = ADE_Blas_level1_configure_axpy(p_snap->p_blas_l1_threshold,&(p_snap->thresh_gain),1,1,p_snap->buff_len,p_snap->p_pow_est,p_snap->p_thresh);
//ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_b1b);
//
///*** fft config ****/
//for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
//{
//    ret_fft=ADE_Fft_Configure(p_snap->dp_fft[fft_idx],ADE_FFT_R2C, ADE_FFT_FORWARD,p_snap->dp_segments[fft_idx],p_snap->dp_spectrum[fft_idx]);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_fft);
//}
//
//     /*** blas1 spectrum whole config ***/
//
//for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
//{
// ret_specw = ADE_Blas_level1_configure_dotc(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],1,1,(p_snap->fft_len/2),(p_snap->dp_spectrum[fft_idx]),(p_snap->dp_spectrum[fft_idx]));
//   //per essere come matlab forse meglio len/2+1
//    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_specw);
//}
//
// /*** blas1 spectrum band config ***/
//
// for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
//{
//    p_init_bin = (ADE_CPLX_T*)p_snap->dp_spectrum[fft_idx];
//    ret_specb = ADE_Blas_level1_configure_dotc(p_snap->dp_blas_l1_pow_spect_band[fft_idx],1,1,band_len,(p_init_bin+sx_bin),(p_init_bin+sx_bin));
//    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Configure,ret_specb);
//
//}
// return ADE_RET_SUCCESS;
//}

/******* Operative methods  ***********************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap)
{

    ADE_UINT32_T slot_len=0;
    ADE_API_RET_T ret_b1=ADE_RET_ERROR;
    ADE_API_RET_T ret_thresh=ADE_RET_ERROR;
    ADE_API_RET_T ret_Xrms2=ADE_RET_ERROR,ret_max=ADE_RET_ERROR,ret_events=ADE_RET_ERROR,ret_recog=ADE_RET_ERROR;
    ADE_API_RET_T ret_detect=ADE_RET_ERROR;
     ADE_API_RET_T ret_memset=ADE_RET_ERROR;
    ADE_UINT32_T b1_idx=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_Configure,p_snap);

     slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
    /*  estimate = pow_est(actual_frame); */

    for (b1_idx=0;b1_idx<p_snap->n_pow_est_slots;b1_idx++)
    {
         p_snap->p_dot_vals[b1_idx]=ADE_Blas_level1_dot(p_snap->dp_blas_l1_pow_est[b1_idx]);
         ADE_Utils_memset_float(&(p_snap->p_pow_est[b1_idx*slot_len]),slot_len,p_snap->p_dot_vals[b1_idx]/slot_len);
    }


    /* thres =thresh_gain.*estimate+thresh_bias; */
    ret_memset=ADE_Utils_memset_float(p_snap->p_thresh,p_snap->buff_len,p_snap->thresh_bias);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_memset);

    ret_b1= ADE_Blas_level1_axpy(p_snap->p_blas_l1_threshold);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_b1);

    /*  detect_energy_threshold(actual_frame,thres); */

    ret_thresh =  ADE_Snap_ThresholdDetection(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_thresh);

    ret_Xrms2 = ADE_Snap_Xrms2(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_Xrms2);

    ret_max =  ADE_Snap_find_local_max(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_max);

    ret_events = ADE_Snap_extract_events(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_events);

    ret_recog = ADE_Snap_snap_recognition(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_recog);

    ret_detect=ADE_Snap_snap_detector(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Step,ret_detect);



    return ADE_RET_SUCCESS;
}



/************************* private methods ***********************/
static ADE_API_RET_T ADE_Snap_SetInBuff(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_SetInBuff,p_snap);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_SetInBuff,p_buff);

    p_snap->p_in=p_buff;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Snap_TeagerKaiser(ADE_SNAP_T *p_snap)
{

    ADE_API_RET_T ret=ADE_RET_ERROR;
    ADE_UINT32_T last_idx=0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_TeagerKaiser,p_snap);

    last_idx=p_snap->buff_len-1;

    ret=ADE_Blas_level2_Elewise(p_snap->p_blas_l2_tgk1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_TeagerKaiser,ret);


    ret=ADE_Blas_level2_Elewise(p_snap->p_blas_l2_tgk2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_TeagerKaiser,ret);
    /* y = [y(1); y; y(L-2)]; % so that lenght(y)=length(x)*/

    (p_snap->p_tgk)[0]=(p_snap->p_tgk)[1];
    (p_snap->p_tgk)[last_idx]=(p_snap->p_tgk)[last_idx-1];

    return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Snap_ThresholdDetection(ADE_SNAP_T *p_snap)
{
    ADE_API_RET_T ret=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_ThresholdDetection,p_snap);

    ret=ADE_Snap_TeagerKaiser(p_snap);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_ThresholdDetection,ret);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Snap_Xrms2(ADE_SNAP_T *p_snap)
{

    /****elaboration In-place on tgk buffer****/

    ADE_UINT32_T len=0;
    ADE_UINT32_T i=0;
    ADE_API_RET_T ret=ADE_RET_ERROR;



    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_Xrms2,p_snap);

   // local_peak=fabs(p_snap->p_tgk[0]);
    len=p_snap->buff_len;

    ret=ADE_Snap_doXrms2(p_snap->p_tgk,p_snap->p_tgk,p_snap->at, p_snap->rt,len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_Xrms2,ret);


return ADE_RET_SUCCESS;


}

static ADE_API_RET_T ADE_Snap_doXrms2(ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_FLOATING_T attack_time, ADE_FLOATING_T release_time,ADE_INT32_T len)
{

    ADE_INT32_T i=0;
    ADE_FLOATING_T coeff=0;
    ADE_FLOATING_T out0_temp=0;
    ADE_FLOATING_T a=0;
    ADE_FLOATING_T local_peak=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_doXrms2,p_in);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_doXrms2,p_out);

local_peak=fabs(p_in[0]);
out0_temp=fabs(p_in[0]);

for (i=1;i<len;i++)
{
    a=fabs(p_in[i]);

    if (a > local_peak)
    {
        coeff = attack_time;
    }
    else
    {
        coeff = release_time;
    }

    local_peak = (1-coeff)*local_peak+coeff*a;

   p_out[i]=local_peak;

}

p_out[0]=out0_temp;

return ADE_RET_SUCCESS;


}

static ADE_API_RET_T ADE_Snap_find_local_max(ADE_SNAP_T *p_snap)
{
   ADE_UINT32_T len=0;
   ADE_UINT32_T i=0,j=0,idx_l=0,idx_r=0;
   ADE_UINT32_T look_ahead_step=0;
   ADE_UINT32_T samples_range=0;
   ADE_FLOATING_T *p_min_thresh=NULL;
   ADE_FLOATING_T *p_data=NULL;
   ADE_FLOATING_T min_data_sensed=1e-10;

   ADE_UINT32_T *p_indexes=NULL;
   ADE_FLOATING_T *p_index_vals=NULL;
   ADE_UINT32_T search_step=0;
   ADE_ULONG_T *p_sort_idx=NULL;
   ADE_UINT32_T index_limit=0;
   ADE_API_RET_T ret = ADE_RET_ERROR;
   ADE_UINT32_T n_found_idxs=0;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_find_local_max,p_snap);

    len=p_snap->buff_len;
    look_ahead_step=p_snap->look_ahead_step;
    samples_range=p_snap->samp_range_search;
    p_min_thresh=p_snap->p_thresh;
    p_data=p_snap->p_tgk;
    p_indexes=p_snap->p_indexes;
    p_index_vals=p_snap->p_index_vals;
    search_step=p_snap->search_step;
    p_sort_idx=p_snap->p_sort_indexes;


 ret = ADE_Snap_dofind_local_max(samples_range,search_step,look_ahead_step,p_min_thresh,p_data,
 p_indexes,p_index_vals,&n_found_idxs,min_data_sensed,len);

 ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_find_local_max,ret);

p_snap->n_found_indexes=n_found_idxs;

//[vals,sort_idx]=sort(vals,'descend');
index_limit=p_snap->n_found_indexes;
/* Max indexes check */
if (p_snap->n_found_indexes > p_snap->n_max_indexes)
{
    index_limit=p_snap->n_max_indexes;
}
if (index_limit>0)
{
ADE_Utils_indexx_descend(index_limit,p_index_vals-1,p_sort_idx-1);
}

for(i=0;i<index_limit;i++)
{
    j=p_sort_idx[i]-1;
    p_indexes[i]=p_indexes[j];
}
//indexes=indexes(sort_idx);

p_snap->n_found_indexes=index_limit;

return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Snap_dofind_local_max(ADE_INT32_T samples_range,ADE_INT32_T search_step,ADE_INT32_T look_ahead_step,
ADE_FLOATING_T *p_min_thresh,ADE_FLOATING_T *p_data,ADE_UINT32_T *p_indexes,ADE_FLOATING_T *p_index_vals,ADE_UINT32_T *p_n_found_idxs,
ADE_FLOATING_T min_data_sensed,ADE_UINT32_T len)
{

 ADE_UINT32_T k=0,last_idx=0;
 ADE_INT32_T i=0,j=0;
 ADE_BOOL_T a1=ADE_FALSE;//,
 ADE_BOOL_T a2=ADE_FALSE;
 ADE_BOOL_T skip=ADE_FALSE;

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_dofind_local_max,p_min_thresh);
 ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_dofind_local_max,p_data);
 ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_dofind_local_max,p_indexes);
 ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_dofind_local_max,p_index_vals);

   k=0;

for (i = 3-1 ;i<=(samples_range-search_step);i+=search_step)
{
    skip = ADE_FALSE;
    if ((p_data[i]<=p_min_thresh[i]) || (p_min_thresh[i]<min_data_sensed))
    {
        skip=ADE_TRUE;
    }
    else
    {

         for (j=i+look_ahead_step;j<=(i+samples_range-look_ahead_step);j+=look_ahead_step)
         {
            a2 =  p_data[i]> p_data[j];
            if (!a2)
            {
                skip=ADE_TRUE;
                break;
            }
         }
        if (!skip)
        {
            for (j = i-look_ahead_step;j>=look_ahead_step;j-=look_ahead_step)
            {
                a1 =  p_data[i]> p_data[j];
                if (!a1)
                {
                    skip=ADE_TRUE;
                    break;
                }
            }
        }

    }

    if (!skip)
    {
        p_indexes[k]=i;
        p_index_vals[k]=p_data[i];
        k=k+1;
    }

    last_idx = i;
}


for (i = last_idx+search_step ;i<=( len -samples_range -search_step);i+=search_step)

{
skip = ADE_FALSE;
    if (p_data[i]<=p_min_thresh[i] || p_min_thresh[i]<min_data_sensed)
    {
        skip=ADE_TRUE;
    }
    else
    {
         for (j=i+look_ahead_step;j<=(i+samples_range-look_ahead_step);j+=look_ahead_step)
         {
            a2 =  p_data[i]> p_data[j];
            if (!a2)
            {
                skip=ADE_TRUE;
                break;
            }
         }
        if (!skip)
        {
            for (j = i-look_ahead_step ; j>=(i-samples_range+look_ahead_step);j-=look_ahead_step)
            {
                a1 =  p_data[i]> p_data[j];
                if (!a1)
                {
                    skip=ADE_TRUE;
                    break;
                }
            }
        }

    }

    if (!skip)
    {

        p_indexes[k]=i;
         p_index_vals[k]=p_data[i];
        k=k+1;
    }
last_idx = i;
}

for (i =  last_idx+search_step;i<=(len-search_step);i+=search_step)
{
    skip = ADE_FALSE;
    if (p_data[i]<=p_min_thresh[i] || p_min_thresh[i]<min_data_sensed)
    {
        skip=ADE_TRUE;
    }
    else
    {
         for (j=i+look_ahead_step;j<=(len-look_ahead_step);j+=look_ahead_step)
         {
            a2 =  p_data[i]> p_data[j];
            if (!a2) //a2==ADE_FALSE
            {
                skip=ADE_TRUE;
                break;
            }
         }
        if (!skip)//skip==0
        {
            for (j = i-look_ahead_step;j>=(i-samples_range+look_ahead_step);j-=look_ahead_step)
            {
                a1 =  p_data[i]> p_data[j];
                if (!a1)//a1==0
                {
                    skip=ADE_TRUE;
                    break;
                }
            }
        }

    }

    if (!skip)//skip==0
    {
         p_indexes[k]=i;
         p_index_vals[k]=p_data[i];
         k=k+1;
    }

}

*p_n_found_idxs=k;

return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Snap_extract_events(ADE_SNAP_T *p_snap)
{

ADE_UINT32_T n_indx=0;
ADE_UINT32_T extracted_allocated_len=0;
ADE_UINT32_T *p_main_idx=NULL;
ADE_INT32_T sample_left=0,sample_right=0;
ADE_FLOATING_T Fs=0;
ADE_FLOATING_T time_left=0;
ADE_FLOATING_T time_right=0;
ADE_UINT32_T frame_len=0;
ADE_UINT32_T actual_calc_len=0;
ADE_FLOATING_T **dp_segments=NULL;
ADE_FLOATING_T *p_in=NULL;
ADE_UINT32_T i=0,j=0;
FILE *p_stream=ADE_STDERR_STREAM;
ADE_API_RET_T ret_set=ADE_RET_ERROR,ret_cpy=ADE_RET_ERROR,ret_split=ADE_RET_ERROR;

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_extract_events,p_snap);

n_indx=p_snap->n_found_indexes;
extracted_allocated_len=p_snap->extract_len;
p_main_idx=p_snap->p_indexes;
Fs=p_snap->Fs;
time_left=p_snap->time_left;
time_right=p_snap->time_right;
frame_len=p_snap->buff_len;
dp_segments=p_snap->dp_segments;
p_in=p_snap->p_in;

for (i=0;i<n_indx;i++)
{

    sample_left = floor(p_main_idx[i]-time_left*Fs);
    sample_right = ceil(p_main_idx[i]+time_right*Fs);
    if (sample_left<0)
    {
        sample_left=0;
    }
    if (sample_right>frame_len-1)
    {
        sample_right=frame_len-1;
    }
    actual_calc_len=sample_right-sample_left+1;
    if (actual_calc_len>extracted_allocated_len)
    {
       ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_SNAP,ADE_METHOD_extract_events,actual_calc_len,"%d",(FILE*)ADE_STD_STREAM);
        fprintf(p_stream,"sample right and left longer than expected in  ADE_Snap_extract_events %d vs max allowed %d \n",actual_calc_len,extracted_allocated_len);
        return ADE_RET_ERROR;
    }

    ret_set=ADE_Utils_memset_float(dp_segments[i],extracted_allocated_len,0);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_extract_events,ret_set);
    //memset(dp_segments[i],0,extracted_allocated_len*sizeof(ADE_FLOATING_T));
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    ret_cpy=ADE_Utils_memcpy_float(dp_segments[i],&(p_in[sample_left]),actual_calc_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_extract_events,ret_cpy);
   // memcpy(dp_segments[i],&(p_in[sample_left]),actual_calc_len*sizeof(ADE_FLOATING_T));
    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
//    for (j=0;j<actual_calc_len;j++)
//    {
//     ret_split=ADE_Fft_FillSplitIn(p_snap->dp_fft[i],p_in[p_main_idx[i]],0,j);
//     ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_extract_events,ret_split);
//     }
    vDSP_ctoz((ADE_DSPComplex *) &(p_in[p_main_idx[i]]), 2*1, &(p_snap->dp_fft[i]->split_in), 1, actual_calc_len/2);
     #else
        #error (ADE_FFT_IMP)
    #endif
}

return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Snap_snap_recognition(ADE_SNAP_T *p_snap)
{

ADE_UINT32_T n_events=p_snap->n_found_indexes;
ADE_UINT32_T i=0;
ADE_FLOATING_T whole_pow_spec=0,sel_pow_spec=0;
ADE_CPLX_T temp_cplx;
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
ADE_DSPSplitComplex temp_cplx_split;
ADE_DSPSplitComplex *p_split_in=NULL;
ADE_DSPSplitComplex split_in2;
#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
ADE_DSPDoubleSplitComplex temp_cplx_split;
ADE_DSPDoubleSplitComplex *p_split_in=NULL;
ADE_DSPDoubleSplitComplex split_in2;
#else
    #error (ADE_FP_PRECISION in ADE_Snap_snap_recognition)
#endif
ADE_API_RET_T ret_fft=ADE_RET_ERROR;
/**2 Improve **/
 #if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
ADE_FLOATING_T freq_step=p_snap->Fs/(p_snap->fft_len-1);
ADE_UINT32_T sx_bin=floor(p_snap->freq_left/freq_step+0.5);
ADE_UINT32_T dx_bin=floor(p_snap->freq_right/freq_step+0.5);
ADE_UINT32_T band_len=dx_bin-sx_bin+1;
ADE_FLOATING_T *p_real_part;
ADE_FLOATING_T *p_imag_part;
#endif
/*****/

ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_snap_recognition,p_snap);

  for (i=0;i<n_events;i++)
  {

        //fff = fft(events(i,1:len_fft));
        ret_fft=ADE_Fft_Step(p_snap->dp_fft[i]);
        ADE_CHECK_ADERETVAL(ADE_CLASS_SNAP,ADE_METHOD_snap_recognition,ret_fft);

        #if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
        p_split_in=&(p_snap->dp_fft[i]->split_out);
            #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
            vDSP_zidotpr (p_split_in,1,p_split_in,1,&temp_cplx_split,(p_snap->fft_len/2));
            #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
             vDSP_zidotprD (p_split_in,1,p_split_in,1,&temp_cplx_split,(p_snap->fft_len/2));
            #else
                #error (ADE_FP_PRECISION in ADE_Snap_snap_recognition)
            #endif

         whole_pow_spec=*(temp_cplx_split.realp);

            p_real_part=p_split_in->realp + sx_bin;
            p_imag_part=p_split_in->imagp + sx_bin;
            split_in2.realp=p_real_part;
            split_in2.imagp=p_imag_part;
         #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
            vDSP_zidotpr (&split_in2,1,&split_in2,1,&temp_cplx_split,band_len);
        #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
            vDSP_zidotprD (&split_in2,1,&split_in2,1,&temp_cplx_split,band_len);
        #else
                #error (ADE_FP_PRECISION in ADE_Snap_snap_recognition)
            #endif
        sel_pow_spec=*(temp_cplx_split.realp);

        #elif ( ADE_FFT_IMP==ADE_USE_FFTW )
//        spectrum(i,:) =real(fff).^2+imag(fff).^2;
//        spectrum(i,1:3) = 0;
((ADE_CPLX_T*)(((ADE_CPLX_T**)p_snap->dp_spectrum)[i]))[0]=ADE_cset(0,0);
((ADE_CPLX_T*)(((ADE_CPLX_T**)p_snap->dp_spectrum)[i]))[1]=ADE_cset(0,0);
((ADE_CPLX_T*)(((ADE_CPLX_T**)p_snap->dp_spectrum)[i]))[2]=ADE_cset(0,0);
//        whole_pow_spec = spectrum(i,1:len_fft/2);
//        tot_pow(i) = sum(whole_pow_spec);

//ADE_API_RET_T ADE_Utils_Split2Complex( ADE_SplitComplex_T *p_in,ADE_UINT32_T Stride_in,ADE_CPLX_T *p_out,ADE_UINT32_T Stride_out,ADE_UINT32_T split_len)

        temp_cplx= ADE_Blas_level1_dotc(p_snap->dp_blas_l1_pow_spect_whole[i]);

        whole_pow_spec=temp_cplx.realpart;

         temp_cplx= ADE_Blas_level1_dotc(p_snap->dp_blas_l1_pow_spect_band[i]);

         sel_pow_spec=temp_cplx.realpart;
//
//         vara(i)=var(sel_pow_spec);
//        sel_pow2(i) = sum(sel_pow_spec);



        #else
            #error ADE_FFT_IMP in ADE_Snap_snap_recognition

        #endif

         p_snap->p_percent_pow[i]=sel_pow_spec/whole_pow_spec;

//        [max_peak(i),bin_peak(i)]=max(sel_pow_spec);
//        peak_idx = sx_bin_b2+bin_peak(i);
//         band_peak(i)=(peak_idx-1)*freq_step;
//         bal_right = peak_idx+bal_range;
//         bal_left = peak_idx-bal_range;
//         rp = sum(spectrum(i,peak_idx:1:bal_right));
//         lp = sum(spectrum(i,peak_idx:-1:bal_left));
//         balance(i)= rp/lp;
    }

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Snap_snap_detector(ADE_SNAP_T *p_snap)
{

ADE_UINT32_T n_events=p_snap->n_found_indexes;
ADE_FLOATING_T percent_pow=0;
ADE_UINT32_T i=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_snap_detector,p_snap);

p_snap->state=ADE_FALSE;

for (i=0;i<n_events;i++)
  {

    percent_pow=p_snap->p_percent_pow[i];

    if (percent_pow>=p_snap->spectral_threshold_schiocco)
    {

        p_snap->p_snaps[i]=ADE_TRUE;
        p_snap->state=ADE_TRUE; /* se almeno uno -> snap true*/

    }
    else
    {
        p_snap->p_snaps[i]=ADE_FALSE;

    }


  }

 return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Snap_Print(ADE_SNAP_T* p_snap, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{

     ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;
       ADE_CHAR_T temp_str[64];
       ADE_UINT32_T i=0;
       ADE_CHAR_T temp_str2[32];

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_Print,p_snap);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_SNAP,ADE_METHOD_Print,p_fid);

            memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);
 memset(temp_str,'\0',sizeof(temp_str));

    if (p_fid!=NULL)
    {
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"Fs = %f\n"),p_snap->Fs);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"buff_len = %u\n"),p_snap->buff_len);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"n_pow_est_slots = %u\n"),p_snap->n_pow_est_slots);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"frame_time_len = %f\n"),p_snap->frame_time_len);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"freq_left = %f\n"),p_snap->freq_left);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"freq_right = %f\n"),p_snap->freq_right);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"spectral_threshold_schiocco = %f\n"),p_snap->spectral_threshold_schiocco);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"thresh_gain = %f\n"),p_snap->thresh_gain);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"thresh_bias = %f\n"),p_snap->thresh_bias);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"at = %f\n"),p_snap->at);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"rt = %f\n"),p_snap->rt);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"time_left = %f\n"),p_snap->time_left);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"time_right = %f\n"),p_snap->time_right);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"samp_range_search_time = %f\n"),p_snap->samp_range_search_time);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"max_range = %p(%f)\n"),p_snap->max_range,p_snap->max_range[0]);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"samp_range_search = %u\n"),p_snap->samp_range_search);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"search_step = %u\n"),p_snap->search_step);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"look_ahead_step = %u\n"),p_snap->look_ahead_step);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"n_max_indexes = %u\n"),p_snap->n_max_indexes);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"n_found_indexes = %u\n"),p_snap->n_found_indexes);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"extract_len = %u\n"),p_snap->extract_len);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"fft_len = %u\n"),p_snap->fft_len);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_in = %p(%f)\n"),p_snap->p_in,p_snap->p_in[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_pow_est = %p(%f)\n"),p_snap->p_pow_est,p_snap->p_pow_est[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_dot_vals = %p(%f)\n"),p_snap->p_dot_vals,p_snap->p_dot_vals[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_thresh = %p(%f)\n"),p_snap->p_thresh,p_snap->p_thresh[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_tgk = %p(%f)\n"),p_snap->p_tgk,p_snap->p_tgk[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_indexes = %p(%f)\n"),p_snap->p_indexes,p_snap->p_indexes[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_sort_indexes = %p(%f)\n"),p_snap->p_sort_indexes,p_snap->p_sort_indexes[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_index_vals = %p(%f)\n"),p_snap->p_index_vals,p_snap->p_index_vals[0]);
        for (i=0;i<p_snap->n_max_indexes;i++)
        {
             strcpy(pri_str,fixed_str);
            fprintf(p_fid,strcat(pri_str,"dp_segments = %p([%d]%p(%f))\n"),p_snap->dp_segments,i,p_snap->dp_segments[i],*(p_snap->dp_segments[i]));
            strcpy(pri_str,fixed_str);
            fprintf(p_fid,strcat(pri_str,"dp_spectrum = %p(to do)\n"),p_snap->dp_spectrum);
        }
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_percent_pow = %p(%f)\n"),p_snap->p_percent_pow,p_snap->p_percent_pow[0]);
         strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_snaps = %p(%f)\n"),p_snap->p_snaps,p_snap->p_snaps[0]);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"state = %d\n"),p_snap->state);
        //strncpy(temp_str,fixed_str,len_str-2);
        //ADE_Iir_Print(p_snap->p_iir,p_fid,"p_iir",temp_str);
        for (i=0;i<p_snap->n_pow_est_slots;i++)
        {
            strncpy(temp_str,fixed_str,len_str-2);
             sprintf(temp_str2,"dp_blas_l1_pow_est[%d]",i);
            ADE_Blas_level1_Print(p_snap->dp_blas_l1_pow_est[i],p_fid,temp_str2,temp_str);
        }
        strcpy(pri_str,fixed_str);
        ADE_Blas_level1_Print(p_snap->p_blas_l1_threshold,p_fid,"p_blas_l1_threshold",temp_str);
         strcpy(pri_str,fixed_str);
        ADE_Blas_level2_Print(p_snap->p_blas_l2_tgk1,p_fid,"p_blas_l2_tgk1",temp_str);
         strcpy(pri_str,fixed_str);
        ADE_Blas_level2_Print(p_snap->p_blas_l2_tgk2,p_fid,"p_blas_l2_tgk2",temp_str);
        for (i=0;i<p_snap->n_max_indexes;i++)
        {
            strncpy(temp_str,fixed_str,len_str-2);
             sprintf(temp_str2,"dp_blas_l1_pow_spect_whole[%d]",i);
            ADE_Blas_level1_Print(p_snap->dp_blas_l1_pow_spect_whole[i],p_fid,temp_str2,temp_str);
            sprintf(temp_str2,"dp_blas_l1_pow_spect_band[%d]",i);
            ADE_Blas_level1_Print(p_snap->dp_blas_l1_pow_spect_band[i],p_fid,temp_str2,temp_str);
        }

       strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"dp_fft = %p to do\n"),p_snap->dp_fft);

    }

    }

