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


/******* Private methods prototypes ***********************/
static ADE_API_RET_T ADE_Snap_TeagerKaiser(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_ThresholdDetection(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_Xrms2(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_find_local_max(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_extract_events(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_snap_recognition(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_snap_detector(ADE_SNAP_T *p_snap);
/******* Init methods  ***********************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap,ADE_UINT32_T buff_len,ADE_UINT32_T Fs_i,ADE_UINT32_T n_pow_slots_i,ADE_UINT32_T n_max_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,ADE_UINT32_T fft_len_i)
{
    ADE_SNAP_T *p_this=NULL;
    ADE_API_RET_T iir_ret=ADE_DEFAULT_RET;
    ADE_API_RET_T blas2_ret_tgk1=ADE_DEFAULT_RET;
    ADE_API_RET_T blas2_ret_tgk2=ADE_DEFAULT_RET;
    ADE_API_RET_T blas1_ret_threshold=ADE_DEFAULT_RET;
    ADE_API_RET_T blas1_ret_pow=ADE_DEFAULT_RET;
    ADE_API_RET_T blas1_ret_specw=ADE_DEFAULT_RET;
    ADE_API_RET_T blas1_ret_specb=ADE_DEFAULT_RET;
    ADE_UINT32_T iir_n_sos=1;
    ADE_UINT32_T i=0;
    ADE_UINT32_T out_buff_fft_len=0;
    #ifdef ADE_CONFIGURATION_INTERACTIVE
     char *p_matpath = ADE_MATLAB_EXE ;
	 char *p_scriptpath=ADE_SNAP_SCRIPT;
	 char *p_matfilepath=ADE_SNAP_WS;
	 ADE_API_RET_T mat_ret=0;
	// ADE_MATLAB_T* p_mat=NULL;
     Engine *p_mateng=NULL;
	#endif

    p_this=calloc(1,sizeof(ADE_SNAP_T));

    if (p_this!=NULL)
    {
        p_this->buff_len=buff_len;
        p_this->Fs=Fs_i;
        p_this->frame_time_len=(buff_len-1)*Fs_i;
        p_this->n_pow_est_slots=n_pow_slots_i;
        p_this->n_max_indexes=n_max_indexes_i;
        p_this->time_left=time_left_i;
        p_this->time_right=time_right_i;
        p_this->extract_len=fft_len_i;//ceil((time_left_i+time_right_i)*Fs_i);
        p_this->fft_len=fft_len_i;
        if (fft_len_i<p_this->extract_len)
        {
             ADE_PRINT_ERRORS(ADE_INCHECKS,fft_len_i,"%d",ADE_Snap_Init);
             fprintf(stderr,"FFT length too short\n");
             return ADE_E44;
        }

         /******** MATLAB ALLOC ********/

            #ifdef ADE_CONFIGURATION_INTERACTIVE

            mat_ret = ADE_Matlab_Init(&(p_this->p_mat),p_mateng,p_scriptpath, p_matfilepath,p_matpath);

            if (mat_ret<0)
            {
                ADE_PRINT_ERRORS(ADE_RETCHECKS,mat_ret,"%d",ADE_Snap_Init);
                return ADE_E25;
            }

            #endif

        /****** ALLOC OUT BUFF TGK ******/
        p_this->p_tgk=calloc(buff_len,sizeof(ADE_FLOATING_T));
        if(p_this->p_tgk==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_tgk,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        p_this->p_indexes=calloc( p_this->n_max_indexes,sizeof(ADE_UINT32_T));
        if(p_this->p_indexes==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_indexes,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        p_this->p_index_vals=calloc( p_this->n_max_indexes,sizeof(ADE_FLOATING_T));
        if(p_this->p_index_vals==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_index_vals,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        p_this->p_sort_indexes=calloc( p_this->n_max_indexes,sizeof(ADE_UINT32_T));
        if(p_this->p_sort_indexes==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_sort_indexes,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        p_this->p_pow_est=calloc(buff_len,sizeof(ADE_FLOATING_T));
        if(p_this->p_pow_est==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_pow_est,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        p_this->p_thresh=calloc(buff_len,sizeof(ADE_FLOATING_T));
        if(p_this->p_thresh==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_thresh,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        /*********** FFT allocations ****************/

        /**** segment allocation *****/
        p_this->dp_segments=(ADE_FLOATING_T**)calloc(p_this->n_max_indexes,sizeof(ADE_FLOATING_T*));

        if(p_this->dp_segments==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_segments,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        for (i=0;i<p_this->n_max_indexes;i++)
        {
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
                p_this->dp_segments[i]=(ADE_FLOATING_T*)fftw_malloc(p_this->extract_len*sizeof(ADE_FLOATING_T));

                if(p_this->dp_segments[i]==NULL)
                {
                ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_segments[i],"%p",ADE_Snap_Init);
                return ADE_E44;
                }
                memset(p_this->dp_segments[i],0,p_this->extract_len*sizeof(ADE_FLOATING_T));
            #else

            p_this->dp_segments[i]=(ADE_FLOATING_T*)calloc(p_this->extract_len,sizeof(ADE_FLOATING_T));

                if(p_this->dp_segments[i]==NULL)
                {
                ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_segments[i],"%p",ADE_Snap_Init);
                return ADE_E44;
                }



            #endif
        }


        /****** spectrum allocation ********/

           p_this->dp_spectrum=(ADE_CPLX_T**)calloc(p_this->n_max_indexes,sizeof(ADE_CPLX_T*));
        if(p_this->dp_spectrum==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_spectrum,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        out_buff_fft_len=(p_this->fft_len/2+1);

        for (i=0;i<p_this->n_max_indexes;i++)
        {
        /* size (p_this->fft_len/2+1) perchè la parte simmetrica non viene calcola */
        /* To improve bisogna rendere coerente questo con il type FFT_R2C  */
            #if ( ADE_FFT_IMP==ADE_USE_FFTW )
            p_this->dp_spectrum[i]=(ADE_CPLX_T*)fftw_malloc(out_buff_fft_len*sizeof(ADE_CPLX_T));

                if(p_this->dp_spectrum[i]==NULL)
                {
                ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_spectrum[i],"%p",ADE_Snap_Init);
                return ADE_E44;
                }
            memset(p_this->dp_spectrum[i],0,out_buff_fft_len*sizeof(ADE_CPLX_T));

            #else

             p_this->dp_spectrum[i]=(ADE_CPLX_T*)calloc(out_buff_fft_len,sizeof(ADE_CPLX_T));

                if(p_this->dp_spectrum[i]==NULL)
                {
                ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_spectrum[i],"%p",ADE_Snap_Init);
                return ADE_E44;
                }


            #endif

        }

        /** percent pow allocation **/

         p_this->p_percent_pow=calloc( p_this->n_max_indexes,sizeof(ADE_FLOATING_T));
        if(p_this->p_percent_pow==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_percent_pow,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        /** percent bool allocation **/

         p_this->p_snaps=malloc( p_this->n_max_indexes*sizeof(ADE_BOOL_T));
        if(p_this->p_snaps==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_snaps,"%p",ADE_Snap_Init);
            return ADE_E44;
        }
        for (i=0;i<p_this->n_max_indexes;i++)
        {
            p_this->p_snaps[i]=ADE_FALSE;
        }

        /* FFt objects allocation */

        p_this->dp_fft=(ADE_FFT_T**)calloc(p_this->n_max_indexes,sizeof(ADE_FFT_T*));
        if(p_this->dp_fft==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_fft,"%p",ADE_Snap_Init);
            return ADE_E44;
        }


         for (i=0;i<p_this->n_max_indexes;i++)
        {

           ADE_Fft_Init(&(p_this->dp_fft[i]),p_this->fft_len);


        }





         /************** ALLOC Blas1********************/

          p_this->dp_blas_l1_pow_est = (ADE_blas_level1_T**)calloc(p_this->n_pow_est_slots,sizeof(ADE_blas_level1_T*));

        if (p_this->dp_blas_l1_pow_est==NULL)
        {
             ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_blas_l1_pow_est,"%p",ADE_Snap_Init);
             return ADE_E44;
        }

        for (i=0;i<p_this->n_pow_est_slots;i++)
        {
                blas1_ret_pow=ADE_Blas_level1_Init(&(p_this->dp_blas_l1_pow_est[i]),ADE_REAL);

                if (blas1_ret_pow<0)
                {
                    ADE_PRINT_ERRORS(ADE_MEM,blas1_ret_pow,"%d",ADE_Snap_Init);
                    return ADE_E44;
                }
        }

        p_this->p_dot_vals=calloc(p_this->n_pow_est_slots,sizeof(ADE_FLOATING_T));

        if (p_this->p_dot_vals==NULL)
        {
             ADE_PRINT_ERRORS(ADE_MEM,p_this->p_dot_vals,"%p",ADE_Snap_Init);
             return ADE_E44;
        }

        blas1_ret_threshold=ADE_Blas_level1_Init(&(p_this->p_blas_l1_threshold),ADE_REAL);
            if (blas1_ret_threshold<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,blas1_ret_threshold,"%d",ADE_Snap_Init);
                return ADE_E44;
            }

            /* whole spectrum allocation */
            p_this->dp_blas_l1_pow_spect_whole = (ADE_blas_level1_T**)calloc(p_this->n_max_indexes,sizeof(ADE_blas_level1_T*));

        if (p_this->dp_blas_l1_pow_spect_whole==NULL)
        {
             ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_blas_l1_pow_spect_whole,"%p",ADE_Snap_Init);
             return ADE_E44;
        }

        for (i=0;i<p_this->n_max_indexes;i++)
        {
                blas1_ret_specw=ADE_Blas_level1_Init(&(p_this->dp_blas_l1_pow_spect_whole[i]),ADE_CPLX);

                if (blas1_ret_pow<0)
                {
                    ADE_PRINT_ERRORS(ADE_MEM,blas1_ret_specw,"%d",ADE_Snap_Init);
                    return ADE_E44;
                }
        }


            /*spectrum band allocation */

             p_this->dp_blas_l1_pow_spect_band = (ADE_blas_level1_T**)calloc(p_this->n_max_indexes,sizeof(ADE_blas_level1_T*));

        if (p_this->dp_blas_l1_pow_spect_band==NULL)
        {
             ADE_PRINT_ERRORS(ADE_MEM,p_this->dp_blas_l1_pow_spect_band,"%p",ADE_Snap_Init);
             return ADE_E44;
        }

        for (i=0;i<p_this->n_max_indexes;i++)
        {
                blas1_ret_specb=ADE_Blas_level1_Init(&(p_this->dp_blas_l1_pow_spect_band[i]),ADE_CPLX);

                if (blas1_ret_pow<0)
                {
                    ADE_PRINT_ERRORS(ADE_MEM,blas1_ret_specb,"%d",ADE_Snap_Init);
                    return ADE_E44;
                }
        }

        /************** ALLOC Blas2********************/

//        p_this->p_blas_l2_threshold = (ADE_blas_level2_T**)calloc(n_thresh_slots_i,sizeof(ADE_blas_level2_T*));
//
//        if (p_this->p_blas_l2_threshold==NULL)
//        {
//             ADE_PRINT_ERRORS(ADE_MEM,p_this->p_blas_l2_threshold,"%p",ADE_Snap_Init);
//             return ADE_E44;
//        }
//
//        for (i=0;i<n_thresh_slots_i;i++)
//        {

       // }

        blas2_ret_tgk1=ADE_Blas_level2_Init(&(p_this->p_blas_l2_tgk1),ADE_REAL);
        if (blas2_ret_tgk1<0)
        {
            ADE_PRINT_ERRORS(ADE_RETCHECKS,blas2_ret_tgk1,"%d",ADE_Snap_Init);
            return ADE_E45;
        }
         blas2_ret_tgk2=ADE_Blas_level2_Init(&(p_this->p_blas_l2_tgk2),ADE_REAL);
        if (blas2_ret_tgk2<0)
        {
            ADE_PRINT_ERRORS(ADE_RETCHECKS,blas2_ret_tgk2,"%d",ADE_Snap_Init);
            return ADE_E45;
        }


        /***************** ALLOC CC IIR *********************/
        iir_ret=ADE_Iir_Init(&(p_this->p_iir),iir_n_sos,buff_len,ADE_IIR_TRASP_II_B);
        if (iir_ret<0)
        {
            ADE_PRINT_ERRORS(ADE_RETCHECKS,iir_ret,"%d",ADE_Snap_Init);
            return ADE_E45;
        }



        *p_snap=p_this;

    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Snap_Init);
        return ADE_E44;
    }

return ADE_DEFAULT_RET;

}

ADE_VOID_T ADE_Snap_Release(ADE_SNAP_T *p_snap)
{
    ADE_UINT32_T i=0;


    ADE_Iir_Release(p_snap->p_iir);

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
        fftw_free(p_snap->dp_segments[i]);
     #else
        ADE_CHECKNFREE(p_snap->dp_segments[i]);
     #endif
    }
    ADE_CHECKNFREE(p_snap->dp_segments);
    /* spectrum release */
    for (i=0;i<p_snap->n_max_indexes;i++)
    {
     #if ( ADE_FFT_IMP==ADE_USE_FFTW )
        fftw_free(p_snap->dp_spectrum[i]);
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
ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap)
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
ADE_API_RET_T ret_b1=ADE_DEFAULT_RET;
ADE_API_RET_T ret_b2=ADE_DEFAULT_RET;
ADE_API_RET_T ret_fft=ADE_DEFAULT_RET;
ADE_API_RET_T ret_specw=ADE_DEFAULT_RET;
ADE_API_RET_T ret_specb=ADE_DEFAULT_RET;
ADE_FLOATING_SP_T slot_len=0, mod_res=0;
ADE_UINT32_T uslot_len=0;


 #ifdef ADE_CONFIGURATION_INTERACTIVE
 double  *p_max_array=NULL;

    freq_left=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_left");
    freq_right=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_right");
    spectral_threshold_schiocco=ADE_Matlab_GetScalar(p_snap->p_mat,"spectral_threshold_schiocco");
    thresh_gain=ADE_Matlab_GetScalar(p_snap->p_mat,"thresh_gain");
    thresh_bias=ADE_Matlab_GetScalar(p_snap->p_mat,"thresh_bias");
    attack_time=ADE_Matlab_GetScalar(p_snap->p_mat,"at");
    release_time=ADE_Matlab_GetScalar(p_snap->p_mat,"rt");
   // at=ADE_Matlab_GetScalar(p_snap->p_mat,"at");
    //rt=ADE_Matlab_GetScalar(p_snap->p_mat,"rt");
  //  freq_step=ADE_Matlab_GetScalar(p_snap->p_mat,"freq_step");
  //  sx_bin=ADE_Matlab_GetScalar(p_snap->p_mat,"sx_bin");
   // dx_bin=ADE_Matlab_GetScalar(p_snap->p_mat,"dx_bin");
   // band_len=ADE_Matlab_GetScalar(p_snap->p_mat,"band_len");
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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_max_array,"%p",ADE_Snap_Configure);
        return ADE_E28;
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
    ADE_PRINT_ERRORS(ADE_INCHECKS,freq_right,"%f",ADE_Snap_Configure);
    fprintf(stderr,"freq_right right %f greater than Fs/2 \n",freq_right);
    return ADE_E46;
}
spectral_threshold_schiocco  = 0.2;
 thresh_gain = 7;
 thresh_bias = 2e-2;
 attack_time=1e-4;
 release_time=50e-3;
samp_range_search_time = 80e-3;
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



    if (p_snap->p_in==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_snap->p_in,"%p",ADE_Snap_Configure);
        return ADE_E46;
    }

