#ifndef _CHE_PDF_FONT_MGR_H_
#define _CHE_PDF_FONT_MGR_H_

#include "../che_base.h"
#include "che_pdf_font.h"
#include <unordered_map>

class CHE_PDF_FontMgr : public CHE_Object
{
public:
	CHE_PDF_FontMgr( CHE_Allocator * pAllocator = NULL );
	
	~CHE_PDF_FontMgr();

	CHE_PDF_Font * LoadFont( const CHE_PDF_ReferencePtr & pReference );

private:
	std::unordered_map<HE_ULONG,CHE_PDF_Font*>	mFontMap;
};


#endif