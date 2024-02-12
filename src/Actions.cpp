/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include "Actions.h"
#include "Engine.h"
#include "Misc.h"
#include "Sprite.h"

#ifndef M_PI_X_2
#define M_PI_X_2 (float)M_PI * 2.0f
#endif


Action::Action()
{
	this->tag		= 0;
	this->target	= NULL;
	this->type		= ACTIONTYPE_UNKNOWN;
}

unsigned int Action::GetTag()
{
	return tag;
}

ActionType_t Action::GetType()
{
	return type;
}

ExecuteResult_t Action::Execute( float deltaTime )
{
	// overriden by any action
	return EXECUTERESULT_DONE;
}

void Action::Start()
{
	// overridden by action
}



ActionsSequence::ActionsSequence( unsigned int tag, Node* target, ... )
{
	int	actionsCount = 0;
	// set tag and target object
	this->tag		= tag;
	this->target	= target;
	// reset all action buffer to NULL
    for( int i = 0; i <= ACTIONSSEQUENCE_MAX_SEQUENCE_ACTIONS; i++ ) {
		this->actions[ i ] = NULL;
    }
	// insert actions into ActionsSequence'list
    va_list ap;
    va_start( ap, target );
	do {
		// extract until a NULL is found
		Action *pAction = va_arg( ap, Action* );
		if( !pAction ) {
			break;
		}
		// insert action into ActionsSequence'list
		this->actions[ actionsCount ] = pAction;
		// increment actions counter
		actionsCount += 1;
	} while( true );
    va_end( ap );
	// set total number of action
	this->totalActions = actionsCount;
}

unsigned int ActionsSequence::GetTag()
{
	return tag;
}

// return total number of action in the sequence
unsigned int ActionsSequence::GetTotalActions()
{
	return totalActions;
}




// ================================== Action Instant =============================================
ActionInstant::ActionInstant()
{
	type = ACTIONTYPE_INSTANT;
}

Show::Show( Node* target )
{
	this->tag = 0;
	this->target = target;
}

ExecuteResult_t Show::Execute( float deltaTime )
{
	target->SetVisible( true );
	return ( EXECUTERESULT_DONE );
}

Hide::Hide( Node* target )
{
	this->tag = 0;
	this->target = target;
}

ExecuteResult_t Hide::Execute( float deltaTime )
{
	target->SetVisible( false );
	return ( EXECUTERESULT_DONE );
}

Place::Place( Node* target, float dstX, float dstY )
{
	this->tag = 0;
	this->target = target;
	this->position.x = dstX;
	this->position.y = dstY;
}

ExecuteResult_t Place::Execute( float deltaTime )
{
	target->SetPosition( position.x, position.y );
	return ( EXECUTERESULT_DONE );
}

PlaceX::PlaceX( Node* target, float dstX )
{
	this->tag = 0;
	this->target = target;
	this->dstX = dstX;
}

ExecuteResult_t PlaceX::Execute( float deltaTime )
{
	target->SetPosition( this->dstX, target->GetPositionY() );
	return ( EXECUTERESULT_DONE );
}

PlaceY::PlaceY( Node* target, float dstY )
{
	this->tag = 0;
	this->target = target;
	this->dstY = dstY;
}

ExecuteResult_t PlaceY::Execute( float deltaTime )
{
	target->SetPosition( target->GetPositionX(), this->dstY );
	return ( EXECUTERESULT_DONE );
}

Rotate::Rotate( Node* target, float angle )
{
	this->tag = 0;
	this->target = target;
	this->angle = angle;
}

ExecuteResult_t Rotate::Execute( float deltaTime )
{
	target->SetAngle( angle );
	return ( EXECUTERESULT_DONE );
}

Scale::Scale( Node* target, float scale )
{
	this->tag = 0;
	this->target = target;
	this->scale = scale;
	
}

ExecuteResult_t Scale::Execute( float deltaTime )
{
	target->SetSizeRate( scale );
	return ( EXECUTERESULT_DONE );
}

Alpha::Alpha( Node* target, int alpha )
{
	this->tag = 0;
	this->target = target;
	this->alpha = alpha;
}

