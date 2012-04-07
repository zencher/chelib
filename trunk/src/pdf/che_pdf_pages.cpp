#include "../../include/pdf/che_pdf_pages.h"

#include <cassert>

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

	mPageCount = pObj->GetNumberPtr()->GetInteger();
	
	CHE_PDF_ArrayPtr pKidArray = pPagesDict->GetElement( "Kids", OBJ_TYPE_ARRAY )->GetArrayPtr();
	if ( pKidArray )
	{
		CHE_PDF_ReferencePtr pRef;
		PDF_RefInfo refInfo;
		if ( pKidArray->GetCount() == mPageCount )
		{
			for ( HE_DWORD i = 0; i < mPageCount; i++ )
			{
				pObj = pKidArray->GetElement( i );
				if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					pRef = pObj->GetRefPtr();
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
					mPageNodeStack.Push( pObj->GetRefPtr() );
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
		PDF_RefInfo refRet = GetPageRefInfo( index );
		CHE_PDF_ObjectPtr pObj = mpFile->GetObject( refRet );
		if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
		{
			return GetAllocator()->New<CHE_PDF_Page>( pObj->GetDictPtr(), this, GetAllocator() );
		}
	}
	return NULL;
}

PDF_RefInfo CHE_PDF_PageTree::GetPageRefInfo( HE_DWORD index )
{
	PDF_RefInfo refInfoRet;
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
				pDict = pObj->GetDictPtr();
				pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME );
				if ( !pObj )
				{
					refInfoRet.objNum = 0;
					refInfoRet.genNum = 0;
					return refInfoRet;
				}
				pName = pObj->GetNamePtr();
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
						pArray = pObj->GetArrayPtr();
						for ( HE_DWORD i = pArray->GetCount(); i > 0; i-- )
						{
							pObj = pArray->GetElement( i-1, OBJ_TYPE_REFERENCE );
							if ( pObj )
							{
								mPageNodeStack.Push( pObj->GetRefPtr() );
							}
						}
					}
				}
			}
		}
	}
	return refInfoRet;
}

HE_VOID CHE_PDF_PageTree::AppendPage( HE_DWORD width, HE_DWORD height )
{
	assert( mpFile );

	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_DictionaryPtr pagesDictPtr;
	CHE_PDF_DictionaryPtr trailerDictPtr = mpFile->GetTrailerDict();

	assert( trailerDictPtr );

	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_DICTIONARY );
	if ( ObjPtr )
	{
		ObjPtr = ObjPtr->GetDictPtr()->GetElement( "Pages", OBJ_TYPE_DICTIONARY );
	}
	if ( ObjPtr )
	{
		pagesDictPtr = ObjPtr->GetDictPtr();
	}
	if ( pagesDictPtr )
	{
		CHE_PDF_ArrayPtr kidsArrayPtr;
		CHE_PDF_DictionaryPtr pageDictPtr;

		ObjPtr = pagesDictPtr->GetElement( "Kids", OBJ_TYPE_ARRAY );
		if ( ObjPtr )
		{
			kidsArrayPtr = ObjPtr->GetArrayPtr();
		}
		if ( kidsArrayPtr )
		{
			PDF_RefInfo refInfo = mpFile->CreateDictObject( pageDictPtr );
			if ( pageDictPtr )
			{
				CHE_PDF_ReferencePtr refPtr = CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, mpFile, GetAllocator() );
				kidsArrayPtr->Append( refPtr );

				ObjPtr = pagesDictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
				if ( ObjPtr )
				{
					CHE_PDF_NumberPtr countNumberPtr;
					countNumberPtr = ObjPtr->GetNumberPtr();
					countNumberPtr->SetValue( countNumberPtr->GetInteger() + 1 );
				}

				pageDictPtr->SetAtName( "Type", "Page" );
				CHE_PDF_ArrayPtr tmpArrayPtr = CHE_PDF_Array::Create( GetAllocator() );
				CHE_PDF_NumberPtr tmpNumberPtr = CHE_PDF_Number::Create( (HE_INT32)(0), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				tmpNumberPtr = CHE_PDF_Number::Create( (HE_INT32)(0), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				tmpNumberPtr = CHE_PDF_Number::Create( (HE_INT32)(width), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				tmpNumberPtr = CHE_PDF_Number::Create( (HE_INT32)(height), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				pageDictPtr->SetAtArray( "MediaBox", tmpArrayPtr );
			}
		}
	}
}