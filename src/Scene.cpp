/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include "Scene.h"

Scene::Scene( unsigned int tag ) 
{
	this->tag = tag;
	totalClickables = 0;
	for( int i = 0; i < SCENE_MAX_CLICKABLES; i++ ) {
		clickables[ i ] = 0;
	}
}

void Scene::Initialize()
{
	// this must be overriden by custom scenes
}

bool Scene::AddClickable( Node *node ) 
{
	if( totalClickables < SCENE_MAX_CLICKABLES ) {
		clickables[ totalClickables ] = node;
		totalClickables += 1;
		return true;
	}
	return false;
}

void Scene::RemoveClickableAt( unsigned int index )
{
	if( ( totalClickables == 1 ) || ( index == ( totalClickables - 1 ) ) ) {
		clickables[ index ] = NULL;
		totalClickables -= 1;
	} else {
		clickables[ index ] = clickables[ totalClickables - 1 ];
		clickables[ totalClickables - 1 ] = NULL;
		totalClickables -= 1;
	}
}

bool Scene::RemoveClickable( Node *node ) 
{
	bool objectFound = false;
	// search index of clickable node
	for( int i = 0; i < totalClickables; i++ ) {
		// we found the index of node in clickables list
		if( clickables[ i ] == node ) {
			// we have found the object
			objectFound = true;
			// remove from the list 
			RemoveClickableAt( i );
			break;
		}
	}
	return objectFound;
}

void Scene::DebugClickablesList()
{
	printf( "Clickable Items %d\n", totalClickables );
	for( int i = 0; i < totalClickables; i++ ) {
		printf( "\tItem %d -> %p\n", i, clickables[ i ] ); 
	}
}
