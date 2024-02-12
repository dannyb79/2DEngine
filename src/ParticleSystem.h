/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include "Node.h"
#include "Engine.h"

// particle modality: gravity o radius
typedef enum {
    MODE_GRAVITY,
    MODE_RADIUS
} PSMode_t;

// shape of emitter
typedef enum {
    ES_RECTANGULAR_AREA,
	ES_RECTANGULAR_PERIMETER,
	ES_CIRCLE_AREA,
	ES_CIRCLE_PERIMETER,
	ES_POINTS_ARRAY
} EmitterShape_t;


// color info using opengl format
class Color4F 
{
public:
	Color4F() {}
	Color4F( float r, float g, float b, float a ) { this->r = r, this->g = g, this->b = b, this->a = a; }
    float r, g, b, a;
};

// this is the coordinate class
class Pointf
{
public:
    Pointf() {}
    Pointf(float _x, float _y)
        : x(_x)
        , y(_y)
    {
    }
    ~Pointf() {}
    float x, y;
	float getAngle();
	void reset();
};



// particle system configuration
typedef struct 
{
	PSMode_t		mode;					// mode of the emitter: GRAVITY or RADIUS
	unsigned int	totalParticles;			// maximum number of particles (200 is a good number for almost everything)
	float			duration;				// how many seconds the emitter will run or use (-1) forever
	float			emissionRate;			// emission rate of particles, that's the number of particles emitted in 1 seconds (10.0f = 10 particles per second)
	EmitterShape_t	emitterShape;			// where particles birth
	Coord_t			*points;				// TODO			
	long			totalPoints;			// TODO			
	Pointf			positionVar;			// position variance of the emitter, values are in pixels
	float			angle;					// angle (degrees) of emitter: 0 right, 90 down, 180 left, 270 up
	float			angleVar;				// angle variance (degrees)
	float			life;					// life (seconds) of each particle 
	float			lifeVar;				// life variance
	float			startSize;				// start size (pixel) of each particle
	float			startSizeVar;			// start size variance
	float			endSize;				// end size (pixel) of each particle
	float			endSizeVar;				// end size variance
	float			startSpin;				// start angle (degrees) of each particle
	float			startSpinVar;			// start angle variance
	float			endSpin;				// end angle (degrees) of each particle
	float			endSpinVar;				// end angle variance
	Color4F			startColor;				// start color of each particle
	Color4F			startColorVar;			// start color variance
	Color4F			endColor;				// end color of each particle
	Color4F			endColorVar;			// end color variance
	int				yCoordFlipped;			// y flipped (???)

	// Mode GRAVITY
	Pointf			gravity;				// set force of gravity (pixels?)
	float			speed;					// set speed (pixels?)
	float			speedVar;				// set speed variance (pixels?)
	float			tangentialAccel;		// sets the tangential acceleration
	float			tangentialAccelVar;		// sets the tangential acceleration variance
	float			radialAccel;			// sets the radial acceleration
	float			radialAccelVar;			// sets the radial acceleration variance
	bool			rotationIsDirection;	// if true fix rotation to the angle of emitter

	// Mode RADIUS
	float			startRadius;			// set start radius (degrees)
	float			startRadiusVar;			// set start radius variance (degrees)
	float			endRadius;				// set end radius (degrees)
	float			endRadiusVar;			// set end radius variance (degrees)
	float			rotatePerSecond;		// set rotation (degrees) per second
	float			rotatePerSecondVar;		// set rotation (degrees) per second variance

} ParticleSystemConfig_t;

// all data of a single particle
class ParticleData
{
public:
    float		posx;
    float		posy;
    float		startPosX;
    float		startPosY;

    float		colorR;
    float		colorG;
    float		colorB;
    float		colorA;

    float		deltaColorR;
    float		deltaColorG;
    float		deltaColorB;
    float		deltaColorA;

    float		size;
    float		deltaSize;
    float		rotation;
    float		deltaRotation;
    float		timeToLive;
	int			textureFrameId;

    // Mode A: gravity, direction, radial accel, tangential accel
    struct {
        float	dirX;
        float	dirY;
        float	radialAccel;
        float	tangentialAccel;
    } modeA;

    // Mode B: radius mode
    struct {
        float	angle;
        float	degreesPerSecond;
        float	radius;
        float	deltaRadius;
    } modeB;

	// this function reset all particle data
	void		reset();
};

/*
	class ParticleSystem
*/
class ParticleSystem : public Node
{
public:

    enum {
        // The Particle emitter lives forever
        DURATION_INFINITY = -1,
        // The starting size of the particle is equal to the ending size
        START_SIZE_EQUAL_TO_END_SIZE = -1,
        // The starting radius of the particle is equal to the ending radius
        START_RADIUS_EQUAL_TO_END_RADIUS = -1,
    };

	// constructor
    ParticleSystem( unsigned int tag, unsigned int zOrder );
	// destructor
    virtual ~ParticleSystem();

	// start or restart particle system (all particles data will be reset)
	void Start();
	// stop system, particles are free terminate themself gracefully
    void Stop();

	// NOTE: to set image source we can use setTexture for a single texture or setTexturesArray
	// for an array of texture; one of these two MUST BE CALLED!
	// Sets a single texture 
	void SetTexture( SDL_Texture* texture );
	// Sets an array of textures 
	// - totalTextures			-> total number of frames (= array total items)
	// - texturesArray			-> pointer to array of items
	// - startFrameId			-> index of start frame for syncing particles or -1 for random
	bool SetTexturesArray( int totalTextures, SDL_Texture** texturesArray, int startFrameId );

	// set system parameters (mode, duration, emission rate, ecc.)
	void SetConfig( ParticleSystemConfig_t *config );

	// set system active or inactive
	void SetActive( bool state ) { _isActive = state;	};
	// get system active state
	bool isActive() { return _isActive; };

	// print an exact copy of the system into another place
	// calculation are done once only for the main position
	bool AddCopy( float copyX, float copyY );
	// delete all copies (if any)
	void DeleteCopies();

	// called every frame	
	void Draw();

private:

	// total number of textures (particle animation)
	#define PARTICLESYSTEM_MAX_TEXTURES		32

	// total number of exact copies of the main animation
	#define PARTICLESYSTEM_MAX_COPIES		32

    // particle data
    std::vector<ParticleData>	particle_data;

	// system parameters (mode, duration, emission rate, ecc.)
	ParticleSystemConfig_t	config;

    // Is the emitter active 
    bool				_isActive;
	// pointer to the renderer
	SDL_Renderer*		_renderer;

	// total number of textures
	int					_totalTextures;
    // textures used by the system (in case of setTexture only the first item is set)
    SDL_Texture*		_texturesArray[ PARTICLESYSTEM_MAX_TEXTURES ];
	// start frame when a particle is added or random if -1 is set
	int					_startFrameId;


	// time elapsed since the start of the system (in seconds)
    float				_elapsed;
    // How many particles can be emitted per second
    float				_emitCounter;
    //  Quantity of particles that are being simulated at the moment 
    int					_particleCount;
	//	this value is set to 1.0 / 25 = 0.04 and refer to 40ms of theoretical elapsed time after each frame
    float				_deltaTime;
	// this value is calculate when we set the emission rate
	float				_rate;

	// coordinate of copies (if any)
	Coord_t				_copies[ PARTICLESYSTEM_MAX_COPIES ];
	// total number of copies
	unsigned int		_totalCopies;

	// add a number of particles to the system
    void				AddParticles(int count);

	// called every frame inside Draw function
    void				Update();

};
