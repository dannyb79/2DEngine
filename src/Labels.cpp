/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <SDL.h>
#include "Labels.h"
#include "Engine.h"
#include "FontManager.h"
#include "Misc.h"

Label::Label( int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder )
{
	this->tag		= tag;
	this->zOrder	= zOrder;
	this->fontId	= fontId;
	this->fontSize	= fontSize;
	this->font		= FontManager::GetTTFont( fontId, fontSize );
	this->color		= color;
	this->width		= 0;
	this->height	= 0;
	this->texture	= NULL;
	this->alignment	= ALIGNMENT_CENTER;
	this->alignment_x_offset	= 0;
}

void Label::UpdateTexture()
{
	SDL_Surface	*textsurface = 0;
	int	w = 0, h = 0;
	// if we have a valid font...
	if( this->font ) {
		// create an SDL_Surface with text (GetTextSurface id overridden)
		textsurface = GetTextSurface();
		// if an SDL_Surface has been successfully created...
		if( textsurface ) {
			w = textsurface->w;
			h = textsurface->h;
			if( texture != NULL ) {
				SDL_DestroyTexture( texture );
			}
			// create a new texture from the surface
			texture = SDL_CreateTextureFromSurface( Engine::GetRenderer(), textsurface );
			if( texture != NULL ) {
				SDL_QueryTexture( texture, NULL, NULL, &this->width, &this->height );
			} else {
				printf( "Null texture!\n" );
			}
			// destroy surface 
			SDL_FreeSurface( textsurface );
		} else {
			printf( "Null text surface!\n" );
		}
	}
	// we must update alignment offset because width could change
	UpdateAlignmentOffset();
}

void Label::UpdateAlignmentOffset()
{
	switch( alignment ) {
		case ALIGNMENT_RIGHT:
			this->alignment_x_offset = -( this->width );
		break;
		case ALIGNMENT_LEFT:
			this->alignment_x_offset = 0;
		break;
		default:
			this->alignment_x_offset = -( this->width / 2 );
		break;
	}
}

void Label::SetColor( Uint32 color )
{
	this->color = color;
	// we must update alignment offset because color has changed
	UpdateTexture();
}

void Label::SetAlignment( LabelAlignment_t alignment )
{
	this->alignment = alignment;
	// we must update offset
	UpdateAlignmentOffset();
}

void Label::SetFont( int fontId )
{
	this->fontId	= fontId;
	this->font		= FontManager::GetTTFont( fontId, fontSize );
	// we must update texture because font has changed
	UpdateTexture();
}

void Label::SetFontSize( int fontSize )
{
	this->fontSize	= fontSize;
	this->font		= FontManager::GetTTFont( fontId, fontSize );
	// we must update texture because font size has changed
	UpdateTexture();
}

SDL_Surface* Label::GetTextSurface() 
{
	// this function must be overriden because we have different method to get surface according to character encoding
	// - for UTF8 GetTextSurface uses TTF_RenderText_Blended() 
	// - for Unicode GetTextSurface uses TTF_RenderUNICODE_Blended 
	return NULL;
}

void Label::Draw()
{
	if( texture != NULL ) {
		Coord_t		world_position;
		SDL_Rect	srcrect;
		SDL_Rect	dstrect;

		// get coordinates int the world, this is normally the sum of coordinates of all parents
		world_position = GetWorldPosition();
		// set source rect (we always draw the entire texture, not a portion of it)
		srcrect.x = 0;
		srcrect.y = 0;
		srcrect.w = width;
		srcrect.h = height;
		// if current size is different from default (1.0)... 	
		if( size != 1 ) {
			// we calculate the anchor point from the original size to let the object in the center
			float		final_w, final_h;
			final_w		= width * size;
			final_h		= height * size;
			dstrect.x	= (int)world_position.x + alignment_x_offset + width/2 - final_w/2;
			dstrect.y	= (int)world_position.y + height/2 - final_h/2;;
			dstrect.w	= final_w;
			dstrect.h	= final_h;
		} else {
			dstrect.x = (int)world_position.x + alignment_x_offset;
			dstrect.y = (int)world_position.y;
			dstrect.w = width;
			dstrect.h = height;
		}
		SDL_SetTextureAlphaMod( texture, alpha );
		SDL_RenderCopyEx( Engine::GetRenderer(), texture, &srcrect, &dstrect, angle, NULL, flip );
	}
}





