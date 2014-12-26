#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#define MAXCHAR (256)
#define MAXVAR (32)

#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };


typedef struct ADE_Uut_params_S
{
    unsigned int uut_idx;
    char *input1;
    char *input2;
    double *output1;
}ADE_Uut_params_T;

ADE_VOID_T UnitTest1_procedure(ADE_MATLAB_T *p_mat, ADE_FIR_T *p_fir, ADE_Uut_params_T *params)
{
    ADE_Fir_setInbuff( p_fir,  (ADE_FLOATING_T*)ADE_Matlab_GetDataPointer(p_mat,params->input1));
    ADE_Fir_setOutbuff( p_fir,  (ADE_FLOATING_T*)(params->output1));
    ADE_Fir_Step(p_fir);
}

ADE_VOID_T UnitTest2_procedure(ADE_MATLAB_T *p_mat, ADE_FIR_T *p_fir, ADE_Uut_params_T *params)
{
    unsigned int i=0;
    unsigned int frame_len = (unsigned int)ADE_Matlab_GetScalar(p_mat,params->input2);

    for (i=0;i<frame_len;i++)
    {
        ADE_Fir_setInbuff( p_fir,  (ADE_FLOATING_T*)ADE_Matlab_GetDataPointer(p_mat,params->input1)+i*frame_len);
        ADE_Fir_setOutbuff( p_fir,  (ADE_FLOATING_T*)(params->output1)+i*frame_len);
        ADE_Fir_Step(p_fir);
    }

}


int main()
{
   Engine *p_ep=NULL;
	char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/fir/";
	char *p_script_name="fir_test.m";
	char *p_matfile_name="fir_test_ws.mat";
	//char *var_list[]={"fir_coeffs","input_vector","input_vector2","frame_len"};
	//unsigned int n_var_list = 4;
	ADE_MATLAB_T *p_mat;
	ADE_Uut_params_T p_add_params;


	char filename[MAXCHAR];
	char matfilename[MAXCHAR];


	 ADE_FIR_T *p_fir;
	 //double **num ,**denom;
	 double *outbuff,*outbuff2;
	  unsigned int fir_order=0,input_len=0,input_len2=0,frame_len=0;


	memset(filename,'\0',sizeof(filename));
//
	strcpy(filename,p_scriptpath);
	strcat(filename,p_script_name);


memset(matfilename,'\0',sizeof(matfilename));
//
	strcpy(matfilename,"./");
	strcat(matfilename,p_matfile_name);
/****** INITIALIZE MATLAB STRUCT AND LAUNCH CONFIGURATION SEGMENT ************/

ADE_Matlab_Init(&p_mat, p_ep,filename,matfilename,p_matpath);

/****** GET MAT VARIABLES *********/

fir_order=ADE_Matlab_GetNCols(p_mat,"fir_coeffs");
input_len = ADE_Matlab_GetNCols(p_mat,"input_vector");
input_len2 = ADE_Matlab_GetNCols(p_mat,"input_vector2");
frame_len = (unsigned int)ADE_Matlab_GetScalar(p_mat,"frame_len");

outbuff=calloc(input_len,sizeof(double));
outbuff2=calloc(input_len2,sizeof(double));
/********** CONFIGURE fir **************/

ADE_Fir_Init(&p_fir, fir_order,input_len);
ADE_Fir_setNum(p_fir,ADE_Matlab_GetDataPointer(p_mat,"fir_coeffs"));
//ADE_Matlab_Configure_fir_sos(p_mat,p_fir, "sosmat", "scaleval");
ADE_Fir_setFilt_Implementation(p_fir,trasp_II_blas);

/************ LAUNCH MATLAB AND C UNIT TEST 1*************/
ADE_Matlab_launch_script_segment(p_mat,"Unit Test 1");
p_add_params.uut_idx=1;
p_add_params.input1="input_vector";
p_add_params.output1=outbuff;
UnitTest1_procedure(p_mat,p_fir,&p_add_params);

/******* CHECK RESULT UNIT TEST 1*************************/
ADE_Matlab_PutVarintoWorkspace(p_mat, outbuff, "outt", 1, input_len, ADE_REAL);
ADE_Matlab_Evaluate_String(p_mat, "figure;plot(out,'or');hold on;");
ADE_Matlab_Evaluate_StringnWait(p_mat, "plot(outt,'b+');hold off;");

/************ LAUNCH MATLAB AND C UNIT TEST 2*************/
ADE_Matlab_launch_script_segment(p_mat,"Unit Test 2");
p_add_params.uut_idx=2;
p_add_params.input1="input_vector2";
p_add_params.output1=outbuff2;
UnitTest1_procedure(p_mat,p_fir,&p_add_params);

/******* CHECK RESULT UNIT TEST 2*************************/
ADE_Matlab_PutVarintoWorkspace(p_mat, outbuff2, "outt", 1, input_len, ADE_REAL);
ADE_Matlab_Evaluate_String(p_mat, "figure;plot(out,'or');hold on;");
ADE_Matlab_Evaluate_StringnWait(p_mat, "plot(outt,'b+');hold off;");


/********** RELEASE MEM****************/

ADE_Matlab_Release(p_mat);
free(outbuff);
free(outbuff2);


return EXIT_SUCCESS;
}
