
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_fft.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_complex.h"
#include "headers/ADE_Bench_Utils.h"
#include <time.h>
#include <errno.h>



int main()
{

    FILE* p_debug_out=stdout;
    ADE_INT32_T ret=0;
    ADE_UINT32_T buff_lenghts[10]= {16,32,64,128,256,512,1024,2048,4096,8192};

    ret=fft_test_procedure(ADE_FFT_C2C,buff_lenghts,10,p_debug_out);

    return ret;
}
