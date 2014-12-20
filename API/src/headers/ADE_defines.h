#ifndef _ADE_DEFINES_H
#define _ADE_DEFINES_H

#define ADE_DEFAULT_RET (0)

#define ADE_USE_SINGLE_PREC (0)
#define ADE_USE_DOUBLE_PREC (1)


#define ADE_CHECKNFREE(x)  if(x!=NULL) { free(x); }
//#define ADE_GOTO_LABEL gently_closure :
#define CHECK_RET_MAIN(s)  if(s<0) {goto gently_closure;}
#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };
#define PRINT_DIFFARRAY(x,y,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, "diff("#x"-"#y ")[%u] =" format "\n",idx,x[idx]-y[idx]); };

/*************************** VERBOSITY LEVELS ***********************/

#define ADE_NO_VERBOSITY (0)
#define ADE_ERROR_LEVEL (1)
#define ADE_WARNING_LEVEL (2)
#define ADE_INFO_LEVEL (3)

#define ADE_VERBOSITY_LEVEL ADE_ERROR_LEVEL

/*************************** BLAS ****************************/
#if defined(_WIN32) || defined(__hpux)
#define FORTRAN_WRAPPER(x) x
#else
#define FORTRAN_WRAPPER(x) x ## _
#endif

#define ADE_USE_BLAS_LIB (1)
#define ADE_USE_BLAS_CUSTOM (0)

#define ADE_CHECK_INPUTS (1) //1 true 0 false
#define ADE_CHECK_RETURNS (1) //1 true 0 false

/****************************** BLOW ***********************************/
#define IIR1_N_SECTIONS (3)
#define IIR2_N_SECTIONS (3)
#define POLY_N_BREAKS (6)
#define POLY_ORDER (3)
/***************************** ENVIRONMENT *******************************/
//ALG LIST
#define BLOW_BIT (0)
#define BLOW_FLAG (1<<BLOW_BIT)


/********************** CTRL DEFINES **********************************/
#define ADE_MATLAB_DEBUG

#ifdef ADE_MATLAB_DEBUG
#define ADE_FP_PRECISION ADE_USE_DOUBLE_PREC //O single 1 double
#else
#define ADE_FP_PRECISION ADE_USE_SINGLE_PREC
#endif

#define ADE_BLAS_IMPLEMENTATION ADE_USE_BLAS_LIB //0 Custom 1 Library


#endif //_ADE_DEFINES_H
