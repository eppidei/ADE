#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_interpolator.h"
#include "headers/ADE_Matlab.h"
#define MAXCHAR (256)
#define MAXVAR (32)

#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };



int main()
{
   Engine *p_ep=NULL;
	char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Dsp/Interpolator/Matlab/Main_scripts/";
	char *p_script_name="Interpolator.m";
	char *p_matfile_name="Interpolator_ws.mat";
	//char *var_list[]={"fir_coeffs","input_vector","input_vector2","frame_len"};
	//unsigned int n_var_list = 4;
    ADE_MATLAB_T *p_mat;
    char filename[MAXCHAR];
    char matfilename[MAXCHAR];
    ADE_INTERPOLATOR_T *p_interpolator;
    ADE_UINT32_T up_fact=0;
    double *p_input,*p_output,*p_num;
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


fir_order=ADE_Matlab_GetNCols(p_mat,"b");
input_len = ADE_Matlab_GetNCols(p_mat,"a");
up_fact= ADE_Matlab_GetScalar(p_mat,"up_fact");


p_input=ADE_Matlab_GetDataPointer(p_mat,"a");
p_num=ADE_Matlab_GetDataPointer(p_mat,"b");
p_output=(double*)calloc(input_len*up_fact,sizeof(double));
/********** CONFIGURE  **************/
ADE_Interpolator_Init(&p_interpolator,input_len,up_fact,fir_order,ADE_INTERPOLATOR_FILTER_FIR,ADE_INTERPOLATOR_ARCH_STANDARD,0);
ADE_Interpolator_Configure(p_interpolator,p_input,p_output,input_len*up_fact*sizeof(double),p_num,NULL,NULL);
/*** Launch C****/
ADE_Interpolator_Step(p_interpolator);
/**** Launch Matlab ********/
ADE_Matlab_launch_script_segment(p_mat,"Interpolator");

/************ Check *************/
ADE_Matlab_PutVarintoWorkspace(p_mat, p_output, "outc", 1, input_len*up_fact, ADE_MATH_REAL);
ADE_Matlab_Evaluate_String(p_mat, "figure;plot(c,'or');hold on;");
ADE_Matlab_Evaluate_StringnWait(p_mat, "plot(outc,'b+');hold off;");
/********** RELEASE MEM****************/
ADE_Interpolator_Release(p_interpolator);
ADE_Matlab_Release(p_mat);
free(p_output);


return EXIT_SUCCESS;
}
