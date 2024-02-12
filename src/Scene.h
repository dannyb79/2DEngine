/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _SCENE_H_INCLUDE
#define _SCENE_H_INCLUDE

#include "Node.h"

// this is the maximum number of clickable object inside a scene
#define SCENE_MAX_CLICKABLES	128

class  Scene : public Node {

public:

	// object Scene constructor
	Scene() {};
	Scene( unsigned int tag );

	// add a new clickable object in current scene
	bool			AddClickable( Node *node );

	// remove a clickable object in current scene
	bool			RemoveClickable( Node *node );

	virtual void	Initialize();

	// total number of clickable objects
	long			totalClickables;
	// pointers of clickable objects
	Node			*clickables[ SCENE_MAX_CLICKABLES ];

private:

	// used by RemoveClickable
	void			RemoveClickableAt( unsigned int index );

	// TODO remove this function
	void			DebugClickablesList();
};

#endif