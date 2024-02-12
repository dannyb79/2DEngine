/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#include <stdio.h>
#include <algorithm>
#include "Node.h"
#include "Engine.h"


Node::Node() {
	// reset children array
	totalChildren = 0;
	for( int i = 0; i < MAX_NODE_CHILDREN; i++ ) {
		children[ i ] = NULL;
	}
	x = 0;			// reset horizontal position
	y = 0;			// reset vertical position	
	width	= 0;	// reset object width
	height	= 0;	// reset object height
	angle = 0;		// reset rotation angle
	size = 1;		// size is default (1.0)
	sizeX = 1;
	sizeY = 1;
	flip = SDL_FLIP_NONE;	// reset flip state
	alpha = 255;	// reset alpha (255 = visible, 0 = not visible)
	visible = true;	// reset visible property (by default an object is visible)
	tag = 0;		// set a default tag
	parent = NULL;	// reset parent pointer, it will be set by addChild
	zOrder = 0;		// reset depth level to zero, every child must have a higher depth level
}

Node::Node( unsigned int tag )
{
	// reset children array
	totalChildren = 0;
	for( int i = 0; i < MAX_NODE_CHILDREN; i++ ) {
		children[ i ] = NULL;
	}
	x = 0;			// reset horizontal position
	y = 0;			// reset vertical position	
	width	= 0;	// reset object width
	height	= 0;	// reset object height
	angle = 0;		// reset rotation angle
	size = 1;		// size is default (1.0)
	flip = SDL_FLIP_NONE;	// reset flip state
	alpha = 255;	// reset alpha (255 = visible, 0 = not visible)
	visible = true;	// reset visible property (by default an object is visible)
	this->tag = tag;	// set tag
	parent = NULL;	// reset parent pointer, it will be set by addChild
	zOrder = 0;		// reset depth level to zero, every child must have a higher depth level
}

void Node::Visit()
{
	// if object is not visible we exit immediately from function,
	// automatically all its children will be not shown
	if( !this->visible ) {
		return;
	}
	// draw current node
	this->Draw();
	// if object has children...
	if( this->totalChildren > 0 ) {
		// ...scan recursively all of them
		for( int i = 0; i < this->totalChildren; i++ ) {
			// visit children
			this->children[ i ]->Visit();
		}
	} 
}

void Node::Delete()
{
	// if object has children...
	if( this->totalChildren > 0 ) {
		// ...scan recursively all of them
		for( int i = 0; i < this->totalChildren; i++ ) {
			// visit children
			this->children[ i ]->Delete();
		}
	} else {
		//this->Delete();	
		delete this;
	}
}

void Node::SetVisible( bool visible )
{
	this->visible = visible;
}

bool Node::IsVisible()
{
	return this->visible;
}

void Node::SetTag( unsigned int tag )
{
	this->tag = tag;
}

int Node::GetTag()
{
	return tag;
}

void Node::SetPosition( float xPos, float yPos ) 
{
	this->x = xPos;
	this->y = yPos;
}

Coord_t Node::GetPosition()
{
	Coord_t point = { this->x, this->y };
	return point;
}

void Node::SetPositionX( float xPos )
{
	this->x = xPos;
}

float Node::GetPositionX()
{
	return this->x;
}

void Node::SetPositionY( float yPos )
{
	this->y = yPos;
}

float Node::GetPositionY()
{
	return this->y;
}

Coord_t Node::GetWorldPosition()
{
	Node *p_parent = NULL;
	float world_x = 0, world_y = 0;

	world_x = this->x;
	world_y = this->y;
	p_parent = this->parent;
	while( p_parent ) {
		world_x += p_parent->x;
		world_y += p_parent->y;
		p_parent = p_parent->parent;
	}
	Coord_t point = { world_x, world_y };
	return point;
}

Size_t Node::GetSize()
{
	Size_t result = { this->width, this->height };
	return result;
}

int Node::GetWidth()
{
	return width;
}

int Node::GetHeight()
{
	return height;
}

void Node::SetSizeRate( float size ) 
{
	this->sizeX = size;
	this->sizeY = size;
}

