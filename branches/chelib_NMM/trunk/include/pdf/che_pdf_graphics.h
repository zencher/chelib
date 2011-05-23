#ifndef _CHE_PDF_GRAPHICS_H_
#define _CHE_PDF_GRAPHICS_H_

#include "../che_base.h"
#include "../che_graphics.h"

class IHE_PDF_DrawGraphics : public CHE_Object
{
public:
	virtual ~IHE_PDF_DrawGraphics() {};

	virtual HE_VOID StrokePath( CHE_Path & path ) = 0;

	//virtual HE_VOID FillPath( CHE_Path & path ) = 0; 
};

#endif