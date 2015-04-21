#include "headers/ADE_Matlab.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_Utils.h"
#include "matrix.h"
#include "mat.h"
#include <string.h>
#include <stdio.h>
#include "headers/ADE_Error_Handler.h"



static ADE_VOID_T ADE_Matlab_Mat2C_copy(double *p_dst, mxArray *p_mx, unsigned int n_rows, unsigned int n_cols);
static ADE_API_RET_T ADE_Matlab_C2Mat_copy(double *p_dst, double *p_src, unsigned int n_rows, unsigned int n_cols,ADE_MATH_ATTRIBUTE_T comp_type);


ADE_API_RET_T ADE_Matlab_Init(ADE_MATLAB_T** dp_this, Engine *p_mateng,char* filename, char* p_matfname,char *p_matpath)
{
    ADE_MATLAB_T* p_this=NULL;
    ADE_UINT32_T i=0,k=0;
    ADE_UINT32_T n_chars = 0;
    mxArray **dp_mxarray=NULL;
    FILE *script_fid=NULL;
    MATFile *p_matfile;
    int n_arrays=0;
    char **dp_dir=NULL;
    char *array_name;
    unsigned int k_valid_array=0;
    unsigned int *p_valid_arr_idx=NULL;
    char **dp_temp_names=NULL;
    double *p_temp,*p_temp2;
    char *p_temp_name=NULL;

     if (!(p_mateng = engOpen(p_matpath))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_MATLAB,ADE_METHOD_Init,p_mateng,"%p",(FILE*)ADE_STD_STREAM);
	    return ADE_RET_ERROR;
	}


    p_this=calloc(1,sizeof(ADE_MATLAB_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,  p_this);

         script_fid=fopen(filename,"r");
         ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, script_fid);

        p_this->p_matscript=script_fid;
        p_this->p_eng=p_mateng;

        ADE_Matlab_launch_script_segment(p_this, "Configuration");




         /****************ALLOC VAR NAMES************/
          p_matfile = matOpen(p_matfname, "r");
          ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, p_matfile);

         dp_dir=matGetDir(p_matfile, &n_arrays);
         mxFree(dp_dir);
          matClose(p_matfile);

           dp_mxarray=(mxArray**)mxCalloc(n_arrays, sizeof(mxArray*));
           ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, dp_mxarray);
           dp_temp_names=(char**)calloc(n_arrays,sizeof(char*));
           ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, dp_temp_names);

            /* Calcola il numero delle variabili double*/
             p_matfile = matOpen(p_matfname, "r");
             ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, p_matfile);

              for (i=0;i<n_arrays;i++)
              {
                 // array_name=&(dp_dir[i][0]);
                  dp_mxarray[i]=matGetNextVariable(p_matfile,(const char**) &array_name);
                  dp_temp_names[i]=calloc(strlen(array_name)+1,sizeof(char));
                  ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, dp_temp_names[i]);
                  strcpy(dp_temp_names[i],array_name);

                    if (mxIsDouble(dp_mxarray[i]) )
                    {
                         k_valid_array++;
                    }

              }
              //mxFree(array_name);
            matClose(p_matfile);

           p_this->n_vars=k_valid_array;
           p_valid_arr_idx=calloc(k_valid_array,sizeof(unsigned int));
           ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, p_valid_arr_idx);
           p_this->dp_var_list=(char**)calloc(k_valid_array,sizeof(char*));
           ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, p_this->dp_var_list);

        k_valid_array=0;
        /* Salva i nomi nella struttura interna  MECHANISM 2 IMPROVE */
         for (i=0;i<n_arrays;i++)
         {

                if (mxIsDouble(dp_mxarray[i]) )
                {
                     n_chars=strlen(dp_temp_names[i])+1;//strlen(array_name)+1;
                //      fprintf(stdout,"%d\n",n_chars);
                     p_this->dp_var_list[k_valid_array]=calloc(n_chars,sizeof(char));
                     ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,  p_this->dp_var_list[k_valid_array]);
                     strcpy(p_this->dp_var_list[k_valid_array],dp_temp_names[i]);
                   //  fprintf(stdout,"%s\n",p_this->dp_var_list[k_valid_array]);
                     p_valid_arr_idx[k_valid_array]=i;
                      k_valid_array++;
                }
         }
