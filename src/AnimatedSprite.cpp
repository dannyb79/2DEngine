/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <SDL.h>
#include "AnimatedSprite.h"
#include "Engine.h"


AnimatedSprite::AnimatedSprite( SDL_Texture **textures, unsigned int n_frames, unsigned int tag, unsigned int zOrder )
{
	this->textures		= textures;
	this->tag			= tag;
	this->zOrder		= zOrder;
	SDL_QueryTexture( textures[ 0 ], NULL, NULL, &this->width, &this->height );
	this->total_frames	= n_frames;
	this->current_frame = 0;
	this->isPlaying		= false;
	this->current_loop	= 0;
	this->total_loops	= 0;
}

void AnimatedSprite::PlayOnce()
{
	isPlaying		= true;
	current_frame	= 0;
	current_loop	= 0;
	total_loops		= 1;
}

void AnimatedSprite::Play( unsigned int n_times )
{
	isPlaying		= true;
	current_frame	= 0;
	current_loop	= 0;
	total_loops		= n_times;
}

void AnimatedSprite::Loop()
{
	isPlaying		= true;
	current_frame	= 0;
	current_loop	= 0;
	total_loops		= -1;
}

void AnimatedSprite::Stop()
{
	isPlaying		= false;
	current_frame	= 0;	// reset to the beginning of animation
	current_loop	= 0;
	total_loops		= 0;
}

void AnimatedSprite::OnClick()
{
	Engine::GetConfig()->ObjectClickedCallback( this, tag );
}

void AnimatedSprite::Draw()
{
	if( textures != NULL ) {
		// check index before use it
		if( current_frame >= total_frames ) {
			current_frame = 0;
		}
		// print current frame
		DrawTexture( textures[ current_frame ], width, height );
		// if animation is active...
		if( isPlaying ) {

			// whatever FPS we don't update stream if at least 20 ms elapsed from last update
			static Uint32	lastUpdateTicks	= 0;
			Uint32 now  = Engine::GetDrawSceneTicks();
			Uint32 diff = now - lastUpdateTicks;
			if( diff < 20 ) {
				return; 
			}
			lastUpdateTicks = now;

			// incremnt current index
			current_frame += 1;
			// if we reached the end of animation...
			if( current_frame >= total_frames ) {
				// reset current frame to the beginning
				current_frame = 0;
				// se total_loops e' negativo significa che l'animazione deve essere ripetuta continuamente,
				// altrimenti se total_loops e' impostata con un valore positivo significa che dobbiamo eseguire
				// un numero determinato di animazioni complete

				// if total_loops is negative the animation must continue infinitely, else if we set total_loops
				// with a positive value, the animation has a finite number of cycles
				if( total_loops > 0 ) {
					// increment of executed animations
					current_loop += 1;
					// if we reached the end...
					if( current_loop >= total_loops ) {
						// ...stop animating sprite
						isPlaying = false;
					}
				}
			}
		}
	}
}