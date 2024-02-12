/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include "MovieManager.h"
#include "Engine.h"
#include "Misc.h"

// maximum length of movie path+filename 
#define MOVIEMANAGER_FILENAME_MAX_LENGTH	128

// this is the maximum number of streams, that is the maximum number of 
// movie file that the game should deal
#define MOVIEMANAGER_MAX_STREAMS			128

/*
	stream data
*/
typedef struct {
	char				filename[ MOVIEMANAGER_FILENAME_MAX_LENGTH ];
	int					movieId;
	int					width;
	int					height;
    AVFormatContext		*pFormatCtx;  
	AVCodecContext		*pCodecCtx;
	AVCodec				*pCodec;
	AVFrame				*pFrame1;
	AVFrame				*pFrame2;
	struct SwsContext	*sws_ctx;
	unsigned int		videoStream;
	int					frame_complete;
	int					frame_data_size;
	uint8_t				*frame_data_buffer;
	SDL_Texture			*frame_texture;
	bool				isPlaying;
	int					current_time;
	int					total_times;
} Stream_t;

/*
	pointer to the table containing all available movies path+filename, example

	const char *movies_filenames[ movieid_max ] =
	{
		"movie_1.flv",
		"movie_2.flv"
	};
*/
static const char		**moviesFiles		= NULL;
// total number of available movie files
static long				totalMoviesFiles	= 0;	
// this is the streams array containing data of each (used) movie files (mov,mpg,mp4) used by the game
static Stream_t			streams[ MOVIEMANAGER_MAX_STREAMS ];
// this is the total number of streams opened by the game
static long				totalStreams		= 0;
// pointer to callback called when a movie ends
static void				( *MovieEndCallback )( int ) = NULL;



// reset movie manager data
void MovieManager::Reset()
{
	// reset MovieManager data
	totalMoviesFiles	= 0;
	moviesFiles			= NULL;
	totalStreams		= 0;
	memset( &streams, 0, sizeof( streams ) );
	MovieEndCallback	= NULL;
}

// set a complete list of all available movies 
bool MovieManager::SetMoviesFilesInfo( long totalFiles, const char **files )
{
	// store total number of available movie files and their names
	moviesFiles			= files;
	totalMoviesFiles	= totalFiles;
	// we can't manage more than MOVIEMANAGER_MAX_STREAMS streams
	if( totalMoviesFiles > MOVIEMANAGER_MAX_STREAMS ) {
		// if total number is greater than total streams returns false
		return false;
	}
	return true;
}

// when a stream ends naturally we communicate to the game the movie index
void MovieManager::SetMovieEndCallback( void (*Callback)(int) )
{
	MovieEndCallback = Callback;
}

// MovieManager termination
void MovieManager::Terminate()
{
	// close all streams and free memorys
	for( int i = 0; i < totalStreams; i++ ) {
		streams[ i ].isPlaying = false;
		avformat_close_input( &streams[ totalStreams ].pFormatCtx );
	}
}

static bool GetFilenameFromMovieId( int movieId, char *filename )
{
	char tmpstr[ MOVIEMANAGER_FILENAME_MAX_LENGTH ];
	// check index of movie
	if( ( movieId < 0 ) || ( movieId >= totalMoviesFiles ) ) {
		// invalid index
		return false;
	}
	// movie must be placed into "movies" folder
	sprintf( filename, "movies/" );
	// last part of string is the file name, e.g. "example.mov"
	sprintf( tmpstr, "%s", moviesFiles[ movieId ] );
	// add file name to folder (= "movies/" + "example.mov")
	strcat( filename, tmpstr );
	return true;
}