/**** Configuration fro tg ************/
/*y = x(2:L-1).^2-x(1:L-2).*x(3:L);*/

    ADE_Blas_level2_Elewise_Config(p_snap->p_blas_l2_tgk1,&((p_snap->p_in)[1]),&((p_snap->p_in)[1]) ,&((p_snap->p_tgk)[1]),1.0,0.0,(p_snap->buff_len)-2);
    ADE_Blas_level2_Elewise_Config(p_snap->p_blas_l2_tgk2,&((p_snap->p_in)[0]),&((p_snap->p_in)[2]) ,&((p_snap->p_tgk)[1]),-1.0,1.0,(p_snap->buff_len)-2);

/*** Configure Blas1 for pow estimate ********/
mod_res=fmodf ((ADE_FLOATING_SP_T)p_snap->buff_len    ,(ADE_FLOATING_SP_T) p_snap->n_pow_est_slots);

if ( mod_res== 0.0F )
{
   slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
}
else
{
    fprintf(stdout,"!!!!!!!!ERROR:Buff len divided by pow_slots is not an integer value\n");
    return ADE_E46;
}

uslot_len=(ADE_UINT32_T)slot_len;
for (b1_idx=0;b1_idx<p_snap->n_pow_est_slots;b1_idx++)
{

    ret_b1 = ADE_Blas_level1_setN(p_snap->dp_blas_l1_pow_est[b1_idx],uslot_len);
    ret_b1 =  ADE_Blas_level1_setINCX(p_snap->dp_blas_l1_pow_est[b1_idx],1);
    ret_b1 =  ADE_Blas_level1_setINCY(p_snap->dp_blas_l1_pow_est[b1_idx],1);
    ret_b1 =  ADE_Blas_level1_setX(p_snap->dp_blas_l1_pow_est[b1_idx],&(p_snap->p_in[b1_idx*uslot_len]));
    ret_b1 =  ADE_Blas_level1_setY(p_snap->dp_blas_l1_pow_est[b1_idx],&(p_snap->p_in[b1_idx*uslot_len]));

}

    /*** configure threshold blas2***/