ExecuteResult_t Alpha::Execute( float deltaTime )
{
	target->SetAlpha( alpha );
	return ( EXECUTERESULT_DONE );
}

ZOrderChange::ZOrderChange( Node* target, int newZOrder )
{
	this->tag = 0;
	this->target = target;
	this->newZOrder = newZOrder;
}

ExecuteResult_t ZOrderChange::Execute( float deltaTime )
{
	target->SetZOrder( newZOrder );
	return ( EXECUTERESULT_DONE );
}


TextureChange::TextureChange( Node* target, SDL_Texture *newTexture )
{
	this->tag = 0;
	this->target = target;
	this->newTexture = newTexture;
}

ExecuteResult_t TextureChange::Execute( float deltaTime )
{
	((Sprite*)target)->SetTexture( newTexture );
	return ( EXECUTERESULT_DONE );
}


PlayFx::PlayFx( int tag, Mix_Chunk *soundFx, int audioChannel )
{
	this->tag = 0;
	this->soundFx = soundFx;
	this->audioChannel = audioChannel;
}

void PlayFx::SetSound( Mix_Chunk *soundFx )
{
	this->soundFx = soundFx;
}

ExecuteResult_t PlayFx::Execute( float deltaTime )
{
	Mix_PlayChannel( audioChannel, soundFx, 0 );
	return ( EXECUTERESULT_DONE );
}



RepeatForever::RepeatForever()
{
	this->tag = 0;
	this->target = NULL;
}

ExecuteResult_t RepeatForever::Execute( float deltaTime )
{
	return ( EXECUTERESULT_REPEAT );
}

RepeatCount::RepeatCount( unsigned int times )
{
	this->tag = 0;
	this->target = NULL;
	this->times = times;
}

ExecuteResult_t RepeatCount::Execute( float deltaTime )
{
	ExecuteResult_t result;
	times -= 1;
	if( times == 0 ) {
		result = EXECUTERESULT_DONE;
	} else {
		result = EXECUTERESULT_REPEAT;
	}
	return result;
}

RepeatBackCount::RepeatBackCount( unsigned int nActions, unsigned int times )
{
	this->tag = 0;
	this->target = NULL;
	this->times = times;
	if( nActions >= EXECUTERESULT_REPEAT_MAX_ACTIONS_BACK ) {
		nActions = EXECUTERESULT_REPEAT_MAX_ACTIONS_BACK - 1;
	}
	this->nActions = nActions;
}

ExecuteResult_t RepeatBackCount::Execute( float deltaTime )
{
	ExecuteResult_t result;
	times -= 1;
	if( times == 0 ) {
		result = EXECUTERESULT_DONE;
	} else {
		result = static_cast<ExecuteResult_t>(nActions);
	}
	return result;
}

RepeatCondition::RepeatCondition( int tag, bool ( *callback )( int ) )
{
	this->tag = tag;
	this->target = NULL;
	this->conditionCallback = callback;
}

ExecuteResult_t RepeatCondition::Execute( float deltaTime )
{
	ExecuteResult_t result = EXECUTERESULT_DONE;
	bool callbackResult = false;
	if( conditionCallback != NULL ) {
		callbackResult = conditionCallback( tag );
		if( callbackResult ) {
			result = EXECUTERESULT_REPEAT;		
		} else {
			result = EXECUTERESULT_DONE;
		}
	}
	return result;
}

RepeatBackCondition::RepeatBackCondition( int tag, unsigned int nActions, bool ( *conditionCallback )( int ) )
{
	this->tag = tag;
	this->target = NULL;
	this->conditionCallback = conditionCallback;
	if( nActions >= EXECUTERESULT_REPEAT_MAX_ACTIONS_BACK ) {
		nActions = EXECUTERESULT_REPEAT_MAX_ACTIONS_BACK - 1;
	}
	this->nActions = nActions;
}

ExecuteResult_t RepeatBackCondition::Execute( float deltaTime )
{
	ExecuteResult_t result = EXECUTERESULT_DONE;
	bool callbackResult = false;
	if( conditionCallback != NULL ) {
		callbackResult = conditionCallback( tag );
		if( callbackResult ) {
			result = static_cast<ExecuteResult_t>( nActions );		
		} else {
			result = EXECUTERESULT_DONE;
		}
	}
	return result;
}

