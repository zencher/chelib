#ifndef _CHE_PDF_CONTENTLISTBUILDER_H_
#define _CHE_PDF_CONTENTLISTBUILDER_H_

#include "che_pdf_contentlist.h"
#include "che_pdf_componentmgr.h"

class CHE_PDF_ContentListBuilder
{
public:
	static bool ParsePageContent(	const CHE_PDF_DictionaryPtr & pageDict,
										CHE_PDF_ContentObjectList & contentList,
										CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = nullptr );

	static bool ParseContentStream(	const CHE_PDF_StreamPtr & stmPtr,
										CHE_PDF_ContentObjectList & contentList,
										CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = nullptr );

	static bool ParseContentStream(	const CHE_PDF_ReferencePtr & refPtr,
										CHE_PDF_ContentObjectList & contentList,
										CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = nullptr );
};

#endif