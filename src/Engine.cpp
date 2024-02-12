/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include "Engine.h"
#include "MovieManager.h"
#include "FontManager.h"
#include "ActionManager.h"

// maximum number of scene we can add
#define ENGINE_MAX_SCENES	24

// game data used by engine
static Engine::Config_t		engineConfig;

// this is the total number of scenes created by the game
static long					totalScenes					= 0;
// this is the array containg pointers to scenes created by the game
static Scene				*scenes[ ENGINE_MAX_SCENES ];
// this is the pointer to the current rendered scene
static Scene				*currentScene				= NULL;

// if this flag is true a software button is currently pressed, this flag will be set to false again when
// the button (that is currently pressed) has been released; 
static bool					buttonCurrentlyPressed		= false;
// set scene transition flag 
static bool					sceneTransitionInProgress	= false;

// TODO
static Uint32				drawSceneTicks = 0;


// engine initialization with game data
void Engine::Initialize( Config_t *config )
{
	// store locally game data to configure engine
	memcpy( &engineConfig, config, sizeof( Engine::Config_t ) );
	// reset total number of scenes created by the game
	totalScenes = 0;
	// reset the array containg pointers to scenes created by the game
	for( int i = 0; i < ENGINE_MAX_SCENES; i++ ) {
		scenes[ i ] = NULL;
	}
	// reset the pointer to the current rendered scene
	currentScene = NULL;
	// reset software button pressed flag
	buttonCurrentlyPressed = false;
}

// close the engine before quit
void Engine::Terminate()
{
	// termination of MovieManager
	MovieManager::Terminate();
	// termination of FontManager
	FontManager::Terminate();
	// delete children of each scene
	for( int i = 0; i < totalScenes; i++ ) {
		scenes[ i ]->Delete();
	}
}

// get current ticks (SDL_GetTicks()) when the scene is redrawn
Uint32 Engine::GetDrawSceneTicks()
{
	return drawSceneTicks;
}

// update current scene and draw it to the screen
void Engine::DrawScene()
{
	// update ticks of call
	drawSceneTicks = SDL_GetTicks();

	// update textures of all active streamings
	MovieManager::Update();

	// update active actions
	ActionManager::Update();

	// clear renderer surface
	SDL_RenderClear( engineConfig.renderer );

	// if we have a valid scene to draw...
	if( currentScene != NULL ) {
		// visit recursively each child of current scene
		for( int i = 0; i < currentScene->totalChildren; i++ ) {
			currentScene->children[ i ]->Visit();
		}
	} 

	// copy render to video
	SDL_RenderPresent( engineConfig.renderer );
}

// return pointer to the renderer
SDL_Renderer* Engine::GetRenderer()
{
	return engineConfig.renderer;
}

// return pointer to current engine configuration data
Engine::Config_t* Engine::GetConfig()
{
	return &engineConfig;
}

//	add a new scene to the engine's list of scenes
bool Engine::AddScene( Scene *scene )
{
	bool result = false;
	// if we can add a new scene...
	if( totalScenes < ENGINE_MAX_SCENES ) {
		// add scene to the list
		scenes[ totalScenes ] = scene;
		// increment total number of scenes
		totalScenes += 1;
		// and returns current scene index (the game should store locally to switch between a scene and another)
		result = true;
	}
	// returns true if the scene has been successfully added
	return result;
}

// set scene to render
bool Engine::SetCurrentScene( unsigned int sceneId )
{
	bool result = false;
	for( int i = 0; i < totalScenes; i++ ) {
		if( scenes[ i ]->GetTag() == sceneId ) {
			// set pointer to current scene, it is used to draw the scene each frame
			currentScene = scenes[ i ];
			result = true;
		}
	}
	return result;
}

void Engine::Touch( int x, int y )
{
	// scan all clickable objects of current scene
	if( currentScene != NULL ) {
		for( int i = 0; i < currentScene->totalClickables; i++ ) {
			// get pointer to object and retrieve position and size
			Node *node = currentScene->clickables[ i ];
			// we must accept touch of visible objects
			if( node->IsVisible() ) {
				Coord_t position	= node->GetWorldPosition();
				Size_t	size		= node->GetSize();
				//printf( "Clickable #%d x %f y %f w %d h %d tag %d\n", i, position.x, position.y, size.w, size.h, node->GetTag() );
				// if user touched an object...
				if( ( x >= position.x ) && ( x <= position.x + size.w ) && ( y >= position.y ) && ( y <= position.y + size.h ) ) {
					node->OnClick();
					break;
				}
			}
		}
	}
}

// set information about software button pressure
void Engine::SetButtonCurrentlyPressed( bool state )
{
	buttonCurrentlyPressed = state;
}

bool Engine::IsButtonPressAllowed()
{
	if( sceneTransitionInProgress ) {
		return false;
	}
	if( buttonCurrentlyPressed ) {
		return false;
	}
	return true;
}

void Engine::SetSceneTransitionInProgress( bool state )
{
	sceneTransitionInProgress = state;
}
