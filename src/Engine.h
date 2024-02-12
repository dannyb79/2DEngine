/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "Scene.h"

#ifndef _ENGINE_H_INCLUDE
#define _ENGINE_H_INCLUDE

namespace Engine {

	// data structure to setup engine with games data
	typedef struct {
		SDL_Renderer	*renderer;
		void			( *ObjectClickedCallback )( Node *, int );
		void			( *ActionEndCallback )( int );
		void			( *ReleasedButtonCallback )( Node *, int );
	} Config_t;

	// engine initialization with game data
	void Initialize( Config_t *config );

	// close the engine before quit
	void Terminate();

	// update current scene and draw it to the screen
	void DrawScene();

	// return pointer to the renderer
	SDL_Renderer* GetRenderer();

	//	add a new scene to the engine's list of scenes
	bool AddScene( Scene *scene );

	// set scene to render
	bool SetCurrentScene( unsigned int sceneId );

	// called when the player touches the screen
	void Touch( int x, int y );

	// return pointer to current engine configuration data
	Config_t *GetConfig();

	/*
		set information about software button pressure
		the engine use this information to lock or unlock some features (e.g. avoid other button pressures)
	*/
	void SetButtonCurrentlyPressed( bool state );

	// this function returns true or false whenever the engine is able to accept software button press
	// ATTENTION when a hardware button is pressed the game also MUST check this flag to refuse action 
	// if another (action of software button) is in progress!!!
	bool IsButtonPressAllowed();

	/*
		set scene transition flag 
		scene transition must be carried on by the game; when the transition start the game must communicate it
		to the engine through 
		SetSceneTransitionInProgress( true )
		the engine use this information to lock or unlock some features (e.g. avoid button pressures)
		when transition is completed the game must communicate it to the engine through
		SetSceneTransitionInProgress( false )
	*/
	void SetSceneTransitionInProgress( bool state );

	// get current ticks (SDL_GetTicks()) when the scene is redrawn
	Uint32 GetDrawSceneTicks();
};


#endif
