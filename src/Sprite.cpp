/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <SDL.h>
#include "Sprite.h"
#include "Engine.h"

// Sprite object constructor
Sprite::Sprite( SDL_Texture *texture, unsigned int tag, unsigned int zOrder )
{
	this->texture	= texture;
	this->tag		= tag;
	this->zOrder	= zOrder;
	SDL_QueryTexture( texture, NULL, NULL, &this->width, &this->height );
}

// set a new texture for the Sprite
void Sprite::SetTexture( SDL_Texture *texture )
{
	this->texture = texture;
}

// called if the Sprite is set as clickable and the player touch it
void Sprite::OnClick()
{
	Engine::GetConfig()->ObjectClickedCallback( this, tag );
}

// function called each frame (override of function in Node class)
void Sprite::Draw()
{
	if( texture != NULL ) {
		DrawTexture( texture, width, height );
	}
}



