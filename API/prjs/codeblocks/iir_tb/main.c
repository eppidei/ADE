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


typedef struct ADE_Uut_params_S
{
    unsigned int uut_idx;
    double *p_prm1;
}ADE_Uut_params_T;

ADE_VOID_T UnitTest1_procedure(ADE_MATLAB_T *p_mat, ADE_IIR_T *p_iir, ADE_Uut_params_T *params)
{
    ADE_Iir_setInbuff( p_iir,  (ADE_FLOATING_T*)ADE_Matlab_GetDataPointer(p_mat,"input_vector"));
    ADE_Iir_setOutbuff( p_iir,  (ADE_FLOATING_T*)(params->p_prm1));
    ADE_Iir_Step(p_iir);
}


int main()
{
   Engine *p_ep=NULL;
	char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/iir/";
	char *p_script_name="iir_test.m";
	char *var_list[]={"scaleval","sosmat","input_vector"};
	unsigned int n_var_list = 3;
	ADE_MATLAB_T *p_mat;
	ADE_Uut_params_T p_add_params;


	char filename[MAXCHAR];


	 ADE_IIR_T *p_iir;
	 //double **num ,**denom;
	 double *outbuff;
	  unsigned int n_sos_sections=0,input_len=0;


	memset(filename,'\0',sizeof(filename));
//
	strcpy(filename,p_scriptpath);
	strcat(filename,p_script_name);

/****** INITIALIZE MATLAB STRUCT AND LAUNCH CONFIGURATION SEGMENT ************/

ADE_Matlab_Init(&p_mat,var_list,n_var_list, p_ep,filename,p_matpath);

/****** GET MAT VARIABLES *********/

n_sos_sections=ADE_Matlab_GetNRows(p_mat,"sosmat");
input_len = ADE_Matlab_GetNCols(p_mat,"input_vector");

outbuff=calloc(input_len,sizeof(double));

/********** CONFIGURE IIR **************/

ADE_Iir_Init(&p_iir, n_sos_sections,input_len);
ADE_Matlab_Configure_Iir_sos(p_mat,p_iir, "sosmat", "scaleval");
ADE_Iir_setFilt_Implementation(p_iir,trasp_II);

/************ LAUNCH MATLAB AND C UNIT TEST *************/
ADE_Matlab_launch_script_segment(p_mat,"Unit Test 1");
p_add_params.uut_idx=1;
p_add_params.p_prm1=outbuff;
UnitTest1_procedure(p_mat,p_iir,&p_add_params);

/******* CHECK RESULT *************************/
ADE_Matlab_PutVarintoWorkspace(p_mat, outbuff, "outt", 1, input_len, ADE_REAL);
ADE_Matlab_Evaluate_String(p_mat, "figure;plot(out,'or');hold on;");
ADE_Matlab_Evaluate_StringnWait(p_mat, "plot(outt,'b+');hold off;");


/********** RELEASE MEM****************/

ADE_Matlab_Release(p_mat);


return EXIT_SUCCESS;
}
