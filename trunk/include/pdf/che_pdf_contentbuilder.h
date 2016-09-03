#ifndef _CHE_PDF_CONTENTBUILDER_H_
#define _CHE_PDF_CONTENTBUILDER_H_

#include "../che_base.h"
#include "che_pdf_contentobjs.h"

class CHE_PDF_ContentBuilder : public CHE_Object
{
public:
	CHE_PDF_ContentBuilder() : CHE_Object( NULL ), mpCurGState( NULL ) {}

	bool ContentToBuf( CHE_PDF_ContentObjectList * pList, CHE_DynBuffer & buf );

private:
	CHE_PDF_GState * mpCurGState;

};

#endif