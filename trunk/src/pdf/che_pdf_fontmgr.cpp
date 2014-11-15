#include "../../include/pdf/che_pdf_fontmgr.h"

CHE_PDF_FontMgr::CHE_PDF_FontMgr( CHE_Allocator * pAllocator /*= NULL*/ ) : CHE_Object( pAllocator )
{
}

CHE_PDF_FontMgr::~CHE_PDF_FontMgr()
{
	CHE_PDF_Font * pFont = NULL;

	std::unordered_map<HE_ULONG,CHE_PDF_Font*>::iterator it;
	for ( it = mFontMap.begin(); it != mFontMap.end(); ++it )
	{
		pFont = it->second;
		if ( pFont )
		{
			pFont->GetAllocator()->Delete( pFont ); 
		}
	}
	mFontMap.clear();
}

CHE_PDF_Font * CHE_PDF_FontMgr::LoadFont( const CHE_PDF_ReferencePtr & pReference )
{
	if ( ! pReference )
	{
		return NULL;
	}
	HE_ULONG objNum = pReference->GetRefNum();
	std::unordered_map<HE_ULONG,CHE_PDF_Font*>::iterator it;
	it = mFontMap.find( objNum );
	if ( it == mFontMap.end() )
	{
		CHE_PDF_ObjectPtr pTmpObj = pReference->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( ! pTmpObj )
		{
			return NULL;
		}
		CHE_PDF_DictionaryPtr pDict = pTmpObj->GetDictPtr();
		if ( pDict )
		{

			CHE_PDF_Font * pNewFont = CHE_PDF_Font::Create( pDict, pDict->GetAllocator() );
			if ( pNewFont )
			{
				mFontMap.insert( pair<HE_ULONG,CHE_PDF_Font*>( objNum, pNewFont ) );
				return pNewFont;
			}
		}
	}
	return it->second;
}