ret_b2 = ADE_Blas_level1_setN(p_snap->p_blas_l1_threshold,p_snap->buff_len);
ret_b2 = ADE_Blas_level1_setALPHA(p_snap->p_blas_l1_threshold,&p_snap->thresh_gain);
ADE_Blas_level1_setINCX(p_snap->p_blas_l1_threshold, 1);
ADE_Blas_level1_setINCY(p_snap->p_blas_l1_threshold, 1);
ret_b2 = ADE_Blas_level1_setY(p_snap->p_blas_l1_threshold,p_snap->p_thresh);
ret_b2 = ADE_Blas_level1_setX(p_snap->p_blas_l1_threshold,p_snap->p_pow_est);

/*** fft config ****/
for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
{
    ret_fft=ADE_Fft_Configure(p_snap->dp_fft[fft_idx],ADE_FFT_R2C, ADE_FFT_FORWARD,p_snap->dp_segments[fft_idx],p_snap->dp_spectrum[fft_idx]);
}



     /*** blas1 spectrum whole config ***/

for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
{
    ret_specw = ADE_Blas_level1_setN(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],(p_snap->fft_len/2));//per essere come matlab forse meglio len/2+1
    ret_specw =  ADE_Blas_level1_setINCX(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],1);
    ret_specw =  ADE_Blas_level1_setINCY(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],1);
    /** to do cast pointer 2 flat or decide to use void in blas **/
    ret_specw =  ADE_Blas_level1_setX(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],(ADE_FLOATING_T*)(p_snap->dp_spectrum[fft_idx]));
    ret_specw =  ADE_Blas_level1_setY(p_snap->dp_blas_l1_pow_spect_whole[fft_idx],(ADE_FLOATING_T*)(p_snap->dp_spectrum[fft_idx]));

}

 /*** blas1 spectrum band config ***/

 for(fft_idx=0;fft_idx<p_snap->n_max_indexes;fft_idx++)
{
    ret_specb = ADE_Blas_level1_setN(p_snap->dp_blas_l1_pow_spect_band[fft_idx],band_len);
    ret_specb =  ADE_Blas_level1_setINCX(p_snap->dp_blas_l1_pow_spect_band[fft_idx],1);
    ret_specb =  ADE_Blas_level1_setINCY(p_snap->dp_blas_l1_pow_spect_band[fft_idx],1);
    ret_specb =  ADE_Blas_level1_setX(p_snap->dp_blas_l1_pow_spect_band[fft_idx],(ADE_FLOATING_T*)(p_snap->dp_spectrum[fft_idx]+sx_bin));
    ret_specb =  ADE_Blas_level1_setY(p_snap->dp_blas_l1_pow_spect_band[fft_idx],(ADE_FLOATING_T*)(p_snap->dp_spectrum[fft_idx]+sx_bin));

}


 return ADE_DEFAULT_RET;


}

