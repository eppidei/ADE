#include "headers/ADE_Utils.h"
#include "headers/ADE_errors.h"
#include <sys/ioctl.h>
#include <math.h>
#include "headers/ADE_complex.h"

/**************************** Private prototypes *********************************/

static ADE_API_RET_T ADE_Utils_PrintRowArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,ADE_UINT32_T n_var_per_printrow,ADE_UTILS_ROW_INFO_T row_info, FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintRowArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_UINT32_T n_var_per_printrow, ADE_UTILS_ROW_INFO_T row_info, FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintColArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx,   FILE *p_stream);
static ADE_API_RET_T ADE_Utils_PrintColArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx,   FILE *p_stream);

static ADE_API_RET_T ADE_Utils_PrintColArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);
static ADE_API_RET_T ADE_Utils_PrintRowArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);

static ADE_VOID_T ADE_Utils_PrintMatrixReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,  FILE *p_stream);
static ADE_VOID_T ADE_Utils_PrintMatrixCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,  FILE *p_stream);

static ADE_API_RET_T ADE_Utils_PrintMatrix(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);

/********************* Functions ***********************************/

ADE_VOID_T ADE_Utils_Get_Terminal_size(ADE_INT32_T *lines ,ADE_INT32_T *columns  )
{
  //  char *temp=NULL;
   // int line_int=0;
//    int col_int=0;
    struct winsize sz;

    ioctl(0, TIOCGWINSZ, &sz);
    if (sz.ws_row==0 )
    {
        ADE_PRINT_WARNINGS(ADE_RETCHECKS,sz.ws_row,"%d",ADE_Get_Terminal_size);
        fprintf(stderr,"setting default value of 80\n");
        *lines=80;

    }
    if (sz.ws_col==0 )
    {
        ADE_PRINT_WARNINGS(ADE_RETCHECKS,sz.ws_col,"%d",ADE_Get_Terminal_size);
        fprintf(stderr,"setting default value of 24\n");
        *columns=24;
    }

    *lines=sz.ws_row;
    *columns=sz.ws_col;
     //printf("Screen width: %i  Screen height: %i\n", sz.ws_col, sz.ws_row);

//    temp =getenv("LINES");
//    if (temp==NULL)
//    {
//        ADE_PRINT_WARNINGS(ADE_RETCHECKS,temp,"%s",ADE_Get_Terminal_size);
//    }
//    line_int = strtol(temp,NULL,10);
//    if (line_int==0)
//    {
//         ADE_PRINT_ERRORS(ADE_RETCHECKS,line_int,"%d",ADE_Get_Terminal_size);
//        return ADE_E35;
//    }
//    temp =getenv("COLUMNS");
//    if (temp==NULL)
//    {
//        ADE_PRINT_WARNINGS(ADE_RETCHECKS,temp,"%s",ADE_Get_Terminal_size);
//    }
//     col_int = strtol(temp,NULL,10);
//
//    if (col_int==0)
//    {
//        ADE_PRINT_ERRORS(ADE_RETCHECKS,line_int,"%d",ADE_Get_Terminal_size);
//        return ADE_E35;
//    }

//     return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Utils_PrintArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{

    ADE_API_RET_T ret = ADE_DEFAULT_RET;
    ADE_UINT32_T n_row=stop_0based_row_idx-start_0based_row_idx+1;
    ADE_UINT32_T n_col=stop_0based_col_idx-start_0based_col_idx+1;

    if (n_row==1)
    {
        ret = ADE_Utils_PrintRowArray(p_var,start_0based_col_idx, stop_0based_col_idx,p_varname, p_stream,math_type);
    }
    else if (n_col==1)
    {
         ret = ADE_Utils_PrintColArray(p_var,start_0based_row_idx,stop_0based_row_idx, p_varname, p_stream,math_type);
    }
    else
    {
        ret = ADE_Utils_PrintMatrix(p_var,start_0based_row_idx,stop_0based_row_idx, start_0based_col_idx, stop_0based_col_idx, p_varname,p_stream,math_type);
    }

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Utils_PrintArray);
        return ADE_E35;

    }
    else
    {
        return ADE_DEFAULT_RET;
    }
}