//fprintf(stdout,"%d\n",k_valid_array);
        // matClose(p_matfile);


         /****************ALLOC SIZES AND TYPE*****************************/
         p_this->n_row      =calloc(k_valid_array,sizeof(unsigned int));
         ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,  p_this->n_row);
         p_this->n_col      =calloc(k_valid_array,sizeof(unsigned int));
         ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,  p_this->n_col);
         p_this->data_size  =calloc(k_valid_array,sizeof(size_t));
         ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,  p_this->data_size);
         p_this->p_vartype =calloc(k_valid_array,sizeof(ADE_MATH_ATTRIBUTE_T));
         ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,  p_this->p_vartype);

         for (i=0;i<k_valid_array;i++)
         {
//
//          p_mxarr=engGetVariable(p_mateng, p_this->dp_var_list[i]);
//           if (p_mxarr==NULL)
//             {
//                 //ADE_PRINT_ERRORS(ADE_MEM,p_mxarr,"%p",ADE_Matlab_Init);
//                 return  ADE_RET_ERROR;//26;
//             }
            k=p_valid_arr_idx[i];
            p_this->n_row[i]=mxGetM(dp_mxarray[k]);
          p_this->n_col[i]=mxGetN(dp_mxarray[k]);
            if (mxIsDouble(dp_mxarray[k]) )
            {

                if (mxIsComplex(dp_mxarray[k]))
                {
                     p_this->data_size[i]=2*p_this->n_row[i]*p_this->n_col[i]*8;
                     p_this->p_vartype[i]=ADE_MATH_CPLX;
                }
                else
                {
                    p_this->data_size[i]=p_this->n_row[i]*p_this->n_col[i]*8;
                     p_this->p_vartype[i]=ADE_MATH_REAL;
                }

            }
            else
            {
            p_temp_name=mxGetClassName(dp_mxarray[k]);
              //  ADE_PRINT_WARNINGS(ADE_INCHECKS,mxGetClassName(dp_mxarray[k]),"%s",ADE_Matlab_Init);
               ADE_PRINT_ERRORS(ADE_WARNING,ADE_INCHECKS,ADE_CLASS_MATLAB,ADE_METHOD_Init,p_temp_name,"%s",(FILE*)ADE_STD_STREAM);
                return ADE_RET_WARNING;
            }
         }


         /****************ALLOC VAR VALUES************/


         p_this->dp_vardouble=(double**)calloc(k_valid_array,sizeof(double*));
          ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init, p_this->dp_vardouble);

         for (i=0;i<k_valid_array;i++)
         {
//             p_mxarr=engGetVariable(p_mateng,p_this->dp_var_list[i]);
//
//             if (p_mxarr==NULL)
//             {
//                 //ADE_PRINT_ERRORS(ADE_MEM,p_mxarr,"%p",ADE_Matlab_Init);
//                 return  ADE_RET_ERROR;//26;
//             }
            k=p_valid_arr_idx[i];
             p_this->dp_vardouble[i]=calloc(1,p_this->data_size[i]);
             ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Init,p_this->dp_vardouble[i]);
             //memcpy(p_this->dp_vardouble[i],(double*)mxGetData(p_mxarr),p_this->data_size[i]);
             if (mxIsComplex(dp_mxarray[k]))
             {
                 p_temp=(double*)mxGetImagData(dp_mxarray[k]);
                 p_temp2=(double*)mxGetData(dp_mxarray[k]);
                 ADE_Matlab_Mat2C_copy(p_this->dp_vardouble[i], (dp_mxarray[k]), p_this->n_row[i], p_this->n_col[i]);
             }
             else
             {
                 ADE_Matlab_Mat2C_copy(p_this->dp_vardouble[i],(dp_mxarray[k]), p_this->n_row[i], p_this->n_col[i]);
             }

         }


        for (i=0;i<n_arrays;i++)
        {
            mxDestroyArray(dp_mxarray[i]);
            free(dp_temp_names[i]);
        }
        mxFree(dp_mxarray);
        free(dp_temp_names);
        free(p_valid_arr_idx);
        *dp_this=p_this;

 return ADE_RET_SUCCESS;
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
    ADE_CHECKNFREE(p_mat->p_vartype);


    ADE_CHECKNFREE(p_mat->n_row);
    ADE_CHECKNFREE(p_mat->n_col);
    ADE_CHECKNFREE(p_mat->data_size);
    engClose(p_mat->p_eng);
    fclose(p_mat->p_matscript);



}

