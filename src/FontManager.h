/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _FONTMANAGER_H_INCLUDE
#define _FONTMANAGER_H_INCLUDE

#include <iostream>
#include <fstream>
#include <string>

#include <stdio.h>
#include <SDL_ttf.h>
#include <SDL.h>

using namespace std;

/*
	structure that game must use for passing data to the engine,
	here an example:	

	BMPFontResource_t	BMPFontResources[ BMPFontId_Max ] = 
	{
		//	file				texture
		{	"BMPfont1.fnt",		&IMG_BMPfont1	},	// BMPFontId_1
		{	"BMPfont2.fnt",		&IMG_BMPfont2	},	// BMPFontId_2
		{	"BMPfont3.fnt",		&IMG_BMPfont3	},	// BMPFontId_3
		{	"BMPfont4.fnt",		&IMG_BMPfont4	}	// BMPFontId_4
	};
*/
typedef struct {
	const char		*filename;
	SDL_Texture		**texture;
} BMPFontResource_t;



// ============================== data below is used internally, DON'T USE THEM IN THE GAME ======================

// character data
typedef struct {
	unsigned int	charID;		// Unicode value of the character
	SDL_Rect		rect;		// origin and size of the character in the texture	
	short			xOffset;	// the X amount the image should be offset when drawing the image (in pixels)
	short			yOffset;	// The Y amount the image should be offset when drawing the image (in pixels)
	short			xAdvance;	// The amount to move the current position after drawing the character (in pixels)
} CharData_t;

// font kerning data
typedef struct {
	unsigned int	firstCharID;	// Unicode value of the first character	
	unsigned int	secondCharID;	// Unicode value of the second character
	short			amount;			// the X amount the image should be offset when drawing the image (in pixels)
} KerningData_t;

// maximum number of loadable characters for each bitmap font
#define MAX_BMPFONT_CHARS					100

// maximum number of loadable kerning data for each bitmap font
#define MAX_KERNING_DATA					100

typedef struct {
	int				top;
	int				bottom;
	int				right;
	int				left;
} BMPFontPadding_t;


// data for each bitmap font
typedef struct {
	int					fontId;
	int					lineHeight;
	BMPFontPadding_t	padding;
	long				totalChars;
	CharData_t			charsData[ MAX_BMPFONT_CHARS ];
	long				totalKerningItems;
	KerningData_t		kerning[ MAX_KERNING_DATA ];
	SDL_Texture			*texture;
} BMPFontData_t;


namespace FontManager {

	// reset font manager data
	void Reset();

	// setup Font Manager with all avalaible True Type fonts files
	bool SetTTFontsFilesInfo( long totalFiles, const char **files );

	// get font with properties (or create it if doesn't exists)
	TTF_Font *GetTTFont( int fontId, int fontSize );

	// load and parse bitmap font files
	bool LoadBMPFontsFiles( long totalBMPFontsFiles, BMPFontResource_t *BMPFontResources );

	// return bitmap font data given the font unique identifier
	BMPFontData_t*	GetBMPFontData( int fontId );

	// free allocated resources
	void Terminate();
};

#endif