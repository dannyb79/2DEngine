/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include "ActionManager.h"

// TODO decommentare per abilitare il debug
//#define ACTIONMANAGER_DEBUG

// this is the maximum number of actions dealt by a single sequence  
#define ACTIONMANAGER_MAX_SEQUENCE_ACTIONS		32

/*
	This class (AMSequence) is used internally by the ActionManager
	DON'T SUBCLASS IN THE GAME!
*/
class AMSequence {

public:
	// constructor
	AMSequence( unsigned int tag );
	// unique identifier of sequence (for any possible delete operation)
	unsigned int	tag;
	// pointer to previous item in the linked list
	AMSequence		*prev;
	// pointer to next item in the linked list
	AMSequence		*next;
	// list of actions
	Action			*actions[ ACTIONMANAGER_MAX_SEQUENCE_ACTIONS ];
	// index of current action
	unsigned int	currentAction;
	// total number of actions in this sequence
	unsigned int	totalActions;
};

// current total number of active sequences
long				totalSequences = 0;
// this is the head of the list of current active sequences
AMSequence			*sequencesList = NULL;


AMSequence::AMSequence( unsigned int tag )
{
	prev	= NULL;
	next	= NULL;
	this->tag = tag;
	totalActions	= 0;
	currentAction	= 0;
	for( int i = 0; i < ACTIONMANAGER_MAX_SEQUENCE_ACTIONS; i++ ) {
		actions[ i ] = NULL;
	}
}

// reset ActionManager data
void ActionManager::Initialize()
{
	sequencesList	= NULL;
	totalSequences	= 0;
}

static AMSequence* DeleteSequence( AMSequence *seq )
{
#ifdef ACTIONMANAGER_DEBUG
	printf( "DeleteSequence %p\n", seq );
#endif
	// delete all action objects of this sequece
	for( unsigned int i = 0; i < seq->totalActions; i++ ) {
		delete seq->actions[ i ];
	}
	// we must store previous and next item of current (pointed by *seq)
	AMSequence *p_Prev = seq->prev;
	AMSequence *p_Next = seq->next;
	// delete current Sequence
	delete seq;
	// and reset previous and next pointers of neighbors:
	// if we have a previous item...
	if( p_Prev != NULL ) {
		// now we must relink previous with next (next may also be NULL)
		p_Prev->next = p_Next;
		// but if next is an allocated object...
		if( p_Next != NULL ) {
			// we must also relink next with the previous
			p_Next->prev = p_Prev;
		}
	} else {
		// if we don't have a previous item it means that we are at the top (head)of the list
		if( p_Next != NULL ) {
			// the new first item of the list is now the next one
			p_Next->prev = NULL;
			// we must reset the head pointer also
			sequencesList = p_Next;
		} else {
			// else we have no more items in the list, reset head to NULL
			sequencesList = NULL;
		}
	}

	// decrement total number of sequences
	totalSequences -= 1;

	return p_Next;
}