ADE_API_RET_T ADE_Utils_FindIndexes(ADE_FLOATING_T *frame_i,ADE_UINT32_T frame_len ,ADE_FLOATING_T *indexes_o,ADE_FLOATING_T *n_indexes_o, ADE_FLOATING_T threshold,ADE_UTILS_CONDITION_T condition)
{
    ADE_UINT32_T i=0,idx=0;

    if (condition==ADE_UTILS_MAJOR)
    {
        for(i=0;i<frame_len;i++)
        {
            if (frame_i[i]>threshold)
            {
                indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else if (condition==ADE_UTILS_MINOR)
    {
        for(i=0;i<frame_len;i++)
        {
            if (frame_i[i]<threshold)
            {
                indexes_o[idx]=i;
                idx++;
            }
        }
    }
    else if (condition==ADE_UTILS_EQUAL)
    {
        for(i=0;i<frame_len;i++)
        {
            if (frame_i[i]==threshold)
            {
                indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else if (condition==ADE_UTILS_MAJEQUAL)
    {
        for(i=0;i<frame_len;i++)
        {
            if (frame_i[i]>=threshold)
            {
                indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else if (condition==ADE_UTILS_MINEQUAL)
    {
        for(i=0;i<frame_len;i++)
        {
            if (frame_i[i]<=threshold)
            {
                indexes_o[idx]=i;
                idx++;
            }
        }

    }
    else
    {
        return ADE_E34;
    }

    *n_indexes_o=idx;

    return ADE_DEFAULT_RET;

}
/*********************** Private Functions ******************************/

static ADE_API_RET_T ADE_Utils_PrintRowArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx,ADE_UINT32_T n_var_per_printrow, ADE_UTILS_ROW_INFO_T row_info,FILE *p_stream)
{
    ADE_UINT32_T i=0,k=0,incremental_idx=0;
    //ADE_INT32_T lines=0,columns=0;
    //ADE_UINT32_T n_var_per_printrow=0;

     //ADE_Get_Terminal_size(&lines ,&columns  );
    ADE_UINT32_T len = 0;

    if (stop_0based_col_idx<start_0based_col_idx)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,stop_0based_col_idx,"%u",ADE_Utils_PrintRowArrayReal);
        return ADE_E34;
    }

    len=stop_0based_col_idx-start_0based_col_idx+1;


    for (i=start_0based_col_idx;i<=stop_0based_col_idx;i++)
    {
        if ( (incremental_idx%n_var_per_printrow)==(0) )
        {
             k++;
             if (len>=k*n_var_per_printrow)
             {
                 if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                 {
                     if (len>1 && start_0based_col_idx==0)
                     {
                         fprintf(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,start_0based_col_idx+k*n_var_per_printrow);
                     }

                 }
                    fprintf(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
             }
             else
             {
                 if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                 {
                     if (len>1 && start_0based_col_idx==0)
                     {
                        fprintf(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,stop_0based_col_idx+1);
                     }
                     if (i==stop_0based_col_idx)
                     {
                         fprintf(p_stream,"%*.*lf\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                     }
                     else
                     {
                          fprintf(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                     }

                 }
                 else
                 {
                     if (i==stop_0based_col_idx)
                     {
                      fprintf(p_stream,"%*.*lf\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                     }
                     else
                     {
                         fprintf(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
                     }
                 }



             }

        }
        else
        {
            if (i==stop_0based_col_idx)
            {
                fprintf(p_stream,"%*.*lf\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
            }
            else
            {
                fprintf(p_stream,"%*.*lf",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,(p_var[i]));
            }


        }
        incremental_idx++;
    }
    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Utils_PrintRowArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_UINT32_T n_var_per_printrow,ADE_UTILS_ROW_INFO_T row_info, FILE *p_stream)
{

    ADE_UINT32_T i=0,k=0,incremental_idx=0;
    //ADE_INT32_T lines=0,columns=0;
    //ADE_UINT32_T n_var_per_printrow=0;

     //ADE_Get_Terminal_size(&lines ,&columns  );
    ADE_UINT32_T len = 0;

    if (stop_0based_col_idx<start_0based_col_idx)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,stop_0based_col_idx,"%u",ADE_Utils_PrintRowArrayReal);
        return ADE_E34;
    }

    len=stop_0based_col_idx-start_0based_col_idx+1;


    for (i=start_0based_col_idx;i<=stop_0based_col_idx;i++)
    {
        if ( (incremental_idx%n_var_per_printrow)==(0) )
        {
             k++;
             if (len>=k*n_var_per_printrow)
             {
                 if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                 {
                     if (len>1 && start_0based_col_idx==0)
                     {
                     fprintf(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,start_0based_col_idx+k*n_var_per_printrow);
                     }
                 }
                    fprintf(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
             }
             else
             {
                 if (row_info==ADE_UTILS_FIRST_PRINT_ROW)
                 {
                     if (len>1 && start_0based_col_idx==0)
                     {
                     fprintf(p_stream,"\nColumns from %u to %u\n",start_0based_col_idx+(k-1)*n_var_per_printrow+1,stop_0based_col_idx+1);
                     }
                     if (i==stop_0based_col_idx)
                     {
                         fprintf(p_stream,"%*.*lf%+*.*lfi\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                     }
                     else
                     {
                          fprintf(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                     }

                 }
                 else
                 {
                     if (i==stop_0based_col_idx)
                     {
                      fprintf(p_stream,"%*.*lf%+*.*lfi\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                     }
                     else
                     {
                          fprintf(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
                     }
                 }



             }

        }
        else
        {
            if (i==stop_0based_col_idx)
            {
                fprintf(p_stream,"%*.*lf%+*.*lfi\n",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
            }
            else
            {
                fprintf(p_stream,"%*.*lf%+*.*lfi",ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));
            }


        }
        incremental_idx++;
    }
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Utils_PrintColArrayReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx,   FILE *p_stream)
{
   ADE_UINT32_T i=0;//,k=0;

   if (stop_0based_row_idx<start_0based_row_idx)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,stop_0based_row_idx,"%u",ADE_Utils_PrintColArrayReal);
        return ADE_E34;
    }

    for (i=start_0based_row_idx;i<=stop_0based_row_idx;i++)
    {
        fprintf(p_stream,"(%u) %*.*lf\n",i+1,ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,p_var[i]);
    }

     return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Utils_PrintColArrayCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, FILE *p_stream)
{
   ADE_UINT32_T i=0;//,k=0;

    if (stop_0based_row_idx<start_0based_row_idx)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,stop_0based_row_idx,"%u",ADE_Utils_PrintColArrayCplx);
        return ADE_E34;
    }

    for (i=start_0based_row_idx;i<=stop_0based_row_idx;i++)
    {
        fprintf(p_stream,"(%u) %*.*lf%+*.*lfi\n",i+1,ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_creal(p_var[i]),ADE_UTILS_PRINTF_FLOAT_WIDTH,ADE_UTILS_PRINTF_FLOAT_PRECISION,ADE_cimag(p_var[i]));

    }

    return ADE_DEFAULT_RET;
}




static ADE_API_RET_T ADE_Utils_PrintRowArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{
    ADE_INT32_T lines=0,columns=0;
    ADE_UINT32_T n_var_per_printrow=0;
ADE_UTILS_ROW_INFO_T row_info=ADE_UTILS_FIRST_PRINT_ROW;
ADE_UINT32_T len = stop_0based_col_idx-start_0based_col_idx+1;

     ADE_Utils_Get_Terminal_size(&lines ,&columns);


    if (math_type==ADE_REAL)
    {
         n_var_per_printrow=floor(columns/ADE_UTILS_PRINTF_FLOAT_WIDTH);
         if (len==1)
         {
             fprintf(p_stream,"\n%s =\n",p_varname);
         }
         else
         {
              fprintf(p_stream,"\n%s(1,%u:%u) =\n",p_varname,start_0based_col_idx+1,stop_0based_col_idx+1);
         }

        ADE_Utils_PrintRowArrayReal((ADE_FLOATING_T*)p_var, start_0based_col_idx,stop_0based_col_idx,n_var_per_printrow,row_info,p_stream);
    }
    else if(math_type==ADE_CPLX)
    {
          n_var_per_printrow=floor(columns/(ADE_UTILS_PRINTF_FLOAT_WIDTH+ADE_UTILS_PRINTF_FLOAT_WIDTH+2));
          if (len==1)
         {
             fprintf(p_stream,"\n%s =\n",p_varname);
         }
         else
         {
             fprintf(p_stream,"\n%s(1,%u:%u) =\n",p_varname,start_0based_col_idx+1,stop_0based_col_idx+1);
         }

        ADE_Utils_PrintRowArrayCplx((ADE_CPLX_T*)p_var,start_0based_col_idx, stop_0based_col_idx,n_var_per_printrow,row_info,p_stream);
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Utils_PrintArray);
        return ADE_E34;
    }

    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Utils_PrintColArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{
    ADE_UINT32_T len = stop_0based_row_idx-start_0based_row_idx+1;

    if (math_type==ADE_REAL)
    {
        if (len==1)
        {
            fprintf(p_stream,"\n%s =\n\n",p_varname);
        }
        else
        {
            fprintf(p_stream,"\n%s(%u:%u,1) =\n\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1);
        }

        ADE_Utils_PrintColArrayReal((ADE_FLOATING_T*)p_var, start_0based_row_idx,stop_0based_row_idx,p_stream);
    }
    else if(math_type==ADE_CPLX)
    {
        if (len==1)
        {
            fprintf(p_stream,"\n%s =\n\n",p_varname);
        }
        else
        {
            fprintf(p_stream,"\n%s(%u:%u,1) =\n\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1);
        }

        ADE_Utils_PrintColArrayCplx((ADE_CPLX_T*)p_var, start_0based_row_idx,stop_0based_row_idx,p_stream);
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Utils_PrintArray);
        return ADE_E34;
    }

    return ADE_DEFAULT_RET;

}




static ADE_VOID_T ADE_Utils_PrintMatrixReal(ADE_FLOATING_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, FILE *p_stream)
{
    ADE_UINT32_T row_idx=0,k=0;
      ADE_INT32_T lines=0,columns=0;
      ADE_UINT32_T n_var_per_printrow=0;
      ADE_UINT32_T n_block_cols=0,n_col=0;
      ADE_UINT32_T start_col_idx_int=0,stop_col_idx_int=0;
      ADE_INT32_T check_col=0;
      ADE_UTILS_ROW_INFO_T row_info;

     ADE_Utils_Get_Terminal_size(&lines ,&columns  );

    n_col=stop_0based_col_idx-start_0based_col_idx+1;
     n_var_per_printrow=floor(columns/ADE_UTILS_PRINTF_FLOAT_WIDTH);
     n_block_cols=ceil((double)(n_col)/n_var_per_printrow); //colonne totali/colonne printabili

    for (k=0;k<n_block_cols;k++)
    {

        start_col_idx_int=k*n_var_per_printrow; // forzo lunghezza array a n_var stampabili così fa tutte le righe e poi passa ad altre colonne
        check_col = (k+1)*n_var_per_printrow-n_col;
        if (check_col>0)
        {
            stop_col_idx_int=start_col_idx_int+n_var_per_printrow-check_col-1;
        }
        else
        {
            stop_col_idx_int=(k+1)*n_var_per_printrow-1;
        }
        for (row_idx=start_0based_row_idx;row_idx<=stop_0based_row_idx;row_idx++)
        {
            if (row_idx==start_0based_row_idx)
            {
                row_info=ADE_UTILS_FIRST_PRINT_ROW;
            }
            else
            {
                row_info=ADE_UTILS_NOTFIRST_PRINT_ROW;
            }


                 ADE_Utils_PrintRowArrayReal(&(p_var[row_idx*n_col]),start_col_idx_int,stop_col_idx_int,n_var_per_printrow,row_info,p_stream);
        }

    }

}


static ADE_VOID_T ADE_Utils_PrintMatrixCplx(ADE_CPLX_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, FILE *p_stream)
{
    ADE_UINT32_T row_idx=0,k=0;
      ADE_INT32_T lines=0,columns=0;
      ADE_UINT32_T n_var_per_printrow=0;
      ADE_UINT32_T n_block_cols=0,n_col=0;
      ADE_UINT32_T start_col_idx_int=0,stop_col_idx_int=0;
      ADE_INT32_T check_col=0;
      ADE_UTILS_ROW_INFO_T row_info;

     ADE_Utils_Get_Terminal_size(&lines ,&columns  );

    n_col=stop_0based_col_idx-start_0based_col_idx+1;
     n_var_per_printrow=floor(columns/(ADE_UTILS_PRINTF_FLOAT_WIDTH+ADE_UTILS_PRINTF_FLOAT_WIDTH+2));
     n_block_cols=ceil((double)(n_col)/n_var_per_printrow); //colonne totali/colonne printabili

    for (k=0;k<n_block_cols;k++)
    {

        start_col_idx_int=k*n_var_per_printrow; // forzo lunghezza array a n_var stampabili così fa tutte le righe e poi passa ad altre colonne
        check_col = (k+1)*n_var_per_printrow-n_col;
        if (check_col>0)
        {
            stop_col_idx_int=start_col_idx_int+n_var_per_printrow-check_col-1;
        }
        else
        {
            stop_col_idx_int=(k+1)*n_var_per_printrow-1;
        }

        for (row_idx=start_0based_row_idx;row_idx<=stop_0based_row_idx;row_idx++)
        {
            if (row_idx==start_0based_row_idx)
            {
                row_info=ADE_UTILS_FIRST_PRINT_ROW;
            }
            else
            {
                row_info=ADE_UTILS_NOTFIRST_PRINT_ROW;
            }
                 ADE_Utils_PrintRowArrayCplx(&(p_var[row_idx*n_col]),start_col_idx_int,stop_col_idx_int,n_var_per_printrow,row_info,p_stream);
        }

    }
}


static ADE_API_RET_T ADE_Utils_PrintMatrix(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type)
{
    if (math_type==ADE_REAL)
    {
         fprintf(p_stream,"\n%s(%u:%u,%u:%u) =\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1,start_0based_col_idx+1,stop_0based_col_idx+1);
        ADE_Utils_PrintMatrixReal( (ADE_FLOATING_T*)p_var,start_0based_row_idx,stop_0based_row_idx,start_0based_col_idx,stop_0based_col_idx,p_stream);
    }
    else if (math_type==ADE_CPLX)
    {
         fprintf(p_stream,"\n%s(%u:%u,%u:%u) =\n",p_varname,start_0based_row_idx+1,stop_0based_row_idx+1,start_0based_col_idx+1,stop_0based_col_idx+1);
        ADE_Utils_PrintMatrixCplx( (ADE_CPLX_T*)p_var,start_0based_row_idx,stop_0based_row_idx,start_0based_col_idx,stop_0based_col_idx,p_stream);
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Utils_PrintMatrix);
        return ADE_E34;
    }

    return ADE_DEFAULT_RET;

}