/******* Operative methods  ***********************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap)
{

    ADE_UINT32_T slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
    ADE_API_RET_T ret_b1=ADE_DEFAULT_RET;
    ADE_API_RET_T ret_thresh=ADE_DEFAULT_RET;
    ADE_API_RET_T ret_Xrms2=ADE_DEFAULT_RET,ret_max=ADE_DEFAULT_RET,ret_events=ADE_DEFAULT_RET,ret_recog=ADE_DEFAULT_RET;
    ADE_API_RET_T ret_detect=ADE_DEFAULT_RET;
    ADE_UINT32_T b1_idx=0;

    /*  estimate = pow_est(actual_frame); */

    for (b1_idx=0;b1_idx<p_snap->n_pow_est_slots;b1_idx++)
    {
         p_snap->p_dot_vals[b1_idx]=ADE_Blas_level1_dot(p_snap->dp_blas_l1_pow_est[b1_idx]);
         ADE_Utils_memset_float(&(p_snap->p_pow_est[b1_idx*slot_len]),slot_len,p_snap->p_dot_vals[b1_idx]/slot_len);
    }


    /* thres =thresh_gain.*estimate+thresh_bias; */
    ADE_Utils_memset_float(p_snap->p_thresh,p_snap->buff_len,p_snap->thresh_bias);
    ret_b1= ADE_Blas_level1_axpy(p_snap->p_blas_l1_threshold);

    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_b1<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_b1,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif

    /*  detect_energy_threshold(actual_frame,thres); */

    ret_thresh =  ADE_Snap_ThresholdDetection(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_thresh<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_thresh,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif
    ret_Xrms2 = ADE_Snap_Xrms2(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_Xrms2<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_Xrms2,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif
    ret_max =  ADE_Snap_find_local_max(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_max<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_max,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif
    ret_events = ADE_Snap_extract_events(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_events<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_events,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif
    ret_recog = ADE_Snap_snap_recognition(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_recog<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_recog,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif

    ret_detect=ADE_Snap_snap_detector(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
    if (ret_detect<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_detect,"%d",ADE_Snap_Step)
        return ADE_E45;
    }
    #endif



    return ADE_DEFAULT_RET;
}


ADE_API_RET_T ADE_Snap_SetInbuff(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff)
{

    if (p_buff==NULL)
    {
         ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Snap_SetInbuff);
            return ADE_E46;
    }

    if (p_snap==NULL)

    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_snap,"%p",ADE_Snap_SetInbuff);
            return ADE_E46;
    }

    p_snap->p_in=p_buff;

    return ADE_DEFAULT_RET;

}
/************************* private methods ***********************/
static ADE_API_RET_T ADE_Snap_TeagerKaiser(ADE_SNAP_T *p_snap)
{

    ADE_API_RET_T ret=ADE_DEFAULT_RET;
    ADE_UINT32_T last_idx=p_snap->buff_len-1;

    ret=ADE_Blas_level2_Elewise(p_snap->p_blas_l2_tgk1);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Snap_TeagerKaiser);
        return ADE_E45;
    }

    #elif (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_FALSE)

    #else
            #error (ADE_CHECK_RETURNS)
    #endif

    ret=ADE_Blas_level2_Elewise(p_snap->p_blas_l2_tgk2);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Snap_TeagerKaiser);
        return ADE_E45;
    }

    #elif (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_FALSE)

    #else
            #error (ADE_CHECK_RETURNS)
    #endif
    /* y = [y(1); y; y(L-2)]; % so that lenght(y)=length(x)*/

    (p_snap->p_tgk)[0]=(p_snap->p_tgk)[1];
    (p_snap->p_tgk)[last_idx]=(p_snap->p_tgk)[last_idx-1];

    return ADE_DEFAULT_RET;

}


