/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _ENGINECOMMON_H_INCLUDE
#define _ENGINECOMMON_H_INCLUDE

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


#define RGBA_R(c)		(((c) >> 0) & 0xFF)
#define RGBA_G(c)		(((c) >> 8) & 0xFF)
#define RGBA_B(c)		(((c) >> 16) & 0xFF)
#define RGBA_A(c)		(((c) >> 24) & 0xFF)
#define RGBA(r,g,b,a)	((r)|((g)<<8)|((b)<<16)|((a)<<24)) 

/*
	coordinates structure
*/
typedef struct {
	float	x;
	float	y;
} Coord_t;

/*
	size structure
*/
typedef struct {
	int		w;
	int		h;
} Size_t;

/*
	size structure
*/
typedef struct {
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	a;
} Color_t;

// this is the maximum number of actions dealt by a single sequence  
#define ACTIONSSEQUENCE_MAX_SEQUENCE_ACTIONS		64

// convert seconds to milliseconds
#define SECONDS(x)	(x*1000)

#define MILLISECONDS(x) (x)

// available audio channels
typedef enum {
	AudioChannel_0,
	AudioChannel_1,
	AudioChannel_2,
	AudioChannel_3,
	AudioChannel_4,
	AudioChannel_5,
	AudioChannel_6,
	AudioChannel_7,
	AudioChannel_8,
	AudioChannel_9,
	AudioChannel_10,
	AudioChannel_max
} AudioChannel_t;

#endif