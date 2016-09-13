#include "../../include/pdf/che_pdf_page.h"
#include "../../include/pdf/che_pdf_contentlistbuilder.h"

#include <cassert>

namespace chelib {

CPDF_PageTree::CPDF_PageTree( const CPDF_DictionaryPtr & PagesDictPtr, CPDF_File * pFile, CAllocator * pAllocator )
	: CObject( pAllocator ), mPageCount(0), mpFile(pFile)
{
	if ( !PagesDictPtr )
	{
		return;
	}
	CPDF_ObjectPtr ObjPtr =  PagesDictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( ! ObjPtr )
	{
		return;
	}

	mPageCount = ObjPtr->GetNumberPtr()->GetInteger();
	
	ObjPtr = PagesDictPtr->GetElement( "Kids", OBJ_TYPE_ARRAY );
	if ( ObjPtr )
	{
		CPDF_ArrayPtr KidsArrayPtr = ObjPtr->GetArrayPtr();

		for ( size_t i = KidsArrayPtr->GetCount() ; i > 0; --i )
		{
			ObjPtr = KidsArrayPtr->GetElement( i - 1 );
			if ( IsPdfRefPtr( ObjPtr ) )
			{
				mPageNodeStack.push( ObjPtr->GetRefPtr() );
			}
		}
	}
}

CPDF_PageTree::~CPDF_PageTree()
{
}

size_t CPDF_PageTree::GetPageCount()
{
	return mPageCount;
}

CPDF_Page * CPDF_PageTree::GetPage( size_t index )
{
    std::unordered_map<size_t, CPDF_Page*>::iterator it = mPageMap.find(index);
    if (it != mPageMap.end()) {
        return it->second;
    }
	if ( mpFile )
	{
		PDF_RefInfo refInfo;
		CPDF_ObjectPtr objPtr;
		if ( GetPageRefInfo( index, refInfo ) )
		{
			objPtr = mpFile->GetObject( refInfo );
		}
		if ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
		{
            CPDF_Page * page = GetAllocator()->New<CPDF_Page>( objPtr->GetDictPtr(), GetAllocator() );
            mPageMap[index] = page;
            return page;
		}
	}
	return nullptr;
}

void CPDF_PageTree::ReleasePage( CPDF_Page * pPage )
{
	if ( pPage )
	{
        std::unordered_map<size_t, CPDF_Page*>::iterator it = mPageMap.begin();
        for (; it != mPageMap.end(); ++it) {
            if (it->second == pPage) {
                mPageMap.erase(it);
                break;
            }
        }
		pPage->GetAllocator()->Delete( pPage );
	}
}

bool CPDF_PageTree::GetPageRefInfo( size_t index, PDF_RefInfo & refRet )
{
	if ( index >= mPageCount )
	{
		return false;
	}

	if ( mPageObjList.size() > index && mPageObjList[index].objNum != 0 )
	{
		refRet = mPageObjList[index];

		return TRUE;
	}
	else
	{
		CPDF_ObjectPtr objPtr;
		CPDF_NamePtr namePtr;
		CPDF_ArrayPtr arrayPtr;
		CPDF_ReferencePtr refPtr;
		CPDF_DictionaryPtr dictPtr;

		while ( !mPageNodeStack.empty() )
		{
			refPtr = mPageNodeStack.top();
			mPageNodeStack.pop();

			objPtr = refPtr->GetRefObj( OBJ_TYPE_DICTIONARY );

			if ( objPtr )
			{ 
				dictPtr = objPtr->GetDictPtr();

				//fixt no parent ref error
				objPtr = dictPtr->GetElement( "Parent", OBJ_TYPE_REFERENCE );
				if ( ! objPtr )
				{
					dictPtr->SetReference( "Parent", refPtr->GetRefInfo().objNum, refPtr->GetRefInfo().genNum, mpFile );
				}

				objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
				
				if ( ! objPtr )
				{
					return false;
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
						for ( size_t i = arrayPtr->GetCount(); i > 0; --i )
						{
							objPtr = arrayPtr->GetElement( i-1, OBJ_TYPE_REFERENCE );

							if ( objPtr )
							{
								mPageNodeStack.push( objPtr->GetRefPtr() );
							}
						}
					}
				}
			}
		}
	}
	return false;
}

