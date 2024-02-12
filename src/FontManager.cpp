/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include "FontManager.h"
#include "Engine.h"


// this is the maximum number of true type fonts
#define FONTMANAGER_MAX_TTF_FONTS			64

// this is the maximum number of bitmap fonts fonts
#define FONTMANAGER_MAX_BMP_FONTS			64

// available allocated True Type font data
typedef struct {
	int			fontId;
	int			fontSize;
	TTF_Font	*font;
} TTFFontData_t;

/*
	pointer to the table containing all available fonts path+filename, example

	const char *fonts_filenames[ fontid_max ] =
	{
		"arial_black.ttf"
	};
*/
static const char		**TTFontsFiles			= NULL;
// total number of available font files
static long				totalTTFontsFiles		= 0;	

// total number of allocated fonts (by font manager)
static long				totalTTFonts			= 0;
// allocated fonts (by Font Manager)
static TTFFontData_t	TTFontsData[ FONTMANAGER_MAX_TTF_FONTS ];

// total number of allocated fonts
static long				totalBMPFonts			= 0;
// data of alla available bitmap fonts
static BMPFontData_t	BMPFontsData[ FONTMANAGER_MAX_BMP_FONTS ];

// reset font manager data
void FontManager::Reset()
{
	// total number of allocated fonts by FontManager
	memset( &TTFontsData, 0, sizeof( TTFontsData ) ); 
	// reset data for each bitmap font
	memset( &BMPFontsData, 0, sizeof( BMPFontsData ) ); 
}

// free allocated resources
void FontManager::Terminate()
{
	// close all opened fonts and free memory
	for( int i = 0; i < totalTTFonts; i++ ) {
		TTF_CloseFont( TTFontsData[ i ].font );
	}	
}

// setup Font Manager with all avalaible True Type fonts files
bool FontManager::SetTTFontsFilesInfo( long totalFiles, const char **files )
{
	// store total number of available font files and their names
	TTFontsFiles		= files;
	totalTTFontsFiles	= totalFiles;
	// we can't manage more than FONTMANAGER_MAX_TTF_FONTS fonts
	if( totalTTFontsFiles > FONTMANAGER_MAX_TTF_FONTS ) {
		return false;
	}
	return true;
}

// get font with properties (or create it if doesn't exists)
TTF_Font* FontManager::GetTTFont( int fontId, int fontSize )
{
	int index = -1;

	// search font with indicated properties in the already allocated fonts
	for( int i = 0; i < totalTTFonts; i++ ) {
		if( ( TTFontsData[ i ].fontId == fontId ) && ( TTFontsData[ i ].fontSize == fontSize ) ) {
			index = i;
			break;
		}
	}
	// if we have found the font
	if( index >= 0 ) {
		// return pointer to the font
		return TTFontsData[ index ].font;
	} else {
		// we must create a new one
		TTFontsData[ totalTTFonts ].font = TTF_OpenFont( TTFontsFiles[ fontId ], fontSize );
		if( TTFontsData[ totalTTFonts ].font != NULL ) {
			TTFontsData[ totalTTFonts ].fontId		= fontId;
			TTFontsData[ totalTTFonts ].fontSize	= fontSize;
			totalTTFonts += 1;
			// return pointer to the newly created font
			return TTFontsData[ totalTTFonts - 1 ].font;
		}
	}
	// in case of errors returns NULL
	return NULL;
}







