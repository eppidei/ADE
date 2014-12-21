#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#define MAXCHAR (256)
#define MAXVAR (32)

#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };


int main()
{
   Engine *p_ep=NULL;
	char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/iir/";
	char *p_script_name="iir_test.m";
	char *var_list[]={"scaleval","sosmat","input_vector"};
	ADE_MATLAB_T *p_mat;


	char filename[MAXCHAR];


	 ADE_IIR_T *p_iir;
	 //double **num ,**denom;
	 double *outbuff;
	  unsigned int n_sos_sections=0,input_len=0;





	memset(filename,'\0',sizeof(filename));
//
	strcpy(filename,p_scriptpath);
	strcat(filename,p_script_name);



ADE_Matlab_Init(&p_mat,var_list,3, p_ep,filename,p_matpath);


n_sos_sections=ADE_Matlab_GetNRows(p_mat,"sosmat");
input_len = ADE_Matlab_GetNCols(p_mat,"input_vector");



outbuff=calloc(input_len,sizeof(double));

ADE_Iir_Init(&p_iir, n_sos_sections,input_len);

ADE_Matlab_Configure_Iir_sos(p_mat,p_iir, "sosmat", "scaleval");

ADE_Iir_setInbuff( p_iir,  (ADE_FLOATING_T*)ADE_Matlab_GetDataPointer(p_mat,"input_vector"));
ADE_Iir_setOutbuff( p_iir,  (ADE_FLOATING_T*)outbuff);
ADE_Iir_setFilt_Implementation(p_iir,trasp_II);
ADE_Iir_Step(p_iir);


ADE_Matlab_PutVarintoWorkspace(p_mat, outbuff, "outt", 1, input_len, ADE_REAL);
//


ADE_Matlab_Evaluate_String(p_mat, "figure;plot(out,'or');hold on;");
ADE_Matlab_Evaluate_StringnWait(p_mat, "plot(outt,'b+');hold off;");

ADE_Matlab_Release(p_mat);


	return EXIT_SUCCESS;
}
