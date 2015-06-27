#ifndef MY_DEFINES_H
#define MY_DEFINES_H

#define LINUX_OS (0)
#define WIN_OS (1)


#define SCDF_OK_RETURN (0)
#undef _MIDI_
#define _STK_
#define _OS_ LINUX_OS /*choose beetwen linux 0 and windows 1*/

#define ARTEMIDE_TEST


/********************** UDP RECEIVER DEFINES ***********************************/

#ifndef ARTEMIDE_TEST

#define MAX_BUFFER_LEN (4096)
#define MAX_BUFFER_TIMESTAMP (128)
#define RECEPTION_MAX_BUFFER_LEN (4)
#define MAX_CHAR_BUFF_LEN (18000)
#define MAX_SENSOR_FLOATS (256)
#define MAX_AUDIO_FLOAT (4096)
#undef VERBOSE_OSC
#undef VERBOSE_UDP
#undef VERBOSE_THREAD

#else

#define OSC_SIZE_256 (1314)
#define MAX_BUFFER_LEN (4096)
#define MAX_BUFFER_TIMESTAMP (128)
#define RECEPTION_MAX_BUFFER_LEN (4)
#define MAX_CHAR_BUFF_LEN OSC_SIZE_256
#define MAX_SENSOR_FLOATS (64)
#define MAX_AUDIO_FLOAT (256)
#undef VERBOSE_OSC
#undef VERBOSE_UDP
#undef VERBOSE_THREAD

#endif

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

/******NO MORE USED****/

/*#define SAT_THRESH (0.7F)
#define N_SAT_THRESH (20.0F)
#define N_POW_THRESH (89.0F)
#define EVAL_TIME_SAMPLES (4441.0F)
#define BBFAST_LEN (45)
#define BBSLOW_LEN (883)*/


/******************** MIDI PARAMETERS ******************************/

#define AUDIO_RATE_H (44100.0)
/*#define MIDI_RATE_H (2.20500)*/
#define APPROX_AUDIO_BUF_TIME_MS (5.0)
/*#define APPROX_MIDI_SINGLE_BUF_TIME_MS (APPROX_AUDIO_SINGLE_BUF_TIME_MS*2.0)
#define MIDI_SINGLE_BUFF_LEN_MULT_FACTOR (2.0) //indica quanto il buffer midi contiene informazioni temporali rispetto al buffer audio
#define APPROX_UDP_MAX_BUFFER_LEN_SAMPLES (APPROX_AUDIO_BUF_TIME_MS*AUDIO_RATE_H*1e-3)*/
/*#define N_MIDI_MESSAGES_PER_BUFFER (AUDIO_RATE/MIDI_RATE)*/
#define MAX_MIDI_MESSAGES_PER_SINGLE_BUF (32)
#define MAX_UDP_LEN_SAMPLES (8192)
#define DB_BUFFER_MULT_CONST (2)
#define STANDARD_MIDI_INFO_PER_MESSAGE (1)//3 per note on e  3 per note off
#define MAX_MIDI_BUFFER_SIZE_ULONG (DB_BUFFER_MULT_CONST*STANDARD_MIDI_INFO_PER_MESSAGE*MAX_MIDI_MESSAGES_PER_SINGLE_BUF)
#define STANDARD_MIDI_MESSAGE_LEN_BYTES (4)
/*#define MIDI_BPM (120)*/
#define MAX_SEM_COUNT (10)
#define SEMAPHORE_INIT_VALUE (1)

/*#undef USE_AFTERTOUCH*/

#define PROG_CHANGE (0xC0)
#define CONTROL_CHANGE (0xB0)
#define POLYPHONIC_AFTERTOUCH (0xA0)
#define CHANNEL_PRESSURE (0xD0)
#define NOTEON  (0x90)
#define NOTEOFF (0x80)
#define INSTRUMENT_CODE (58)
#define MIDI_DEVICE_ID (0)
#define BREATH_CONTROL_COARSE (2)
#define BREATH_CONTROL_FINE (34)
#define EXPRESSION (11)


/********** wav parameters ****************/

#define N_UDP_SAMPLES (256)
#define N_FRAMES_FOR_SINGLE_BUFF (16)
#define MAX_WAV_BUFFER_LEN (N_UDP_SAMPLES*N_FRAMES_FOR_SINGLE_BUFF*DB_BUFFER_MULT_CONST)
/*#define MAX_FRAMES_PER_SINGLE_WAV_BUFF (MAX_WAV_BUFFER_LEN/DB_BUFFER_MULT_CONST/N_UDP_SAMPLES)*/




#endif