Callback::Callback( int tag, void ( *callback )( int ) )
{
	this->tag = tag;
	this->target = NULL;
	this->callback = callback;
}

ExecuteResult_t Callback::Execute( float deltaTime )
{
	if( callback != NULL ) {
		callback( tag );
	}
	return ( EXECUTERESULT_DONE );
}

CallbackInteger::CallbackInteger( int tag, void ( *callback )( int, int ), int value )
{
	this->tag = tag;
	this->target = NULL;
	this->callback = callback;
	this->value = value;
}

ExecuteResult_t CallbackInteger::Execute( float deltaTime )
{
	if( callback != NULL ) {
		callback( tag, value );
	}
	return ( EXECUTERESULT_DONE );
}


// this action (ReleaseButton) is internally reserved to Buttons, the game MUST NOT USE IT! 
ReleaseButton::ReleaseButton( Button *target )
{
	this->tag = 0;
	this->target = target;
}

ExecuteResult_t ReleaseButton::Execute( float deltaTime )
{
	((Button*)target)->SetPressed( false );
	Engine::SetButtonCurrentlyPressed( false );
	Engine::GetConfig()->ReleasedButtonCallback( target, target->GetTag() );
	return ( EXECUTERESULT_DONE );
}






// ================================== Action Interval =============================================

// Linear
static float InterpolatorLinear( float percentage )
{
	return percentage;
}

// Sine Ease
static float InterpolatorSineEaseIn(float time)
{
    return -1 * cosf(time * (float)M_PI_2) + 1;
}
    
static float InterpolatorSineEaseOut(float time)
{
    return sinf(time * (float)M_PI_2);
}
    
static float InterpolatorSineEaseInOut(float time)
{
    return -0.5f * (cosf((float)M_PI * time) - 1);
}

// Expo Ease
static float InterpolatorExpoEaseIn(float time)
{
    return time == 0 ? 0 : powf(2, 10 * (time/1 - 1)) - 1 * 0.001f;
}

static float InterpolatorExpoEaseOut(float time)
{
    return time == 1 ? 1 : (-powf(2, -10 * time / 1) + 1);
}

static float InterpolatorExpoEaseInOut(float time)
{
    if(time == 0 || time == 1) 
        return time;
    
    if (time < 0.5f)
        return 0.5f * powf(2, 10 * (time * 2 - 1));

    return 0.5f * (-powf(2, -10 * (time * 2 - 1)) + 2);
}

// Bounce Ease
static float InterpolatorBounceTime(float time)
{
    if (time < 1 / 2.75f)
    {
        return 7.5625f * time * time;
    }
    else if (time < 2 / 2.75f)
    {
        time -= 1.5f / 2.75f;
        return 7.5625f * time * time + 0.75f;
    }
    else if(time < 2.5f / 2.75f)
    {
        time -= 2.25f / 2.75f;
        return 7.5625f * time * time + 0.9375f;
    }

    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}

static float InterpolatorBounceEaseIn(float time)
{
    return 1 - InterpolatorBounceTime(1 - time);
}

static float InterpolatorBounceEaseOut(float time)
{
    return InterpolatorBounceTime(time);
}

static float InterpolatorBounceEaseInOut(float time)
{
    float newT = 0;
    if (time < 0.5f)
    {
        time = time * 2;
        newT = (1 - InterpolatorBounceTime(1 - time)) * 0.5f;
    }
    else
    {
        newT = InterpolatorBounceTime(time * 2 - 1) * 0.5f + 0.5f;
    }

    return newT;
}

// Elastic Ease (0.9 ~ 0.1)
static float period = 0.3f;

