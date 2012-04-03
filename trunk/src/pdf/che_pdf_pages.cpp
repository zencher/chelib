#include "../../include/pdf/che_pdf_pages.h"

CHE_PDF_PageTree::CHE_PDF_PageTree( const CHE_PDF_DictionaryPtr & pPagesDict, CHE_PDF_File * pFile, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mPageCount(0), mCurPageIndex(0), mpFile(pFile)
{
	if ( !pPagesDict )
	{
		return;
	}
	CHE_PDF_ObjectPtr pObj =  pPagesDict->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( !pObj )
	{
		return;
	}

	mPageCount = pObj->GetNumber()->GetInteger();
	
	CHE_PDF_ArrayPtr pKidArray = pPagesDict->GetElement( "Kids", OBJ_TYPE_ARRAY ).GetArrayPtr();
	if ( pKidArray )
	{
		CHE_PDF_ReferencePtr pRef;
		HE_PDF_RefInfo refInfo;
		if ( pKidArray->GetCount() == mPageCount )
		{
			for ( HE_DWORD i = 0; i < mPageCount; i++ )
			{
				pObj = pKidArray->GetElement( i );
				if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					pRef = pObj.GetReferencePtr();
					refInfo.objNum = pRef->GetRefNum();
					refInfo.genNum = pRef->GetGenNum();
					mPageObjList.push_back( refInfo );
				}
			}
			mCurPageIndex = mPageCount-1;
		}else{
			for ( HE_DWORD i = 0; i < pKidArray->GetCount(); i++ )
			{
				pObj = pKidArray->GetElement( i );
				if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					mPageNodeStack.Push( pObj.GetReferencePtr() );
				}
			}
			mCurPageIndex = 0;	
		}
	}
}

CHE_PDF_PageTree::~CHE_PDF_PageTree()
{
	if ( !mPageNodeStack.IsEmpty() )
	{
		mPageNodeStack.Clear();
	}
}

HE_DWORD CHE_PDF_PageTree::GetPageCount()
{
	return mPageCount;
}

CHE_PDF_Page * CHE_PDF_PageTree::GetPage( HE_DWORD index )
{
	if ( mpFile )
	{
		HE_PDF_RefInfo refRet = GetPageRefInfo( index );
		CHE_PDF_ObjectPtr pObj = mpFile->GetObject( refRet );
		if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			return GetAllocator()->New<CHE_PDF_Page>( pObj.GetDictPtr(), this, GetAllocator() );
		}
	}
	return NULL;
}

HE_PDF_RefInfo CHE_PDF_PageTree::GetPageRefInfo( HE_DWORD index )
{
	HE_PDF_RefInfo refInfoRet;
	refInfoRet.objNum = 0;
	refInfoRet.genNum = 0;
	if ( index >= mPageCount )
	{
		return refInfoRet;
	}
	if ( mPageObjList.size() > index && mPageObjList[index].objNum != 0 )
	{
		return mPageObjList[index];
	}else{
		CHE_PDF_ObjectPtr pObj;
		CHE_PDF_ReferencePtr pRef;
		CHE_PDF_DictionaryPtr pDict;
		CHE_PDF_ArrayPtr pArray;
		CHE_PDF_NamePtr pName;
		while ( mPageNodeStack.Pop( pRef ) == TRUE )
		{
			pObj = pRef->GetRefObj( OBJ_TYPE_DICTIONARY );
			if ( pObj )
			{ 
				pDict = pObj.GetDictPtr();
				pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME );
				if ( !pObj )
				{
					refInfoRet.objNum = 0;
					refInfoRet.genNum = 0;
					return refInfoRet;
				}
				pName = pObj.GetNamePtr();
				if ( pName->GetString() == "Page" )
				{
					refInfoRet.objNum = pRef->GetRefNum();
					refInfoRet.genNum = pRef->GetGenNum();
					mPageObjList.push_back( refInfoRet );
					mCurPageIndex++;
					if ( mCurPageIndex-1 == index )
					{
						return refInfoRet;
					}
				}else if ( pName->GetString() == "Pages" )
				{
					pObj = pDict->GetElement( "Kids", OBJ_TYPE_ARRAY );
					if ( pObj )
					{
						pArray = pObj.GetArrayPtr();
						for ( HE_DWORD i = pArray->GetCount(); i > 0; i-- )
						{
							pObj = pArray->GetElement( i-1, OBJ_TYPE_REFERENCE );
							if ( pObj )
							{
								mPageNodeStack.Push( pObj.GetReferencePtr() );
							}
						}
					}
				}
			}
		}
	}
	return refInfoRet;
}