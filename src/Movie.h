/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _MOVIE_H_INCLUDE
#define _MOVIE_H_INCLUDE

#include <SDL.h>
#include "Node.h"

class  Movie : public Node {
	
public:

	Movie( unsigned int movieId, unsigned int tag, unsigned int zOrder );

	void SetMovieId( unsigned int movieId );

	// funzione di stampa (override della funzione nella classe Node)
	void Draw();

	// perform action if object is touched by the user	
	void OnClick();

private:

	unsigned int	movieId;

};


#endif