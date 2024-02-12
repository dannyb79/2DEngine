/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _ANIMATEDSPRITE_H_INCLUDE
#define _ANIMATEDSPRITE_H_INCLUDE

#include <SDL.h>
#include "Node.h"

class AnimatedSprite : public Node {
	
public:

	/*
		object AnimatedSprite constructor

		object draw continuously a sequence of images about every 33.3 ms 

		- textures : pointer to textures array
		- tag : unique identifier of object
		- zOrder : depth level
	*/
	AnimatedSprite( SDL_Texture **textures, unsigned int n_frames, unsigned int	tag, unsigned int zOrder );

	// play the animation only once
	void PlayOnce();
	
	// play the animation for a number of times
	void Play( unsigned int n_times );
	
	// play the animation continously
	void Loop();
	
	// stop the animation and reset to the first frame
	void Stop();

	// ========================= functions below are used internally, don't use in the game =======================

	// function called each frame (override of function in Node class)
	void Draw();

	// perform action if object is touched by the user	
	void OnClick();

private:

	SDL_Texture		**textures;		// array of textures
	unsigned int	total_frames;	// total number of images
	unsigned int	current_frame;	// current frame
	bool			isPlaying;		// animation in progress 
	int				current_loop;	// number of executed loops
	int				total_loops;	// total number of loops
};


#endif