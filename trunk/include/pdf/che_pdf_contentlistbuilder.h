#ifndef _CHELIB_PDF_CONTENTLISTBUILDER_H_
#define _CHELIB_PDF_CONTENTLISTBUILDER_H_

#include "che_pdf_contentlist.h"
#include "che_pdf_componentmgr.h"

namespace chelib {

class CPDF_ContentListBuilder
{
public:
	static bool ParsePageContent(	const CPDF_DictionaryPtr & pageDict,
										CPDF_ContentObjectList & contentList,
										CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );

	static bool ParseContentStream(	const CPDF_StreamPtr & stmPtr,
										CPDF_ContentObjectList & contentList,
										CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );

	static bool ParseContentStream(	const CPDF_ReferencePtr & refPtr,
										CPDF_ContentObjectList & contentList,
										CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );
};

}//namespace

#endif