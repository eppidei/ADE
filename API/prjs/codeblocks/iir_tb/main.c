#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_typedefs.h"
#define MAXCHAR (256)
#define MAXVAR (32)
#define WAIT printf("Hit return to continue\n\n");fgetc(stdin);
#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };

typedef struct var_list_S
{
    unsigned int n_vars;
    char dp_var_list[MAXVAR][MAXCHAR];
    double* dp_vardouble[MAXVAR];
    unsigned int n_row[MAXVAR];
    unsigned int n_col[MAXVAR];
    size_t data_size[MAXVAR];

}var_list_T;

int launch_matlab_script(char* filename,Engine *p_ep)
{
    FILE *script_fid=NULL;
     char temp_str[MAXCHAR];



    script_fid=fopen(filename,"r");

	if (script_fid==NULL)
	{
	    return -2;
	}

	while(feof(script_fid)==0)
	{
	    fgets(temp_str,MAXCHAR,script_fid);
	    engEvalString(p_ep,temp_str);
	    memset(temp_str,'\0',sizeof(temp_str));

	}

	fclose(script_fid);

	return 0;
}

int load_vars(var_list_T *vars,Engine *p_ep)
{
    unsigned int n_vars = vars->n_vars,i=0;
    mxArray *p_mxarr=NULL;
    char str[MAXCHAR];
    unsigned int data_size=0;

    for (i=0;i<n_vars;i++)
    {
        memset(str,'\0',sizeof(str));
        strcpy(str,&(vars->dp_var_list[i][0]));
        p_mxarr=engGetVariable(p_ep,str);
        vars->n_row[i]=mxGetM(p_mxarr);
        vars->n_col[i]=mxGetN(p_mxarr);
        if (!strcmp(mxGetClassName(p_mxarr),"double"))
        {
            vars->data_size[i]=vars->n_row[i]*vars->n_col[i]*8;
            vars->dp_vardouble[i]=malloc( vars->data_size[i]);
            memcpy(vars->dp_vardouble[i],(double*)mxGetData(p_mxarr),vars->data_size[i]);

        }

    }
    mxDestroyArray(p_mxarr);
    return 0;
}

int main()
{
   Engine *p_ep;
	char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/iir/";
	char *p_script_name="iir_test.m";
	mxArray *p_sosmatrix = NULL, *p_scalevals=NULL,*outt=NULL;
	double *p_sosdouble,*p_scaledouble;
	size_t sossize=0,scalesize=0;
	//char buffer[BUFSIZE+1];
	//double time[10] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };

	char filename[MAXCHAR];
	 var_list_T variabili_matlab;
	 unsigned int i=0,k=0;
	 ADE_IIR_T *p_iir;
	 double **num ,**denom;
	 double *inbuff,*outbuff;
	  unsigned int n_sos_sections=0,input_len=0,section_n_apb_coeffs=0;

	 memset(&variabili_matlab,0,sizeof(variabili_matlab));
	 variabili_matlab.n_vars=3;
	 strcpy(&(variabili_matlab.dp_var_list[0][0]),"scaleval");
	 strcpy(&(variabili_matlab.dp_var_list[1][0]),"sosmat");
     strcpy(&(variabili_matlab.dp_var_list[2][0]),"input_vector");



	memset(filename,'\0',sizeof(filename));

	strcpy(filename,p_scriptpath);
	strcat(filename,p_script_name);


	/*
	 * Call engOpen with a NULL string. This starts a MATLAB process
     * on the current host using the command "matlab".
	 */
	if (!(p_ep = engOpen(p_matpath))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		return EXIT_FAILURE;
	}



    launch_matlab_script(filename,p_ep);

load_vars(&variabili_matlab,p_ep);
//p_scalevals= engGetVariable(p_ep,"scaleval");
//p_sosmatrix= engGetVariable(p_ep,"sosmat");
//
//sossize = mxGetN(p_sosmatrix)*mxGetM(p_sosmatrix)*sizeof(double);
//scalesize = mxGetN(p_scalevals)*mxGetM(p_scalevals)*sizeof(double);
//
//p_scaledouble=malloc(scalesize);
//p_sosdouble=malloc(sossize);
//
//memcpy(p_scaledouble,(double*)mxGetData(p_scalevals),scalesize);
//memcpy(p_sosdouble,(double*)mxGetData(p_sosmatrix),sossize);


//PRINT_ARRAY(p_scaledouble,i,scalesize/sizeof(double),"%f");
//PRINT_ARRAY(p_sosdouble,i,sossize/sizeof(double),"%f");
//for (i=0;i<variabili_matlab.n_vars;i++)
//{
//    PRINT_ARRAY(variabili_matlab.dp_vardouble[i],k,variabili_matlab.data_size[i]/8,"%f");
//}
n_sos_sections=variabili_matlab.n_row[1];
input_len = variabili_matlab.n_col[2];
num=(double**)calloc(n_sos_sections,sizeof(double*));
denom=(double**)calloc(n_sos_sections,sizeof(double*));
//inbuff=calloc(input_len,sizeof(double));
outbuff=calloc(input_len,sizeof(double));

section_n_apb_coeffs=variabili_matlab.n_col[1];
for (i=0;i<n_sos_sections;i++)
{
    num[i]=variabili_matlab.dp_vardouble[1]+(i*section_n_apb_coeffs);
    denom[i]=variabili_matlab.dp_vardouble[1]+3+(i*section_n_apb_coeffs);
}

ADE_Iir_Init(&p_iir, n_sos_sections,input_len);
//CHECK_RET_MAIN(ret);


ADE_Iir_setGains( p_iir, (ADE_FLOATING_T*)variabili_matlab.dp_vardouble[0]);
ADE_Iir_setNums( p_iir,  (ADE_FLOATING_T**)num);
ADE_Iir_setDenoms( p_iir, (ADE_FLOATING_T**)denom);
ADE_Iir_setInbuff( p_iir,  (ADE_FLOATING_T*)variabili_matlab.dp_vardouble[2]);
ADE_Iir_setOutbuff( p_iir,  (ADE_FLOATING_T*)outbuff);
ADE_Iir_setFilt_Implementation(p_iir,trasp_II_blas);
ADE_Iir_Step(p_iir);

//PRINT_ARRAY(outbuff,i,50,"%f");

outt=mxCreateDoubleMatrix(1, input_len, mxREAL);
memcpy((void *)mxGetPr(outt), (void *)outbuff, input_len*sizeof(ADE_FLOATING_T));

engPutVariable(p_ep, "outt", outt);


engEvalString(p_ep, "figure;plot(out,'or');hold on;");
engEvalString(p_ep, "plot(outt,'b+');hold off;");
	//engEvalString(ep, "title('Position vs. Time for a falling object');");
	WAIT

//free(p_scaledouble);
//free(p_sosdouble);
//	mxDestroyArray(p_scalevals);
//	mxDestroyArray(p_sosmatrix);
free(num);
free(denom);
//free(inbuff);
free(outbuff);
engClose(p_ep);

	return EXIT_SUCCESS;
}