LabelValue::LabelValue( long value, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder ) : Label( fontId, fontSize, color, tag, zOrder )
{
	// here Label constructor initialize all common data, after that
	// we can set our UTF16 text containing currency formatted string
	this->value			= value;
	UpdateText( value );
}

void LabelValue::SetValue( const long value )
{
	this->value = value;
	UpdateText( this->value );
}

void LabelValue::UpdateText( long value )
{
	sprintf( this->text, "%d", value );
	UpdateTexture();
}

SDL_Surface* LabelValue::GetTextSurface()
{
	SDL_Color textcolor = { RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) };
	return TTF_RenderText_Blended( this->font, this->text, textcolor );
}





LabelUTF8::LabelUTF8( const char *text, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder ) : Label( fontId, fontSize, color, tag, zOrder )
{
	// here Label constructor initialize all common data, after that
	// we can set our UTF8 text
	SetText( text );
}

SDL_Surface* LabelUTF8::GetTextSurface()
{
	SDL_Color textcolor = { RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) };
	return TTF_RenderText_Blended( this->font, this->text, textcolor );
}

void LabelUTF8::SetText( const char *text )
{
	if( text != NULL ) {
		sprintf( this->text, "%s", text );
		UpdateTexture();
	} 	
}






LabelUTF16::LabelUTF16( const wchar_t *text, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder ) : Label( fontId, fontSize, color, tag, zOrder )
{
	// here Label constructor initialize all common data, after that
	// we can set our UTF8 text
	SetText( text );
}

SDL_Surface* LabelUTF16::GetTextSurface()
{
	SDL_Color textcolor = { RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) };
	return TTF_RenderUNICODE_Blended( this->font, (Uint16*)this->text, textcolor );
}

void LabelUTF16::SetText( const wchar_t *text )
{
	if( text != NULL ) {
		wsprintf( this->text, L"%s", text ); 
		UpdateTexture();
	} 	
}




LabelCurrency::LabelCurrency( long value, int fontId, int fontSize, Uint32 color, unsigned int tag, unsigned int zOrder ) : Label( fontId, fontSize, color, tag, zOrder )
{
	// here Label constructor initialize all common data, after that
	// we can set our UTF16 text containing currency formatted string
	
	// by default the string is complete: with euro symbol and decimals
	this->euro_symbol	= true;
	this->decimals		= true;
	this->value			= value;
	UpdateText( value );
}

void LabelCurrency::SetValue( const long value )
{
	this->value = value;
	UpdateText( this->value );
}

void LabelCurrency::SetEuroSymbol( bool state )
{
	this->euro_symbol = state;
	UpdateText( this->value );
}

void LabelCurrency::SetDecimals( bool state )
{
	this->decimals = state;
	UpdateText( this->value );
}

void LabelCurrency::UpdateText( long value )
{
	Misc::CurrencyValueToUTF16String( value, this->text, this->decimals, this->euro_symbol );
	UpdateTexture();
}

SDL_Surface* LabelCurrency::GetTextSurface()
{
	SDL_Color textcolor = { RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) };
	return TTF_RenderUNICODE_Blended( this->font, (Uint16*)this->text, textcolor );
}


BMPLabelText::BMPLabelText( int fontId, unsigned int tag, unsigned int zOrder )
{
	this->fontId = fontId;
	this->zOrder = zOrder;
	memset( this->text, 0, sizeof( text ) );
	this->texture = NULL;
	this->alignment	= ALIGNMENT_CENTER;
	this->alignment_x_offset	= 0;
}

