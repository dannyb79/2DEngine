/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <SDL.h>
#include <EngineCommon.h>


#ifndef _MOVIEMANAGER_H_INCLUDE
#define _MOVIEMANAGER_H_INCLUDE

extern "C"  
{  
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libswscale/swscale.h"  
#include "libavutil/pixfmt.h"  
};  

#pragma comment(lib, "avcodec.lib")

/*
	structure to pass data to all Movie object to display current streaming texture
*/
typedef struct {
	bool				isPlaying;
	SDL_Texture			*texture;
	int					width;
	int					height;
} MovieData_t;


namespace  MovieManager {

	// reset all data
	void Reset();

	/*
		MovieManager initialization

		MovieManager needs to know a complete list of all available movies and where to find them
		This function is called in engine initialization and tell MovieManager the total number of
		available movies (totalMoviesFilenames) and a list of "path+filename" (e.g "subfolder/example.mov")

		const char *movies_filenames[ movieid_max ] =
		{
			"movie_1.flv",
			"movie_2.flv"
		};
	*/
	bool SetMoviesFilesInfo( long totalMoviesFiles, const char **moviesFiles );

	// when a stream ends naturally we communicate to the game the movie index
	void SetMovieEndCallback( void (*Callback)(int) );

	// MovieManager termination
	void Terminate();

	/*
		Start play a movie
		
		This function start from scratch the streaming of a movie identified by movieId:
		movieId should be a enumerative number related to the file names passed with SetMoviesFilenames

		typedef enum {
			movieid_movie_1,	// this id is related to movie_1.flv (see example above)
			movieid_movie_2,	// this id is related to movie_2.flv (see example above)
			movieid_max
		} movieid_t;
	*/
	bool PlayMovie( int movieId, int n_times );

	/*
		Stop playing a movie
		
		This function stop streaming of a movie identified by movieId. Immediately all Movie objects
		playing this video should disappear
	*/
	bool StopMovie( int movieId );

	// return width and height of movie related to movieId
	Size_t GetMovieSize( int movieId );

	// This function is called each frame by the engine and update textures of all active streamings
	void Update();

	// This function is called each frame by all visible Movie objects in the scene to retrieve current 
	// stream texture to display 		
	bool GetMovieData( int movieId, MovieData_t* movieData );
};


#endif