static ADE_API_RET_T ADE_Snap_ThresholdDetection(ADE_SNAP_T *p_snap)
{
    ADE_API_RET_T ret=ADE_DEFAULT_RET;
    ADE_UINT32_T n_indexes=0;

    ret=ADE_Snap_TeagerKaiser(p_snap);
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Snap_ThresholdDetection);
        return ADE_E45;
    }

    #elif (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_FALSE)

    #else
            #error (ADE_CHECK_RETURNS)
    #endif
    /* indexes=find(out_tg>thresh); */
    //ret=ADE_Utils_FindIndexes(p_snap->p_tgk,p_snap->buff_len, p_snap->p_indexes,&n_indexes, p_snap->p_thresh,ADE_UTILS_MAJOR);


    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Snap_Xrms2(ADE_SNAP_T *p_snap)
{

    /****elaboration In-place on tgk buffer****/
    ADE_FLOATING_T local_peak=fabs(p_snap->p_tgk[0]);
    ADE_UINT32_T len=p_snap->buff_len;
    ADE_UINT32_T i=0;
    ADE_FLOATING_T a=0,coeff=0;
    ADE_FLOATING_T out0_temp=0;

out0_temp=local_peak;

for (i=1;i<len;i++)
{
    a=fabs(p_snap->p_tgk[i]);

    if (a > local_peak)
    {
        coeff = p_snap->at;
    }
    else
    {
        coeff = p_snap->rt;
    }

    local_peak = (1-coeff)*local_peak+coeff*a;

   p_snap->p_tgk[i]=local_peak;

}

p_snap->p_tgk[0]=out0_temp;

return ADE_DEFAULT_RET;


}

