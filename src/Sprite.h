/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _SPRITE_H_INCLUDE
#define _SPRITE_H_INCLUDE

#include <SDL.h>
#include "Node.h"

class  Sprite : public Node {
	
public:

	/*
		object Sprite constructor
		- texture : pointer to texture or NULL
		- tag : unique identifier inside the scene ( must be a positive value )
		- zOrder : depth level ( must be a positive value )
	*/
	Sprite( SDL_Texture *texture, unsigned int tag, unsigned int zOrder );

	// set a new texture 
	void SetTexture( SDL_Texture *texture );

	// ========================= functions below are used internally, don't use in the game =======================

	// called if the Sprite is set as clickable and the player touch it
	void OnClick();

	// function called each frame (override of function in Node class)
	void Draw();

private:

	// current image of sprite
	SDL_Texture *texture;	
};

#endif