#ifndef _MIDI_PACKAGER_H
#define _MIDI_PACKAGER_H

#include "my_typedefs.h"
#include <Windows.h>

#ifdef __cplusplus
extern "C"
{
#endif
	int swap_addresses(unsigned long* start_address_i, unsigned long* half_address_i, unsigned long** producer_address_io, unsigned long** consumer_address_io);
int midi_packer(unsigned short* blow_array,unsigned int dec_factor, unsigned long *midi_buffer,dsp_float_t* pow_array,
	unsigned int audio_buffer_per_midi_buffer,unsigned int *n_audio_buff_count_o,unsigned int *midi_long_wrote,DWORD dWnoteon_ticks);
	int wav_swap_addresses(short* start_address_i, short* half_address_i, short** producer_address_io, short** consumer_address_io);

#ifdef __cplusplus
}
#endif


#endif