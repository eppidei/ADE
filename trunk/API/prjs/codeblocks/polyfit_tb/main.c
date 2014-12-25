#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_polyfit.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#define MAXCHAR (256)
#define MAXVAR (32)

#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };


typedef struct ADE_Uut_params_S
{
    unsigned int uut_idx;
    char *input1;
    double *output1;
}ADE_Uut_params_T;

ADE_VOID_T UnitTest1_procedure(ADE_MATLAB_T *p_mat,ADE_POLYFIT_T *p_poly, ADE_Uut_params_T *p_add_params,ADE_UINT32_T frame_len_i)
{
    ADE_Polyfit_Step(p_poly,ADE_Matlab_GetDataPointer(p_mat,p_add_params->input1),p_add_params->output1,frame_len_i);
}


int main()
{
   Engine *p_ep=NULL;
	char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/polyfit/";
	char *p_script_name="polyfit_test.m";
	char *var_list[]={"breaks","coeffs","test_inp"};
	unsigned int n_var_list = 3;
	ADE_MATLAB_T *p_mat;
	ADE_Uut_params_T p_add_params;


	char filename[MAXCHAR];


	 ADE_POLYFIT_T *p_poly;
	 //double **num ,**denom;
	 double *outbuff;
	  unsigned int order=0,input_len=0,input_len2=0,break_len=0;


	memset(filename,'\0',sizeof(filename));
//
	strcpy(filename,p_scriptpath);
	strcat(filename,p_script_name);

/****** INITIALIZE MATLAB STRUCT AND LAUNCH CONFIGURATION SEGMENT ************/

ADE_Matlab_Init(&p_mat,var_list,n_var_list, p_ep,filename,p_matpath);

/****** GET MAT VARIABLES *********/


input_len = ADE_Matlab_GetLength(p_mat,"test_inp");
break_len = ADE_Matlab_GetLength(p_mat,"breaks");
order = ADE_Matlab_GetNCols(p_mat,"coeffs")-1;


outbuff=calloc(input_len,sizeof(double));
//outbuff2=calloc(input_len2,sizeof(double));
/********** CONFIGURE Poly **************/

ADE_Polyfit_Init (&p_poly,order,break_len);
ADE_Polyfit_SetBreaks(p_poly,ADE_Matlab_GetDataPointer(p_mat,"breaks"));
ADE_Polyfit_SetCoeffs(p_poly,ADE_Matlab_GetDataPointer(p_mat,"coeffs"));
/************ LAUNCH MATLAB AND C UNIT TEST 1*************/
ADE_Matlab_launch_script_segment(p_mat,"Unit Test 1");
p_add_params.uut_idx=1;
p_add_params.input1=&(var_list[2][0]);
p_add_params.output1=outbuff;
UnitTest1_procedure(p_mat,p_poly, &p_add_params,input_len);

/******* CHECK RESULT UNIT TEST 1*************************/
ADE_Matlab_PutVarintoWorkspace(p_mat, outbuff, "outt", 1, input_len, ADE_REAL);
ADE_Matlab_Evaluate_String(p_mat, "figure;plot(out,'or');hold on;");
ADE_Matlab_Evaluate_StringnWait(p_mat, "plot(outt,'b+');hold off;");


/********** RELEASE MEM****************/

ADE_Matlab_Release(p_mat);
free(outbuff);



return EXIT_SUCCESS;
}
