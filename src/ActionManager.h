/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _ACTIONMANAGER_H_INCLUDE
#define _ACTIONMANAGER_H_INCLUDE

#include "Actions.h"
#include "Engine.h"

/*
	This is the ActionManager, it take cares of animation made by sequences of action
*/
namespace ActionManager {
	
	// reset action manager data
	void Initialize();

	// add a single action to the sequences list
	void RunAction( Action *action );

	// add a sequence of actions to the sequences list
	void RunSequence( ActionsSequence *sequence );

	// delete a sequence identified by tag
	void DeleteSequenceByTag( int tag );

	// This function is called each frame by the engine and update textures of all active streamings
	void Update();
};

#endif