#ifndef _DEFINES_H
#define _DEFINES_H

#define SAFE_RET_VAL (0)
#define MAX_BUFFER_LEN (1024U)
#define MAX_IIR_FILT_LEN (3U)
#define MAX_FIR_FILT_LEN (256U)



/*********************** DSP PARAMETERS ***************************************/


#define N_BIT (16.0F)
#define MARGIN		(0.3F)
#define POW_THRESH_HIGH  (0.25F)
#define POW_THRESH_LOW  (4e-3F)
#define EVAL_TIME		(100e-3F)
#define FS			(44100.0F)
#define TIME_POW_THRESH_ATTACK  (1.0e-3F)
#define TIME_POW_THRESH_RELEASE  (2.0e-3F)
#define RUNNING_POW_WIN_TIME_FAST  (1.0e-3F)
#define RUNNING_POW_WIN_TIME_SLOW  (20.0e-3F)
#define MAX_FILT_LEN (512U)
#define N_SOS_SECTIONS (3U)
#define N_FIR (1U)

#endif
