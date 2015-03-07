#include "headers/ADE_Snap.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_level2.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_Utils.h"
#include <math.h>

/******* Private methods prototypes ***********************/
static ADE_API_RET_T ADE_Snap_TeagerKaiser(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_ThresholdDetection(ADE_SNAP_T *p_snap);
static ADE_API_RET_T ADE_Snap_Xrms2(ADE_SNAP_T *p_snap);
/******* Init methods  ***********************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap,ADE_UINT32_T buff_len,ADE_UINT32_T Fs_i,ADE_UINT32_T n_pow_slots_i,ADE_UINT32_T n_max_indexes_i)
{
    ADE_SNAP_T *p_this=NULL;
    ADE_API_RET_T iir_ret=ADE_DEFAULT_RET;
    ADE_API_RET_T blas2_ret_tgk1=ADE_DEFAULT_RET;
    ADE_API_RET_T blas2_ret_tgk2=ADE_DEFAULT_RET;
    ADE_API_RET_T blas1_ret_threshold=ADE_DEFAULT_RET;
    ADE_API_RET_T blas1_ret_pow=ADE_DEFAULT_RET;
    ADE_UINT32_T iir_n_sos=1;
    ADE_UINT32_T i=0;

    p_this=calloc(1,sizeof(ADE_SNAP_T));

    if (p_this!=NULL)
    {
        p_this->buff_len=buff_len;
        p_this->Fs=Fs_i;
        p_this->n_pow_est_slots=n_pow_slots_i;
        p_this->n_max_indexes=n_max_indexes_i;
        /****** ALLOC OUT BUFF TGK ******/
        p_this->p_tgk=calloc(buff_len,sizeof(ADE_FLOATING_T));
        if(p_this->p_tgk==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_tgk,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

        p_this->p_indexes=calloc( p_this->n_max_indexes,sizeof(ADE_FLOATING_T));
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
        iir_ret=ADE_Iir_Init(&(p_this->p_iir),iir_n_sos,buff_len);
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
     ADE_CHECKNFREE(p_snap->p_dot_vals);
    ADE_Blas_level1_Release(p_snap->p_blas_l1_threshold);
    ADE_Blas_level2_Release(p_snap->p_blas_l2_tgk1);
    ADE_Blas_level2_Release(p_snap->p_blas_l2_tgk2);
    ADE_CHECKNFREE(p_snap->p_pow_est);
    ADE_CHECKNFREE(p_snap->p_thresh);
    ADE_CHECKNFREE(p_snap->p_tgk);
    ADE_CHECKNFREE(p_snap->p_indexes);
    ADE_CHECKNFREE(p_snap);

}
/******* Configure methods  ***********************/
ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap)
{

ADE_FLOATING_T frame_time_len = 300e-3;
ADE_UINT32_T frame_len = floor(frame_time_len*p_snap->Fs+0.5)-1;
ADE_FLOATING_T freq_left=1800;
ADE_FLOATING_T freq_right=3200;
ADE_FLOATING_T spectral_threshold_schiocco  = 0.2;
ADE_FLOATING_T thresh_gain = 7;
ADE_FLOATING_T thresh_bias = 2e-2;
ADE_FLOATING_T attack_time=1e-4;
ADE_FLOATING_T release_time=50e-3;
ADE_FLOATING_T at = 1-exp(-2.2/(p_snap->Fs*attack_time));
ADE_FLOATING_T rt = 1-exp(-2.2/(p_snap->Fs*release_time));
ADE_FLOATING_T time_left=0.5e-3;
ADE_FLOATING_T time_right=6e-3;
ADE_FLOATING_T samp_range_search_time = 80e-3;
ADE_UINT32_T samp_range_search = ceil(samp_range_search_time*p_snap->Fs)-1;
ADE_FLOATING_T max_range[2]  = {2000,3000};
ADE_UINT32_T search_step = 3;
ADE_UINT32_T look_ahead_step = 3;


ADE_UINT32_T b1_idx=0,thresh_idx=0;
ADE_API_RET_T ret_b1=ADE_DEFAULT_RET;
ADE_API_RET_T ret_b2=ADE_DEFAULT_RET;
ADE_FLOATING_SP_T slot_len=0, mod_res=0;
ADE_UINT32_T uslot_len=0;

/*** to put into set methods***/
p_snap->frame_time_len;
p_snap->buff_len=frame_len;
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

if ( mod_res!= 0.0F )
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

    /*** to do configure threshold blas2***/

ret_b2 = ADE_Blas_level1_setN(p_snap->p_blas_l1_threshold,p_snap->buff_len);
ret_b2 = ADE_Blas_level1_setALPHA(p_snap->p_blas_l1_threshold,&p_snap->thresh_gain);
ADE_Blas_level1_setINCX(p_snap->p_blas_l1_threshold, 1);
ADE_Blas_level1_setINCY(p_snap->p_blas_l1_threshold, 1);
ret_b2 = ADE_Blas_level1_setY(p_snap->p_blas_l1_threshold,p_snap->p_thresh);
ret_b2 = ADE_Blas_level1_setX(p_snap->p_blas_l1_threshold,p_snap->p_pow_est);

     return ADE_DEFAULT_RET;

}

