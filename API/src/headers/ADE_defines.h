#ifndef _ADE_DEFINES_H
#define _ADE_DEFINES_H

#ifndef ADE_GENERAL_MALLOC_BUFF_LEN
#define ADE_GENERAL_MALLOC_BUFF_LEN (4096)
#endif


/****************** Macros **************************/
#define ADE_CHECKNFREE(x)  if(x!=NULL) { free(x); }
//#define ADE_GOTO_LABEL gently_closure :
#define CHECK_RET_MAIN(s)  if(s<0) {goto gently_closure;}
#define PRINT_ARRAY(x,idx,len,format) ADE_LOG(stdout,"\n"); for(idx=0;idx<len;idx++) { ADE_LOG(stdout, #x "[%u] =" format "\n",idx,x[idx]); };
#define PRINT_2D_ARRAY(x,idx1,idx2,len1,len2,format) ADE_LOG(stdout,"\n"); for(idx1=0;idx1<len1;idx1++) { for(idx2=0;idx2<len2;idx2++) {ADE_LOG(stdout, #x "[%u][%u] =" format "\n",idx1,idx2,x[idx1*len2+idx2]); }};
#define PRINT_DIFFARRAY(x,y,idx,len,format) ADE_LOG(stdout,"\n"); for(idx=0;idx<len;idx++) { ADE_LOG(stdout, "diff("#x"-"#y ")[%u] =" format "\n",idx,x[idx]-y[idx]); };
/********************** CTRL DEFINES **********************************/
/* Parameters */

#define ADE_STDERR_STREAM stderr
#define ADE_STDOUT_STREAM stdout


#define ADE_RELEASE (0)
#define ADE_DEBUG (1)

#define ADE_USE_SINGLE_PREC (0)
#define ADE_USE_DOUBLE_PREC (1)

#define ADE_PC_NORMAL (1)
#define ADE_PC_MATLAB (2)
#define ADE_ANDROID (3)
#define ADE_IOS (4)

#define ADE_USE_FFTW (0)
#define ADE_USE_ACCEL_FMW_FFT (1)

#define ADE_USE_LIN_ALG_BLAS (0)
#define ADE_USE_LIN_ALG_CUSTOM (1)

#define ADE_UTILS_USE_BLAS (0)
#define ADE_UTILS_USE_CUSTOM (1)

#define ADE_FIR_USE_BLAS (0)
#define ADE_FIR_USE_CUSTOM (1)

#define ADE_IIR_USE_BLAS (0)
#define ADE_IIR_USE_CUSTOM (1)

#define ADE_USE_BLAS_CUSTOM (0)
#define ADE_USE_BLAS_LIB (1)
#define ADE_USE_CBLAS_LIB (2)
#define ADE_USE_OPENBLAS_LIB (3)
#define ADE_USE_BLAS_NEON (4)

#define ADE_CHECK_INPUTS_TRUE (1)
#define ADE_CHECK_INPUTS_FALSE (0)

#define ADE_CHECK_RETURNS_TRUE (1)
#define ADE_CHECK_RETURNS_FALSE (0)

#define ADE_CHECK_MEM_TRUE (1)
#define ADE_CHECK_MEM_FALSE (0)

/* Control */

#ifndef ADE_TARGET_TYPE
#define ADE_TARGET_TYPE ADE_PC_NORMAL
#endif //ADE_TARGET_TYPE

#ifndef ADE_TARGET_MODE
#define ADE_TARGET_MODE ADE_DEBUG
#endif


#define ADE_LIN_ALG_IMP ADE_USE_LIN_ALG_BLAS
#define ADE_UTILS_IMP ADE_UTILS_USE_BLAS
#define ADE_FIR_IMP ADE_FIR_USE_BLAS
#define ADE_IIR_IMP ADE_IIR_USE_BLAS
#define ADE_STD_STREAM ADE_STDERR_STREAM
#define ADE_UPSAMPLER_IMP ADE_UPSAMPLER_USE_BLAS
#define ADE_DOWNSAMPLER_IMP ADE_DOWNSAMPLER_USE_BLAS




#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
    #define ADE_CONFIGURATION_INTERACTIVE
    #define ADE_FP_PRECISION ADE_USE_DOUBLE_PREC //O single 1 double
    #define ADE_FFT_IMP ADE_USE_FFTW
    #define ADE_USE_FFTW_THREADS
     #ifndef ADE_BLAS_IMPLEMENTATION //to make it overrideable from makefile
        #define ADE_BLAS_IMPLEMENTATION ADE_USE_BLAS_LIB
    #endif
