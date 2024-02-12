/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <windows.h>
#include "Misc.h"
#include "Engine.h"


void Misc::CurrencyValueToUTF16String( int value, wchar_t *str, bool force_decimal, bool euro_symbol )
{
	wchar_t	tmp[ 10 ];
	int		integer = 0, n;

	// negative value are reset to zero
	if( value < 0 ) {
		value = 0;
	}
	// empty string with null terminator
	wsprintf( str, L"" );
	// insert the euro unicode simbol (0x20ac) into the final string
	if( euro_symbol ) {
		wsprintf( str, L"%c", 0x20ac );
	}
	// calculate integer part of value (in cents)
	integer		= value / 100;
	// XXX.000.000,00
	if( integer >= 1000000 ) {
		n = integer / 1000000;
		wsprintf( tmp, L"%d.", n );
		wcscat( str, tmp );
	}
	// 000.XXX.000,00
	if( integer >= 1000 ) {
		n = ( integer % 1000000 ) / 1000;
		if( integer >= 1000000 ) {
			wsprintf( tmp, L"%03d.", n );
		} else {
			wsprintf( tmp, L"%d.", n );
		}
		wcscat( str, tmp );
	}
	// 000.000.XXX,00
	n = ( integer % 1000 );
	if( integer >= 1000 ) {
		wsprintf( tmp, L"%03d", n );
	} else {
		wsprintf( tmp, L"%d", n );
	}
	wcscat( str, tmp );
	// 000.000.000,XX
	n = ( value % 100 );
	if( ( force_decimal ) || ( n > 0 ) ) {
		wcscat( str, L"," );
		wsprintf( tmp, L"%02d", n );
		wcscat( str, tmp );	
	}
}

float Misc::RANDOM_M11(unsigned int* seed)
{
    *seed = *seed * 134775813 + 1;
    union
    {
        uint32_t d;
        float f;
    } u;
    u.d = (((uint32_t)(*seed) & 0x7fff) << 8) | 0x40000000;
    return u.f - 3.0f;
}


SDL_Texture* Misc::CreateTexture( int width, int height )
{
	SDL_Texture *result = 0;
	// pixel format SDL_PIXELFORMAT_ARGB8888 is the most common supported
	result = SDL_CreateTexture( Engine::GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height );
	if( !result ) {
		printf( "Error cannot create texture! %s\n", SDL_GetError() );
	}
	return result;
}

bool Misc::ClearTexture( SDL_Texture* texture, Uint32 color )
{
	SDL_Renderer *renderer = Engine::GetRenderer();
	bool result	= false;

	do {

		SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
		SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );

		// set destination texture as current renderer target
		if( SDL_SetRenderTarget( renderer, texture ) < 0 ) {
			break;
		}

		// set color of render drawing
		if( SDL_SetRenderDrawColor( renderer, RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) ) < 0 ) {
			break;
		}
		// fill the destination texture with color
		if( SDL_RenderClear( renderer ) < 0 ) {
			break;
		}
		// set the renderer target as default
		if( SDL_SetRenderTarget( renderer, NULL ) < 0 ) {
			break;
		}
		result = true;
	} while( 0 );

	return result;
}

bool Misc::DrawLine( SDL_Texture* texture, Uint32 color, int x1, int y1, int x2, int y2, int thickness )
{
	SDL_Renderer *renderer = Engine::GetRenderer();

	// set destination texture as current renderer target
	SDL_SetRenderTarget( renderer, texture );
	// set color of render drawing
	if( SDL_SetRenderDrawColor( renderer, RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) ) < 0 ) {
		return false;
	}
	SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );
	// render the line
	for( int i = 0; i < thickness; i++ ) {
		SDL_RenderDrawLine( renderer, x1, y1 + i, x2, y2 + i );
	}	
	// set current renderer target as default
	SDL_SetRenderTarget( renderer, NULL );
	return true;
}

bool Misc::DrawLines( SDL_Texture* texture, Uint32 color, const SDL_Point* points, int maxPoints, int thickness )
{
	SDL_Renderer *renderer = Engine::GetRenderer();

	// set destination texture as current renderer target
	SDL_SetRenderTarget( renderer, texture );
	// set color of render drawing
	if( SDL_SetRenderDrawColor( renderer, RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) ) < 0 ) {
		return false;
	}
	SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );
	// render the line
	for( int j = 0; j < ( maxPoints - 1 ); j++ ) {
		for( int i = 0; i < thickness; i++ ) {
			SDL_RenderDrawLine( renderer, 
				points[ j ].x,
				points[ j ].y + i,
				points[ j + 1 ].x,
				points[ j + 1 ].y + i );
		}	
	}
	// set current renderer target as default
	SDL_SetRenderTarget( renderer, NULL );
	return true;
}


