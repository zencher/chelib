#ifndef _CHE_PDF_RENDERER_H_
#define _CHE_PDF_RENDERER_H_

#include "../che_base.h"
#include "../CHE_DynBuffer.h"
#include "che_pdf_pages.h"

class CHE_PDF_Renderer : public CHE_Object
{
public:
	HE_BOOL GetPageContent( CHE_PDF_Page & page, CHE_DynBuffer & buffer );
};

#endif