static ADE_API_RET_T ADE_Snap_find_local_max(ADE_SNAP_T *p_snap)
{
   ADE_UINT32_T len=p_snap->buff_len;
   ADE_UINT32_T k=0,i=0,j=0,last_idx=0,idx_l=0,idx_r=0;
   ADE_UINT32_T look_ahead_step=p_snap->look_ahead_step;
   ADE_UINT32_T samples_range=p_snap->samp_range_search;
   ADE_FLOATING_T *p_min_thresh=p_snap->p_thresh;
   ADE_FLOATING_T *p_data=p_snap->p_tgk;
   ADE_FLOATING_T min_data_sensed=1e-10;
   ADE_BOOL_T a1=ADE_FALSE;//,
   ADE_BOOL_T a2=ADE_FALSE;
   ADE_BOOL_T skip=ADE_FALSE;
   ADE_UINT32_T *p_indexes=p_snap->p_indexes;
   ADE_FLOATING_T *p_index_vals=p_snap->p_index_vals;
   ADE_UINT32_T search_step=p_snap->search_step;
   ADE_ULONG_T *p_sort_idx=p_snap->p_sort_indexes;
   ADE_UINT32_T index_limit=0;


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
p_snap->n_found_indexes=k;

//[vals,sort_idx]=sort(vals,'descend');
index_limit=p_snap->n_found_indexes;
/* Max indexes check */
if (p_snap->n_found_indexes > p_snap->n_max_indexes)
{
    index_limit=p_snap->n_max_indexes;
}
if (index_limit>0)
{
/** To substitute with descending **/
ADE_Utils_indexx_descend(index_limit,p_index_vals-1,p_sort_idx-1);
}

for(i=0;i<index_limit;i++)
{
    j=p_sort_idx[i]-1;
    p_indexes[i]=p_indexes[j];
}
//indexes=indexes(sort_idx);

p_snap->n_found_indexes=index_limit;

return ADE_DEFAULT_RET;

}


