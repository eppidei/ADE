#include "headers/ADE_Matlab.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_errors.h"
#include "matrix.h"
#include <string.h>
#include <stdio.h>

static ADE_VOID_T ADE_Matlab_Mat2C_copy(double *p_dst, double *p_src, unsigned int n_rows, unsigned int n_cols);
static ADE_VOID_T ADE_Matlab_C2Mat_copy(double *p_dst, double *p_src, unsigned int n_rows, unsigned int n_cols);


ADE_API_RET_T ADE_Matlab_Init(ADE_MATLAB_T** dp_this, char ** dp_var_list, ADE_UINT32_T n_vars, Engine *p_mateng,char* filename, char *p_matpath)
{
    ADE_MATLAB_T* p_this=NULL;
    ADE_UINT32_T i=0;
    ADE_UINT32_T n_chars = 0;


    mxArray *p_mxarr=NULL;
    FILE *script_fid=NULL;



     if (!(p_mateng = engOpen(p_matpath))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		ADE_PRINT_ERRORS(ADE_RETCHECKS,p_mateng,"%p",ADE_Matlab_Init);
	    return ADE_E27;
	}


    p_this=calloc(1,sizeof(ADE_MATLAB_T));



    if (p_this!=NULL)
    {

         script_fid=fopen(filename,"r");

            if (script_fid==NULL)
            {

                ADE_PRINT_ERRORS(ADE_RETCHECKS,script_fid,"%p",ADE_Matlab_Init);
                return ADE_E27;
            }



        p_this->p_matscript=script_fid;
        p_this->p_eng=p_mateng;

        ADE_Matlab_launch_script_segment(p_this, "Configuration");

         p_this->n_vars=n_vars;


         /****************ALLOC VAR NAMES************/

         p_this->dp_var_list=(char**)calloc(n_vars,sizeof(char*));

         for (i=0;i<n_vars;i++)
         {
             n_chars=strlen(dp_var_list[i])+1;
             p_this->dp_var_list[i]=calloc(n_chars,sizeof(char));
             strcpy(p_this->dp_var_list[i],dp_var_list[i]);
         }

         /****************ALLOC SIZES *****************************/
         p_this->n_row      =calloc(n_vars,sizeof(unsigned int));
         p_this->n_col      =calloc(n_vars,sizeof(unsigned int));
         p_this->data_size  =calloc(n_vars,sizeof(size_t));

         for (i=0;i<n_vars;i++)
         {

          p_mxarr=engGetVariable(p_mateng, p_this->dp_var_list[i]);
           if (p_mxarr==NULL)
             {
                 ADE_PRINT_ERRORS(ADE_MEM,p_mxarr,"%p",ADE_Matlab_Init);
                 return  ADE_E26;
             }
            p_this->n_row[i]=mxGetM(p_mxarr);
          p_this->n_col[i]=mxGetN(p_mxarr);
            if (!strcmp(mxGetClassName(p_mxarr),"double"))
            {
                 p_this->data_size[i]=p_this->n_row[i]*p_this->n_col[i]*8;
            }
            else
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,mxGetClassName(p_mxarr),"%s",ADE_Matlab_Init);
                return ADE_E28;
            }
         }


         /****************ALLOC VAR VALUES************/


         p_this->dp_vardouble=(double**)calloc(n_vars,sizeof(double*));

         for (i=0;i<n_vars;i++)
         {
             p_mxarr=engGetVariable(p_mateng,p_this->dp_var_list[i]);

             if (p_mxarr==NULL)
             {
                 ADE_PRINT_ERRORS(ADE_MEM,p_mxarr,"%p",ADE_Matlab_Init);
                 return  ADE_E26;
             }

             p_this->dp_vardouble[i]=calloc(1,p_this->data_size[i]);
             //memcpy(p_this->dp_vardouble[i],(double*)mxGetData(p_mxarr),p_this->data_size[i]);
             ADE_Matlab_Mat2C_copy(p_this->dp_vardouble[i], (double*)mxGetData(p_mxarr), p_this->n_row[i], p_this->n_col[i]);
         }

        mxDestroyArray(p_mxarr);
        *dp_this=p_this;
    }
    else
    {

        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Matlab_Init);
        return  ADE_E26;

    }


 return ADE_DEFAULT_RET;
}

