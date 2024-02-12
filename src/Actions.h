/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdarg.h>
#include "EngineCommon.h"
#include "Node.h"
#include "Buttons.h"


#ifndef _ACTION_H_INCLUDE
#define _ACTION_H_INCLUDE

// result of an action
typedef enum {
	EXECUTERESULT_IN_PROGRESS = -2,
	EXECUTERESULT_DONE,
	EXECUTERESULT_REPEAT,
	EXECUTERESULT_REPEAT_1_ACTION_BACK,
	EXECUTERESULT_REPEAT_2_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_3_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_4_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_5_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_6_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_7_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_8_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_9_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_10_ACTIONS_BACK,
	EXECUTERESULT_REPEAT_MAX_ACTIONS_BACK
} ExecuteResult_t;

// type of interpolators
typedef enum {
	INTERPOLATION_LINEAR,
	INTERPOLATION_SINE_EASE_IN,
	INTERPOLATION_SINE_EASE_OUT,
	INTERPOLATION_SINE_EASE_IN_OUT,
	INTERPOLATION_EXPO_EASE_IN,
	INTERPOLATION_EXPO_EASE_OUT,
	INTERPOLATION_EXPO_EASE_IN_OUT,
	INTERPOLATION_BOUNCE_EASE_IN,
	INTERPOLATION_BOUNCE_EASE_OUT,
	INTERPOLATION_BOUNCE_EASE_IN_OUT,
	INTERPOLATION_ELASTIC_EASE_IN,
	INTERPOLATION_ELASTIC_EASE_OUT,
	INTERPOLATION_ELASTIC_EASE_IN_OUT
} Interpolation_t;

// type of action
typedef enum {
	ACTIONTYPE_INTERVAL,
	ACTIONTYPE_INSTANT,
	ACTIONTYPE_UNKNOWN,
} ActionType_t;

// base class for actions
class Action {
	
public:

	// constructor
	Action();

	// get unique action identifier
	unsigned int			GetTag();
	
	// returns type of action (interval or instant)
	ActionType_t			GetType();

	// this function must be overriden by actions
	virtual ExecuteResult_t Execute( float deltaTime );

	// this function may be overridden by actions
	virtual void			Start();


protected:
	// target of action
	Node*					target;		
	// unique identifier for action
	unsigned int			tag;	
	// type of action: instant or interval
	ActionType_t			type;
};



// base class for sequence of actions
class ActionsSequence {
	
public:

	// constructor
	ActionsSequence( unsigned int tag, Node* target, ... );

	// get unique action identifier
	unsigned int			GetTag();

	// return total number of action in the sequence
	unsigned int			GetTotalActions();

	// pointer to actions
	Action					*actions[ ACTIONSSEQUENCE_MAX_SEQUENCE_ACTIONS ];
	// total number of actions
	unsigned int			totalActions;

protected:
	// target of action
	Node*					target;		
	// unique identifier for action
	unsigned int			tag;		

private:


};


// ================================== Action Instant =============================================
// These action can be inserted in a ActionsSequence object, when the ActionManager encounters them
// it executes them immediately.


class ActionInstant : public Action {
public:
	// constructor
	ActionInstant();

};

// Show the object -> call setVisible( true )
class Show : public ActionInstant {
public:
	Show( Node* target );
	ExecuteResult_t Execute( float deltaTime );
};

// Hide the object -> call setVisible( false )
class Hide : public ActionInstant {
public:
	Hide( Node* target );
	ExecuteResult_t Execute( float deltaTime );
};

// Place the object at a specific position
class Place : public ActionInstant {
public:
	Place( Node* target, float dstX, float dstY );
	ExecuteResult_t Execute( float deltaTime );
private:
	Coord_t		position;
};

// Modify only the horizontal coordinate of the object
class PlaceX : public ActionInstant {
public:
	PlaceX( Node* target, float dstX );
	ExecuteResult_t Execute( float deltaTime );
private:
	float		dstX;
};