ADE_UINT32_T ADE_Matlab_GetVarIndex(ADE_MATLAB_T* p_mat, char *varname)
{

    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetVarIndex,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetVarIndex,varname);

    for (i=0;i<p_mat->n_vars;i++)
    {
        if (!strcmp(varname,p_mat->dp_var_list[i]))
        {
            return i;
        }

    }

    fprintf(stderr,"WARNING - ADE_Matlab_GetVarIndex -> Variable \"%s\" not found \n",varname);
    return ADE_RET_ERROR;//29;
}

ADE_UINT32_T ADE_Matlab_GetNRows(ADE_MATLAB_T* p_mat, char *varname)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetNRows,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetNRows,varname);

    return p_mat->n_row[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

ADE_UINT32_T ADE_Matlab_GetNCols(ADE_MATLAB_T* p_mat, char *varname)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetNCols,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetNCols,varname);

    return p_mat->n_col[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

ADE_UINT32_T ADE_Matlab_GetSize(ADE_MATLAB_T* p_mat, char *varname)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetSize,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetSize,varname);
    return p_mat->data_size[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

ADE_UINT32_T ADE_Matlab_GetLength(ADE_MATLAB_T* p_mat, char *varname)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetLength,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetLength,varname);

    if (ADE_Matlab_GetNCols(p_mat,varname)==1)
    {
       return  ADE_Matlab_GetNRows(p_mat,varname);
    }
    else if (ADE_Matlab_GetNRows(p_mat,varname)==1)
    {
        return  ADE_Matlab_GetNCols(p_mat,varname);
    }
    else
    {
        fprintf(stderr,"ADE - > ADE_Matlab_GetLength array is a matrix and not a vector length=n_rows\n");
        return  ADE_Matlab_GetNRows(p_mat,varname);
    }


}

ADE_UINT32_T ADE_Matlab_GetNElements(ADE_MATLAB_T* p_mat, char *varname)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetNElements,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_GetNElements,varname);

    return (p_mat->n_col[ADE_Matlab_GetVarIndex(p_mat,varname)])*(p_mat->n_row[ADE_Matlab_GetVarIndex(p_mat,varname)]);
}

double* ADE_Matlab_GetDataPointer(ADE_MATLAB_T* p_mat, char *varname)
{
    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_MATLAB,ADE_METHOD_GetDataPointer,p_mat);
    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_MATLAB,ADE_METHOD_GetDataPointer,varname);
    return p_mat->dp_vardouble[ADE_Matlab_GetVarIndex(p_mat,varname)];
}

double ADE_Matlab_GetScalar(ADE_MATLAB_T* p_mat, char *varname)
{
     ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_MATLAB,ADE_METHOD_GetScalar,p_mat);
    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_MATLAB,ADE_METHOD_GetScalar,varname);
    return *(p_mat->dp_vardouble[ADE_Matlab_GetVarIndex(p_mat,varname)]);
}

