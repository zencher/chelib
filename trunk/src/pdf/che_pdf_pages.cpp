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
		PDF_RefInfo refInfo;
		CHE_PDF_ObjectPtr objPtr;
		if ( GetPageRefInfo( index, refInfo ) )
		{
			objPtr = mpFile->GetObject( refInfo );
		}
		if ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
		{
			return GetAllocator()->New<CHE_PDF_Page>( objPtr->GetDictPtr(), GetAllocator() );
		}
	}
	return NULL;
}

HE_BOOL CHE_PDF_PageTree::GetPageRefInfo( HE_DWORD index, PDF_RefInfo & refRet )
{
	if ( index >= mPageCount )
	{
		return FALSE;
	}

	if ( mPageObjList.size() > index && mPageObjList[index].objNum != 0 )
	{
		refRet = mPageObjList[index];

		return TRUE;
	}
	else
	{
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_NamePtr namePtr;
		CHE_PDF_ArrayPtr arrayPtr;
		CHE_PDF_ReferencePtr refPtr;
		CHE_PDF_DictionaryPtr dictPtr;

		while ( mPageNodeStack.Pop( refPtr ) == TRUE )
		{
			objPtr = refPtr->GetRefObj( OBJ_TYPE_DICTIONARY );

			if ( objPtr )
			{ 
				dictPtr = objPtr->GetDictPtr();

				//fixt no parent ref error
				objPtr = dictPtr->GetElement( "Parent", OBJ_TYPE_REFERENCE );
				if ( ! objPtr )
				{
					dictPtr->SetAtReference( "Parent", refPtr->GetRefInfo().objNum, refPtr->GetRefInfo().genNum, mpFile );
				}

				objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
				
				if ( ! objPtr )
				{
					return FALSE;
				}

				namePtr = objPtr->GetNamePtr();

				if ( namePtr->GetString() == "Page" )
				{
					refRet.objNum = refPtr->GetRefNum();
					refRet.genNum = refPtr->GetGenNum();

					mPageObjList.push_back( refRet );

					mCurPageIndex++;
					
					if ( mCurPageIndex-1 == index )
					{
						return TRUE;
					}
				}
				else if ( namePtr->GetString() == "Pages" )
				{
					objPtr = dictPtr->GetElement( "Kids", OBJ_TYPE_ARRAY );

					if ( objPtr )
					{
						arrayPtr = objPtr->GetArrayPtr();
						for ( HE_DWORD i = arrayPtr->GetCount(); i > 0; --i )
						{
							objPtr = arrayPtr->GetElement( i-1, OBJ_TYPE_REFERENCE );

							if ( objPtr )
							{
								mPageNodeStack.Push( objPtr->GetRefPtr() );
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
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

				++mPageCount;
				mPageObjList.push_back( refInfo );
			}
		}
	}
}

CHE_PDF_DictionaryPtr CHE_PDF_Page::GetResourcesDict()
{
	CHE_PDF_DictionaryPtr resDict;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
	if ( objPtr )
	{
		resDict = objPtr->GetDictPtr();
		return resDict;
	}

	//Search Parent Dict
	CHE_PDF_ObjectPtr tmpObjPtr;
	objPtr = mpPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	while ( objPtr )
	{
		tmpObjPtr = objPtr->GetDictPtr()->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
		if ( tmpObjPtr )
		{
			resDict = tmpObjPtr->GetDictPtr();
			break;
		}
		objPtr = objPtr->GetDictPtr()->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	}

	return resDict;
}

CHE_PDF_ArrayPtr CHE_PDF_Page::GetMediaBoxArray()
{
	CHE_PDF_ArrayPtr mediaBox;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "MediaBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		mediaBox = objPtr->GetArrayPtr();
		return mediaBox;
	}

	//Search Parent Dict
	CHE_PDF_ObjectPtr tmpObjPtr;
	objPtr = mpPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	while ( objPtr )
	{
		tmpObjPtr = objPtr->GetDictPtr()->GetElement( "MediaBox", OBJ_TYPE_ARRAY );
		if ( tmpObjPtr )
		{
			mediaBox = tmpObjPtr->GetArrayPtr();
			break;
		}
		objPtr = objPtr->GetDictPtr()->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	}

	return mediaBox;
}

CHE_PDF_ArrayPtr CHE_PDF_Page::GetCropBoxArray()
{
	CHE_PDF_ArrayPtr cropBox;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "CropBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		cropBox = objPtr->GetArrayPtr();
		return cropBox;
	}

	//Search Parent Dict
	CHE_PDF_ObjectPtr tmpObjPtr;
	objPtr = mpPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	while ( objPtr )
	{
		tmpObjPtr = objPtr->GetDictPtr()->GetElement( "CropBox", OBJ_TYPE_ARRAY );
		if ( tmpObjPtr )
		{
			cropBox = tmpObjPtr->GetArrayPtr();
			break;
		}
		objPtr = objPtr->GetDictPtr()->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	}

	if ( ! cropBox )
	{
		cropBox = GetMediaBoxArray();
	}

	return cropBox;
}

HE_DWORD CHE_PDF_Page::GetRotate()
{
	HE_DWORD ret = 0;

	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "Rotate", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		ret = objPtr->GetNumberPtr()->GetInteger();
		return ret;
	}

	//Search Parent Dict
	CHE_PDF_ObjectPtr tmpObjPtr;
	objPtr = mpPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	while ( objPtr )
	{
		tmpObjPtr = objPtr->GetDictPtr()->GetElement( "Rotate", OBJ_TYPE_ARRAY );
		if ( tmpObjPtr )
		{
			ret = tmpObjPtr->GetNumberPtr()->GetInteger();
			break;
		}
		objPtr = objPtr->GetDictPtr()->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
	}

	return ret;
}

HE_BOOL	CHE_PDF_Page::ReleasePage( CHE_PDF_Page * pPage )
{
	if ( pPage )
	{
		pPage->GetAllocator()->Delete( pPage );
		return TRUE;
	}
	return FALSE;
}