static float InterpolatorElasticEaseIn(float time)
{

    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        float s = period / 4;
        time = time - 1;
        newT = -powf(2, 10 * time) * sinf((time - s) * M_PI_X_2 / period);
    }

    return newT;
}
static float InterpolatorElasticEaseOut(float time)
{

    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        float s = period / 4;
        newT = powf(2, -10 * time) * sinf((time - s) * M_PI_X_2 / period) + 1;
    }

    return newT;
}
static float InterpolatorElasticEaseInOut(float time)
{

    float newT = 0;
    if (time == 0 || time == 1)
    {
        newT = time;
    }
    else
    {
        time = time * 2;
        if (! period)
        {
            period = 0.3f * 1.5f;
        }

        float s = period / 4;

        time = time - 1;
        if (time < 0)
        {
            newT = -0.5f * powf(2, 10 * time) * sinf((time -s) * M_PI_X_2 / period);
        }
        else
        {
            newT = powf(2, -10 * time) * sinf((time - s) * M_PI_X_2 / period) * 0.5f + 1;
        }
    }
    return newT;
}




ActionInterval::ActionInterval()
{
	elapsed			= 0;
	duration		= 0;
	Interpolator	= InterpolatorLinear;
	type			= ACTIONTYPE_INTERVAL;
}

void ActionInterval::SetInterpolation( Interpolation_t interpolation )
{
	switch( interpolation ) {
		case INTERPOLATION_SINE_EASE_IN:			Interpolator = InterpolatorSineEaseIn;			break;
		case INTERPOLATION_SINE_EASE_OUT:			Interpolator = InterpolatorSineEaseOut;			break;
		case INTERPOLATION_SINE_EASE_IN_OUT:		Interpolator = InterpolatorSineEaseInOut;		break;
		case INTERPOLATION_EXPO_EASE_IN:			Interpolator = InterpolatorExpoEaseIn;			break;
		case INTERPOLATION_EXPO_EASE_OUT:			Interpolator = InterpolatorExpoEaseOut;			break;
		case INTERPOLATION_EXPO_EASE_IN_OUT:		Interpolator = InterpolatorExpoEaseInOut;		break;
		case INTERPOLATION_BOUNCE_EASE_IN:			Interpolator = InterpolatorBounceEaseIn;		break;
		case INTERPOLATION_BOUNCE_EASE_OUT:			Interpolator = InterpolatorBounceEaseOut;		break;
		case INTERPOLATION_BOUNCE_EASE_IN_OUT:		Interpolator = InterpolatorBounceEaseInOut;		break;
		case INTERPOLATION_ELASTIC_EASE_IN:			Interpolator = InterpolatorElasticEaseIn;		break;
		case INTERPOLATION_ELASTIC_EASE_OUT:		Interpolator = InterpolatorElasticEaseOut;		break;
		case INTERPOLATION_ELASTIC_EASE_IN_OUT:		Interpolator = InterpolatorElasticEaseInOut;	break;
		default:									Interpolator = InterpolatorLinear;				break;
	}
}

float ActionInterval::GetElapsedPercentage()
{
	float percentage = 0;
	if( duration > 0 ) {
		percentage = elapsed / duration;
		if( percentage > 1 ) {
			percentage = 1;
		}
	} else {
		percentage = 1;
	}
	return percentage;
}





MoveTo::MoveTo( unsigned int tag, Node* target, float dstX, float dstY, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	dst.x = dstX;
	dst.y = dstY;
	Start();
}

void MoveTo::Start()
{
	this->elapsed	= 0;
	Coord_t point	= target->GetPosition();
	src.x			= point.x;
	src.y			= point.y;
	diff.x			= dst.x - src.x;
	diff.y			= dst.y - src.y;
}

ExecuteResult_t MoveTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	float			cur_x, cur_y;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current position by time percentage
	cur_x = src.x + diff.x * interpolated_percentage;
	cur_y = src.y + diff.y * interpolated_percentage;
	// set target position 
	target->SetPosition( cur_x, cur_y );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}


RotateTo::RotateTo( unsigned int tag, Node* target, float finalAngle, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->finalAngle = finalAngle;
	Start();
}

void RotateTo::Start()
{
	this->elapsed	= 0;
	startAngle		= target->GetAngle();
	diffAngle		= finalAngle - startAngle;
}