#elif (ADE_TARGET_TYPE==ADE_PC_NORMAL )
    #undef ADE_CONFIGURATION_INTERACTIVE
    #ifndef ADE_FP_PRECISION
        #define ADE_FP_PRECISION ADE_USE_SINGLE_PREC
    #endif
    #define ADE_FFT_IMP ADE_USE_FFTW
    #define ADE_USE_FFTW_THREADS
    #ifndef ADE_BLAS_IMPLEMENTATION //to make it overrideable from makefile
        #define ADE_BLAS_IMPLEMENTATION ADE_USE_BLAS_LIB
    #endif
#elif (ADE_TARGET_TYPE==ADE_ANDROID)
    #undef ADE_CONFIGURATION_INTERACTIVE
    #ifndef ADE_FP_PRECISION
        #define ADE_FP_PRECISION ADE_USE_SINGLE_PREC
    #endif
    #define ADE_FFT_IMP ADE_USE_FFTW
    #define ADE_USE_FFTW_THREADS
    #ifndef ADE_BLAS_IMPLEMENTATION //to make it overrideable from makefile
        #define ADE_BLAS_IMPLEMENTATION ADE_USE_OPENBLAS_LIB
    #endif
#elif (ADE_TARGET_TYPE==ADE_IOS)
    #undef ADE_CONFIGURATION_INTERACTIVE
    #ifndef ADE_FP_PRECISION
        #define ADE_FP_PRECISION ADE_USE_SINGLE_PREC
    #endif
    #define ADE_FFT_IMP ADE_USE_ACCEL_FMW_FFT
    #ifndef ADE_BLAS_IMPLEMENTATION //to make it overrideable from makefile
        #define ADE_BLAS_IMPLEMENTATION ADE_USE_CBLAS_LIB
    #endif
#elif (ADE_TARGET_TYPE==ADE_ZYNQ)
    #undef ADE_CONFIGURATION_INTERACTIVE
    #ifndef ADE_FP_PRECISION
        #define ADE_FP_PRECISION ADE_USE_SINGLE_PREC
    #endif
    #undef ADE_FFT_IMP
    #ifndef ADE_BLAS_IMPLEMENTATION //to make it overrideable from makefile
        #define ADE_BLAS_IMPLEMENTATION ADE_USE_BLAS_NEON
    #endif
#endif


#if (ADE_TARGET_MODE==ADE_DEBUG)
#define ADE_CHECK_INPUTS ADE_CHECK_INPUTS_TRUE
#define ADE_CHECK_RETURNS ADE_CHECK_RETURNS_TRUE
#define ADE_CHECK_MEM ADE_CHECK_MEM_TRUE
#elif  (ADE_TARGET_MODE==ADE_RELEASE)
#define ADE_CHECK_INPUTS ADE_CHECK_INPUTS_FALSE
#define ADE_CHECK_RETURNS ADE_CHECK_RETURNS_FALSE
#define ADE_CHECK_MEM ADE_CHECK_MEM_FALSE
#endif

#ifndef ADE_MEX_PRINT
#define ADE_LOG(...) fprintf(__VA_ARGS__)
#else
#define ADE_LOG(...) mexPrintf(__VA_ARGS__)
#endif

#define ADE_PRINT_FLOAT_WIDTH (20)
#define ADE_PRINT_FLOAT_PRECISION (18)


/*************************** VERBOSITY LEVELS ***********************/

//#define ADE_NO_VERBOSITY (0)
//#define ADE_RET_ERROR;//RROR_LEVEL (1)
//#define ADE_WARNING_LEVEL (2)
//#define ADE_INFO_LEVEL (3)

//#define ADE_VERBOSITY_LEVEL ADE_RET_ERROR;//RROR_LEVEL

/*************************** BLAS ****************************/
#if defined(_WIN32) || defined(__hpux)
#define FORTRAN_WRAPPER(x) x
#else
#define FORTRAN_WRAPPER(x) x ## _
#endif

