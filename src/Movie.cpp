/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <SDL.h>
#include "Movie.h"
#include "MovieManager.h"
#include "Engine.h"


Movie::Movie( unsigned int movieId, unsigned int tag, unsigned int zOrder )
{
	this->movieId	= movieId;
	this->tag		= tag;
	this->zOrder	= zOrder;
	Size_t movieSize = MovieManager::GetMovieSize( movieId );
	this->width		= movieSize.w;
	this->height	= movieSize.h;
}

void Movie::SetMovieId( unsigned int movieId )
{
	this->movieId	= movieId;
	Size_t movieSize = MovieManager::GetMovieSize( movieId );
	this->width		= movieSize.w;
	this->height	= movieSize.h;
}

void Movie::OnClick()
{
	Engine::GetConfig()->ObjectClickedCallback( this, tag );
}

void Movie::Draw()
{
	MovieData_t movieData;
	bool		result = false;
	
	result = MovieManager::GetMovieData( movieId, &movieData );
	if( result ) {
		//printf( "Movie::Draw frameData.texture %p %dx%d\n", movieData.texture, movieData.width, movieData.height );
		if( movieData.texture != NULL && movieData.isPlaying ) { 
			DrawTexture( movieData.texture, movieData.width, movieData.height );
		}
	}
}

