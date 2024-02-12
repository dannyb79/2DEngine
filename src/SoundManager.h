/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _SOUNDMANAGER_H_INCLUDE
#define _SOUNDMANAGER_H_INCLUDE

#include <SDL.h>
#include <SDL_mixer.h>

namespace SoundManager {

	/*
		this function is only a wrapper for Mix_PlayChannel to join common operations whenever a sound is played (e.g. log) 

		channel		Channel to play on, or -1 for the first free unreserved channel. 
		chunk		Sample to play. 
		loops		Number of loops, -1 is infinite loops. Passing one here plays the sample twice (1 loop). 

		Play chunk on channel, or if channel is -1, pick the first free unreserved channel. The sample will play for loops+1 number of times, 
		unless stopped by halt, or fade out, or setting a new expiration time of less time than it would have originally taken to play the loops, 
		or closing the mixer.
		Note: this just calls Mix_PlayChannelTimed() with ticks set to -1.

		Returns: the channel the sample is played on. On any errors, -1 is returned. 
	*/
	int Play( int channel, Mix_Chunk *chunk, int loops );

	/*
		this function is only a wrapper for Mix_PlayChannelTimed to join common operations whenever a sound is played (e.g. log) 

		channel    Channel to play on, or -1 for the first free unreserved channel. 
		chunk	   Sample to play. 
		loops		Number of loops, -1 is infinite loops. Passing one here plays the sample twice (1 loop). 
		ticks		Millisecond limit to play sample, at most. If not enough loops or the sample chunk is not long enough, 
					then the sample may stop before this timeout occurs. -1 means play forever. 

		If the sample is long enough and has enough loops then the sample will stop after ticks milliseconds. 
		Otherwise this function is the same as 4.3.3 Mix_PlayChannel.

		Returns: the channel the sample is played on. On any errors, -1 is returned. 
	*/
	int PlayTimed( int channel, Mix_Chunk *chunk, int loops, int duration );

	/*
		this function is only a wrapper for Mix_HaltChannel to join common operations whenever a sound is stopped (e.g. log) 

		channel		Channel to stop playing, or -1 for all channels. 

		Halt channel playback, or all channels if -1 is passed in.
		Any callback set by Mix_ChannelFinished will be called.

		Returns: always returns zero. (kinda silly) 
	*/
	void StopChannel( int channel );
};

#endif