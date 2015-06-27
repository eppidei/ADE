#include "midi_packager.h"
#include "my_defines.h"
#include <stdio.h>


int swap_addresses(unsigned long* start_address_i, unsigned long* half_address_i, unsigned long** producer_address_io, unsigned long** consumer_address_io)
{
	

	if (*producer_address_io==start_address_i && *consumer_address_io==half_address_i)
	{
		*producer_address_io=half_address_i;
		*consumer_address_io=start_address_i;
	}
	else if (*producer_address_io==half_address_i && *consumer_address_io==start_address_i)
	{
		*producer_address_io=start_address_i;
		*consumer_address_io=half_address_i;
	}
	else
	{
		printf("error in double buffering scheme\n");
		return -1;
	}


	return 0;
}

int wav_swap_addresses(short* start_address_i, short* half_address_i, short** producer_address_io, short** consumer_address_io)
{
	

	if (*producer_address_io==start_address_i && *consumer_address_io==half_address_i)
	{
		*producer_address_io=half_address_i;
		*consumer_address_io=start_address_i;
	}
	else if (*producer_address_io==half_address_i && *consumer_address_io==start_address_i)
	{
		*producer_address_io=start_address_i;
		*consumer_address_io=half_address_i;
	}
	else
	{
		printf("error in double buffering scheme\n");
		return -1;
	}


	return 0;
}



int midi_packer(unsigned short* blow_array,unsigned int dec_factor, unsigned long *midi_buffer,dsp_float_t* pow_array,unsigned int audio_buffer_per_midi_buffer,
	unsigned int *n_audio_buff_count_o,unsigned int *midi_long_wrote,DWORD dWnoteon_ticks)

{

	unsigned int i=0;
	unsigned int base_idx=0;
	static unsigned int k=0;
	static unsigned int midi_buff_idx=0;
	static unsigned int n_audio_buff_count = 0;
	midimessage_T message_note;

	if (n_audio_buff_count==0)
	{
		memset(midi_buffer,0,MAX_MIDI_BUFFER_SIZE_ULONG*4/DB_BUFFER_MULT_CONST);
	}



	for (i=k;i<MAX_BUFFER_LEN;i+=dec_factor)
	{
		if (blow_array[i]==3)
		{
		 message_note.data[0] =NOTEON;  // MIDI note-on message (requires to data bytes)
		 
		}
		else
		{
			message_note.data[0] =NOTEOFF;
		}
		message_note.data[1] = 0x3d;    // MIDI note-on message: Key number (60 = middle C)
		message_note.data[2] = 0x64;//(unsigned char)pow_array[i];   // MIDI note-on message: Key velocity (100 = loud)
		message_note.data[3] = 0;     // Unused parameter

		midi_buffer[midi_buff_idx]=dWnoteon_ticks;
		midi_buffer[midi_buff_idx+1]=0;
		midi_buffer[midi_buff_idx+2]=((unsigned long)MEVT_SHORTMSG<<24) | *(unsigned long*)&(message_note);;


		midi_buff_idx+=STANDARD_MIDI_INFO_PER_MESSAGE;
	}

	k = (i)%MAX_BUFFER_LEN;

	n_audio_buff_count++;

	if (n_audio_buff_count==audio_buffer_per_midi_buffer)
	{
		n_audio_buff_count=0;
	//	k=0;
		*midi_long_wrote=midi_buff_idx;
		midi_buff_idx=0;

	}

	*n_audio_buff_count_o=n_audio_buff_count;

	return 0;
}