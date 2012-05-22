#ifndef _CHE_PDF_CONTENTBUILDER_H_
#define _CHE_PDF_CONTENTBUILDER_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_contentobjs.h"

class CHE_PDF_ContentString
{
public:
	static HE_BOOL PathToBuf( CHE_PDF_Path * pPath, CHE_DynBuffer & buf );

	static HE_BOOL TextToBuf( CHE_PDF_Text * pText, CHE_DynBuffer & buf );

	static HE_VOID TextBlockBeginToBuf( CHE_DynBuffer & buf );

	static HE_VOID TextBlockEndToBuf( CHE_DynBuffer & buf );

	static HE_BOOL RefImageToBuf( CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf );

	static HE_BOOL InlineImageToBuf( CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf );

	static HE_BOOL ShadingToBuf( CHE_PDF_Shading * pShading, CHE_DynBuffer & buf );

	static HE_BOOL MarkToBuf( CHE_PDF_Mark * pMark, CHE_DynBuffer & buf );

	static HE_BOOL FormToBuf( CHE_PDF_Form * pForm, CHE_DynBuffer & buf );

	static HE_BOOL TextStateToBuf( CHE_PDF_GState * pState, CHE_DynBuffer & buf );

	static HE_BOOL TextStateToBuf( CHE_PDF_GState * pCurGSData, CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf );

	static HE_BOOL GSDataToBuf( CHE_PDF_GState * pGSData, CHE_DynBuffer & buf );

	static HE_BOOL GSDataToBuf( CHE_PDF_GState * pCurGSData, CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf );

	static HE_BOOL PdfObjPtrToBuf( const CHE_PDF_ObjectPtr & objPtr, CHE_DynBuffer & buf );

private:
	static HE_BOOL PathDataToBuf( CHE_PDF_Path * pPath, CHE_DynBuffer & buf );

	static HE_BOOL FloatToBuf( HE_FLOAT val, CHE_DynBuffer & buf );

	static HE_BOOL IntegerToBuf( HE_INT32 val, CHE_DynBuffer & buf );

	static HE_BOOL DWORDToBuf( HE_DWORD val, CHE_DynBuffer & buf );

	static HE_BOOL PdfMatrixToBuf( CHE_PDF_Matrix & matrix, CHE_DynBuffer & buf );

	static HE_BOOL SpaceToBuf( CHE_DynBuffer & buf );

	static HE_BOOL StringToBuf( HE_LPCSTR pChar, CHE_DynBuffer & buf );

	static HE_BOOL StringToBuf( const CHE_ByteString & str, CHE_DynBuffer & buf );

	static HE_BOOL ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf );

	static HE_BOOL ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf );
}; 

class CHE_PDF_ContentBuilder : public CHE_Object
{
public:
	CHE_PDF_ContentBuilder() : CHE_Object( NULL ), mpCurGState( NULL ) {}

	HE_BOOL ContentToBuf( CHE_PDF_ContentObjectList * pList, CHE_DynBuffer & buf );

private:
	CHE_PDF_GState * mpCurGState;

};

#endif