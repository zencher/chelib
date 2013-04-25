#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "../che_drawer.h"
#include "che_pdf_contentobjs.h"

class CHE_PDF_Renderer
{
public:
	static HE_VOID Render( CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & pDraw );
};

#endif
