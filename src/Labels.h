/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <windows.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "Node.h"
#include "EngineCommon.h"


#ifndef _LABELS_H_INCLUDE
#define _LABELS_H_INCLUDE

// maximum length of text inside label
#define LABELS_TEXT_MAX_LENGTH		128

// type of alignment relative to position
typedef enum {
	ALIGNMENT_CENTER,
	ALIGNMENT_RIGHT,
	ALIGNMENT_LEFT,
	ALIGNMENT_MAX
} LabelAlignment_t;

/*
	Class Label
*/
class  Label : public Node {
	
public:

	// update color of text
	void SetColor( Uint32 color );

	// update alignment
	void SetAlignment( LabelAlignment_t alignment );

	// update font
	void SetFont( int fontId );

	// update font size
	void SetFontSize( int fontSize );

protected:

	// object Label constructor (is protected because we don't want the game create an object
	// directly with this class, the game must use one of subclasses: LabelValue, LabelUTF8,...)
	Label( int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder );

	// function called each frame (override of function in Node class)
	void Draw();


	TTF_Font		*font;				// pointer to current font of label
	int				fontId;				// current font id 
	int				fontSize;			// current font size
	SDL_Texture		*texture;			// texture that contains text
	Uint32			color;				// current color of text
	int				alignment;			// current alignment (center, right, left)s
	int				alignment_x_offset;	// current horizontal offset due to alignment

	// update current texture containing text
	void			UpdateTexture();
	
	// update current alignment horizontal offset
	void			UpdateAlignmentOffset();

	/*
		this function must be overriden because we have different method to get surface according to character encoding
		- for UTF8 GetTextSurface uses TTF_RenderText_Blended() 
		- for Unicode GetTextSurface uses TTF_RenderUNICODE_Blended 
	*/
	virtual SDL_Surface* GetTextSurface();
};


/*
	Class Label Value
*/
class  LabelValue : public Label {
	
public:

	// object LabelUTF8 constructor
	LabelValue( long value, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder );

	// update text of label
	void SetValue( const long value );

private:
	// current value of label
	long			value;
	// array containing current text of label
	char			text[ LABELS_TEXT_MAX_LENGTH ];
	// ovverride of GetTextSurface of Label class
	SDL_Surface*	GetTextSurface();
	// update value of label
	void UpdateText( const long value );
};



/*
	Class Label UTF8 (char*)
*/
class  LabelUTF8 : public Label {
	
public:

	// object LabelUTF8 constructor
	LabelUTF8( const char *text, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder );

	// update text of label
	void SetText( const char *text );

private:
	
	// array containing current text of label
	char			text[ LABELS_TEXT_MAX_LENGTH ];

	// ovverride of GetTextSurface of Label class
	SDL_Surface*	GetTextSurface();
};




/*
	Class Label UTF16 (Unicode) 
*/
class  LabelUTF16 : public Label {
	
public:

	// object LabelUTF8 constructor
	LabelUTF16( const wchar_t *text, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder );

	// update text of label
	void SetText( const wchar_t *text );

private:

	// array containing current text of label
	wchar_t			text[ LABELS_TEXT_MAX_LENGTH ];

	// ovverride of GetTextSurface of Label class
	SDL_Surface*	GetTextSurface();
};





/*
	Class Label Currency (Unicode) 
*/
class  LabelCurrency : public Label {
	
public:

	// object LabelUTF8 constructor
	LabelCurrency( long value, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder );

	// update value of label
	void SetValue( const long value );

	// if true include into the format the symbol of Euro (€)
	void SetEuroSymbol( bool state );

	// if true include into the format decimal digits (,00)
	void SetDecimals( bool state );

private:

	// current value of label
	long			value;
	// include (if true) symbol of Euro
	bool			euro_symbol;
	// include (if true) decimals
	bool			decimals;
	// array containing current text of label
	wchar_t			text[ LABELS_TEXT_MAX_LENGTH ];

	// ovverride of GetTextSurface of Label class
	SDL_Surface*	GetTextSurface();

	// update value of label
	void UpdateText( const long value );
};











/*
	Class Bitmap Font Label Text
*/
class  BMPLabelText : public Node {
	
public:

	BMPLabelText() {};
	BMPLabelText( int fontId, unsigned int tag, unsigned int zOrder );

	// update text
	void SetText( const wchar_t *text );

	// change font 
	void SetBitmapFontId( int fontId );

	// update alignment
	void SetAlignment( LabelAlignment_t alignment );

protected:

	#define MAX_TEXT_LENGTH		128


	int				fontId;						// current font id 
	SDL_Texture		*texture;					// current texture based on text
	int				alignment;					// current alignment (center, right, left)s
	int				alignment_x_offset;			// current horizontal offset due to alignment
	wchar_t			text[ MAX_TEXT_LENGTH ];	// current text

	// function called each frame (override of function in Node class)
	void Draw();

	void			UpdateTexture();

	// update current alignment horizontal offset
	void			UpdateAlignmentOffset();
};



class  BMPLabelValue : public BMPLabelText {
	
public:

	BMPLabelValue() {};
	BMPLabelValue( int fontId, unsigned int tag, unsigned int zOrder );

	// update value
	void SetValue( int value );

private:
	int				value;
};


class  BMPLabelCurrency : public BMPLabelText {
	
public:

	BMPLabelCurrency() {};
	BMPLabelCurrency( int fontId, unsigned int tag, unsigned int zOrder );

	// update value
	void SetAmount( int amount );

	// if true include into the format the symbol of Euro (€)
	void SetEuroSymbol( bool state );

	// if true include into the format decimal digits (,00)
	void SetDecimals( bool state );

private:
	// amount value
	int				amount;
	// include (if true) symbol of Euro
	bool			euro_symbol;
	// include (if true) decimals
	bool			decimals;

	// update text according to amount value 
	void UpdateText();
};


#endif