void BMPLabelText::UpdateTexture()
{
	SDL_Renderer	*renderer;
	BMPFontData_t	*fontData;
	long			texture_w;
	long			texture_h;
	SDL_Rect		srcrect;
	SDL_Rect		dstrect;
	int				xPos		= 0;
	int				kerning		= 0;
	int				textLength	= 0;
	int				textWidth	= 0;

	// if a previous texture has been allocated...
	if( texture != NULL ) {
		// destroy it!
		SDL_DestroyTexture( texture );
	}

	// get bitmap font data
	fontData = FontManager::GetBMPFontData( fontId );
	if( !fontData ) {
		return;
	}
	// get renderer pointer
	renderer = Engine::GetRenderer();
	
	// get total number of characters according to current text
	textLength = wcslen( text );

	// calculate total width of texture doing same calculations of drawing iteration
	xPos = 0;
	for( int j = 0; j < textLength; j++ ) {
		for( int i = 0; i < fontData->totalChars; i++ ) {
			if( fontData->charsData[ i ].charID == text[ j ] ) {
				// kerning is calculate previously for current character, update current char position
				xPos += kerning;
				// update current horizontal print position accordint to xAdvance value of character
				xPos += fontData->charsData[ i ].xAdvance;
				// calculate kerning of following character
				kerning	= 0;
				// if we have a further character to draw...
				if( ( j + 1 ) < textLength ) {
					// we must search in kerning data if there is a corresponding couple with current and next character
					for( int k = 0; k < fontData->totalKerningItems; k++ ) {
						// if we found any...
						if( ( text[ j ] == fontData->kerning[ k ].firstCharID ) && ( text[ j + 1 ] == fontData->kerning[ k ].secondCharID ) ) {
							// set the offset value for next character
							kerning = fontData->kerning[ k ].amount;
							break;
						}
					}
				}
				break;
			}
		}
	}
	// xPos contains the total width of texture
	texture_w = xPos;
	texture_h = fontData->lineHeight;

	// create a new texture 
	texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, texture_w, texture_h );
	if( texture != NULL ) {

		// these settings is for alpha
		SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
		SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );
		// we must convert the render target from screen to texture
		SDL_SetRenderTarget( renderer, texture );
		// fill the texture with trasnparency
		SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
        SDL_RenderClear( renderer );
		
		SDL_SetTextureAlphaMod( fontData->texture, alpha );

		// reset current x position
		xPos = 0;
		// scan all characters of the text 
		for( int j = 0; j < textLength; j++ ) {
			// search if a character match with any in font data
			for( int i = 0; i < fontData->totalChars; i++ ) {
				// if we find a corresponding character...
				if( fontData->charsData[ i ].charID == text[ j ] ) {
					
					// debug
#if 0
					printf( "Char %c %3d width %3d xadvance %3d kerning %3d xPos %d\n", 
						text[ j ], 
						text[ j ],
						fontData->charsData[ i ].rect.w, 
						fontData->charsData[ i ].xAdvance, 
						kerning,
						xPos );
#endif

					// set coordinate of character inside bitmap font texture
					srcrect.x = fontData->charsData[ i ].rect.x;
					srcrect.y = fontData->charsData[ i ].rect.y;
					srcrect.w = fontData->charsData[ i ].rect.w;
					srcrect.h = fontData->charsData[ i ].rect.h;

					// kerning is calculate previously for current character, update current char position
					xPos += kerning;

					// set where to draw current char inside the texture
					dstrect.x = xPos;
					dstrect.y = fontData->charsData[ i ].yOffset;
					dstrect.w = fontData->charsData[ i ].rect.w;
					dstrect.h = fontData->charsData[ i ].rect.h;

					// update current horizontal print position accordint to xAdvance value of character
					xPos += fontData->charsData[ i ].xAdvance;

					// calculate kerning of following character
					kerning	= 0;
					// if we have a further character to draw...
					if( ( j + 1 ) < textLength ) {
						// we must search in kerning data if there is a corresponding couple with current and next character
						for( int k = 0; k < fontData->totalKerningItems; k++ ) {
							// if we found any...
							if( ( text[ j ] == fontData->kerning[ k ].firstCharID ) && ( text[ j + 1 ] == fontData->kerning[ k ].secondCharID ) ) {
								// set the offset value for next character
								kerning = fontData->kerning[ k ].amount;
								break;
							}
						}
					}

					// copy character into the texture
					SDL_RenderCopyEx( renderer, fontData->texture, &srcrect, &dstrect, angle, NULL, flip );

					// go ahead with next char inside text
					break;
				}
			}
		}
		// reset target of renderer (the screen)
		SDL_SetRenderTarget( renderer, NULL );
	}

	// set super class width and height for printing
	width	= texture_w;
	height	= texture_h;
}

