/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include "ParticleSystem.h"
#include <algorithm>
#include <assert.h>
#include <string>
#include <math.h>

inline float Deg2Rad(float a)
{
    return a * 0.01745329252f;
}

inline float Rad2Deg(float a)
{
    return a * 57.29577951f;
}

inline float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive)
    {
        std::swap(min_inclusive, max_inclusive);
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}

inline void normalize_point(float x, float y, Pointf* out)
{
    float n = x * x + y * y;
    // Already normalized.
    if (n == 1.0f) {
        return;
    }
    n = sqrt(n);
    // Too close to zero.
    if (n < 1e-5) {
        return;
    }
    n = 1.0f / n;
    out->x = x * n;
    out->y = y * n;
}

/**
	A more effect random number getter function, get from ejoy2d.
*/
inline static float RANDOM_M11(unsigned int* seed)
{
    *seed = *seed * 134775813 + 1;
    union
    {
        uint32_t d;
        float f;
    } u;
    u.d = (((uint32_t)(*seed) & 0x7fff) << 8) | 0x40000000;
    return u.f - 3.0f;
}

float Pointf::getAngle()
{
    return atan2f(y, x);
}

void Pointf::reset()
{
	x = 0, y = 0;
}

void ParticleData::reset()
{
    posx		= 0;
    posy		= 0;
	startPosX	= 0;
	startPosY	= 0;

    colorR		= 0;
    colorG		= 0;
    colorB		= 0;
    colorA		= 0;

    deltaColorR = 0;
    deltaColorG = 0;
    deltaColorB = 0;
    deltaColorA = 0;

    size		= 0;
    deltaSize	= 0;
    rotation	= 0;
    deltaRotation	= 0;
    timeToLive		= 0;
	textureFrameId	= 0;

	modeA.dirX = 0;
	modeA.dirY = 0;
	modeA.radialAccel		= 0;
	modeA.tangentialAccel	= 0;

	modeB.angle	= 0;
	modeB.degreesPerSecond = 0;
	modeB.radius = 0;
	modeB.deltaRadius = 0;
}

// implementation ParticleSystem

