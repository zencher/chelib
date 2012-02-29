#include "../../include/pdf/che_pdf_fontmgr.h"
#include "../../include/che_datastructure.h"

CHE_PDF_FontMgr::~CHE_PDF_FontMgr()
{
	HE_LPCVOID lpVoid = NULL;
	CHE_PDF_Font * pFont = NULL;
	for ( HE_DWORD i = 0; i < mNumToFontMap.GetCount(); ++i )
	{
		lpVoid = mNumToFontMap.GetItemByIndex( i );
		if ( lpVoid )
		{
			pFont = (CHE_PDF_Font *)( lpVoid );
			pFont->GetAllocator()->Delete( pFont ); 
		}
	}
	mNumToFontMap.Clear();
}

CHE_PDF_Font * CHE_PDF_FontMgr::LoadFont( CHE_PDF_Reference * pReference )
{
	if ( pReference == NULL )
	{
		return NULL;
	}
	HE_DWORD objNum = pReference->GetRefNum();
	HE_DWORD genNum = pReference->GetGenNum();
	HE_DWORD tmpValue = ( objNum << 5 ) + genNum;
	HE_LPCVOID lpVoid = mNumToFontMap.GetItem( tmpValue );
	if ( lpVoid == NULL )
	{
		CHE_PDF_Object * pTmpObj = pReference->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( pTmpObj == NULL )
		{
			return NULL;
		}
		CHE_PDF_Dictionary * pDict = pTmpObj->ToDict();
		if ( pDict )
		{
			CHE_PDF_Font * pTmpFont = GetAllocator()->New<CHE_PDF_Font>( pDict, GetAllocator() );
			if ( pTmpFont )
			{
				mNumToFontMap.Append( tmpValue, (HE_LPVOID)( pTmpFont ) );
				return pTmpFont;
			}
		}
	}
	return (CHE_PDF_Font*)(lpVoid);
}