ADE_API_RET_T ADE_Matlab_PutVarintoWorkspace(ADE_MATLAB_T* p_mat, double *p_var, char *var_matname, ADE_UINT32_T var_rows, ADE_UINT32_T var_cols, ADE_MATH_ATTRIBUTE_T comp_type)
{

    mxArray *p_tmp=NULL;
    int ret_engPutVariable=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_PutVarintoWorkspace,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_PutVarintoWorkspace,p_var);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_PutVarintoWorkspace,var_matname);

    if (comp_type==ADE_MATH_REAL)
    {
        p_tmp=mxCreateDoubleMatrix(var_rows, var_cols, mxREAL);
    }
    else if (comp_type==ADE_MATH_CPLX)
    {
         p_tmp=mxCreateDoubleMatrix(var_rows, var_cols, mxCOMPLEX);
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_MATLAB,ADE_METHOD_PutVarintoWorkspace,comp_type,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    //memcpy((void *)mxGetPr(p_tmp), (void *)p_var, var_rows*var_cols*sizeof(double));
    ADE_Matlab_C2Mat_copy((void *)mxGetPr(p_tmp), p_var, var_rows, var_cols, comp_type);

    ret_engPutVariable=engPutVariable(p_mat->p_eng,var_matname, p_tmp);

    if (ret_engPutVariable==1)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_MATLAB,ADE_METHOD_PutVarintoWorkspace,ret_engPutVariable,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    mxDestroyArray(p_tmp);

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Matlab_Evaluate_String(ADE_MATLAB_T* p_mat, char *matcode)
{

    int ret_engEvalString = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Evaluate_String,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Evaluate_String,matcode);

    ret_engEvalString = engEvalString(p_mat->p_eng, matcode);

    if (ret_engEvalString==1)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_MATLAB,ADE_METHOD_Evaluate_String,ret_engEvalString,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Matlab_Evaluate_StringnWait(ADE_MATLAB_T* p_mat, char *matcode)
{

    ADE_API_RET_T ret_Matlab_Evaluate_String = ADE_RET_SUCCESS;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Evaluate_StringnWait,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Evaluate_StringnWait,matcode);

    ret_Matlab_Evaluate_String=ADE_Matlab_Evaluate_String(p_mat,matcode);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MATLAB,ADE_METHOD_Evaluate_StringnWait,ret_Matlab_Evaluate_String);

    ADE_MAT_WAIT;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Matlab_Configure_Iir_sos(ADE_MATLAB_T* p_mat,ADE_IIR_T *p_iir, char *sosmatrix_varname, char *scalevalues_varname)
{

    double **dp_num=NULL ,**dp_denom=NULL;
    ADE_UINT32_T n_sos_sections=0,section_n_apb_coeffs=0,i=0;
    double *p_gains=NULL;
    ADE_API_RET_T ret_set=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,p_iir);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,sosmatrix_varname);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,scalevalues_varname);

    n_sos_sections=ADE_Matlab_GetNRows(p_mat,sosmatrix_varname);

    dp_num=(double**)calloc(n_sos_sections,sizeof(double*));
    ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,dp_num);

    dp_denom=(double**)calloc(n_sos_sections,sizeof(double*));
    ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,dp_denom);

    section_n_apb_coeffs=ADE_Matlab_GetNCols(p_mat,sosmatrix_varname);

    for (i=0;i<n_sos_sections;i++)
    {
        dp_num[i]=ADE_Matlab_GetDataPointer(p_mat,sosmatrix_varname)+(i*section_n_apb_coeffs);
        dp_denom[i]=ADE_Matlab_GetDataPointer(p_mat,sosmatrix_varname)+3+(i*section_n_apb_coeffs);
    }

    p_gains = ADE_Matlab_GetDataPointer(p_mat,scalevalues_varname);

    ret_set=ADE_Iir_setGains( p_iir, (ADE_FLOATING_T*)p_gains);
    ADE_CHECK_ADERETVAL(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,ret_set);

    ret_set=ADE_Iir_setNums( p_iir,  (ADE_FLOATING_T**)dp_num);
     ADE_CHECK_ADERETVAL(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,ret_set);

    ret_set=ADE_Iir_setDenoms( p_iir, (ADE_FLOATING_T**)dp_denom);
     ADE_CHECK_ADERETVAL(ADE_CLASS_MATLAB,ADE_METHOD_Configure_Iir_sos,ret_set);

    ADE_CHECKNFREE(dp_num);
    ADE_CHECKNFREE(dp_denom);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Matlab_launch_script_segment(ADE_MATLAB_T *p_mat, char *p_stopword)
{
    char *temp_str;
    char *p_prefix="%end ";
    char *test_str;
    char *segment_str;
    unsigned int i=0;
    int ret_eng=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_launch_script_segment,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_launch_script_segment,p_stopword);

    temp_str=calloc(ADE_MAX_CHARS,sizeof(char));
    ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_launch_script_segment,temp_str);

    test_str=calloc(ADE_MAX_CHARS,sizeof(char));
    ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_launch_script_segment,test_str);

    segment_str=calloc(ADE_MAX_CHARS*ADE_MAX_SEGMENT_LINES,sizeof(char));
    ADE_CHECK_MEMALLOC(ADE_CLASS_MATLAB,ADE_METHOD_launch_script_segment,segment_str);

    strcpy(test_str,p_prefix);
    strcat(test_str,p_stopword);
    strcat(test_str,"\n");

    while( strcmp(temp_str,test_str) && (feof(p_mat->p_matscript)==0) )
    {
        memset(temp_str,'\0',ADE_MAX_CHARS*sizeof(char));
        fgets(temp_str,ADE_MAX_CHARS,p_mat->p_matscript);
        strcat(segment_str,temp_str);
        if (i==0)
        {
            fprintf(stdout,"Loading  Segment starting with -> %s\n",temp_str);


        }
         i++;

    }

     fprintf(stdout,"Loading  Segment ending with -> %s\n",temp_str);
     if (i>ADE_MAX_SEGMENT_LINES)
     {
         fprintf(stderr,"WARNING SEGMENT MIGHT BE TOO LONG CONSIDER INCREASING ""ADE_MAX_SEGMENT_LINES"" or CHECK THE end of configuration marker found %d max %d \n",i,ADE_MAX_SEGMENT_LINES);
     }
     ret_eng=engEvalString(p_mat->p_eng,segment_str);

     if (ret_eng==1)
     {
         fprintf(stderr,"Invalid Matlab session Pointer or session no longer running!\n");
         return ADE_RET_ERROR;//27;
     }



    if (feof(p_mat->p_matscript))
    {
        fprintf(stdout,"REACHED EOF of SCRIPT\n");
    }

    ADE_CHECKNFREE(temp_str);
    ADE_CHECKNFREE(test_str);
    ADE_CHECKNFREE(segment_str);

     return ADE_RET_SUCCESS;
}

