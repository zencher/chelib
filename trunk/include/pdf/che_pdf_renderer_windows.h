#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "../che_drawer_windows.h"
#include "che_pdf_contentobjs.h"

class CHE_PDF_Renderer
{
public:
	static HE_VOID Render(	CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & drawer, CHE_Rect pageRect,
							HE_FLOAT scale = 1.0, HE_FLOAT dipx = 96, HE_FLOAT dipy = 96, CHE_Rect * pClipRect = NULL );
};

#endif
