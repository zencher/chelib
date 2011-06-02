#ifndef _CHE_PDF_GRAPHICS_H_
#define _CHE_PDF_GRAPHICS_H_

#include "../che_base.h"
#include "../che_graphics.h"

class IHE_PDF_DrawGraphics : public CHE_Object
{
public:
	IHE_PDF_DrawGraphics( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}

	virtual ~IHE_PDF_DrawGraphics() {};

	virtual HE_VOID SetMatrixToDevice( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f ) = 0;

	virtual HE_FLOAT GetX( HE_FLOAT x, HE_FLOAT y ) = 0;

	virtual HE_FLOAT GetY( HE_FLOAT x, HE_FLOAT y ) = 0;

	virtual HE_VOID DrawPath( CHE_PDF_PathObject * pPath ) = 0;

	virtual HE_VOID DrawText( CHE_PDF_TextObject * pText ) = 0;
};

#endif