static ADE_VOID_T ADE_Matlab_Mat2C_copy(double *p_dst, mxArray *p_mx, unsigned int n_rows, unsigned int n_cols)
{
    unsigned int i=0,k=0,idx_dst=0,idx_src=0;
    double *p_src_r = NULL;
    double *p_src_i = NULL;

    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_MATLAB,ADE_METHOD_Mat2C_copy,p_dst);
    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_MATLAB,ADE_METHOD_Mat2C_copy,p_mx);

    if (mxIsComplex(p_mx))
    {
        p_src_r=(double*)mxGetData(p_mx);
        p_src_i=(double*)mxGetImagData(p_mx);

        for (i=0;i<n_rows;i++)
        {
            for (k=0;k<n_cols;k++)
            {
                idx_src=i+k*n_rows;
                idx_dst=k+i*n_cols;
                p_dst[2*idx_dst]=p_src_r[idx_src];
                p_dst[2*idx_dst+1]=p_src_i[idx_src];
            }
        }

    }
    else
    {
        p_src_r=(double*)mxGetData(p_mx);

        for (i=0;i<n_rows;i++)
        {
            for (k=0;k<n_cols;k++)
            {
                idx_src=i+k*n_rows;
                idx_dst=k+i*n_cols;
                p_dst[idx_dst]=p_src_r[idx_src];
            }
        }

    }



}

static ADE_API_RET_T ADE_Matlab_C2Mat_copy(double *p_dst, double *p_src, unsigned int n_rows, unsigned int n_cols,ADE_MATH_ATTRIBUTE_T comp_type)
{
    unsigned int i=0,k=0,idx_dst=0,idx_src=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_C2Mat_copy,p_dst);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_C2Mat_copy,p_src);

    if (comp_type==ADE_MATH_REAL)
    {
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
    else if (comp_type==ADE_MATH_CPLX)
    {

        for (i=0;i<n_rows;i++)
            {
                for (k=0;k<n_cols;k++)
                {
                    idx_dst=i+k*n_rows;
                    idx_src=k+i*n_cols;
                    p_dst[2*idx_dst]=p_src[2*idx_src];
                    p_dst[2*idx_dst+1]=p_src[2*idx_src+1];
                }
            }


    }
    else
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_MATLAB,ADE_METHOD_C2Mat_copy,comp_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }


return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_Matlab_Print(ADE_MATLAB_T *p_mat, FILE *p_fid)
{
   // FILE *p_fid;
    ADE_UINT32_T i=0;
    ADE_UINT32_T n_vars=p_mat->n_vars;
    ADE_CHAR_T *p_varname=NULL;
    double *p_data=NULL;
    ADE_MATH_ATTRIBUTE_T var_type;
    //ADE_UINT32_T n_var_per_printrow = 1;
    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    ADE_UINT32_T n_row=0,n_col=0;

    //p_fid=fopen("./mat2C_variables.txt","w");
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Print,p_mat);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_MATLAB,ADE_METHOD_Print,p_fid);

    if (p_fid!=NULL)
    {

        for (i=0;i<n_vars;i++)
        {
            p_varname=p_mat->dp_var_list[i];
            p_data = p_mat->dp_vardouble[i];
            var_type = p_mat->p_vartype[i];
            n_row = p_mat->n_row[i];
            n_col=p_mat->n_col[i];

          ret = ADE_Utils_PrintArray(p_data,0,(n_row-1),0,(n_col-1) , p_varname, stdout,var_type);
          ADE_CHECK_ADERETVAL(ADE_CLASS_MATLAB,ADE_METHOD_Print,ret);

        }

       // fclose(p_fid);

        return ADE_RET_SUCCESS;
    }

    else
    {

        return ADE_RET_ERROR;
    }




}