ParticleSystem::ParticleSystem( unsigned int tag, unsigned int zOrder )
{
	// set zOrder and tag
	this->tag = tag;
	this->zOrder = zOrder;
    // emitter is not active at creation
    _isActive = false;
	// set local pointer to renderer
	_renderer = Engine::GetRenderer();

	// reset system parameters (mode, duration, emission rate, ecc.)
	memset( &config, 0, sizeof( ParticleSystemConfig_t ) );

	// reset textures data
	_totalTextures	= 0;
	for( int i = 0; i < PARTICLESYSTEM_MAX_TEXTURES; i++ ) {
		_texturesArray[ i ]	= NULL;	
	}
	// start frame of particles in case of animation
	_startFrameId	= 0;

    // time elapsed since the start of the system (in seconds)
    _elapsed		= 0;
    // How many particles can be emitted per second
    _emitCounter	= 0;
    // Quantity of particles that are being simulated at the moment 
    _particleCount	= 0;
	// calculate theoretical elapsed time after each frame (40ms)
	_deltaTime		= 1.0 / 25;
	// reset rate (used internally)
	_rate			= 0;

	// reset copies data
	_totalCopies				= 0;
	memset( &_copies, 0, sizeof( Coord_t ) * PARTICLESYSTEM_MAX_COPIES );  

    // reset particle data array
	particle_data.clear();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Start()
{
    _isActive	= true;
    _elapsed	= 0;
	for( unsigned int i = 0; i < particle_data.size(); i++ ) {
		particle_data[ i ].reset();
	}
}

void ParticleSystem::Stop()
{
    _isActive		= false;
	_elapsed		= config.duration;
    _emitCounter	= 0;
}

void ParticleSystem::SetConfig( ParticleSystemConfig_t *config )
{
	memcpy( &this->config, config, sizeof( ParticleSystemConfig_t ) );
	for( unsigned int i = 0; i < particle_data.size(); i++ ) {
		particle_data[ i ].reset();
	}
    if( particle_data.size() < config->totalParticles ) {
        particle_data.resize( config->totalParticles );
    }
	if( config->emissionRate > 0 ) {
		_rate = 1.0f / config->emissionRate;
	}
}


void ParticleSystem::AddParticles(int count)
{
	// if system is not active exit immediately
	if( !_isActive ) {
        return;
    }
	// seed for local random generator (used locally only!)
    uint32_t RANDSEED = rand();
	// store current number of particles
    int start = _particleCount;
	// increment number of particle adding new requested particles
	_particleCount += count;

    // life time
    for( int i = start; i < _particleCount; ++i ) {
		// reset particle data
		particle_data[ i ].reset();
		// calculate particle life time
		float theLife = config.life + config.lifeVar * RANDOM_M11(&RANDSEED);
        particle_data[ i ].timeToLive = (std::max)( 0.0f, theLife );
    }

	// position 
    for( int i = start; i < _particleCount; ++i ) {
		particle_data[ i ].startPosX  = this->x;
        particle_data[ i ].startPosY  = this->y;
    }

	// emitter shape (circular perimeter or area, rectangular perimeter or area)
	switch( config.emitterShape ) {
		case ES_RECTANGULAR_AREA:
			// calculate start position in an rectangular area
			for( int i = start; i < _particleCount; ++i ) {
				particle_data[ i ].posx = config.positionVar.x * RANDOM_M11( &RANDSEED );
				particle_data[ i ].posy = config.positionVar.y * RANDOM_M11( &RANDSEED );
			}
		break;
		case ES_RECTANGULAR_PERIMETER:
			// calculate start position on a rectangular perimeter
			for( int i = start; i < _particleCount; ++i ) {
				int rnd = rand()%4;
				switch( rnd ) {
					case 0:		particle_data[ i ].posx = config.positionVar.x;
								particle_data[ i ].posy = config.positionVar.y * RANDOM_M11( &RANDSEED );
					break;
					case 1:		particle_data[ i ].posx = -config.positionVar.x;
								particle_data[ i ].posy = config.positionVar.y * RANDOM_M11( &RANDSEED );
					break;
					case 2:		particle_data[ i ].posx = config.positionVar.x * RANDOM_M11( &RANDSEED );
								particle_data[ i ].posy = config.positionVar.y;
					break;
					default:	particle_data[ i ].posx = config.positionVar.x * RANDOM_M11( &RANDSEED );
								particle_data[ i ].posy = -config.positionVar.y;
					break;
				}
			}
		break;
		case ES_CIRCLE_AREA:
			// calculate start position on a circle area
			for( int i = start; i < _particleCount; ++i ) {
				float newAngle = RANDOM_M11( &RANDSEED ) * M_PI * 2; // get a random angle
				particle_data[ i ].posx = config.positionVar.x * cosf( newAngle ) * RANDOM_M11( &RANDSEED );	// x radius
				particle_data[ i ].posy = config.positionVar.y * sinf( newAngle ) * RANDOM_M11( &RANDSEED );	// y radius
			}
		break;
		case ES_CIRCLE_PERIMETER:
			// calculate start position on a circle perimeter
			for( int i = start; i < _particleCount; ++i ) {
				float newAngle = RANDOM_M11( &RANDSEED ) * M_PI * 2; // get a random angle
				particle_data[ i ].posx = config.positionVar.x * cosf( newAngle );	// x radius
				particle_data[ i ].posy = config.positionVar.y * sinf( newAngle );	// y radius
			}
		break;
		case ES_POINTS_ARRAY:
			// calculate start position on a circle perimeter
			for( int i = start; i < _particleCount; ++i ) {
				//float newAngle = RANDOM_M11( &RANDSEED ) * M_PI * 2; // get a random angle
				int rndPoint = rand()%config.totalPoints;
				particle_data[ i ].posx = config.points[ rndPoint ].x + config.positionVar.x * RANDOM_M11( &RANDSEED );	
				particle_data[ i ].posy = config.points[ rndPoint ].y + config.positionVar.y * RANDOM_M11( &RANDSEED );	
			}
		break;
	}

    // color
#define SET_COLOR(c, b, v)													\
	for( int i = start; i < _particleCount; ++i ) {							\
        particle_data[ i ].c = clampf( b + v * RANDOM_M11(&RANDSEED), 0, 1 );	\
    }

	SET_COLOR(colorR, config.startColor.r, config.startColorVar.r);
    SET_COLOR(colorG, config.startColor.g, config.startColorVar.g);
    SET_COLOR(colorB, config.startColor.b, config.startColorVar.b);
    SET_COLOR(colorA, config.startColor.a, config.startColorVar.a);

	SET_COLOR(deltaColorR, config.endColor.r, config.endColorVar.r);
    SET_COLOR(deltaColorG, config.endColor.g, config.endColorVar.g);
    SET_COLOR(deltaColorB, config.endColor.b, config.endColorVar.b);
    SET_COLOR(deltaColorA, config.endColor.a, config.endColorVar.a);

#define SET_DELTA_COLOR(c, dc)                                                                              \
	for( int i = start; i < _particleCount; ++i ) {															\
        particle_data[ i ].dc = (particle_data[ i ].dc - particle_data[ i ].c) / particle_data[ i ].timeToLive; \
    }

    SET_DELTA_COLOR(colorR, deltaColorR);
    SET_DELTA_COLOR(colorG, deltaColorG);
    SET_DELTA_COLOR(colorB, deltaColorB);
    SET_DELTA_COLOR(colorA, deltaColorA);


    // size
    for( int i = start; i < _particleCount; ++i ) {
		particle_data[ i ].size = config.startSize + config.startSizeVar * RANDOM_M11(&RANDSEED);
        particle_data[ i ].size = (std::max)(0.0f, particle_data[ i ].size);
    }

	// calcualte delta difference from start and end size
    if( config.endSize != START_SIZE_EQUAL_TO_END_SIZE ) {
        for( int i = start; i < _particleCount; ++i ) {
			float endSize = config.endSize + config.endSizeVar * RANDOM_M11(&RANDSEED);
            endSize = (std::max)(0.0f, endSize);
            particle_data[ i ].deltaSize = ( endSize - particle_data[ i ].size) / particle_data[ i ].timeToLive;
        }
    } else {
        for( int i = start; i < _particleCount; ++i ) {
            particle_data[ i ].deltaSize = 0.0f;
        }
    }

    // calculate rotation delta
    for( int i = start; i < _particleCount; ++i ) {
		particle_data[ i ].rotation = config.startSpin + config.startSpinVar * RANDOM_M11(&RANDSEED);
    }
    for( int i = start; i < _particleCount; ++i ) {
		float endA = config.endSpin + config.endSpinVar * RANDOM_M11(&RANDSEED);
        particle_data[ i ].deltaRotation = ( endA - particle_data[ i ].rotation ) / particle_data[ i ].timeToLive;
    }

    // Mode Gravity: A
	if( config.mode == MODE_GRAVITY ) {
        // radial accel
        for (int i = start; i < _particleCount; ++i) {
            particle_data[ i ].modeA.radialAccel = config.radialAccel + config.radialAccelVar * RANDOM_M11(&RANDSEED);
        }
        // tangential accel
        for (int i = start; i < _particleCount; ++i) {
            particle_data[i].modeA.tangentialAccel = config.tangentialAccel + config.tangentialAccelVar * RANDOM_M11(&RANDSEED);
        }
        // rotation is dir
        if( config.rotationIsDirection ) {
            for( int i = start; i < _particleCount; ++i ) {
                float a = Deg2Rad( config.angle + config.angleVar * RANDOM_M11(&RANDSEED));
                Pointf v(cosf(a), sinf(a));
                float s = config.speed + config.speedVar * RANDOM_M11(&RANDSEED);
				Pointf dir = v;
				dir.x *= s;
				dir.y *= s;
                particle_data[ i ].modeA.dirX = dir.x;    
                particle_data[ i ].modeA.dirY = dir.y;
                particle_data[ i ].rotation = -Rad2Deg( dir.getAngle() );
            }
        } else {
            for( int i = start; i < _particleCount; ++i ) {
                float a = Deg2Rad( config.angle + config.angleVar * RANDOM_M11(&RANDSEED));
                Pointf v(cosf(a), sinf(a));
                float s = config.speed + config.speedVar * RANDOM_M11(&RANDSEED);
				Pointf dir = v;
				dir.x *= s;
				dir.y *= s;
                particle_data[ i ].modeA.dirX = dir.x;    
                particle_data[ i ].modeA.dirY = dir.y;
            }
        }
    } else {
	    // Mode Radius: B
		for( int i = start; i < _particleCount; ++i ) {
            particle_data[ i ].modeB.radius = config.startRadius + config.startRadiusVar * RANDOM_M11(&RANDSEED);
            particle_data[ i ].modeB.angle = Deg2Rad( config.angle + config.angleVar * RANDOM_M11(&RANDSEED));
            particle_data[ i ].modeB.degreesPerSecond = Deg2Rad( config.rotatePerSecond + config.rotatePerSecondVar * RANDOM_M11(&RANDSEED));
        }

        if( config.endRadius == START_RADIUS_EQUAL_TO_END_RADIUS ) {
            for (int i = start; i < _particleCount; ++i) {
                particle_data[ i ].modeB.deltaRadius = 0.0f;
            }
        } else {
            for (int i = start; i < _particleCount; ++i) {
                float endRadius = config.endRadius + config.endRadiusVar * RANDOM_M11(&RANDSEED);
                particle_data[ i ].modeB.deltaRadius = (endRadius - particle_data[ i ].modeB.radius) / particle_data[ i ].timeToLive;
            }
        }
    }

	// particles may have animation (array of textures), check if we must set the first frame 
	// randomly (-1) or to a proper value
	if( _startFrameId >= 0 ) {
		for( int i = start; i < _particleCount; ++i ) {
			particle_data[ i ].textureFrameId = _startFrameId;
		}
	} else {
		for( int i = start; i < _particleCount; ++i ) {
			particle_data[ i ].textureFrameId = rand()%_totalTextures;
		}
	}
}

void ParticleSystem::Update()
{
	// we add new particles only if the system is active and emission rate is set (> 0)
    if( ( _isActive ) && ( config.emissionRate ) ) {
        //float rate = 1.0f / _emissionRate;
        int totalParticles = config.totalParticles;

        //issue #1201, prevent bursts of particles, due to too high emitCounter
        if( _particleCount < totalParticles ) {
            _emitCounter += _deltaTime;
            if( _emitCounter < 0.f ) {
                _emitCounter = 0.f;
            }
        }
		// calculate how many particles can be emitted per second
        int emitCount = (std::min)( 1.0f * ( totalParticles - _particleCount ), _emitCounter / _rate);
        AddParticles( emitCount );
        _emitCounter -= _rate * emitCount;
		// update total time elapsed from system start, _elapsed is used for stopping system if it
		// has a finite time duration
        _elapsed += _deltaTime;
        if( _elapsed < 0.f ) {
            _elapsed = 0.f;
        }
		// if a timeout has been set for the system check we must stop it
        if( ( config.duration != DURATION_INFINITY ) && ( config.duration < _elapsed ) ) {
			// disactivate the system, anyway continue display particle until their end of life
			// to terminate the animation gracefully
			this->Stop();
        }
	} else {
		// if the system is not active or has just been disable we continue 
		// updating alive particles to let them terminate gracefully
	}

	// update particles life time
    for( int i = 0; i < _particleCount; ++i ) {
		// decrement frame time (1.0 / 25 = 0.04 = 40ms) from total life time of the particle
        particle_data[ i ].timeToLive -= _deltaTime;
		// if a particle has reached its end of life...
        if( particle_data[ i ].timeToLive <= 0.0f ) {
			// move the last particle of the array into the current position
			particle_data[ i ] = particle_data[ _particleCount - 1 ];
			// ..and decrement total particle counter
            _particleCount -= 1;
        }
    }

	// check if emitter modality is gravity or radial
	if( config.mode == MODE_GRAVITY ) {
		// emitter is set in gravity mode
        for( int i = 0; i < _particleCount; ++i ) {

			Pointf radial, tangential, tmp;
			// reset points
			radial.reset();
			tangential.reset();
			tmp.reset();

            // calculate radial acceleration
            if( ( particle_data[ i ].posx ) || ( particle_data[ i ].posy ) ) {
                normalize_point( particle_data[ i ].posx, particle_data[ i ].posy, &radial );
            }
            tangential = radial;
            radial.x *= particle_data[ i ].modeA.radialAccel;
            radial.y *= particle_data[ i ].modeA.radialAccel;

            // calculate tangential acceleration
            std::swap( tangential.x, tangential.y );
            tangential.x *= -particle_data[ i ].modeA.tangentialAccel;
            tangential.y *= particle_data[ i ].modeA.tangentialAccel;

            // (gravity + radial + tangential) * _deltaTime
            tmp.x = radial.x + tangential.x + config.gravity.x;
            tmp.y = radial.y + tangential.y + config.gravity.y;
            tmp.x *= _deltaTime;
            tmp.y *= _deltaTime;

            particle_data[ i ].modeA.dirX += tmp.x;
            particle_data[ i ].modeA.dirY += tmp.y;

            // calculate y flip
            tmp.x = particle_data[ i ].modeA.dirX * _deltaTime * config.yCoordFlipped;
            tmp.y = particle_data[ i ].modeA.dirY * _deltaTime * config.yCoordFlipped;
            particle_data[ i ].posx += tmp.x;
            particle_data[ i ].posy += tmp.y;
        }
    } else {
		// emitter is set in gravity mode
        for( int i = 0; i < _particleCount; ++i ) {
            particle_data[ i ].modeB.angle += particle_data[ i ].modeB.degreesPerSecond * _deltaTime;
            particle_data[ i ].modeB.radius += particle_data[ i ].modeB.deltaRadius * _deltaTime;
            particle_data[ i ].posx = -cosf( particle_data[ i ].modeB.angle ) * particle_data[ i ].modeB.radius;
            particle_data[ i ].posy = -sinf( particle_data[ i ].modeB.angle ) * particle_data[ i ].modeB.radius * config.yCoordFlipped;
        }
    }

    // update color, size and rotation
    for( int i = 0; i < _particleCount; ++i ) {
        particle_data[ i ].colorR += particle_data[ i ].deltaColorR * _deltaTime;
        particle_data[ i ].colorG += particle_data[ i ].deltaColorG * _deltaTime;
        particle_data[ i ].colorB += particle_data[ i ].deltaColorB * _deltaTime;
        particle_data[ i ].colorA += particle_data[ i ].deltaColorA * _deltaTime;
        particle_data[ i ].size += (particle_data[ i ].deltaSize * _deltaTime);
        particle_data[ i ].size = (std::max)( 0.0f, particle_data[ i ].size );
        particle_data[ i ].rotation += particle_data[ i ].deltaRotation * _deltaTime;
    }

	// if we have more than 1 texture (that's we have an animation)...
	if( _totalTextures > 1 ) {
		// update current frame for all particles
		for( int i = 0; i < _particleCount; ++i ) {
			particle_data[ i ].textureFrameId += 1;
			if( particle_data[ i ].textureFrameId >= _totalTextures ) {
				particle_data[ i ].textureFrameId = 0;
			}
		}
	}
}

void ParticleSystem::Draw()
{
	SDL_Texture *pTexture;
    SDL_Rect	r;
	SDL_Color	c;

	//printf( "_particleCount %d\n", _particleCount );

	// scan all particles
	for( int i = 0; i < _particleCount; i++ ) {

		// get particle for data extraction 
		ParticleData p = particle_data[ i ];

		// if partitcle is not visible continue with the next particle
		if( ( p.size <= 0 ) || ( p.colorA <= 0 ) ) {
            continue;
        }
		// get current texture of this particle, if texture is not set continue
		pTexture = _texturesArray[ p.textureFrameId ];
		if( !pTexture ) {
			continue;
		}

		// set color of texture for this particle
		c.r = Uint8( p.colorR * 255 );
		c.g = Uint8( p.colorG * 255 );
		c.b = Uint8( p.colorB * 255 );
		c.a = Uint8( p.colorA * 255 );
        SDL_SetTextureColorMod( pTexture, c.r, c.g, c.b );
        SDL_SetTextureAlphaMod( pTexture, c.a );
        SDL_SetTextureBlendMode( pTexture, SDL_BLENDMODE_BLEND );

		// calculate current position and size of particle		
		r.x = int( p.posx + p.startPosX - p.size / 2 );
		r.y = int( p.posy + p.startPosY - p.size / 2 );
		r.w = int( p.size );
		r.h = int( p.size );
        SDL_RenderCopyEx( _renderer, pTexture, NULL, &r, p.rotation, NULL, SDL_FLIP_NONE );

		// print copies (if any)
		for( unsigned int j = 0; j < _totalCopies; j++ ) {
			r.x = int( p.posx + _copies[ j ].x - p.size / 2 );
			r.y = int( p.posy + _copies[ j ].y - p.size / 2 );
			SDL_RenderCopyEx( _renderer, pTexture, NULL, &r, p.rotation, NULL, SDL_FLIP_NONE );
		}
    }
	// update state of all particles
	Update();
}

void ParticleSystem::SetTexture( SDL_Texture* texture )
{
	// insert the texture into the first element of array
    _texturesArray[ 0 ] = texture;
	_totalTextures	= 1;
	_startFrameId	= 0;
}

bool ParticleSystem::SetTexturesArray( int totalTextures, SDL_Texture** texturesArray, int startFrameId )
{
	bool result = false;
	if( ( totalTextures >= 0 ) && ( totalTextures < PARTICLESYSTEM_MAX_TEXTURES ) ) {
		// store textures pointers into local array
		for( int i = 0; i < totalTextures; i++ ) {
			_texturesArray[ i ] = texturesArray[ i ];
		}	
		// store total textures number
		_totalTextures = totalTextures;
		// and store the start frame id
		_startFrameId = startFrameId;
		// OK!
		result = true;
	}
	return result;
}

bool ParticleSystem::AddCopy( float copyX, float copyY )
{
	bool result = false;
	if( _totalCopies < PARTICLESYSTEM_MAX_COPIES ) {
		// add a position where will be drawn an exact copy of original particle system
		_copies[ _totalCopies ].x = copyX;
		_copies[ _totalCopies ].y = copyY;
		// increment number of total copies
		_totalCopies += 1;
		// OK!
		result = true;
	}
	return result;
}

void ParticleSystem::DeleteCopies() 
{
	// delete all copies
	_totalCopies = 0;
	memset( &_copies, 0, sizeof( Coord_t ) * PARTICLESYSTEM_MAX_COPIES );
}