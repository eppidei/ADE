#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_upsampler.h"
#include "headers/ADE_downsampler.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_Error_Handler.h"
//#include "headers/ADE_typedefs.h"

//#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };



int main()
{

    ADE_FLOATING_T a[11]={1,2,3,4,5,6,7,8,9,10,11};
     ADE_FLOATING_T c[11]={12,13,14,15,16,17,18,19,20,21,22};
    ADE_UINT32_T up=3;
    ADE_UPSAMPLER_T *p_upsamp;
    ADE_DOWNSAMPLER_T *p_downsamp;
    ADE_FLOATING_T b[11*up];
    ADE_UINT32_T down=up;
    ADE_API_RET_T ret = ADE_RET_ERROR;

    memset(b,0,sizeof(b));

    ret = ADE_Upsampler_Init(&p_upsamp);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Upsampler_Configure(p_upsamp,11,up,a,b);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Upsampler_Step(p_upsamp);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Utils_PrintArray(b,0,32,0,0,"b",stdout,ADE_MATH_REAL);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Upsampler_Configure(p_upsamp,11,up,c,b);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Upsampler_Step(p_upsamp);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Utils_PrintArray(b,0,32,0,0,"b",stdout,ADE_MATH_REAL);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ADE_Upsampler_Release(p_upsamp);


    ret = ADE_Downsampler_Init(&p_downsamp);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Downsampler_Configure(p_downsamp,11,down,b,a);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ret = ADE_Downsampler_Step(p_downsamp);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

     ret = ADE_Utils_PrintArray(a,0,10,0,0,"a",stdout,ADE_MATH_REAL);
     ADE_CHECK_ADERETVAL(ADE_CLASS_MAIN,ADE_METHOD_Main,ret);

    ADE_Downsampler_Release(p_downsamp);



return EXIT_SUCCESS;
}