static ADE_API_RET_T ADE_Snap_extract_events(ADE_SNAP_T *p_snap)
{

ADE_UINT32_T n_indx=p_snap->n_found_indexes;
ADE_UINT32_T extracted_allocated_len=p_snap->extract_len;
ADE_UINT32_T *p_main_idx=p_snap->p_indexes;
ADE_INT32_T sample_left=0,sample_right=0;
ADE_FLOATING_T Fs=p_snap->Fs;
ADE_FLOATING_T time_left=p_snap->time_left;
ADE_FLOATING_T time_right=p_snap->time_right;
ADE_UINT32_T frame_len=p_snap->buff_len;
ADE_UINT32_T actual_calc_len=0;
ADE_FLOATING_T **dp_segments=p_snap->dp_segments;
ADE_FLOATING_T *p_in=p_snap->p_in;
ADE_UINT32_T i=0,j=0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,actual_calc_len,"%d",ADE_Snap_Configure);
        fprintf(stderr,"sample right and left longer than expected in  ADE_Snap_extract_events %d vs max allowed %d \n",actual_calc_len,extracted_allocated_len);
        return ADE_E46;
    }

    ADE_Utils_memset_float(dp_segments[i],extracted_allocated_len,0);
    //memset(dp_segments[i],0,extracted_allocated_len*sizeof(ADE_FLOATING_T));
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    ADE_Utils_memcpy_float(dp_segments[i],&(p_in[sample_left]),actual_calc_len);
   // memcpy(dp_segments[i],&(p_in[sample_left]),actual_calc_len*sizeof(ADE_FLOATING_T));
    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
    for (j=0;j<actual_calc_len;j++)
    {
     ADE_Fft_FillSplitIn(p_snap->p_fft[i],p_in[p_main_idx[i]],0,j);
     }
     #else
        #error (ADE_FFT_IMP)
    #endif
}

