/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <SDL.h>
#include "EngineCommon.h"

#ifndef _MISCELLANEOUS_H_INCLUDE
#define _MISCELLANEOUS_H_INCLUDE

class Misc {

public:
	
	/*
		create a UNICODE string with italian currency format given a value in cents
		value 500000000 -> string €5.000.000,00
		the string pointed by "str" parameter must be big enough to contain the value
		e.g. 
			for the value 5000000000 -> €5.000.000,00 = 13 wchar_t lenght 
			we need to declare at least an array like this 
			wchar_t string[ 15 ];
		parameter force_decimal set to true force the function to add decimal part (",00")
		even if decimal part is empty; if the last parameter is set to false the decimal part is
		added only if needed that is greater than zero (e.g.: XXX.25, XXX.50, XXX.75 but not XXX.00)
	*/
	static void CurrencyValueToUTF16String( int value, wchar_t *str, bool force_decimal, bool euro_symbol );

	// returns a random float value between 0 and 1
	static float RANDOM_M11(unsigned int* seed);

	// create a texture
	static SDL_Texture* CreateTexture( int width, int height );

	// clear a texture
	static bool ClearTexture( SDL_Texture* texture, Uint32 color );

	// draw a line on a texture
	static bool DrawLine( SDL_Texture* texture, Uint32 color, int x1, int y1, int x2, int y2, int thickness );

	// draw multiple lines on a texture
	static bool DrawLines( SDL_Texture* texture, Uint32 color, const SDL_Point* points, int maxPoints, int thickness );

	/*
		which type of contour to search
	*/
	typedef enum {
		FIND_LEFT_CONTOUR	= 1 << 0,
		FIND_RIGHT_CONTOUR	= 1 << 1,
		FIND_TOP_CONTOUR	= 1 << 2,
		FIND_BOTTOM_CONTOUR	= 1 << 3,
		FIND_WHOLE_CONTOUR	= 1 << 4,
	} FindContour_t;

	/*
		find contour in an alpha image, returns an array of points (Coord_t)
		duplicates are automatically removed from the list.

		- imageFilename		-> path + filename of PNG image 
		- points			-> pointer to an empty array of Coord_t object
		- maxPoints			-> size of array pointed by "points"
		- totalPoints		-> actual number of points found
		- stepX				-> step of search: 1 = each pixel of a col, 2 = check every two pixels, 3 = check every three pixels, ...
		- stepY				-> step of search: 1 = each pixel of a row, 2 = check every two pixels, 3 = check every three pixels, ...
		- whichContour		-> see FindContour_t
	*/
	static bool FindImageContour( const char *imageFilename, Coord_t *points, int maxPoints, int *totalPoints, int stepX, int stepY, int whichContour ); 

	// TODO
	static void Test();
};

#endif