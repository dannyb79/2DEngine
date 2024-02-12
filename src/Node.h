/*
 	The information contained herein is confidential.
 	The use, copying, transfer or disclosure of such information is proibited
 	except by press written agreement with the author
*/

#ifndef _NODE_H_INCLUDE
#define _NODE_H_INCLUDE

#include <SDL.h>
#include "EngineCommon.h"

// maximum number of children 
#define MAX_NODE_CHILDREN		1024

class  Node {
	
public:
		// object Node constructor
		Node();
		Node( unsigned int tag );

		// current total number of children
		long totalChildren;
		// pointers to children
		Node *children[ MAX_NODE_CHILDREN ];


		// set current position (relative to parent)
		void SetPosition( float xPos, float yPos );
		// get current position (relative to parent)
		Coord_t GetPosition();

		// set only horizontal position (relative to parent)
		void SetPositionX( float xPos );
		// get only horizontal position (relative to parent)
		float GetPositionX();

		// set only horizontal position (relative to parent)
		void SetPositionY( float yPos );
		// get only horizontal position (relative to parent)
		float GetPositionY();

		// get current position (absolute)
		Coord_t GetWorldPosition();

		// get size, width or height of object (whenever possible)
		Size_t GetSize();
		int	GetWidth();
		int GetHeight();

		// get/set current size (0.5=half, 1=normal, 1.5=middle, 2=double)
		void SetSizeRate( float size );
		float GetSizeRate();

		// set/get current horizontal size (0.5=half, 1=normal, 1.5=middle, 2=double)
		void SetXSizeRate( float size );
		float GetXSizeRate();

		// set/get current vertical size (0.5=half, 1=normal, 1.5=middle, 2=double)
		void SetYSizeRate( float size );
		float GetYSizeRate();

		// get/set current rotation angle (from 0° to 360*)
		void SetAngle( float angle );
		float GetAngle();

		// get/set flip (SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL or SDL_FLIP_VERTICAL)
		void SetFlip( SDL_RendererFlip flip );
		SDL_RendererFlip GetFlip();

		// get/set alpha (0 ... 255) default is 255
		void SetAlpha( unsigned char alpha );
		unsigned char GetAlpha();

		// object can be visible (true) or not (false)
		bool IsVisible();
		// all children inherit visible property 
		void SetVisible( bool visible );

		// get/set a unique identifier for the object
		int	 GetTag();
		void SetTag( unsigned int tag );

		// set parent node
		void SetParent( Node* node );
		// return parent's position
		Coord_t GetParentPosition();

		// add a child to this object
		bool AddChild( Node *node );

		// set depth level (0 = far, n = near)
		void SetZOrder( int zOrder );

		// ========================= functions below are used internally, don't use in the game =======================
		
		// called each frame to visit object for children
		void Visit();

		// each object knows how to draw itself, this function must be override 
		virtual void Draw();

		// overriden by object that may be touched 
		virtual void OnClick();

		// delete recursively child of a node
		void Delete();

protected:

		float				x;			// horizontal position (relative to parent)
		float				y;			// vertical position (relative to parent)
		int					width;		// width of current object (whenever possible)
		int					height;		// height of current object (whenever possible)
		float				angle;		// rotation angle (from 0° to 360°)
		float				size;		// current size (0.5=half, 1=normal, 1.5=middle, 2=double)

		float				sizeX;		// current horizontal size (0.5=half, 1=normal, 1.5=middle, 2=double)
		float				sizeY;		// current vertical size (0.5=half, 1=normal, 1.5=middle, 2=double)

		SDL_RendererFlip	flip;		// see SDL_RendererFlip declaration, default = SDL_FLIP_NONE
		unsigned char		alpha;		// alpha for semi-transparency of texture (default is 255)

		bool				visible;	// visible (true) or not (false)
		unsigned int		tag;		// unique identifier
		Node*				parent;		// pointer to parent
		unsigned int		zOrder;		// depth level

		// draw texture according to object parameters (position, size, angle, ...)
		void				DrawTexture( SDL_Texture *texture, int original_w, int original_h );

private:

		// compare function for reording objects according to depth level
		static int zOrderCmp( Node *a, Node *b );
};


#endif