ExecuteResult_t RotateTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	float			curAngle;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current angle by time percentage
	curAngle = startAngle + diffAngle * interpolated_percentage;
	// set target position 
	target->SetAngle( curAngle );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}

ScaleTo::ScaleTo( unsigned int tag, Node* target, float finalSize, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->finalSize = finalSize;
	Start();
}

void ScaleTo::Start()
{
	this->elapsed	= 0;
	startSize	= target->GetSizeRate();
	diffSize	= finalSize - startSize;
}

ExecuteResult_t ScaleTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	float			curSize;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current size by time percentage
	curSize = startSize + diffSize * interpolated_percentage;
	// set target position 
	target->SetSizeRate( curSize );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}

ScaleXTo::ScaleXTo( unsigned int tag, Node* target, float finalSize, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->finalSize = finalSize;
	Start();
}

void ScaleXTo::Start()
{
	this->elapsed	= 0;
	startSize	= target->GetXSizeRate();
	diffSize	= finalSize - startSize;
}

ExecuteResult_t ScaleXTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	float			curSize;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current size by time percentage
	curSize = startSize + diffSize * interpolated_percentage;
	// set target position 
	target->SetXSizeRate( curSize );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}

ScaleYTo::ScaleYTo( unsigned int tag, Node* target, float finalSize, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->finalSize = finalSize;
	Start();
}

void ScaleYTo::Start()
{
	this->elapsed	= 0;
	startSize	= target->GetYSizeRate();
	diffSize	= finalSize - startSize;
}

ExecuteResult_t ScaleYTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	float			curSize;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current size by time percentage
	curSize = startSize + diffSize * interpolated_percentage;
	// set target position 
	target->SetYSizeRate( curSize );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}

Blink::Blink( unsigned int tag, Node* target, int blinks, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->blinks	= blinks;
}

void Blink::Start()
{
	this->elapsed	= 0;
}

ExecuteResult_t Blink::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current visibility by number of blinks / duration
    float slice = 1.0f / blinks;
    float m = fmodf( interpolated_percentage, slice );
    target->SetVisible( m > slice / 2 ? true : false );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}

AlphaTo::AlphaTo( unsigned int tag, Node* target, int finalAlpha, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->finalAlpha = finalAlpha;
	Start();
}

void AlphaTo::Start()
{
	this->elapsed	= 0;
	startAlpha = target->GetAlpha();
	diffAlpha = finalAlpha - startAlpha;
}

ExecuteResult_t AlphaTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	float			curAlpha;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current alpha by time percentage
	curAlpha = startAlpha + diffAlpha * interpolated_percentage;
	// set target position 
	target->SetAlpha( curAlpha );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}

DelayTime::DelayTime( float duration )
{
	this->tag		= 0;
	this->target	= NULL;
	this->duration	= duration;
}

ExecuteResult_t DelayTime::Execute( float deltaTime )
{
	float			percentage;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}


SplineTo::SplineTo( unsigned int tag, Node* target, unsigned int totalPoints, Coord_t *pointsArray, float tension, float duration )
{
	this->tag			= tag;
	this->target		= target;
	this->duration		= duration;
	this->tension		= tension;
	this->totalPoints	= totalPoints;
	this->deltaT		= (float) 1 / totalPoints;

	for( unsigned int i = 0; i < totalPoints; i++ ) {
		this->points[ i ] = pointsArray[ i ];
	}
	Start();
}

void SplineTo::Start()
{
	this->elapsed	= 0;
	this->target->SetPosition( points[ 0 ].x, points[ 0 ].y );
}

Coord_t SplineTo::GetPointAt( unsigned int index )
{
    index = min( totalPoints - 1, max( index, 0 ) );
	return points[ index ];	
}

