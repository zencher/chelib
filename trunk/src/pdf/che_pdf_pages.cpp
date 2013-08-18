#include "../../include/pdf/che_pdf_pages.h"

#include <cassert>

CHE_PDF_PageTree::CHE_PDF_PageTree( const CHE_PDF_DictionaryPtr & PagesDictPtr, CHE_PDF_File * pFile, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mPageCount(0), mpFile(pFile)
{
	if ( !PagesDictPtr )
	{
		return;
	}
	CHE_PDF_ObjectPtr ObjPtr =  PagesDictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( ! ObjPtr )
	{
		return;
	}

	mPageCount = ObjPtr->GetNumberPtr()->GetInteger();
	
	ObjPtr = PagesDictPtr->GetElement( "Kids", OBJ_TYPE_ARRAY );
	if ( ObjPtr )
	{
		CHE_PDF_ArrayPtr KidsArrayPtr = ObjPtr->GetArrayPtr();

		for ( HE_ULONG i = KidsArrayPtr->GetCount() ; i > 0; --i )
		{
			ObjPtr = KidsArrayPtr->GetElement( i - 1 );
			if ( IsPdfRefPtr( ObjPtr ) )
			{
				mPageNodeStack.Push( ObjPtr->GetRefPtr() );
			}
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

HE_ULONG CHE_PDF_PageTree::GetPageCount()
{
	return mPageCount;
}

CHE_PDF_Page * CHE_PDF_PageTree::GetPage( HE_ULONG index )
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

HE_VOID CHE_PDF_PageTree::ReleasePage( CHE_PDF_Page * pPage )
{
	if ( pPage )
	{
		pPage->GetAllocator()->Delete( pPage );
	}
}

HE_BOOL CHE_PDF_PageTree::GetPageRefInfo( HE_ULONG index, PDF_RefInfo & refRet )
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
					refRet = refPtr->GetRefInfo();

					mPageObjList.push_back( refRet );

					if ( mPageObjList.size() == index + 1 )
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
						for ( HE_ULONG i = arrayPtr->GetCount(); i > 0; --i )
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

HE_VOID CHE_PDF_PageTree::AppendPage( HE_ULONG width, HE_ULONG height )
{
	assert( mpFile );

	ParseAllPageRefInfo();

	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_ReferencePtr ParentRefPtr;
	CHE_PDF_DictionaryPtr pagesDictPtr;
	CHE_PDF_DictionaryPtr trailerDictPtr = mpFile->GetTrailerDict();

	assert( trailerDictPtr );

	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_DICTIONARY );
	if ( ObjPtr )
	{
		CHE_PDF_ObjectPtr tmpObjPtr;
		tmpObjPtr = ObjPtr->GetDictPtr()->GetElement( "Pages", OBJ_TYPE_REFERENCE );
		if ( tmpObjPtr )
		{
			ParentRefPtr = tmpObjPtr->GetRefPtr();
		}

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

				if ( ParentRefPtr )
				{
					pageDictPtr->SetAtReference( "Parent", ParentRefPtr->GetRefNum(), ParentRefPtr->GetGenNum(), mpFile );
				}

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

HE_VOID CHE_PDF_PageTree::ParseAllPageRefInfo()
{
	if ( mPageNodeStack.IsEmpty() )
	{
		return;
	}

	//Get the last page ref info to make all page ref be get
	PDF_RefInfo refRet;
	GetPageRefInfo( mPageCount-1, refRet );
}

CHE_PDF_DictionaryPtr CHE_PDF_Page::GetResourcesDict() const
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

CHE_Rect CHE_PDF_Page::GetMediaBox() const
{
	return ArrayToRect( GetMediaBoxArray() );
}

CHE_Rect CHE_PDF_Page::GetPageRect() const
{
	return ArrayToRect( GetArtBoxArray() );
}

CHE_PDF_ArrayPtr CHE_PDF_Page::GetMediaBoxArray() const
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

CHE_PDF_ArrayPtr CHE_PDF_Page::GetCropBoxArray() const
{
	CHE_PDF_ArrayPtr arrayBox;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "CropBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetMediaBoxArray();
}

CHE_PDF_ArrayPtr CHE_PDF_Page::GetBleedBoxArray() const
{
	CHE_PDF_ArrayPtr arrayBox;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "CropBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetCropBoxArray();
}

CHE_PDF_ArrayPtr CHE_PDF_Page::GetTrimBoxArray() const
{
	CHE_PDF_ArrayPtr arrayBox;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "TrimBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetCropBoxArray();
}

CHE_PDF_ArrayPtr CHE_PDF_Page::GetArtBoxArray() const
{
	CHE_PDF_ArrayPtr arrayBox;
	CHE_PDF_ObjectPtr objPtr = mpPageDict->GetElement( "ArtBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetCropBoxArray();
}

CHE_Rect CHE_PDF_Page::ArrayToRect( CHE_PDF_ArrayPtr arrayPtr ) const
{
	CHE_Rect rect;
	if ( arrayPtr && arrayPtr->GetCount() >= 4 )
	{
		HE_FLOAT llx = 0, lly = 0, rux = 0, ruy = 0;
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_NumberPtr numberPtr;
		objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			llx = numberPtr->GetFloat();
		}
		objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			lly = numberPtr->GetFloat();
		}
		objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			rux = numberPtr->GetFloat();
		}
		objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			ruy = numberPtr->GetFloat();
		}
		rect.left = llx;
		rect.bottom = lly;
		rect.width = rux - llx;
		rect.height = ruy - lly;
	}
	return rect;
}

HE_INT32 CHE_PDF_Page::GetRotate() const
{
	HE_INT32 ret = 0;

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