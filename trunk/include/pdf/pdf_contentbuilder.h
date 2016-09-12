#ifndef _CHELIB_PDF_CONTENTBUILDER_H_
#define _CHELIB_PDF_CONTENTBUILDER_H_

#include "../base.h"
#include "pdf_contentobjs.h"

namespace chelib {

class CPDF_ContentBuilder : public CObject
{
public:
	CPDF_ContentBuilder() : CObject( NULL ), mpCurGState( NULL ) {}

	bool ContentToBuf( CPDF_ContentObjectList * pList, CDynBuffer & buf );

private:
	CPDF_GState * mpCurGState;
};

}//namespace

#endif