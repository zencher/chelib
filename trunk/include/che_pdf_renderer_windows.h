#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "../che_drawer_windows.h"
#include "che_pdf_contentobjs.h"

class CHE_PDF_Renderer
{
public:
	static void Render(	CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & drawer, 
							CHE_Rect pageRect, uint32 rotate, FLOAT scale = 1.0,
							FLOAT dipx = 96, FLOAT dipy = 96, CHE_Rect * pClipRect = nullptr );
};

#endif
