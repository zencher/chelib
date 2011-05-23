#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_pages.h"
#include "che_pdf_graphics.h"

class CHE_PDF_Renderer : public CHE_Object
{
public:
	CHE_PDF_Renderer( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}
	HE_VOID	Render( CHE_PDF_Page * page, IHE_PDF_DrawGraphics * pIHE_draw );
};

#endif
