#ifndef _CHE_PDF_TEXT_H_
#define _CHE_PDF_TEXT_H_

#include "../che_base.h"
#include "che_pdf_pages.h"

class CHE_PDF_TextExtractor : CHE_Object
{
public:
	HE_DWORD Extract( CHE_PDF_Page * page, HE_WCHAR* buffer, HE_DWORD size );
};

#endif