// Modify only the vertical coordinate of the object
class PlaceY : public ActionInstant {
public:
	PlaceY( Node* target, float dstY );
	ExecuteResult_t Execute( float deltaTime );
private:
	float		dstY;
};

// Set rotation angle of the object
class Rotate : public ActionInstant {
public:
	Rotate( Node* target, float angle );
	ExecuteResult_t Execute( float deltaTime );
private:
	float		angle;
};

// Set scale rate of the object
class Scale : public ActionInstant {
public:
	Scale( Node* target, float scale );
	ExecuteResult_t Execute( float deltaTime );
private:
	float		scale;
};

// Set transparency of the object
class Alpha : public ActionInstant {
public:
	Alpha( Node* target, int alpha );
	ExecuteResult_t Execute( float deltaTime );
private:
	int			alpha;
};

// Set new Z order of the object
class ZOrderChange : public ActionInstant {
public:
	ZOrderChange( Node* target, int newZOrder );
	ExecuteResult_t Execute( float deltaTime );
private:
	int			newZOrder;
};


// for a Sprite object only!! Set new texture 
class TextureChange : public ActionInstant {
public:
	TextureChange( Node* target, SDL_Texture *newTexture );
	ExecuteResult_t Execute( float deltaTime );
private:
	SDL_Texture		*newTexture;
};

// Play sound in a sequence of actions
class PlayFx : public ActionInstant {
public:
	PlayFx( int tag, Mix_Chunk *soundFx, int audioChannel );
	void SetSound( Mix_Chunk *soundFx );
	ExecuteResult_t Execute( float deltaTime );
private:
	Mix_Chunk		*soundFx;
	int				audioChannel;
};


// Repeat the sequence from the beginning forever
class RepeatForever : public ActionInstant {
public:
	RepeatForever();
	ExecuteResult_t Execute( float deltaTime );
};

// Repeat the full sequence from the beginning n-times 
class RepeatCount : public ActionInstant {
public:
	RepeatCount( unsigned int times );
	ExecuteResult_t Execute( float deltaTime );
private:
	unsigned int times;
};


// Repeat the sequence n-actions back for n-times (max up to 10 actions back!)
class RepeatBackCount : public ActionInstant {
public:
	RepeatBackCount( unsigned int nActions, unsigned int times );
	ExecuteResult_t Execute( float deltaTime );
private:
	unsigned int nActions;
	unsigned int times;
};

/*
	Repeat the sequence from the beginning or continue with following actions
	The action must carry a callback function that MUST RETURN:
	- true if the sequence must be repeated
	- false if the sequence must continue forward
*/
class RepeatCondition : public ActionInstant {
public:
	RepeatCondition( int tag, bool ( *conditionCallback )( int ) );
	ExecuteResult_t Execute( float deltaTime );
private:
	bool ( *conditionCallback )( int );
};

/*
	Repeat the sequence from a number of actions back (nActions) or continue with following actions
	The action must carry a callback function that MUST RETURN:
	- true if the sequence must be repeated
	- false if the sequence must continue forward
*/
class RepeatBackCondition : public ActionInstant {
public:
	RepeatBackCondition( int tag, unsigned int nActions, bool ( *conditionCallback )( int ) );
	ExecuteResult_t Execute( float deltaTime );
private:
	bool ( *conditionCallback )( int );
	unsigned int nActions;
};





// Callback to perform some action in the game
class Callback : public ActionInstant {
public:
	Callback( int tag, void ( *callback )( int ) );
	ExecuteResult_t Execute( float deltaTime );
private:
	void ( *callback )( int );
};

// Callback to perform some action in the game
class CallbackInteger : public ActionInstant {
public:
	CallbackInteger( int tag, void ( *callback )( int, int ), int value );
	ExecuteResult_t Execute( float deltaTime );
private:
	void ( *callback )( int, int );
	int		value;
};




