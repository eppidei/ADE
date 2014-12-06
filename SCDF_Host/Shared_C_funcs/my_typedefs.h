#ifndef _MY_TYPEDEFS_H
#define _MY_TYPEDEFS_H
#include "my_defines.h"

#include <stdint.h>
#if (_OS_==1)
#include <Windows.h>
typedef BOOL s_bool;
#else
#include <stdbool.h>
typedef bool s_bool;
#endif



typedef int8_t  s_int8;
typedef char    s_char;
typedef int16_t   s_int16;
typedef int32_t   s_int32;
typedef int64_t   s_int64;
typedef long    s_long;

typedef unsigned char 	s_byte;
typedef uint16_t 		s_uint16;
typedef uint32_t   		s_uint32;
typedef uint64_t 		s_uint64;
typedef unsigned long    s_ulong;

typedef float   s_float;
typedef double  s_double;
typedef s_float s_sample;

typedef s_float dsp_float_t;

/*typedef  s_int32 SensorType;*/

typedef enum SensorType{
    Invalid=-1,
    Accelerometer,
    Gyroscope,
    Magnetometer,
    Proximity,
    Light,
    AudioInput,
    NumTypes }SensorType_T ;


#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
typedef struct SensorData {
        SensorType_T type;
        s_int32 rate;           /* time of the sensor reading as reported by the sensor*/
        s_uint64 timeid;                /* will be the same for all data harvested in the same call*/
        s_int32 num_frames;
        s_int32 numChannels;
        s_uint64 *ptimestamp; 
        s_sample *pdata;
        s_sample data[MAX_BUFFER_LEN]; /*uint32 per nthol*/
        s_uint64 timestamp[MAX_BUFFER_TIMESTAMP];
    } SensorData_T;

#pragma pack(pop)


/*********************************** MIDI***************************************/
#if (_OS_==1) 

typedef  union
{
    unsigned long word;
    unsigned char data[STANDARD_MIDI_MESSAGE_LEN_BYTES];
} midimessage_T ;

typedef struct midi_thread_params {
	HMIDISTRM p_handle;
	MIDIHDR   *p_midi_header;

} midi_thread_params_T;

typedef struct midiout_thread_params {
	HMIDIOUT p_handle;
	midimessage_T   *p_midi_message;

} midiout_thread_params_T;

typedef struct wavout_thread_params {

HWAVEOUT p_handle;
LPWAVEHDR p_header;
UINT head_size;

}wavout_thread_params_T;

#else

#endif

/***************************************BLOW*********************************************/

typedef struct Blowargs {

dsp_float_t* in_buff;
unsigned int n_sos_sections;
dsp_float_t** section_num;
dsp_float_t** section_denom;
dsp_float_t* section_gain;
dsp_float_t** section_states;
dsp_float_t** section_buffers;
unsigned int n_fir;
dsp_float_t** n_fir_coeffs;
dsp_float_t** fir_coeffs;
dsp_float_t* fir_gains;
dsp_float_t** fir_states;
dsp_float_t* blow_state_array;
dsp_float_t* run_pow_fast;
dsp_float_t* run_pow_slow;
dsp_float_t* eval_counter;
dsp_float_t* eval_pow;
dsp_float_t* eval_timer;
unsigned short *pstate;
unsigned int num_samples;
unsigned int running_pow_win_fast;
unsigned int running_pow_win_slow;
dsp_float_t pow_thresh_high;
dsp_float_t pow_thresh_low;
dsp_float_t sat_thresh;
unsigned int n_sat_thres;
unsigned int eval_time_samples;
unsigned int  n_pow_thres_attack;
unsigned int  n_pow_thres_release;
}Blowargs_T;

 
#endif
