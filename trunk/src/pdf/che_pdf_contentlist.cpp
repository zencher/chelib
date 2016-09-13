#include "../../include/che_pdf_contentlist.h"

namespace chelib {

CPDF_ContentObjectList::~CPDF_ContentObjectList()
{
	Clear();
}

// CPDF_ContentObjectList * CPDF_ContentObjectList::Clone()
// {
// 	CPDF_ContentObjectList * pObjList = GetAllocator()->New<CPDF_ContentObjectList>();
// 
// 	ContentObjectList::iterator it = Begin();
// 	for ( ; it != End(); ++it )
// 	{
// 		pObjList->Append( *it );
// 	}
// 	return pObjList;
// }

void CPDF_ContentObjectList::Clear()
{
	ContentObjectList::iterator it = Begin();
	CPDF_ContentObject * pObj = nullptr;
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


bool CPDF_ContentObjectList::Append( CPDF_ContentObject * pObj )
{
	if ( pObj )
	{
		mList.push_back( pObj );
		return true;
	}
	return false;
}

ContentObjectList::iterator CPDF_ContentObjectList::Begin()
{
	return mList.begin();
}

ContentObjectList::iterator CPDF_ContentObjectList::End()
{
	return mList.end();
}

void CPDF_ContentObjectList::SetType3BBox( const int32 type, std::vector<FLOAT> & param )
{
	mType3DType = type;
	mType3Param = param;
}

bool CPDF_ContentObjectList::GetType3BBox( int32 & type, std::vector<FLOAT> & param )
{
	type = mType3DType;
	param = mType3Param;
	return TRUE;
}

}//namespace