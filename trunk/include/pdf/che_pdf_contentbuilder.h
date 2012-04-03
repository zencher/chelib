#ifndef _CHE_PDF_CONTENTBUILDER_H_
#define _CHE_PDF_CONTENTBUILDER_H_

#include "../che_base.h"
#include "che_pdf_contentobjs.h"

class CHE_PDF_ContentString
{
public:
	static CHE_ByteString PathGetString( CHE_PDF_Path * pPath );

	static CHE_ByteString TextGetString( CHE_PDF_Path * pText );

	static CHE_ByteString RefImageGetString( CHE_PDF_RefImage * pImage );

	static CHE_ByteString InlineImageGetString( CHE_PDF_InlineImage * pImage );

	static CHE_ByteString ShadingGetString( CHE_PDF_Shading * pShading );

	static CHE_ByteString MarkGetString( CHE_PDF_Mark * pMark );

	static CHE_ByteString GSDataToSring( CHE_PDF_GState * pGSData );

	static CHE_ByteString GSDataToSring( CHE_PDF_GState * pCurGSData, CHE_PDF_GState * pTargetGSData );
}; 

class CHE_PDF_ContentBuilder : public CHE_Object
{
public:
	static CHE_ByteString ContentGetString( CHE_PDF_ContentObjectList * pList );

	static CHE_PDF_Stream ContentGetStream( CHE_PDF_ContentObjectList * pList );
};

#endif