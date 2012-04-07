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

CHE_PDF_Font * CHE_PDF_FontMgr::LoadFont( const CHE_PDF_ReferencePtr & pReference )
{
	if ( ! pReference )
	{
		return NULL;
	}
	HE_DWORD objNum = pReference->GetRefNum();
	HE_LPCVOID lpVoid = mNumToFontMap.GetItem( objNum );
	if ( lpVoid == NULL )
	{
		CHE_PDF_ObjectPtr pTmpObj = pReference->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( ! pTmpObj )
		{
			return NULL;
		}
		CHE_PDF_DictionaryPtr pDict = pTmpObj->GetDictPtr();
		if ( pDict )
		{
			CHE_PDF_Font * pTmpFont = GetAllocator()->New<CHE_PDF_Font>( pDict, GetAllocator() );
			if ( pTmpFont )
			{
				mNumToFontMap.Append( objNum, (HE_LPVOID)( pTmpFont ) );
				return pTmpFont;
			}
		}
	}
	return (CHE_PDF_Font*)(lpVoid);
}