/*
	This particular instant action is used internally to perform the pressure animation of a button
	DON'T USE IT IN THE GAME!
*/
class ReleaseButton : public ActionInstant {
public:
	ReleaseButton( Button *target );
	ExecuteResult_t Execute( float deltaTime );
};




// ================================== Action Interval =============================================
// Interval action are used for action with a predefined duration and can be inserted in a 
// ActionsSequence object; you can interpolate these action with the SetInterpolation function.
// These actions may have a restart function that should reset the action in case of loop


// class for action with a predefined duration
class ActionInterval : public Action {
	
public:

	// constructor
	ActionInterval();

	// set interpolation of action 
	void SetInterpolation( Interpolation_t interpolation );

protected:
	// total time elapsed from the start of action
	float			elapsed;		
	// total duration of action in milliseconds
	float			duration;		
	// calculate the percentage (0.0 to 1.0) of the action (=elapsed/duration)
	float			GetElapsedPercentage();
	// interpolatore function pointer (default is linear)
	float			( *Interpolator )( float );
};

// move to a position
class MoveTo : public ActionInterval {
public:
	MoveTo( unsigned int tag, Node* target, float dstX, float dstY, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	Coord_t		src;
	Coord_t		diff;
	Coord_t		dst;
};

// rotate to an angle
class RotateTo : public ActionInterval {
public:
	RotateTo( unsigned int tag, Node* target, float finalAngle, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	float		startAngle;
	float		diffAngle;
	float		finalAngle;
};

// resize object to a size
class ScaleTo : public ActionInterval {
public:
	ScaleTo( unsigned int tag, Node* target, float finalSize, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	float		startSize;
	float		diffSize;
	float		finalSize;
};

// resize object horizontally to a size
class ScaleXTo : public ActionInterval {
public:
	ScaleXTo( unsigned int tag, Node* target, float finalSize, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	float		startSize;
	float		diffSize;
	float		finalSize;
};

// resize object vertically to a size
class ScaleYTo : public ActionInterval {
public:
	ScaleYTo( unsigned int tag, Node* target, float finalSize, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	float		startSize;
	float		diffSize;
	float		finalSize;
};


// set alpha
class AlphaTo : public ActionInterval {
public:
	AlphaTo( unsigned int tag, Node* target, int finalAlpha, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	int			startAlpha;
	int			diffAlpha;
	int			finalAlpha;
};

// blink
class Blink : public ActionInterval {
public:
	Blink( unsigned int tag, Node* target, int blinks, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	int			blinks;
};

// delay (no action for an interval)
class DelayTime : public ActionInterval {
public:
	DelayTime( float duration );
	ExecuteResult_t Execute( float deltaTime );
};

// spline
class SplineTo : public ActionInterval {
public:
	SplineTo( unsigned int tag, Node* target, unsigned int totalPoints, Coord_t *pointsArray, float tension, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	#define ACTIONSPLINETO_MAX_POINTS	48
	Coord_t			points[ ACTIONSPLINETO_MAX_POINTS ];
	unsigned int	totalPoints;
	float			tension;
	float			deltaT;
	Coord_t			GetPointAt( unsigned int index );
	Coord_t			GetSplinePointAt( Coord_t p0, Coord_t p1, Coord_t p2, Coord_t p3, float tension, float t );
};

// shake
class Shake : public ActionInterval {
public:
	Shake( unsigned int tag, Node* target, float radius, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	Coord_t			originalPosition;
	float			radius;
	uint32_t		seed;
};

// TintTo (for SDL_Texture objects only!, not Node*)
class TintTo : public ActionInterval {
public:
	TintTo( unsigned int tag, SDL_Texture* texture, unsigned char r, unsigned char g, unsigned char b, float duration );
	ExecuteResult_t Execute( float deltaTime );
	void Start();
private:
	SDL_Texture		*targetTexture;
	Color_t			startColor;
	Color_t			finalColor;
	int				diffR;
	int				diffG;
	int				diffB;
};



#endif