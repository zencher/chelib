#ifndef _CHE_PDF_CONTENTSTRING_H_
#define _CHE_PDF_CONTENTSTRING_H_

#include <vector>
#include "che_pdf_contentobjs.h"

using namespace std;

class CHE_PDF_ContentString
{
public:

	static bool ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf );


	static bool ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf );


	static bool PdfMatrixToBuf( const CHE_Matrix & matrix, CHE_DynBuffer & buf );


	static bool PathDataToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );


	static bool PathToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );


	static void TextBlockBeginToBuf( CHE_DynBuffer & buf );


	static void TextBlockEndToBuf( CHE_DynBuffer & buf );


	static bool TextToBuf( const CHE_PDF_Text * pText, CHE_DynBuffer & buf );


	//static bool RefImageToBuf( const CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf );


	static bool InlineImageToBuf( const CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf );


	//static bool ShadingToBuf( const CHE_PDF_Shading * pShading, CHE_DynBuffer & buf );


	static bool MarkToBuf( const CHE_PDF_Mark * pMark, CHE_DynBuffer & buf );

    static bool ComponetToBuf( const CHE_PDF_ComponentPtr compt, CHE_DynBuffer & buf );


	//static bool FormToBuf( const CHE_PDF_Form * pForm, CHE_DynBuffer & buf );


	static bool TextStateToBuf(	const CHE_PDF_GState * pTextState, CHE_DynBuffer & buf, bool bNewBlock = false );


	static bool TextStateToBuf(	const CHE_PDF_GState * pCurGSData, const CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf, bool bNewBlock = false );


	static bool ExtGStateToBuf( const CHE_PDF_ExtGState * pExtGState, CHE_DynBuffer & buf );


	static bool ExtGStateToBuf( const CHE_PDF_ExtGState * pCurExtGState, const CHE_PDF_ExtGState * pTargetExtGState, CHE_DynBuffer & buf );


	static bool ClipStateToBuf( CHE_Matrix & curMatrix, CHE_PDF_ClipState * pClipState, CHE_DynBuffer & buf, bool bInTextBlock = false );


	static bool ClipStateToBuf(	CHE_Matrix & curMatrix, CHE_PDF_ClipState * pCurClipState,
									const CHE_PDF_ClipState * pTargetClipState, CHE_DynBuffer & buf, bool bInTextBlock = false );


	static bool GStateToBuf( CHE_PDF_GState * & pGSData, CHE_DynBuffer & buf, stack<CHE_PDF_GState*> & gstack, bool bInTextBlock = false );



	static bool GStateToBuf( CHE_PDF_GState * & pCurGSData, CHE_PDF_GState * & pTargetGSData, CHE_DynBuffer & buf, stack<CHE_PDF_GState*> & gstack, bool bInTextBlock = false );
};

#endif