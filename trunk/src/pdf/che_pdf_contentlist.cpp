#include "../../include/pdf/che_pdf_contentlist.h"

CHE_PDF_ContentObjectList::~CHE_PDF_ContentObjectList()
{
	Clear();
}

// CHE_PDF_ContentObjectList * CHE_PDF_ContentObjectList::Clone()
// {
// 	CHE_PDF_ContentObjectList * pObjList = GetAllocator()->New<CHE_PDF_ContentObjectList>();
// 
// 	ContentObjectList::iterator it = Begin();
// 	for ( ; it != End(); ++it )
// 	{
// 		pObjList->Append( *it );
// 	}
// 	return pObjList;
// }

void CHE_PDF_ContentObjectList::Clear()
{
	ContentObjectList::iterator it = Begin();
	CHE_PDF_ContentObject * pObj = nullptr;
	for ( ; it != End(); ++it )
	{
		pObj = *it;
		if ( pObj )
		{
			pObj->GetAllocator()->Delete( pObj );
		}
	}
	mList.clear();
}


bool CHE_PDF_ContentObjectList::Append( CHE_PDF_ContentObject * pObj )
{
	if ( pObj )
	{
		mList.push_back( pObj );
		return true;
	}
	return false;
}

ContentObjectList::iterator CHE_PDF_ContentObjectList::Begin()
{
	return mList.begin();
}

ContentObjectList::iterator CHE_PDF_ContentObjectList::End()
{
	return mList.end();
}

void CHE_PDF_ContentObjectList::SetType3BBox( const int32 type, std::vector<FLOAT> & param )
{
	mType3DType = type;
	mType3Param = param;
}

bool CHE_PDF_ContentObjectList::GetType3BBox( int32 & type, std::vector<FLOAT> & param )
{
	type = mType3DType;
	param = mType3Param;
	return TRUE;
}