/*
	create a new stream related to a single movie file
*/
static bool CreateNewStream( int movieId ) 
{
	AVPixelFormat	dst_fix_fmt		= AV_PIX_FMT_BGRA;
	char			filename[ MOVIEMANAGER_FILENAME_MAX_LENGTH ];

	// reset string that will contain name of fila, e.g. "subfolder/example.mov" 
	memset( filename, 0, sizeof( filename ) );
	// get filename from movie index
	if( GetFilenameFromMovieId( movieId, filename ) == false ) {
		return false;
	}
	// open video file
	if( avformat_open_input( &streams[ totalStreams ].pFormatCtx, filename, NULL, NULL ) != 0 ) {  
        return false; // Couldn't open file  
    }  
    // find stream info
    if( avformat_find_stream_info( streams[ totalStreams ].pFormatCtx, NULL ) < 0 ) {  
        return false; // Couldn't find stream information  
    }  
    // print stream information on standard error  
    av_dump_format( streams[ totalStreams ].pFormatCtx, 0, filename, 0 ); 
    // find the first stream of type "video"
	streams[ totalStreams ].videoStream = -1;
    for( unsigned int i = 0; i < streams[ totalStreams ].pFormatCtx->nb_streams; i++ ) {  
        if( streams[ totalStreams ].pFormatCtx->streams[ i ]->codec->codec_type == AVMEDIA_TYPE_VIDEO ) {  
            streams[ totalStreams ].videoStream = i;  
            break;  
        }  
    }  
	// if any video stream terminates with error
    if( streams[ totalStreams ].videoStream == -1 ) {  
        return false; 
    }  
    // get pointer to codec
    streams[ totalStreams ].pCodecCtx = streams[ totalStreams ].pFormatCtx->streams[ streams[ totalStreams ].videoStream ]->codec;  

    // find a valid decoder
    streams[ totalStreams ].pCodec = avcodec_find_decoder( streams[ totalStreams ].pCodecCtx->codec_id );  
	// if any valid decoder is found exit with error (= unsupported codec)
	if( streams[ totalStreams ].pCodec == NULL ) {  
		return false; 
    } 		

	// open codec
    AVDictionary		*optionsDict	= NULL;		// this is needed for opening codec
    if( avcodec_open2( streams[ totalStreams ].pCodecCtx, streams[ totalStreams ].pCodec, &optionsDict ) <0 ) {  
        return false; 
    }  

	// allocate memory for frame buffers 
    streams[ totalStreams ].pFrame1	= av_frame_alloc();  
    streams[ totalStreams ].pFrame2	= av_frame_alloc();  
    if( streams[ totalStreams ].pFrame1 == NULL || streams[ totalStreams ].pFrame2 == NULL ) {  
        return false;  
    }  

	// allocate memory for frame data
	streams[ totalStreams ].frame_data_size = avpicture_get_size( dst_fix_fmt, streams[ totalStreams ].pCodecCtx->width, streams[ totalStreams ].pCodecCtx->height );  
	streams[ totalStreams ].frame_data_buffer = (uint8_t *)av_malloc( streams[ totalStreams ].frame_data_size * sizeof( uint8_t ) );  
	if( !streams[ totalStreams ].frame_data_buffer ) {
		return false;
	}
	// like frames to data buffer
	avpicture_fill((AVPicture *)streams[ totalStreams ].pFrame2, streams[ totalStreams ].frame_data_buffer, dst_fix_fmt, streams[ totalStreams ].pCodecCtx->width, streams[ totalStreams ].pCodecCtx->height );  

 	// get context for frame scaling 
    streams[ totalStreams ].sws_ctx = sws_getContext(  
		streams[ totalStreams ].pCodecCtx->width,  
		streams[ totalStreams ].pCodecCtx->height,  
		streams[ totalStreams ].pCodecCtx->pix_fmt,  
		streams[ totalStreams ].pCodecCtx->width,	
		streams[ totalStreams ].pCodecCtx->height,  
		dst_fix_fmt, 
		SWS_BILINEAR,  
		NULL,  
		NULL, 
		NULL );  

	// save name of file: each movie file has its own stream 
	// NOTE it would be better an integer as search key, it should be faster during search!
	sprintf( streams[ totalStreams ].filename, filename );

	// create the texture related to the stream
	streams[ totalStreams ].frame_texture = SDL_CreateTexture( 
		Engine::GetRenderer(), 
		SDL_PIXELFORMAT_ARGB8888, 
		SDL_TEXTUREACCESS_STREAMING, 
		streams[ totalStreams ].pCodecCtx->width,  
		streams[ totalStreams ].pCodecCtx->height );
	if( !streams[ totalStreams ].frame_texture ) {
		printf( "Unable to create streaming texture for %s with size %dx%d\n", 
			filename, streams[ totalStreams ].pCodecCtx->width, streams[ totalStreams ].pCodecCtx->height );
		return false;
	}
	// texture render modality
	SDL_SetTextureBlendMode( streams[ totalStreams ].frame_texture, SDL_BLENDMODE_BLEND );
	// clear texture
	Misc::ClearTexture( streams[ totalStreams ].frame_texture, RGBA( 0, 0, 0, 0 ) );
	// store locally movie id and size
	streams[ totalStreams ].movieId = movieId;
	streams[ totalStreams ].width	= streams[ totalStreams ].pCodecCtx->width;
	streams[ totalStreams ].height	= streams[ totalStreams ].pCodecCtx->height;

	// increment total number of streams
	totalStreams += 1;

	// stream successfully created
	return true;
}

