/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include "SoundManager.h"


int SoundManager::Play( int channel, Mix_Chunk *chunk, int loops )
{
	int used_channel = Mix_PlayChannel( channel, chunk, loops );
	// in case of error log the fact and return value
	if( used_channel < 0 ) {
		printf( "Error playing sound, return value %d\n", used_channel );
	}
	// return used channel for this sound
	return used_channel;
}

int SoundManager::PlayTimed( int channel, Mix_Chunk *chunk, int loops, int duration )
{
	int used_channel = Mix_PlayChannelTimed( channel, chunk, loops, duration );	
	if( used_channel < 0 ) {
		printf( "Error playing sound, return value %d\n", used_channel );
	}
	// return used channel for this sound
	return used_channel;
}

void SoundManager::StopChannel( int channel )
{
	// stop current playing channel 
	Mix_HaltChannel( channel );
}