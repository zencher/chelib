#include "../../include/pdf/che_pdf_pages.h"

CHE_PDF_PageTree::CHE_PDF_PageTree( CHE_PDF_Dictionary * pPagesDict, CHE_PDF_File * pFile, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mPageCount(0), mCurPageIndex(0), mpFile(pFile)
{
	if ( pPagesDict == NULL )
	{
		return;
	}
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj =  pPagesDict->GetElement( "Count", OBJ_TYPE_NUMBER, objCollector );
	if ( pObj == NULL )
	{
		return;
	}

	mPageCount = pObj->ToNumber()->GetInteger();
	
	pObj = pPagesDict->GetElement( "Kids", OBJ_TYPE_ARRAY, objCollector );
	if ( pObj == NULL )
	{
		return;
	}

	CHE_PDF_Array * pKidArray = pObj->ToArray();
	if ( pKidArray != NULL )
	{
		CHE_PDF_Reference * pRef = NULL;
		HE_PDF_RefInfo refInfo;
		if ( pKidArray->GetCount() == mPageCount )
		{
			for ( HE_DWORD i = 0; i < mPageCount; i++ )
			{
				pObj = pKidArray->GetElement( i );
				if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					pRef = pObj->ToReference();
					refInfo.objNum = pRef->GetRefNum();
					refInfo.genNum = pRef->GetGenNum();
					mPageObjList.push_back( refInfo );
				}
			}
			mCurPageIndex = mPageCount-1;
		}else{
			for ( HE_DWORD i = 0; i < mPageCount; i++ )
			{
				pObj = pKidArray->GetElement( i );
				if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					mPageNodeStack.Push( pObj->ToReference()->Clone() );
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
		CHE_PDF_Reference * pRef = NULL;
		while( mPageNodeStack.Pop( pRef ) )
		{
			if ( pRef )
			{
				pRef->Release();
			}
		}
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
		CHE_PDF_Object * pObj = mpFile->GetObject( refRet );
		if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			return GetAllocator()->New<CHE_PDF_Page>( pObj->ToDict(), this, GetAllocator() );
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
		CHE_PDF_Object * pObj = NULL;
		CHE_PDF_Reference * pRef = NULL;
		CHE_PDF_Dictionary * pDict = NULL;
		CHE_PDF_Array * pArray = NULL;
		CHE_PDF_Name * pName = NULL;
		CHE_PDF_ObjectCollector objCollector( GetAllocator() );
		while ( mPageNodeStack.Pop( pRef ) == TRUE )
		{
			pObj = pRef->GetRefObj( OBJ_TYPE_DICTIONARY, objCollector );
			if ( pObj )
			{ 
				pDict = pObj->ToDict();
				pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME, objCollector );
				if ( pObj == NULL )
				{
					refInfoRet.objNum = 0;
					refInfoRet.genNum = 0;
					return refInfoRet;
				}
				pName = pObj->ToName();
				if ( pName->GetString() == "Page" )
				{
					refInfoRet.objNum = pRef->GetRefNum();
					refInfoRet.genNum = pRef->GetGenNum();
					mPageObjList.push_back( refInfoRet );
					mCurPageIndex++;
					if ( mCurPageIndex-1 == index )
					{
						if ( pRef )
						{
							pRef->Release();
						}
						return refInfoRet;
					}
				}else if ( pName->GetString() == "Pages" )
				{
					pObj = pDict->GetElement( "Kids", OBJ_TYPE_ARRAY, objCollector );
					if ( pObj )
					{
						pArray = pObj->ToArray();
						for ( HE_DWORD i = pArray->GetCount(); i > 0; i-- )
						{
							pObj = pArray->GetElement( i-1, OBJ_TYPE_REFERENCE, objCollector );
							if ( pObj )
							{
								mPageNodeStack.Push( pObj->ToReference()->Clone() );
							}
						}
					}
				}
			}
			if ( pRef )
			{
				pRef->Release();
				pRef = NULL;
			}
		}
	}
	return refInfoRet;
}