ADE_VOID_T ADE_Matlab_Release(ADE_MATLAB_T* p_mat)
{

    ADE_UINT32_T i = 0;

    for (i=0;i<p_mat->n_vars;i++)
    {
        ADE_CHECKNFREE(p_mat->dp_var_list[i]);
        ADE_CHECKNFREE(p_mat->dp_vardouble[i]);

    }

    ADE_CHECKNFREE(p_mat->dp_vardouble);
    ADE_CHECKNFREE(p_mat->dp_var_list);


    ADE_CHECKNFREE(p_mat->n_row);
    ADE_CHECKNFREE(p_mat->n_col);
    ADE_CHECKNFREE(p_mat->data_size);
    engClose(p_mat->p_eng);
    fclose(p_mat->p_matscript);



}

ADE_UINT32_T ADE_Matlab_GetVarIndex(ADE_MATLAB_T* p_mat, char *varname)
{

    ADE_UINT32_T i=0;

    for (i=0;i<p_mat->n_vars;i++)
    {
        if (!strcmp(varname,p_mat->dp_var_list[i]))
        {
            return i;
        }

    }


    return ADE_E29;
}

ADE_UINT32_T ADE_Matlab_GetNRows(ADE_MATLAB_T* p_mat, char *varname)
{

    return p_mat->n_row[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

ADE_UINT32_T ADE_Matlab_GetNCols(ADE_MATLAB_T* p_mat, char *varname)
{

    return p_mat->n_col[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

ADE_UINT32_T ADE_Matlab_GetSize(ADE_MATLAB_T* p_mat, char *varname)
{

    return p_mat->data_size[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

ADE_UINT32_T ADE_Matlab_GetNElements(ADE_MATLAB_T* p_mat, char *varname)
{

    return (p_mat->n_col[ADE_Matlab_GetVarIndex(p_mat,varname)])*(p_mat->n_row[ADE_Matlab_GetVarIndex(p_mat,varname)]);
}

double* ADE_Matlab_GetDataPointer(ADE_MATLAB_T* p_mat, char *varname)
{

    return p_mat->dp_vardouble[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

double ADE_Matlab_GetScalar(ADE_MATLAB_T* p_mat, char *varname)
{

    return *(p_mat->dp_vardouble[ADE_Matlab_GetVarIndex(p_mat,varname)]);
}

ADE_API_RET_T ADE_Matlab_PutVarintoWorkspace(ADE_MATLAB_T* p_mat, double *p_var, char *var_matname, ADE_UINT32_T var_rows, ADE_UINT32_T var_cols, ADE_UINT32_T comp_type)
{

    mxArray *p_tmp=NULL;
    int ret_engPutVariable=0;

    if (comp_type==ADE_REAL)
    {
        p_tmp=mxCreateDoubleMatrix(var_rows, var_cols, mxREAL);
    }
    else if (comp_type==ADE_CPLX)
    {
         p_tmp=mxCreateDoubleMatrix(var_rows, var_cols, mxCOMPLEX);
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,comp_type,"%u",ADE_Matlab_PutVarintoWorkspace);
        return ADE_E28;

    }

    //memcpy((void *)mxGetPr(p_tmp), (void *)p_var, var_rows*var_cols*sizeof(double));
    ADE_Matlab_C2Mat_copy((void *)mxGetPr(p_tmp), p_var, var_rows, var_cols);

    ret_engPutVariable=engPutVariable(p_mat->p_eng,var_matname, p_tmp);

    if (ret_engPutVariable==1)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_engPutVariable,"%d",ADE_Matlab_Evaluate_String);
        return ADE_E27;
    }

    mxDestroyArray(p_tmp);



     return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Matlab_Evaluate_String(ADE_MATLAB_T* p_mat, char *matcode)
{

    int ret_engEvalString = 0;

    ret_engEvalString = engEvalString(p_mat->p_eng, matcode);

    if (ret_engEvalString==1)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_engEvalString,"%d",ADE_Matlab_Evaluate_String);
        return ADE_E27;
    }

    return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Matlab_Evaluate_StringnWait(ADE_MATLAB_T* p_mat, char *matcode)
{

    ADE_API_RET_T ret_Matlab_Evaluate_String = ADE_DEFAULT_RET;

    ret_Matlab_Evaluate_String=ADE_Matlab_Evaluate_String(p_mat,matcode);

    if (ret_Matlab_Evaluate_String<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret_Matlab_Evaluate_String,"%d",ADE_Matlab_Evaluate_StringnWait);
        return ADE_E27;
    }

    ADE_MAT_WAIT;

    return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Matlab_Configure_Iir_sos(ADE_MATLAB_T* p_mat,ADE_IIR_T *p_iir, char *sosmatrix_varname, char *scalevalues_varname)
{

    double **dp_num=NULL ,**dp_denom=NULL;
    ADE_UINT32_T n_sos_sections=0,section_n_apb_coeffs=0,i=0;
    double *p_gains=NULL;

    n_sos_sections=ADE_Matlab_GetNRows(p_mat,sosmatrix_varname);

    dp_num=(double**)calloc(n_sos_sections,sizeof(double*));
    dp_denom=(double**)calloc(n_sos_sections,sizeof(double*));

    section_n_apb_coeffs=ADE_Matlab_GetNCols(p_mat,sosmatrix_varname);

    for (i=0;i<n_sos_sections;i++)
    {
        dp_num[i]=ADE_Matlab_GetDataPointer(p_mat,sosmatrix_varname)+(i*section_n_apb_coeffs);
        dp_denom[i]=ADE_Matlab_GetDataPointer(p_mat,sosmatrix_varname)+3+(i*section_n_apb_coeffs);
    }

    p_gains = ADE_Matlab_GetDataPointer(p_mat,scalevalues_varname);

    ADE_Iir_setGains( p_iir, (ADE_FLOATING_T*)p_gains);
    ADE_Iir_setNums( p_iir,  (ADE_FLOATING_T**)dp_num);
    ADE_Iir_setDenoms( p_iir, (ADE_FLOATING_T**)dp_denom);

    ADE_CHECKNFREE(dp_num);
    ADE_CHECKNFREE(dp_denom);

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Matlab_launch_script_segment(ADE_MATLAB_T *p_mat, char *p_stopword)
{
    char temp_str[ADE_MAX_CHARS];
    char *p_prefix="%end ";
    char test_str[ADE_MAX_CHARS];


    memset(temp_str,'\0',sizeof(temp_str));
    memset(test_str,'\0',sizeof(test_str));
    strcpy(test_str,p_prefix);
    strcat(test_str,p_stopword);
    strcat(test_str,"\n");

    while( strcmp(temp_str,test_str) && (feof(p_mat->p_matscript)==0) )
    {
        memset(temp_str,'\0',sizeof(temp_str));
        fgets(temp_str,ADE_MAX_CHARS,p_mat->p_matscript);
        engEvalString(p_mat->p_eng,temp_str);

    }

    if (feof(p_mat->p_matscript))
    {
        fprintf(stderr,"REACHED EOF of SCRIPT\n");
    }

     return ADE_DEFAULT_RET;
}

static ADE_VOID_T ADE_Matlab_Mat2C_copy(double *p_dst, double *p_src, unsigned int n_rows, unsigned int n_cols)
{
    unsigned int i=0,k=0,idx_dst=0,idx_src=0;

    for (i=0;i<n_rows;i++)
    {
        for (k=0;k<n_cols;k++)
        {
            idx_src=i+k*n_rows;
            idx_dst=k+i*n_cols;
            p_dst[idx_dst]=p_src[idx_src];
        }
    }

}

static ADE_VOID_T ADE_Matlab_C2Mat_copy(double *p_dst, double *p_src, unsigned int n_rows, unsigned int n_cols)
{
    unsigned int i=0,k=0,idx_dst=0,idx_src=0;

    for (i=0;i<n_rows;i++)
    {
        for (k=0;k<n_cols;k++)
        {
            idx_dst=i+k*n_rows;
            idx_src=k+i*n_cols;
            p_dst[idx_dst]=p_src[idx_src];
        }
    }

}



