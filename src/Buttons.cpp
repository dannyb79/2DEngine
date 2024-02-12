/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <SDL.h>
#include "Buttons.h"
#include "Engine.h"
#include "Actions.h"
#include "ActionManager.h"

Button::Button()
{
	this->enabled		= true;
	this->pressed		= false;
	this->locked		= false;
	this->clickSound	= NULL;
}

void Button::SetEnabled( bool state )
{
	this->enabled = state;
}

bool Button::IsEnabled()
{
	return this->enabled;
}

void Button::SetPressed( bool state )
{
	this->pressed = state;
}

void Button::SetLocked( bool lockState )
{
	this->locked = lockState;
}

void Button::SetDefaultAnimation( bool enabled )
{
	this->defaultAnimation = enabled;
}

void Button::StartPushAnimation()
{
	// create a sequence that will raise the button after a little interval
	DelayTime		*delayTime		= new DelayTime( 100 );
	ReleaseButton	*releaseButton	= new ReleaseButton( this );
	ActionsSequence *releaseSeq = new ActionsSequence( 0, this, delayTime, releaseButton, NULL );
	ActionManager::RunSequence( releaseSeq );

	// create a sequence that animates button: it become a little bigger and return back to default size
	if( defaultAnimation ) {
		ScaleTo			*scale1			= new ScaleTo( 0, this, 1.2, MILLISECONDS( 50 ) );
		ScaleTo			*scale2			= new ScaleTo( 0, this, 1, MILLISECONDS( 50 ) );
		ActionsSequence *scaleSeq = new ActionsSequence( 0, this, scale1, scale2, NULL );
		ActionManager::RunSequence( scaleSeq );
	}

	// button is currently pressed
	pressed = true;
	// set global flag that a software button has been pressed
	Engine::SetButtonCurrentlyPressed( true );
	// play sound of this button
	if( clickSound != NULL ) {
		Mix_PlayChannel( AudioChannel_0, clickSound, 0 );
	}
}

void Button::Press()
{
	OnClick();
}

void Button::Draw()
{
	// overriden by buttons own Draw functions
}





SimpleButton::SimpleButton( unsigned int tag, unsigned int zOrder, ButtonSkin_t *skin, Mix_Chunk *clickSound )
{
	this->tag			= tag;
	this->zOrder		= zOrder;
	memcpy( &this->skin, skin, sizeof( ButtonSkin_t ) );
	SDL_QueryTexture( skin->img_enabled, NULL, NULL, &this->width, &this->height );
	this->clickSound	= clickSound;
}

void SimpleButton::SetSkin( ButtonSkin_t *skin )
{
	memcpy( &this->skin, skin, sizeof( ButtonSkin_t ) );
}

void SimpleButton::OnClick()
{
	// we cannot accept the click..
	// ..if the button is disabled 
	if( !enabled ) {
		return;
	}
	// ..if another button is already pressed (and therefore another action is in progress)
	if( Engine::IsButtonPressAllowed() == false ) {
		return;
	}
	// ..or if the button is temporarily locked
	if( locked ) {
		return;
	}
	// press the button 
	StartPushAnimation();
	// inform the game that a button (identified by the tag) has been pressed
	Engine::GetConfig()->ObjectClickedCallback( this, tag );
}

void SimpleButton::Draw()
{
	if( pressed ) {
		DrawTexture( skin.img_pressed, width, height );
	} else {
		if( enabled ) {
			DrawTexture( skin.img_enabled, width, height );
		} else {
			DrawTexture( skin.img_disabled, width, height );
		}
	}
}



ToggleButton::ToggleButton( unsigned int tag, unsigned int zOrder, ButtonSkin_t *skinOff, ButtonSkin_t *skinOn, Mix_Chunk *clickSound )
{
	this->tag				= tag;
	this->zOrder			= zOrder;
	memcpy( &this->skinOn, skinOn, sizeof( ButtonSkin_t ) );
	memcpy( &this->skinOff, skinOff, sizeof( ButtonSkin_t ) );
	SDL_QueryTexture( skinOn->img_enabled, NULL, NULL, &this->width, &this->height );
	this->state				= false;
	this->clickSound		= clickSound;
	this->currentSkin		= &this->skinOff;
}

bool ToggleButton::GetState()
{
	return this->state;
}

void ToggleButton::SetState( bool state )
{
	this->state = state;
	// update pointer to current skin according to new state
	if( state ) {
		currentSkin = &this->skinOn;
	} else {
		currentSkin = &this->skinOff;
	}
}

void ToggleButton::OnClick()
{
	// we cannot accept the click..
	// ..if the button is disabled 
	if( !enabled ) {
		return;
	}
	// ..if another button is already pressed (and therefore another action is in progress)
	if( Engine::IsButtonPressAllowed() == false ) {
		return;
	}
	// ..or if the button is temporarily locked
	if( locked ) {
		return;
	}
	// press the button 
	StartPushAnimation();
	// invert the state of toggle button
	state = !state;
	// update pointer to current skin according to new state
	if( state ) {
		currentSkin = &this->skinOn;
	} else {
		currentSkin = &this->skinOff;
	}
	// inform the game that a button (identified by the tag) has been pressed
	Engine::GetConfig()->ObjectClickedCallback( this, tag );
}

void ToggleButton::Draw()
{
	if( pressed ) {
		DrawTexture( currentSkin->img_pressed, width, height );
	} else {
		if( enabled ) {
			DrawTexture( currentSkin->img_enabled, width, height );
		} else {
			DrawTexture( currentSkin->img_disabled, width, height );
		}
	}
}