float Node::GetSizeRate()
{
	// if we set a global size with SetSizeRate sizeX and sizeY have the same value
	return this->sizeX;
}

void Node::SetXSizeRate( float size ) 
{
	this->sizeX = size;
}

float Node::GetXSizeRate()
{
	return this->sizeX;
}

void Node::SetYSizeRate( float size ) 
{
	this->sizeY = size;
}

float Node::GetYSizeRate()
{
	return this->sizeY;
}

void Node::SetAngle( float angle ) 
{
	this->angle = angle;
}

float Node::GetAngle()
{
	return this->angle;
}

void Node::SetFlip( SDL_RendererFlip flip ) 
{
	this->flip = flip;
}

SDL_RendererFlip Node::GetFlip()
{
	return this->flip;
}

void Node::SetAlpha( unsigned char alpha ) 
{
	this->alpha = alpha;
}

unsigned char Node::GetAlpha()
{
	return this->alpha;
}


void Node::SetParent( Node *node )
{
	parent = node;
}

Coord_t Node::GetParentPosition()
{
	Coord_t point = { 0, 0 };
	if( parent != NULL ) {
		point = parent->GetPosition();
	}
	return point;
}

int Node::zOrderCmp( Node *a, Node *b )
{
    return a->zOrder < b->zOrder;
}

bool Node::AddChild( Node *node ) 
{
	bool result = false;
	if( this->totalChildren < MAX_NODE_CHILDREN ) {
		// set parent of this node
		node->SetParent( this );
		// add child to the children list
		this->children[ this->totalChildren ] = node;
		this->totalChildren += 1;
		// sort children according to depth level (zOrder)
		std::sort( children, children + totalChildren, zOrderCmp );
		result = true;
	}
	return result;
}

void Node::SetZOrder( int zOrder )
{
	// store object depth level 
	this->zOrder = zOrder;
	// when we set depth level of an object we must reorder all of its sibling
	Node *parent = this->parent;
	// if node has a parent
	if( parent != NULL ) {
		// sort sibling according to depth level (zOrder)
		std::sort( parent->children, parent->children + parent->totalChildren, zOrderCmp );
	}
}

void Node::DrawTexture( SDL_Texture *texture, int original_w, int original_h )
{
	Coord_t		world_position;
	SDL_Rect	srcrect;
	SDL_Rect	dstrect;

	// get coordinates int the world, this is normally the sum of coordinates of all parents
	world_position = GetWorldPosition();
	// set source rect (we always draw the entire texture, not a portion of it)
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = original_w;
	srcrect.h = original_h;

	// we calculate the anchor point from the original size to let the object in the center
	float		final_w, final_h;
	final_w		= original_w * sizeX;
	final_h		= original_h * sizeY;
	dstrect.x	= (int)world_position.x + original_w/2 - final_w/2;
	dstrect.y	= (int)world_position.y + original_h/2 - final_h/2;;
	dstrect.w	= final_w;
	dstrect.h	= final_h;

	// TODO sistemare!
	/*
	// if current size is different from default (1.0)... 	
	if( size != 1 ) {
		// we calculate the anchor point from the original size to let the object in the center
		float		final_w, final_h;
		final_w		= original_w * size;
		final_h		= original_h * size;
		dstrect.x	= (int)world_position.x + original_w/2 - final_w/2;
		dstrect.y	= (int)world_position.y + original_h/2 - final_h/2;;
		dstrect.w	= final_w;
		dstrect.h	= final_h;
	} else {
		dstrect.x = (int)world_position.x;
		dstrect.y = (int)world_position.y;
		dstrect.w = original_w;
		dstrect.h = original_h;
	}
	*/
	SDL_SetTextureAlphaMod( texture, alpha );
	SDL_RenderCopyEx( Engine::GetRenderer(), texture, &srcrect, &dstrect, angle, NULL, flip );
}

void Node::Draw()
{
	// this function must be overriden, each object has it own method to draw itself
}

void Node::OnClick()
{
	// this function must be overriden by touchable objects
}
