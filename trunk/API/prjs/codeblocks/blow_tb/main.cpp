#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE_Blow.h"
#define MAXCHAR (256)
#define MAXVAR (32)

#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };

int main()
{

	ADE_BLOW_T *p_blow;
	ADE_UINT32_T Frame_len=256;
	ADE_FLOATING_T Fs_i=44100;
ADE_API_RET_T init_ret=ADE_RET_ERROR;
ADE_FLOATING_T *p_inbuff;
ADE_UINT32_T i=0,n_iterations=0;
double *p_inbuffmat=NULL;
ADE_FLOATING_T *p_global_outbuff=NULL;
ADE_FLOATING_T *p_cycle_outbuff=NULL;
	init_ret = ADE_Blow_Init(&p_blow, Frame_len, Fs_i,Fs_i);

	ADE_Blow_GetOutBuff(p_blow,&p_cycle_outbuff);

	#ifdef ADE_CONFIGURATION_INTERACTIVE

n_iterations=ADE_Matlab_GetScalar(p_blow->p_mat,"n_iterations");
p_global_outbuff=(ADE_FLOATING_T*)calloc(Frame_len*n_iterations,sizeof(ADE_FLOATING_T));

    for (i=0;i<n_iterations;i++)
    {
        p_inbuffmat=ADE_Matlab_GetDataPointer(p_blow->p_mat,"audio_left")+i*Frame_len;
        ADE_Blow_SetInBuff(p_blow,p_inbuffmat);

         ADE_Blow_Step(p_blow);
         memcpy(p_global_outbuff+i*Frame_len,p_cycle_outbuff,p_blow->buff_len_o*sizeof(ADE_FLOATING_T));
    }
    ADE_Matlab_PutVarintoWorkspace(p_blow->p_mat, p_global_outbuff, "outt", 1, Frame_len*n_iterations, ADE_REAL);
    ADE_Matlab_launch_script_segment(p_blow->p_mat,"Blow");
ADE_Matlab_Evaluate_StringnWait(p_blow->p_mat, "figure;hold on;plot(outt,'or');plot(expanded_pow_iir,'+b');hold off;");
   ADE_Blow_Release(p_blow);
   free(p_global_outbuff);


#endif


return EXIT_SUCCESS;
}