void ActionManager::Update()
{
	static Uint64	lastTime = 0;
	Uint64			now; 
	double			deltaTime;
	AMSequence		*p_Sequence;
	ExecuteResult_t	result;

	now = SDL_GetPerformanceCounter();
	deltaTime = (double)((now - lastTime)*1000 / (double)SDL_GetPerformanceFrequency() );
	lastTime = now;
	//printf("deltaTime %f\n", deltaTime ); 

	// initizialize pointer to scan all active sequences
	p_Sequence = sequencesList;
	// check each sequence until the end of the list	
	while( p_Sequence != NULL ) {

#ifdef ACTIONMANAGER_DEBUG
		printf( "Check sequence %p Tag %d Total actions %d\n", p_Sequence, p_Sequence->tag, p_Sequence->totalActions );
#endif
		// the control flow below is a little messy but it's necessary to have correct actions timings:
		// if in the sequence there are consecutive instant actions they must be executed all immediately until the
		// end of the sequence or the next interval action
		// this flag is set is the sequence has finished
		bool endOfSequence		= false;
		// we must perform only one update per frame of interval actions
		bool actionIntervalDone = false;
		// start flow
		do {
			// we are executing an action, if the action type is interval we have already done an interval action for this sequence,
			if( p_Sequence->actions[ p_Sequence->currentAction ]->GetType() == ACTIONTYPE_INTERVAL ) {
				actionIntervalDone = true;
			}
			// execute action and get result 
			result = p_Sequence->actions[ p_Sequence->currentAction ]->Execute( deltaTime );

#ifdef ACTIONMANAGER_DEBUG
			printf( "Current Action Type %d Result %d actionIntervalDone %d\n", p_Sequence->actions[ p_Sequence->currentAction ]->GetType(), result, actionIntervalDone );
#endif

			// if we have an action in progress...
			if( result == EXECUTERESULT_IN_PROGRESS ) {
				// exit from this loop and proceed with the next sequence in the list
				break;
			} else {
				// if we completed an action (either interval or instant)...
				if( result == EXECUTERESULT_DONE ) {
					// ...we must go to the next action (if any)
					p_Sequence->currentAction += 1;
					// if we have reached the end of the sequence...
					if( p_Sequence->currentAction >= p_Sequence->totalActions ) {
						// store the flag that will delete the sequence once out of this loop
						endOfSequence = true;
						// exit, delete this sequence and proceed with the next one
						break;
					} else {
						// if we have a following valid action we must start it with current parameters
						p_Sequence->actions[ p_Sequence->currentAction ]->Start();
					}
				} else if( result >= EXECUTERESULT_REPEAT ) {
					if( result == EXECUTERESULT_REPEAT ) {
						// if we must repeat the sequence from the beginning, reset the index of current action...
						p_Sequence->currentAction = 0;
						// ..and restart the first action of the sequence
						p_Sequence->actions[ p_Sequence->currentAction ]->Start();
					} else {
						// we must turn back of a predefined number of actions, obviously we check that the 
						// number of action does not overflow the beginning of the sequence
						if( result <= p_Sequence->currentAction ) {
							p_Sequence->currentAction -= result;
						} else {
							// in case of overflow we reset the index to the beginning
							p_Sequence->currentAction = 0;
						}
						// restart the action of the sequence
						p_Sequence->actions[ p_Sequence->currentAction ]->Start();
					}
				}
				// now we must check which kind of type the next action is : theoretically the next action should
				// be execeuted the next frame, but if the action is instant then must be executed immediately
				if( p_Sequence->actions[ p_Sequence->currentAction ]->GetType() == ACTIONTYPE_INSTANT ) {
					// current action pointed by p_Sequence->currentAction is an instant action, go to the top 
					// of the flow and execute the action
					continue;
				} else {
					// current action pointed by p_Sequence->currentAction is an interval action: we must execute it
					// immediately only if we don't have already update another interval action in this loop
					if( actionIntervalDone ) {
						// if here we have already update another interval action, exit form the loop and go 
						// checking the nexe sequence
						break;
					}
				}
			}
		} while( true );

#ifdef ACTIONMANAGER_DEBUG
		printf( "endOfSequence %d\n", endOfSequence );
#endif
		// if we have reached the end of current sequence (pointed by p_Sequence) we must delete it from the list
		if( endOfSequence ) {
			// store the tag of sequence
			int sequenceTag = p_Sequence->tag;
			// delete sequence from the list, DeleteSequence returns a pointer to the current sequence (or NULL) after deletion
			p_Sequence = DeleteSequence( p_Sequence );
			// inform the game the this action is ended
			if( Engine::GetConfig()->ActionEndCallback != NULL ) {
				Engine::GetConfig()->ActionEndCallback( sequenceTag );
			}
			// continue to the top (if p_Sequence points to NULL exit, if points to a valid sequence proceed normally) 
			continue;
		}
		// go ahead and check current sequence (if any) of actions
		p_Sequence = p_Sequence->next;
	}
}

void ActionManager::DeleteSequenceByTag( int tag )
{
	AMSequence		*p_Sequence;
	// initizialize pointer to scan all active sequences
	p_Sequence = sequencesList;
	while( p_Sequence != NULL ) {
		if( p_Sequence->tag == tag ) {
			DeleteSequence( p_Sequence );
			break;
		}
		// go ahead and check current sequence (if any) of actions
		p_Sequence = p_Sequence->next;
	}
}

static void AddSequence( AMSequence *seq )
{
#ifdef ACTIONMANAGER_DEBUG
	printf( "AddSequence %p\n", seq );
#endif
	if( sequencesList != NULL ) {
		// sequencesList is not empty, we must find the last element
		AMSequence *p_Sequence = sequencesList;
		// we detect the last item by next parameter that must be NULL
		while( p_Sequence->next != NULL ) {
			p_Sequence = p_Sequence->next;
		}
		// now p_Sequence points to the last item, his next item becomes the new one
		p_Sequence->next = seq;
		// and the previous parameter of the new item is the last item we found, now
		// the last item is the new one added to the bottom of the list
		seq->prev = p_Sequence;
	} else {
		// this is the first element of the list pointed by sequencesList, it has
		// no previous and no next item to set
		sequencesList = seq;
	}

	// increment total number of sequences
	totalSequences += 1;
}


void ActionManager::RunAction( Action *action )
{
	AMSequence *seq = new AMSequence( action->GetTag() );
	seq->actions[ 0 ] = action;
	seq->totalActions = 1;
	AddSequence( seq );
}

void ActionManager::RunSequence( ActionsSequence *sequence )
{
	AMSequence *seq = new AMSequence( sequence->GetTag() );
	for( unsigned int i = 0; i < sequence->totalActions; i++ ) {
		seq->actions[ i ] = sequence->actions[ i ];	
	}
	seq->totalActions = sequence->totalActions;
	AddSequence( seq );
	// delete sequence object, no longer needed: all data has been tranferred
	// into the ActionManager sequences list
	delete sequence;
}