static void ParseInfoArguments( string line, BMPFontPadding_t *padding )
{
    // padding
    int index = line.find("padding=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2-index);
    sscanf(value.c_str(), "padding=%d,%d,%d,%d", &padding->top, &padding->right, &padding->bottom, &padding->left);
}

static void ParseCommonArguments( string line, int *lineHeight )
{
    // Height
    int index = line.find("lineHeight=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2-index);
    sscanf(value.c_str(), "lineHeight=%d", lineHeight );
}

static void ParseCharacterDefinition( string line, CharData_t *charData )
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // char id=32   x=0     y=0     width=0     height=0     xoffset=0     yoffset=44    xadvance=14     page=0  chnl=0 
    //////////////////////////////////////////////////////////////////////////

    // Character ID
    int index = line.find("id=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2-index);
    sscanf(value.c_str(), "id=%u", &charData->charID);

    // Character x
    index = line.find("x=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
    sscanf(value.c_str(), "x=%d", &charData->rect.x);
    // Character y
    index = line.find("y=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
    sscanf(value.c_str(), "y=%d", &charData->rect.y);
    // Character width
    index = line.find("width=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
	sscanf(value.c_str(), "width=%d", &charData->rect.w);
    // Character height
    index = line.find("height=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
    sscanf(value.c_str(), "height=%d", &charData->rect.h);
    // Character xoffset
    index = line.find("xoffset=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
    sscanf(value.c_str(), "xoffset=%hd", &charData->xOffset);
    // Character yoffset
    index = line.find("yoffset=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
    sscanf(value.c_str(), "yoffset=%hd", &charData->yOffset);
    // Character xadvance
    index = line.find("xadvance=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
    sscanf(value.c_str(), "xadvance=%hd", &charData->xAdvance);
}

static void ParseKerningEntry( string line, KerningData_t *kerningItem )
{        
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // kerning first=121  second=44  amount=-7
    //////////////////////////////////////////////////////////////////////////

    // first
    int index = line.find("first=");
    int index2 = line.find(' ', index);
    std::string value = line.substr(index, index2-index);
	sscanf(value.c_str(), "first=%d", &kerningItem->firstCharID );

    // second
    index = line.find("second=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
	sscanf(value.c_str(), "second=%d", &kerningItem->secondCharID );

    // amount
    index = line.find("amount=");
    index2 = line.find(' ', index);
    value = line.substr(index, index2-index);
	sscanf(value.c_str(), "amount=%d", &kerningItem->amount );
}


static bool ParseBMPFontFile( const char *filename, BMPFontData_t *fontData )
{
	string		line;
	ifstream	fontfile( filename );	// open the bitmap descriptor font file 

	// if we can't open the file returns error
	if( !fontfile ) {
		printf( "Cannot open input file.\n" );
		return false;
	}

	/*
		read the file line by line, a typical file should appear like this 

		info face=font size=72 bold=1 italic=0 charset= unicode= stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=0,0 outline=0
		common lineHeight=80 base=53 scaleW=383 scaleH=512 pages=1 packed=0
		page id=0 file="font.png"
		chars count=55
		char id=65 x=2 y=2 width=61 height=61 xoffset=0 yoffset=1 xadvance=52 page=0 chnl=15
		...
	*/
	while( getline( fontfile, line )  ) {

		// debug file line by line
		// cout << line << endl;
		//wprintf( L"%s", line );

		if( line.substr( 0, strlen( "info face" ) ) == "info face" ) {
			// we found the first line of the .fnt file, extract padding
			ParseInfoArguments( line, &fontData->padding );
		} else if( line.substr( 0, strlen( "common lineHeight" ) ) == "common lineHeight" ) {
			// we found the second line of the .fnt file, extract lineheight
			ParseCommonArguments( line, &fontData->lineHeight );
		} else if( line.substr( 0, strlen( "page id" ) ) == "page id" ) {
			// we found the third line, we are not interested because we use always one file onlys 
		} else if( line.substr( 0, strlen( "chars c" ) ) == "chars c" ) {
			// we found the fourth line, indicating total number of chars
			// TODO we could use this value for double checking extracted data
		} else if( line.substr( 0, strlen( "char" ) ) == "char" ) {
			// we find a line describing a character, if we have not reached maximum chars number..
			if( fontData->totalChars < MAX_BMPFONT_CHARS ) {
				// parse the content of the line
				ParseCharacterDefinition( line, &fontData->charsData[ fontData->totalChars ] );
				// increase total number of chars
				fontData->totalChars += 1;
			}
		} else if( line.substr( 0, strlen( "kerning first" ) ) == "kerning first" ) {
			// we find a line describing a kerning case, if we have not reached maximum number of cases..
			if( fontData->totalKerningItems < MAX_KERNING_DATA ) {
				// parse the content of the line
				ParseKerningEntry( line, &fontData->kerning[ fontData->totalKerningItems ] );
				// increase total number of cases
				fontData->totalKerningItems += 1;
			}
		}
	}
	// close the file
	fontfile.close();

	return true;
}

bool FontManager::LoadBMPFontsFiles( long totalBMPFontsFiles, BMPFontResource_t *BMPFontResources )
{
	BMPFontResource_t *pBMPFontResource;
	// point to the first item of the table containing bitmap fonts data
	pBMPFontResource = BMPFontResources;
	// scan all bitmap font initialization data
	for( int i = 0; i < totalBMPFontsFiles; i++ ) {
		// try to parse the configuration file 
		if( ParseBMPFontFile( pBMPFontResource->filename, &BMPFontsData[ i ] ) ) {
			// if we have no problems with the file, store the id of the bitmap font
			BMPFontsData[ i ].fontId = i;
			// store the resource texture 
			BMPFontsData[ i ].texture = *pBMPFontResource->texture;
			// point to the nexe item
			pBMPFontResource++;
			// increment the total number of available bitmap fonts
			totalBMPFonts += 1;
		} 
	}
	return true;
}

BMPFontData_t*	FontManager::GetBMPFontData( int fontId )
{
	BMPFontData_t*	result = NULL;
	// scan all loaded fonts... 
	for( int i = 0; i < totalBMPFonts; i++ ) {
		// if we find the font with the given id
		if( BMPFontsData[ i ].fontId == fontId ) {
			// return the pointer to font data
			result = &BMPFontsData[ i ];
			break;
		}
	}
	return result;
}