/************************** FIR ********************************/
#define ADE_FIR_MAX_ORDER (1024)
/************************** IIR ********************************/
#define ADE_IIR_MAX_N_SECTIONS (5)
#define ADE_IIR_MAX_BUFF_LEN (ADE_GENERAL_MALLOC_BUFF_LEN)
/************************** POLYFIT ********************************/
#define ADE_POLYFIT_MAX_N_BREAKS (64)
#define ADE_POLYFIT_MAX_POLY_ORDER (5)
/************************ DOWNSAMPLER ****************************/
#define ADE_DOWNSAMPLER_PRINT_FLOAT_WIDTH (20)
#define ADE_DOWNSAMPLER_PRINT_FLOAT_PRECISION (18)
//#define ADE_DOWNSAMPLER_MAX_BUFF_LEN (ADE_GENERAL_MALLOC_BUFF_LEN)
/************************ UPSAMPLER ****************************/
#define ADE_UPSAMPLER_PRINT_FLOAT_WIDTH (20)
#define ADE_UPSAMPLER_PRINT_FLOAT_PRECISION (18)
//#define ADE_UPSAMPLER_MAX_BUFF_LEN (ADE_GENERAL_MALLOC_BUFF_LEN)

/****************************** BLOW ***********************************/
#define IIR1_N_SECTIONS (4)
#define IIR2_N_SECTIONS (1)
#define POLY_N_BREAKS (6)
#define POLY_ORDER (3)
#define ADE_BLOW_PRINT_FLOAT_WIDTH (20)
#define ADE_BLOW_PRINT_FLOAT_PRECISION (18)
//#define ADE_BLOW_MATLAB_PATH "/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Main_scripts/"
//#define ADE_BLOW_MAT_SCRIPT "blow_fine_control_swlike.m"
/***************************** ENVIRONMENT *******************************/


/*************************** MATLAB *******************************/
#define ADE_MAX_CHARS (128)
#define ADE_MAX_SEGMENT_LINES (256)
#define ADE_MAT_WAIT printf("Hit return to continue\n\n");fgetc(stdin);
#define ADE_MATLAB_EXE "/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab"
#define ADE_SNAP_SCRIPT "/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Snap/Matlab/Main_scripts/impulse_frame.m"
#define ADE_SNAP_WS "./snap_config_ws.mat"
/****** FFT ******/
#define ADE_FFT_MAX_BUFF_LEN (1024)
/**************************** FFTW **********************************/
#define ADE_FFTW_PLAN_FLAGS FFTW_ESTIMATE //FFTW_MEASURE //FFTW_PATIENT // //read manual pg.18
#define ADE_FLOAT_SIZE (4)
#define ADE_DOUBLE_SIZE (8)
#ifdef ADE_USE_FFTW_THREADS
#define ADE_FFTW_NTHREADS (2)
#else
#define ADE_FFTW_NTHREADS (0)
#endif
/******************************* UTILS ******************************/
#define PLUS_ONE(x) (x+1)
#define PRINT_IDX(x) PLUS_ONE(x)
#define ADE_UTILS_PRINTF_FLOAT_WIDTH (24)
#define ADE_UTILS_PRINTF_FLOAT_PRECISION (16)
/* Period parameters Marsenne twister*/
#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define MT_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define MT_LOWER_MASK 0x7fffffffUL /* least significant r bits */

/****************************** Error Handler *******************************/
#define ADE_ERRSTACK_DEPTH (128)
#define ADE_ERRSTRING_LEN (128)

/********************** NR Utils ***************/
#define NR_M 7
#define NR_STACK 50
/********************* PROXIMITY *********************/
#define ADE_PROXIMITY_MAX_BUFF_LEN (1024)

/***** DECIMATOR ********/


/**** SNAP ****/
#define ADE_SNAP_MAX_BUFF_LEN (ADE_GENERAL_MALLOC_BUFF_LEN)
#define ADE_SNAP_MAX_INDEXES (4)
#define ADE_SNAP_MAX_N_POW_SLOTS (4)
#define ADE_SNAP_MAX_EXTRACT_LEN (1024)
#define ADE_SNAP_MAX_FFT_LEN (ADE_FFT_MAX_BUFF_LEN)
/*********** SOURCES ***************/

#define ADE_SOURCES_MAX_BUFF_LEN (ADE_GENERAL_MALLOC_BUFF_LEN)
/* Period parameters Marsenne twister*/
#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define MT_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define MT_LOWER_MASK 0x7fffffffUL /* least significant r bits */

/**************** UDP SENDER ********************/
#define ADE_UDP_SENDER_MAX_BUFFER_BYTES (1522)
/**************** UDP RECEIVER ********************/
#define ADE_UDP_RECEIVER_MAX_BUFFER_BYTES (1522)






#endif //_ADE_DEFINES_H