void BMPLabelText::SetText( const wchar_t *text )
{
	// store current text
	wsprintf( this->text, L"%s", text );
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelText::SetBitmapFontId( int fontId )
{
	// store current id
	this->fontId = fontId;
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelText::SetAlignment( LabelAlignment_t alignment )
{
	this->alignment = alignment;
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelText::UpdateAlignmentOffset()
{
	switch( alignment ) {
		case ALIGNMENT_RIGHT:
			this->alignment_x_offset = -( this->width );
		break;
		case ALIGNMENT_LEFT:
			this->alignment_x_offset = 0;
		break;
		default:
			this->alignment_x_offset = -( this->width / 2 );
		break;
	}
}

void BMPLabelText::Draw()
{
	if( texture != NULL ) {
		Coord_t		world_position;
		SDL_Rect	srcrect;
		SDL_Rect	dstrect;

		// get coordinates int the world, this is normally the sum of coordinates of all parents
		world_position = GetWorldPosition();
		// set source rect (we always draw the entire texture, not a portion of it)
		srcrect.x = 0;
		srcrect.y = 0;
		srcrect.w = width;
		srcrect.h = height;
		// if current size is different from default (1.0)... 	
		if( size != 1 ) {
			// we calculate the anchor point from the original size to let the object in the center
			float		final_w, final_h;
			final_w		= width * size;
			final_h		= height * size;
			dstrect.x	= (int)world_position.x + alignment_x_offset + width/2 - final_w/2;
			dstrect.y	= (int)world_position.y + height/2 - final_h/2;;
			dstrect.w	= final_w;
			dstrect.h	= final_h;
		} else {
			dstrect.x = (int)world_position.x + alignment_x_offset;
			dstrect.y = (int)world_position.y;
			dstrect.w = width;
			dstrect.h = height;
		}
		SDL_SetTextureAlphaMod( texture, alpha );
		SDL_RenderCopyEx( Engine::GetRenderer(), texture, &srcrect, &dstrect, angle, NULL, flip );
	}
}




BMPLabelValue::BMPLabelValue( int fontId, unsigned int tag, unsigned int zOrder )
{
	this->fontId = fontId;
	this->value = 0;
	wsprintf( this->text, L"%d", this->value );
	this->texture = NULL;
	this->alignment	= ALIGNMENT_CENTER;
	this->alignment_x_offset	= 0;
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelValue::SetValue( int value )
{
	this->value = value;
	wsprintf( this->text, L"%d", this->value );
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}




BMPLabelCurrency::BMPLabelCurrency( int fontId, unsigned int tag, unsigned int zOrder )
{
	this->fontId = fontId;
	this->amount = 0;
	wsprintf( this->text, L"%d", this->amount );
	this->texture = NULL;
	this->alignment	= ALIGNMENT_CENTER;
	this->alignment_x_offset	= 0;
	// update text with amount value
	UpdateText();
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelCurrency::SetAmount( int amount )
{
	this->amount = amount;
	wsprintf( this->text, L"%d", this->amount );
	// update text with amount value
	UpdateText();
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelCurrency::SetEuroSymbol( bool state )
{
	this->euro_symbol = state;
	UpdateText();
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelCurrency::SetDecimals( bool state )
{
	this->decimals = state;
	UpdateText();
	// update texture
	UpdateTexture();
	// we must update offset
	UpdateAlignmentOffset();
}

void BMPLabelCurrency::UpdateText()
{
	Misc::CurrencyValueToUTF16String( this->amount, this->text, this->decimals, this->euro_symbol );
}