static bool AddPointToList( Coord_t *points, int *totalPoints, int maxPoints, int newX, int newY )
{
	// we cannot overflow the buffer, check total points!
	if( *totalPoints >= maxPoints ) {
		return false;
	}
	// we don't want duplicates, if the point is already in list we will not add it!
	for( int i = 0; i < *totalPoints; i++ ) {
		if( ( newX == points[ i ].x ) && ( newY == points[ i ].y ) ) {
			return false;
		} 
	}
	// insert point to current index
	points[ *totalPoints ].x = newX;
	points[ *totalPoints ].y = newY;
	// increment total number of points
	*totalPoints += 1;

	return true;
}

bool Misc::FindImageContour( const char *imageFilename, Coord_t *points, int maxPoints, int *totalPoints, int stepX, int stepY, int searchMode ) 
{
	SDL_Surface *surface = NULL;
	bool result = false;
	
	// check input data
	if( totalPoints != NULL ) {
		*totalPoints = 0;
	} else {
		return false;
	}
	// we must have the path of image file	
	if( !imageFilename ) {
		return false;
	}

	// check searchMode
	if( searchMode & FIND_WHOLE_CONTOUR ) {
		searchMode = ( FIND_RIGHT_CONTOUR | FIND_LEFT_CONTOUR | FIND_TOP_CONTOUR | FIND_BOTTOM_CONTOUR );
	}

	// load image as a SDL_Surface (it seems we cannot use SDL_Texture for this task)
	surface = IMG_Load( imageFilename );
	// if file has been loaded...
	if( surface != NULL ) {
	
		if( searchMode & FIND_LEFT_CONTOUR ) {
			// search the first non-transparent pixel for each row from LEFT side to RIGHT side
			for( int y = 0; y < surface->h; y += stepY ) {
				for( int x = 0; x < surface->w; x++ ) {
					Uint32 *target_pixel = (Uint32*)((Uint8 *) surface->pixels + y * surface->pitch + x * sizeof( *target_pixel ) );
					if( ( *target_pixel & 0xff000000 ) != 0 ) {
						AddPointToList( points, totalPoints, maxPoints, x, y );
						break;
					}
				}
			}
		}

		if( searchMode & FIND_RIGHT_CONTOUR ) {
			// search the first non-transparent pixel for each row from RIGHT side to LEFT side
			for( int y = 0; y < surface->h; y += stepY ) {
				for( int x = surface->w; x >= 0; x-- ) {
					Uint32 *target_pixel = (Uint32*)((Uint8 *) surface->pixels + y * surface->pitch + x * sizeof( *target_pixel ) );
					if( ( *target_pixel & 0xff000000 ) != 0 ) {
						AddPointToList( points, totalPoints, maxPoints, x, y );
						break;
					}
				}
			}
		}

		if( searchMode & FIND_TOP_CONTOUR ) {
			// search the first non-transparent pixel for each column from TOP side to BOTTOM side
			for( int x = 0; x < surface->w; x += stepX ) {
				for( int y = 0; y < surface->h; y++ ) {
					Uint32 *target_pixel = (Uint32*)((Uint8 *) surface->pixels + y * surface->pitch + x * sizeof *target_pixel );
					if( ( *target_pixel & 0xff000000 ) != 0 ) {
						AddPointToList( points, totalPoints, maxPoints, x, y );
						break;
					}
				}
			}
		}

		if( searchMode & FIND_BOTTOM_CONTOUR ) {
			// search the first non-transparent pixel for each column from BOTTOM side to TOP side
			for( int x = 0; x < surface->w; x += stepX ) {
				for( int y = surface->h; y >= 0; y-- ) {
					Uint32 *target_pixel = (Uint32*)((Uint8 *) surface->pixels + y * surface->pitch + x * sizeof *target_pixel );
					if( ( *target_pixel & 0xff000000 ) != 0 ) {
						AddPointToList( points, totalPoints, maxPoints, x, y );
						break;
					}
				}
			}
		}

		// if we didn't find any point there's something wrong
		if( *totalPoints > 0 ) {
			result = true;
		}
		// free temporary surface
		SDL_FreeSurface( surface );
	}
	// returns true if succesfull search
	return result;
}


void Misc::Test()
{
}