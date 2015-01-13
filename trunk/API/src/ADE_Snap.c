#include "headers/ADE_Snap.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level2.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_defines.h"



ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap,ADE_UINT32_T buff_len)
{
    ADE_SNAP_T *p_this=NULL;
    ADE_API_RET_T iir_ret=ADE_DEFAULT_RET;
    ADE_API_RET_T blas2_ret_tgk1=ADE_DEFAULT_RET;
    ADE_API_RET_T blas2_ret_tgk2=ADE_DEFAULT_RET;
    ADE_UINT32_T iir_n_sos=1;

    p_this=calloc(1,sizeof(ADE_SNAP_T));

    if (p_this!=NULL)
    {
        p_this->buff_len=buff_len;
        /****** ALLOC OUT BUFF TGK ******/
        p_this->p_tgk=calloc(buff_len,sizeof(ADE_FLOATING_T));
        if(p_this->p_tgk==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_tgk,"%p",ADE_Snap_Init);
            return ADE_E44;
        }

//        p_this->p_tgk_temp=calloc(buff_len,sizeof(ADE_FLOATING_T));
//        if(p_this->p_tgk_temp==NULL)
//        {
//            ADE_PRINT_ERRORS(ADE_MEM,p_this>p_tgk_temp,"%p",ADE_Snap_Init);
//            return ADE_E44;
//        }

        /************** ALLOC Blas2********************/
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
    ADE_Iir_Release(p_snap->p_iir);
    ADE_Blas_level2_Release(p_snap->p_blas_l2_tgk1);
    ADE_Blas_level2_Release(p_snap->p_blas_l2_tgk2);
    ADE_CHECKNFREE(p_snap->p_tgk);
    // ADE_CHECKNFREE(p_snap->p_tgk_temp);
    ADE_CHECKNFREE(p_snap);

}

ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap)
{
    if (p_snap->p_in==NULL)
    {

    }

/*y = x(2:L-1).^2-x(1:L-2).*x(3:L);
y = [y(1); y; y(L-2)]; % so that lenght(y)=length(x)*/

    ADE_Blas_level2_Elewise_Config(p_snap->p_blas_l2_tgk1,&((p_snap->p_in)[1]),&((p_snap->p_in)[1]) ,&((p_snap->p_tgk)[1]),1.0,0.0,(p_snap->buff_len)-2);
     ADE_Blas_level2_Elewise_Config(p_snap->p_blas_l2_tgk2,&((p_snap->p_in)[0]),&((p_snap->p_in)[2]) ,&((p_snap->p_tgk)[1]),-1.0,1.0,(p_snap->buff_len)-2);

}




static ADE_Snap_TeagerKaiser(ADE_SNAP_T *p_snap)
{



}


ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap)
{


    return ADE_DEFAULT_RET;
}