Coord_t	SplineTo::GetSplinePointAt( Coord_t p0, Coord_t p1, Coord_t p2, Coord_t p3, float tension, float t )
{
    float	t2 = t * t;
    float	t3 = t2 * t;
	Coord_t	result;

    float s = (1 - tension) / 2;
	
    float b1 = s * ((-t3 + (2 * t2)) - t);                      // s(-t3 + 2 t2 - t)P1
    float b2 = s * (-t3 + t2) + (2 * t3 - 3 * t2 + 1);          // s(-t3 + t2)P2 + (2 t3 - 3 t2 + 1)P2
    float b3 = s * (t3 - 2 * t2 + t) + (-2 * t3 + 3 * t2);      // s(t3 - 2 t2 + t)P3 + (-2 t3 + 3 t2)P3
    float b4 = s * (t3 - t2);                                   // s(t3 - t2)P4
    
    float newX = (p0.x*b1 + p1.x*b2 + p2.x*b3 + p3.x*b4);
    float newY = (p0.y*b1 + p1.y*b2 + p2.y*b3 + p3.y*b4);
	
	result.x = newX;
	result.y = newY;

	return result;
}

ExecuteResult_t SplineTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
    unsigned int	p;
    float			lt;

	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current point and lt(??)
	if( percentage == 1 ) {
        p	= totalPoints - 1;
        lt	= 1;
    } else {
        p	= percentage / deltaT;
        lt	= ( percentage - deltaT * (float)p) / deltaT;
    }
    Coord_t pp0 = GetPointAt( p - 1 );
    Coord_t pp1 = GetPointAt( p + 0 );
    Coord_t pp2 = GetPointAt( p + 1 );
    Coord_t pp3 = GetPointAt( p + 2 );
	Coord_t newPos = GetSplinePointAt( pp0, pp1, pp2, pp3, tension, lt );
	// set new position of target
	target->SetPosition( newPos.x, newPos.y );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}



Shake::Shake( unsigned int tag, Node* target, float radius, float duration )
{
	this->tag		= tag;
	this->target	= target;
	this->duration	= duration;

	this->radius			= radius;
	this->originalPosition	= target->GetPosition();
	Start();
}

void Shake::Start()
{
	this->elapsed	= 0;
	this->seed		= rand();
}

ExecuteResult_t Shake::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	Coord_t			curPosition;
	ExecuteResult_t	result;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current position by time percentage
	if( percentage >= 1 ) {
		curPosition.x = originalPosition.x;
		curPosition.y = originalPosition.y;
		result = EXECUTERESULT_DONE;
	} else {
		curPosition.x = originalPosition.x + Misc::RANDOM_M11(&seed) * radius;
		curPosition.y = originalPosition.y + Misc::RANDOM_M11(&seed) * radius;
		result = EXECUTERESULT_IN_PROGRESS;
	}
	// set target position 
	target->SetPosition( curPosition.x, curPosition.y );
	// return proper result
	return result;
}



TintTo::TintTo( unsigned int tag, SDL_Texture* texture, unsigned char r, unsigned char g, unsigned char b, float duration )
{	
	this->tag		= tag;
	this->target	= NULL;
	this->duration	= duration;

	this->targetTexture	= texture;
	finalColor.r = r;
	finalColor.g = g;
	finalColor.b = b;

	Start();
}

void TintTo::Start()
{
	this->elapsed	= 0;

	SDL_GetTextureColorMod( targetTexture, &startColor.r, &startColor.g, &startColor.b );
	diffR = (int)finalColor.r - startColor.r;
	diffG = (int)finalColor.g - startColor.g;
	diffB = (int)finalColor.b - startColor.b;
}

ExecuteResult_t TintTo::Execute( float deltaTime )
{
	float			percentage;
	float			interpolated_percentage;
	Color_t			curColor;
	// update elapsed time from start of action
	this->elapsed += deltaTime;
	// get execution percentage (from 0.0 to 1.0) of action (elapsed/duration)
	percentage = GetElapsedPercentage();
	// get percentage modified by interpolation
	interpolated_percentage = Interpolator( percentage );
	// calculate current color by time percentage
	curColor.r = startColor.r + diffR * interpolated_percentage;
	curColor.g = startColor.g + diffG * interpolated_percentage;
	curColor.b = startColor.b + diffB * interpolated_percentage;
	// set target color
	SDL_SetTextureColorMod( targetTexture, curColor.r, curColor.g, curColor.b );
	// return proper result
	return ( percentage >= 1 ? EXECUTERESULT_DONE : EXECUTERESULT_IN_PROGRESS );
}



