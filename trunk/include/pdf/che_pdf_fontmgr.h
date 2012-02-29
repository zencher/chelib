#ifndef _CHE_PDF_FONT_MGR_H_
#define _CHE_PDF_FONT_MGR_H_

#include "../che_base.h"
#include "che_pdf_define.h"
#include "che_pdf_font.h"

class CHE_PDF_FontMgr : public CHE_Object
{
public:
	CHE_PDF_FontMgr(  CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator) {};
	~CHE_PDF_FontMgr();

	CHE_PDF_Font * LoadFont( CHE_PDF_Reference * pReference );

private:
	CHE_NumToPtrMap mNumToFontMap;
};

#endif
