/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _BUTTONS_H_INCLUDE
#define _BUTTONS_H_INCLUDE

#include <SDL.h>
#include <SDL_mixer.h>
#include "Node.h"

// texture for default states of button: on, off and pressed
typedef struct {
	SDL_Texture		*img_enabled; 
	SDL_Texture		*img_pressed; 
	SDL_Texture		*img_disabled;
} ButtonSkin_t;


class Button : public Node {
	
public:

	// constructor of Button class
	Button();

	// enable or disable a button
	void		SetEnabled( bool state );

	// returns if a button is enabled or disabled
	bool		IsEnabled();

	/*
		sometimes we want lock the button even if it's enabled: in the gamble, for example,
		once the player choose a button, the choosen button (a toggle button) must remain active
		and refuse all subsequent touch until the card is extracted;
		this feature is useless if the button is already disabled
	*/
	void		SetLocked( bool lockState );

	/*
		default animation scale up a little the button when clicked, you can disable animation passing false;
		default value is "enabled" when Button is created
	*/
	void		SetDefaultAnimation( bool enabled );

	/*
		sometimes we want to press the button without user interaction (e.g. inactivity timeout)
		this is the function that game must call to press the button
	*/
	void		Press();

protected:

	bool		enabled;			// if true the button is currently enabled
	bool		pressed;			// if true the button is currently pressed
	bool		locked;				// if true the button is temporarily locked even if is enabled	
	Mix_Chunk	*clickSound;		// sound of click

	/*
		- start pressure animation of the button
		- set the flag pressed to true
		- set global flag that a software button is pressed (to avoid further button pressures)
	*/
	void		StartPushAnimation();

private:

	// state of default animation (scale)
	bool		defaultAnimation;

	// function called each frame (override of function in Node class)
	void Draw();

	// this function is called at the end of pressure animation to release the button
	void SetPressed( bool state );

	//friend class Engine;
	friend class ReleaseButton;	
};


class  SimpleButton : public Button {
	
public:

	// Simple button constructor
	SimpleButton( unsigned int tag, unsigned int zOrder, ButtonSkin_t *skin, Mix_Chunk *clickSound  );

	// change skin of button, setup new images for enabled, pressed and disabled state
	void SetSkin( ButtonSkin_t *skin );

private:

	// function called each frame (override of function in Node class)
	void Draw();

	// perform action if buttos is touched by the user	
	void OnClick();

	// textures for states of button
	ButtonSkin_t	skin;
};




class  ToggleButton : public Button {
	
public:

	// Simple button constructor
	ToggleButton( unsigned int tag, unsigned int zOrder, ButtonSkin_t *skinOff, ButtonSkin_t *skinOn, Mix_Chunk *clickSound );

	// force the state of the toggle button
	void SetState( bool state );

	// returns current state of toggle button
	bool GetState();

	// ========================= functions below are used internally, don't use in the game =======================


private:

	// function called each frame (override of function in Node class)
	void Draw();

	// perform action if buttos is touched by the user	
	void OnClick();

	// textures for states of button
	bool			state;
	ButtonSkin_t	skinOff; 
	ButtonSkin_t	skinOn;
	ButtonSkin_t	*currentSkin;
};

#endif