void CPDF_PageTree::AppendPage( size_t width, size_t height )
{
	assert( mpFile );

	ParseAllPageRefInfo();

	CPDF_ObjectPtr ObjPtr;
	CPDF_ReferencePtr ParentRefPtr;
	CPDF_DictionaryPtr pagesDictPtr;
	CPDF_DictionaryPtr trailerDictPtr = mpFile->GetTrailerDict();

	assert( trailerDictPtr );

	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_DICTIONARY );
	if ( ObjPtr )
	{
		CPDF_ObjectPtr tmpObjPtr;
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
		CPDF_ArrayPtr kidsArrayPtr;
		CPDF_DictionaryPtr pageDictPtr;

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
				CPDF_ReferencePtr refPtr = CPDF_Reference::Create( refInfo.objNum, refInfo.genNum, mpFile, GetAllocator() );
				kidsArrayPtr->Append( refPtr );

				ObjPtr = pagesDictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
				if ( ObjPtr )
				{
					CPDF_NumberPtr countNumberPtr;
					countNumberPtr = ObjPtr->GetNumberPtr();
					countNumberPtr->SetValue( countNumberPtr->GetInteger() + 1 );
				}

				pageDictPtr->SetName( "Type", "Page" );

				if ( ParentRefPtr )
				{
					pageDictPtr->SetReference( "Parent", ParentRefPtr->GetRefNum(), ParentRefPtr->GetGenNum(), mpFile );
				}

				CPDF_ArrayPtr tmpArrayPtr = CPDF_Array::Create( GetAllocator() );
				CPDF_NumberPtr tmpNumberPtr = CPDF_Number::Create( (int32)(0), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				tmpNumberPtr = CPDF_Number::Create( (int32)(0), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				tmpNumberPtr = CPDF_Number::Create( (int32)(width), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				tmpNumberPtr = CPDF_Number::Create( (int32)(height), GetAllocator() );
				tmpArrayPtr->Append( tmpNumberPtr );
				pageDictPtr->SetArray( "MediaBox", tmpArrayPtr );

				++mPageCount;
				mPageObjList.push_back( refInfo );
			}
		}
	}
}

void CPDF_PageTree::ParseAllPageRefInfo()
{
	if ( mPageNodeStack.empty() )
	{
		return;
	}

	//Get the last page ref info to make all page ref be get
	PDF_RefInfo refRet;
	GetPageRefInfo( mPageCount-1, refRet );
}

CPDF_DictionaryPtr CPDF_Page::GetResourcesDict() const
{
	CPDF_DictionaryPtr resDict;
	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
	if ( objPtr )
	{
		resDict = objPtr->GetDictPtr();
		return resDict;
	}

	//Search Parent Dict
	CPDF_ObjectPtr tmpObjPtr;
	objPtr = mPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
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

CRect CPDF_Page::GetMediaBox() const
{
	CRect rect;
	CPDF_ArrayPtr arrayPtr = GetMediaBoxArray();
	arrayPtr->GetRect( rect );
	return rect;
}

CRect CPDF_Page::GetPageRect() const
{
	CRect rect;
    CRect mediaRect;
    CPDF_ArrayPtr arrayPtr = GetCropBoxArray();
    uint32 rotate = GetRotate();
    rotate = rotate % 360;

	arrayPtr->GetRect( rect );
    arrayPtr = GetMediaBoxArray();
    arrayPtr->GetRect( mediaRect );
    if (mediaRect.width < rect.height || mediaRect.height < rect.height)
    {
        rect = mediaRect;
    }
	return rect;
}

CPDF_ArrayPtr CPDF_Page::GetMediaBoxArray() const
{
	CPDF_ArrayPtr mediaBox;
	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "MediaBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		mediaBox = objPtr->GetArrayPtr();
		return mediaBox;
	}

	//Search Parent Dict
	CPDF_ObjectPtr tmpObjPtr;
	objPtr = mPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
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

CPDF_ArrayPtr CPDF_Page::GetCropBoxArray() const
{
	CPDF_ArrayPtr arrayBox;
	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "CropBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetMediaBoxArray();
}

CPDF_ArrayPtr CPDF_Page::GetBleedBoxArray() const
{
	CPDF_ArrayPtr arrayBox;
	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "CropBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetCropBoxArray();
}

CPDF_ArrayPtr CPDF_Page::GetTrimBoxArray() const
{
	CPDF_ArrayPtr arrayBox;
	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "TrimBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetCropBoxArray();
}

CPDF_ArrayPtr CPDF_Page::GetArtBoxArray() const
{
	CPDF_ArrayPtr arrayBox;
	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "ArtBox", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		arrayBox = objPtr->GetArrayPtr();
		return arrayBox;
	}
	return GetCropBoxArray();
}

int32 CPDF_Page::GetRotate() const
{
	int32 ret = 0;

	CPDF_ObjectPtr objPtr = mPageDict->GetElement( "Rotate", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		ret = objPtr->GetNumberPtr()->GetInteger();
		return ret;
	}

	//Search Parent Dict
	CPDF_ObjectPtr tmpObjPtr;
	objPtr = mPageDict->GetElement( "Parent", OBJ_TYPE_DICTIONARY );
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

bool	CPDF_Page::ReleasePage( CPDF_Page * pPage )
{
	if ( pPage )
	{
		pPage->GetAllocator()->Delete( pPage );
		return TRUE;
	}
	return false;
}

bool CPDF_Page::ParsePageContent(CPDF_ComponentMgr * pComMgr)
{
	if (mbParsed == false)
	{
		mbParsed = true;
		return CPDF_ContentListBuilder::ParsePageContent(mPageDict, mContentList, pComMgr, GetAllocator());
	}
	return true;
}

}//namespace