/******* Operative methods  ***********************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap)
{

    ADE_UINT32_T slot_len=(p_snap->buff_len)/p_snap->n_pow_est_slots;
    ADE_API_RET_T ret_b1=ADE_DEFAULT_RET;
    ADE_API_RET_T ret_thresh=ADE_DEFAULT_RET;
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

    /*  detect_energy_threshold(actual_frame,thres); */

    ret_thresh =  ADE_Snap_ThresholdDetection(p_snap);




    return ADE_DEFAULT_RET;
}

/************************* private methods ***********************/
static ADE_API_RET_T ADE_Snap_PowerEstimate(ADE_SNAP_T *p_snap)
{

}

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
    ADE_FLOATING_T local_peak=abs(p_snap->p_tgk[0]);
    ADE_UINT32_T len=p_snap->buff_len;
    ADE_UINT32_T i=0;
    ADE_FLOATING_T a=0,coeff=0;

p_snap->p_tgk[0]=local_peak;

for (i=1;i<len;i++)
{
    a=abs(p_snap->p_tgk[i]);

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


}

static ADE_API_RET_T ADE_Snap_find_local_max(ADE_SNAP_T *p_snap)
{
   ADE_UINT32_T len=p_snap->buff_len;
max_indexes = 10;
indexes=zeros(1,max_indexes);
vals=zeros(1,max_indexes);
k=1;

for i = 3: search_step :samples_range

    idx_l = i-look_ahead_step:-look_ahead_step:look_ahead_step;
    idx_r = i+look_ahead_step:look_ahead_step:i+samples_range;
    a1 =  data(i)> data(idx_l);
    a2 =  data(i)> data(idx_r);
     b1 =  ones(size(idx_l)) ;
    b =  ones(size(idx_r)) ;
    if ( sum(a1-b1)==0) && (sum(a2-b)==0) && data(i)>min_thresh(i)

        indexes(k)=i;
        vals(k)=data(i);
        k=k+1;
    end

    last_idx = i;

end

for i = last_idx+search_step : search_step :len -samples_range -search_step

    idx_l = i-look_ahead_step:-look_ahead_step:i-samples_range;
    idx_r = i+look_ahead_step:look_ahead_step:i+samples_range;
    a1 =  data(i)> data(idx_l);
    a2 =  data(i)> data(idx_r);
    b =  ones(size(idx_r)) ;
    if (sum(a1-b)==0) && ( sum(a2-b)==0) && data(i)>min_thresh(i)

        indexes(k)=i;
        vals(k)=data(i);
        k=k+1;
    end
last_idx = i;
end

for i = last_idx+search_step: search_step : len

    idx_l = i-look_ahead_step:-look_ahead_step:i-samples_range;
    idx_r = i+look_ahead_step:look_ahead_step:len;
    a1 =  data(i)> data(idx_l);
    a2 =  data(i)> data(idx_r);
    b =  ones(size(idx_l)) ;
     b2 =  ones(size(idx_r)) ;


    if (sum(a1-b)==0) && ( (sum(a2-b2)==0)) && data(i)>min_thresh(i)

        indexes(k)=i;
        vals(k)=data(i);
        k=k+1;
    end

end

[vals,sort_idx]=sort(vals,'descend');
indexes=indexes(sort_idx);

n_indexes=k-1;

}


