#ifndef _CHE_PDF_FONT_MGR_H_
#define _CHE_PDF_FONT_MGR_H_

#include "../che_base.h"
#include "che_pdf_define.h"

class CHE_PDF_FontMgr : public CHE_Object
{
public:
	CHE_PDF_FontMgr(  CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_FontMgr();
};

#endif