return ADE_DEFAULT_RET;

}
static ADE_API_RET_T ADE_Snap_snap_recognition(ADE_SNAP_T *p_snap)

{

ADE_UINT32_T n_events=p_snap->n_found_indexes;
ADE_UINT32_T i=0;
ADE_FLOATING_T whole_pow_spec=0,sel_pow_spec=0;
ADE_CPLX_T temp_cplx;

  for (i=0;i<n_events;i++)
  {

        //fff = fft(events(i,1:len_fft));
        ADE_Fft_Step(p_snap->dp_fft[i]);
//        spectrum(i,:) =real(fff).^2+imag(fff).^2;
//        spectrum(i,1:3) = 0;
(p_snap->dp_spectrum[i])[0]=ADE_cset(0,0);
(p_snap->dp_spectrum[i])[1]=ADE_cset(0,0);
(p_snap->dp_spectrum[i])[2]=ADE_cset(0,0);
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

    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Snap_snap_detector(ADE_SNAP_T *p_snap)
{

ADE_UINT32_T n_events=p_snap->n_found_indexes;
ADE_FLOATING_T percent_pow=0;
ADE_UINT32_T i=0;

for (i=0;i<n_events;i++)
  {

    percent_pow=p_snap->p_percent_pow[i];

    if (percent_pow>=p_snap->spectral_threshold_schiocco)
    {

        p_snap->p_snaps[i]=ADE_TRUE;

    }
    else
    {
        p_snap->p_snaps[i]=ADE_FALSE;

    }


  }

 return ADE_DEFAULT_RET;
}
