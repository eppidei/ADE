#ifndef _ADE_SCDF_INTERFACE_H
#define _ADE_SCDF_INTERFACE_H
#include "headers/ADE_typedefs.h"

#define BLOW_BIT (0)
#define SNAP_BIT (1)
#define PROXY_BIT (2)

#define BLOW_FLAG (1<<BLOW_BIT)
#define SNAP_FLAG (1<<SNAP_BIT)
#define PROXY_FLAG (1<<PROXY_BIT)

typedef enum ADE_SensorType_S{
    ADE_Invalid=-1,
    ADE_Accelerometer,
    ADE_Gyroscope,
    ADE_Magnetometer,
    ADE_Proximity,
    ADE_Light,
    ADE_AudioInput,
    ADE_NumTypes }ADE_SensorType_T ;

typedef struct ADE_SCDF_Input_Int_S
{
ADE_SensorType_T type;
ADE_INT32_T rate;
ADE_UINT64_T timeid;                // will be the same for all data harvested in the same call
ADE_INT32_T num_frames;
ADE_INT32_T num_channels;
ADE_UINT64_T *timestamp;             // time of the sensor reading as reported by the sensor
ADE_FLOATING_T *data;
} ADE_SCDF_Input_Int_T;

typedef struct ADE_SCDF_Output_Int_S
{
ADE_FLOATING_T Fs_data;
ADE_FLOATING_T *p_data;
ADE_UINT32_T n_data;
ADE_BOOL_T state;
ADE_UINT32_T n_calls_state_logic;
ADE_BOOL_T toggle;
ADE_UINT32_T n_calls_toggle_logic;
} ADE_SCDF_Output_Int_T;
#endif //_ADE_SCDF_INTERFACE_H