static void RewindStream( int streamId ) 
{
	av_seek_frame( streams[ streamId ].pFormatCtx, 0, 0, 0 );
	streams[ streamId ].frame_complete = false;
	memset( streams[ streamId ].frame_data_buffer, 0, streams[ streamId ].frame_data_size * sizeof( uint8_t ) );
}

static int GetStreamIdFromMovieId( int movieId )
{
	for( int i = 0; i < totalStreams; i++ ) {
		if( streams[ i ].movieId == movieId ) {
			return i;
		}
	}
	return -1;
}

bool MovieManager::PlayMovie( int movieId, int n_times ) 
{
	signed int	streamId = 0;
	// get stream index in the "streams" array
	streamId = GetStreamIdFromMovieId( movieId );
	if( streamId < 0 ) {
		// the stream for this movie doesn't exist, we'll create a new one
		if( CreateNewStream( movieId ) == false ) {
			printf( "Unable to find or create stream for movie id %d. Missing file?\n", movieId );
			return false;
		} 
		// the new stream is always added after last item
		streamId = ( totalStreams - 1 );
	}
	// restart the stream from the beginning of file
	RewindStream( streamId );
	// current stream is playing, each frame MovieManager::Update() is called and updates
	// streaming of streams with this flag set to true
	streams[ streamId ].isPlaying = true;
	// reset total number of complete plays
	streams[ streamId ].current_time	= 0;
	// store total number of plays requested
	streams[ streamId ].total_times		= n_times;

	// TODO pulizia texture???
	//Misc::ClearTexture( streams[ streamId ].frame_texture, RGBA( 0, 0, 0, 0 ) );

	return true;
}

bool MovieManager::StopMovie( int movieId ) 
{
	int streamId = 0;
	// get stream index from movie index
	streamId = GetStreamIdFromMovieId( movieId );
	if( streamId < 0 ) {
		return false;
	} 
	// stop streaming, all Movie object should immediately disappear
	streams[ streamId ].isPlaying = false;
	// reposition read pointer to the beginning of file
	RewindStream( streamId );
	return true;
}

