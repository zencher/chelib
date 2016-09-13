#ifndef _CHELIB_PDF_CONTENTSTRING_H_
#define _CHELIB_PDF_CONTENTSTRING_H_

#include "che_pdf_gstate.h"
#include "che_pdf_contentobject.h"
#include "che_pdf_contentlist.h"

#include <stack>

namespace chelib {

class CPDF_ContentString
{
public:

	static bool ColorToBuf( const CPDF_Color & color, CDynBuffer & buf );

	static bool ColorSpaceToBuf( const CPDF_ColorSpace & colorSpace, CDynBuffer & buf );

	static bool PdfMatrixToBuf( const CMatrix & matrix, CDynBuffer & buf );

	static bool PathDataToBuf( const CPDF_Path * pPath, CDynBuffer & buf );

	static bool PathToBuf( const CPDF_Path * pPath, CDynBuffer & buf );

	static void TextBlockBeginToBuf( CDynBuffer & buf );

	static void TextBlockEndToBuf( CDynBuffer & buf );

	static bool TextToBuf( const CPDF_Text * pText, CDynBuffer & buf );

	//static bool RefImageToBuf( const CPDF_RefImage * pImage, CDynBuffer & buf );

	static bool InlineImageToBuf( const CPDF_InlineImage * pImage, CDynBuffer & buf );

	static bool MarkToBuf( const CPDF_Mark * pMark, CDynBuffer & buf );

	static bool TextStateToBuf(	const CPDF_GState * pTextState, CDynBuffer & buf, bool bNewBlock = false );

	static bool TextStateToBuf(	const CPDF_GState * pCurGSData, const CPDF_GState * pTargetGSData, CDynBuffer & buf, bool bNewBlock = false );

	static bool ExtGStateToBuf( const CPDF_ExtGState * pExtGState, CDynBuffer & buf );

	static bool ExtGStateToBuf( const CPDF_ExtGState * pCurExtGState, const CPDF_ExtGState * pTargetExtGState, CDynBuffer & buf );

	static bool ClipStateToBuf( CMatrix & curMatrix, CPDF_ClipState * pClipState, CDynBuffer & buf, bool bInTextBlock = false );

	static bool ClipStateToBuf(	CMatrix & curMatrix, CPDF_ClipState * pCurClipState,
									const CPDF_ClipState * pTargetClipState, CDynBuffer & buf, bool bInTextBlock = false );
    
    static bool GStateToBuf( CPDF_GState * & pGSData, CDynBuffer & buf, std::stack<CPDF_GState*> & gstack, bool bInTextBlock = false );

	static bool GStateToBuf( CPDF_GState * & pCurGSData, CPDF_GState * & pTargetGSData, CDynBuffer & buf, std::stack<CPDF_GState*> & gstack, bool bInTextBlock = false );
};

}//namespace

#endif