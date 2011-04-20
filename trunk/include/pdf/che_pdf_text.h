#ifndef _CHE_PDF_TEXT_H_
#define _CHE_PDF_TEXT_H_

#include "../che_base.h"
#include "che_pdf_pages.h"

class CHE_PDF_TextExtractor : CHE_Object
{
public:
	HE_DWORD Extract( CHE_PDF_Page * page, CHE_DynWideByteBuffer & buf );

	HE_DWORD Extract(	CHE_PDF_Stream * pContent, CHE_PDF_Dictionary * pResourceDict,
						IHE_PDF_GetFontCodeMgr * pIHE_FontCodeMgr, IHE_PDF_GetInObj * pIHE_InObj,
						CHE_DynWideByteBuffer & buf );

	HE_DWORD Extract(	CHE_DynBuffer & content, CHE_PDF_Dictionary * pResourceDict,
						IHE_PDF_GetFontCodeMgr * pIHE_FontCodeMgr, IHE_PDF_GetInObj * pIHE_InObj,
						CHE_DynWideByteBuffer & buf );
};

#endif