void MovieManager::Update()
{
	SDL_Rect		srcRect; 
	AVPacket		packet; 
	int				read_result = 0;

	// whatever FPS we don't update stream if at least 20 ms elapsed from last update
	static Uint32	lastUpdateTicks	= 0;
	Uint32 now  = Engine::GetDrawSceneTicks();
	Uint32 diff = now - lastUpdateTicks;
	if( diff < 20 ) {
		return; 
	}
	lastUpdateTicks = now;

	// scan all streams
	for( int i = 0; i < totalStreams; i++ ) {
		// if the stream is currently playing...
		if( streams[ i ].isPlaying ) {
			// reset complete frame flag
			streams[ i ].frame_complete = 0;  
			// start reading stream packet after packet...
			do {
				// read a new packet
				read_result = av_read_frame( streams[ i ].pFormatCtx, &packet );
				// if we have a positive result...
				if( read_result >= 0 ) {
					// ...we check that packet is related to video stream (not audio or else) 
					if( packet.stream_index == streams[ i ].videoStream ) {
						// decode packet 
						avcodec_decode_video2( streams[ i ].pCodecCtx, streams[ i ].pFrame1, &streams[ i ].frame_complete, &packet );  
						// once we have a complete frame...
						if( streams[ i ].frame_complete ) {  
							// free memory allocated for packet...
							av_free_packet( &packet );  
							// ...and quit from loop
							break;
						} else {
							// we need more packets to complete the frame
						}
					} else {
						// packet is not related to our video stream, discard it!
					}
				} else {
					// END OF STREAM
					// however we reset the read pointer to the start of the stream
					RewindStream( i );

					// now we need to check how many times we must play this movie
					// if a total number of plays has been set...
					if( streams[ i ].total_times > 0 ) {
						// ...increment the counter of total whole plays
						streams[ i ].current_time += 1;
						// ...if we reached the total number of plays...
						if( streams[ i ].current_time >= streams[ i ].total_times ) {
							// ...stop the streaming
							streams[ i ].isPlaying = false;

							/*
								qui dobbiamo comunicare al gioco in qualche modo che la riproduzione del video è terminata:
								potremmo usare una callback oppure un evento comunicando l'indice (movieId) del video
							*/
							if( MovieEndCallback != NULL ) {
								MovieEndCallback( streams[ i ].movieId );
							}
						}
					} else {
						// if streams[ i ].total_times is negative or zero we let play this movie infinitely
					}

					// in case of negative value we should not free packet with av_free_packet( &packet )
					// https://stackoverflow.com/questions/18024835/ffmpeg-libavcodec-memory-management

					// we have reache the end of the stream: we must exit the loop in any case!
					break;
				}
				// free allocated memory for current packet and proceed reading the stream
				av_free_packet( &packet );  

			} while( true );

			if( streams[ i ].frame_complete ) {
				// rescaling of packets data
				sws_scale(  
					streams[ i ].sws_ctx,  
					(uint8_t const * const *)streams[ i ].pFrame1->data,  
					streams[ i ].pFrame1->linesize,  
					0,  
					streams[ i ].pCodecCtx->height,  
					streams[ i ].pFrame2->data,  
					streams[ i ].pFrame2->linesize  
					);  
				// set source area with movie widht and height
				srcRect.x = 0;  
				srcRect.y = 0;  
				srcRect.w = streams[ i ].pCodecCtx->width;  
				srcRect.h = streams[ i ].pCodecCtx->height;  
				// update texture with current frame, this texture is used by Movie objects			
				SDL_UpdateTexture( 
					streams[ i ].frame_texture, 
					&srcRect, 
					streams[ i ].pFrame2->data[ 0 ], 
					streams[ i ].pFrame2->linesize[ 0 ] 
				);  
			}
		}
	}
}

bool MovieManager::GetMovieData( int movieId, MovieData_t* movieData )
{
	int		streamId = 0;

	streamId = GetStreamIdFromMovieId( movieId );
	if( streamId < 0 ) {
		return false;
	}	

	memset( movieData, 0, sizeof( MovieData_t ) );
	movieData->texture		= streams[ streamId ].frame_texture;
	movieData->width		= streams[ streamId ].width;
	movieData->height		= streams[ streamId ].height;
	movieData->isPlaying	= streams[ streamId ].isPlaying;
	return true;
}

Size_t MovieManager::GetMovieSize( int movieId )
{
	signed int	streamId = 0;
	Size_t		result;

	memset( &result, 0, sizeof( Size_t ) );
	// get stream index in the "streams" array
	streamId = GetStreamIdFromMovieId( movieId );
	if( streamId < 0 ) {
		// the stream for this movie doesn't exist, we'll create a new one
		if( CreateNewStream( movieId ) == false ) {
			printf( "Unable to find or create stream for movie id %d. Missing file?\n", movieId );
			return result;
		} 
		// the new stream is always added after last item
		streamId = ( totalStreams - 1 );
	}
	result.w = streams[ streamId ].width; 
	result.h = streams[ streamId ].height;
	return result;
}
