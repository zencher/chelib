#include "../../include/pdf/pdf_outline.h"

namespace chelib {

CPDF_OutlineItem::CPDF_OutlineItem(CAllocator * pAllocator /*= nullptr*/)
: CObject(pAllocator), mCount(0), mFlag(0), mpParent(nullptr), mpPrev(nullptr), mpNext(nullptr), mpFirst(nullptr), mpLast(nullptr)
{
	mRGBColor[0] = 0;
	mRGBColor[1] = 0;
	mRGBColor[2] = 0;
}

CPDF_Outline::CPDF_Outline(CAllocator * pAllocator /*= nullptr*/)
: CObject(pAllocator), mCount(0), mpFirst(nullptr), mpLast(nullptr), mpNameDict(nullptr) {}

CPDF_Outline::~CPDF_Outline()
{
	Clear();
}

void	CPDF_Outline::Clear()
{
	if ( mpFirst && mpLast )
	{
		CleanChildTree( mpFirst, mpLast );
	}
}

bool CPDF_Outline::Parse( const CPDF_ReferencePtr & refPtr, CPDF_NameDict * pNameDict /*= nullptr*/ )
{
	mpNameDict = pNameDict;

	if ( !refPtr )
	{
		return false;
	}

	if ( mpFirst && mpLast )
	{
		return TRUE;
	}

	CPDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_DICTIONARY );
	if ( ! objPtr )
	{
		return false;
	}

	CPDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
	if ( ! dictPtr->CheckName( "Type", "Outlines", false ) )
	{
		return false;
	}

	objPtr = dictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( !objPtr )
	{
		return false;
	}
	mCount = objPtr->GetNumberPtr()->GetInteger();

	CPDF_ReferencePtr firstRefPtr, lastRefPtr;
	objPtr = dictPtr->GetElement( "First", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		firstRefPtr = objPtr->GetRefPtr();
	}
	objPtr = dictPtr->GetElement( "Last", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		lastRefPtr = objPtr->GetRefPtr();
	}

	if ( !firstRefPtr && !lastRefPtr )
	{
		mCount = 0;
		return false;
	}

	mRefPtr = refPtr;
	CPDF_OutlineItem * pTmp = GetAllocator()->New<CPDF_OutlineItem>( GetAllocator() );
	mCount = BuildChildTree( pTmp, firstRefPtr, lastRefPtr, TRUE );
	mpFirst = pTmp->mpFirst;
	mpLast = pTmp->mpLast;
	GetAllocator()->Delete( pTmp );
	return TRUE;
}

bool CPDF_Outline::GetOutlineItem( CPDF_OutlineItem * pItem, const CPDF_DictionaryPtr & dictPtr )
{
	if ( pItem == nullptr )
	{
		return false;
	}

	if ( dictPtr )
	{
		CPDF_ObjectPtr objPtr = dictPtr->GetElement( "Title", OBJ_TYPE_STRING );
		if ( objPtr )
		{
			pItem->mTitle = objPtr->GetStringPtr()->GetString();
		}
		objPtr = dictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			pItem->mCount = objPtr->GetNumberPtr()->GetInteger();
		}
		objPtr = dictPtr->GetElement( "C", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			CPDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
			if ( arrPtr->GetCount() == 3 )
			{
				objPtr = arrPtr->GetElement( 0, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					pItem->mRGBColor[0] = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrPtr->GetElement( 1, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					pItem->mRGBColor[1] = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrPtr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					pItem->mRGBColor[2] = objPtr->GetNumberPtr()->GetFloat();
				}
			}
		}
		objPtr = dictPtr->GetElement( "F", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			pItem->mFlag = objPtr->GetNumberPtr()->GetInteger();
		}

		//todod
		objPtr = dictPtr->GetElement( "Dest" );
		if ( objPtr )
		{
			if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
			{
				objPtr = objPtr->GetRefPtr()->GetRefObj();
			}

			if ( objPtr->GetType() == OBJ_TYPE_ARRAY )
			{
				pItem->mDestObj = objPtr;
				pItem->mDest = CPDF_Destination::Create(objPtr->GetArrayPtr(), GetAllocator());
			}else{
				if ( objPtr->GetType() == OBJ_TYPE_NAME  )
				{
					if ( mpNameDict )
					{
						objPtr = mpNameDict->GetDest( objPtr->GetNamePtr()->GetString() );
					}
				}else if ( objPtr->GetType() == OBJ_TYPE_STRING )
				{
					if ( mpNameDict )
					{
						objPtr = mpNameDict->GetDest( objPtr->GetStringPtr()->GetString() );
					}
				}
				if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
				{
					objPtr = objPtr->GetRefPtr()->GetRefObj();
				}
				if ( objPtr->GetType() == OBJ_TYPE_DICTIONARY )
				{
					objPtr = objPtr->GetDictPtr()->GetElement( "D", OBJ_TYPE_ARRAY );
					if ( objPtr )
					{
						pItem->mDestObj = objPtr;
						pItem->mDest = CPDF_Destination::Create(objPtr->GetArrayPtr(), GetAllocator());
					}
				}
			}
		}else{
			objPtr = dictPtr->GetElement( "A", OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				pItem->mActionObj = objPtr;
				pItem->mAction = CPDF_Action::Create(objPtr->GetDictPtr(), mpNameDict, GetAllocator());
			}
		}

		objPtr = dictPtr->GetElement( "SE", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			pItem->mSE = objPtr->GetDictPtr();
		}
	}

	return TRUE;
}

uint32 CPDF_Outline::BuildChildTree( CPDF_OutlineItem * pCurItem, const CPDF_ReferencePtr & firstRef, const CPDF_ReferencePtr & lastRef, bool bRoot /*= false*/ )
{
	if ( pCurItem == nullptr )
	{
		return 0;
	}

	uint32 count = 0;
	CPDF_ObjectPtr objPtr1;
	CPDF_ObjectPtr objPtr2;
	objPtr1 = firstRef->GetRefObj( OBJ_TYPE_DICTIONARY );
	if ( !objPtr1 )
	{
		return 0;
	}

	CPDF_ReferencePtr tmpRef = firstRef;
	CPDF_DictionaryPtr dictPtr = objPtr1->GetDictPtr();

	CPDF_OutlineItem * pTmpItem = GetAllocator()->New<CPDF_OutlineItem>( GetAllocator() );
	pTmpItem->mRefPtr = tmpRef;
	pTmpItem->mpParent = pCurItem;
	if ( bRoot )
	{
		pTmpItem->mpParent = nullptr;
	}

	pCurItem->mpFirst = pTmpItem;
	while ( TRUE )
	{
		count++;
		GetOutlineItem( pTmpItem, dictPtr );
		objPtr1 = dictPtr->GetElement( "First", OBJ_TYPE_REFERENCE );
		objPtr2 = dictPtr->GetElement( "Last", OBJ_TYPE_REFERENCE );
		if ( objPtr1 && objPtr2 )
		{
			uint32 childCount = BuildChildTree( pTmpItem, objPtr1->GetRefPtr(), objPtr2->GetRefPtr() );
			if ( childCount != pTmpItem->mCount )
			{
				pTmpItem->mCount = childCount;
			}
		}
		objPtr1 = dictPtr->GetElement( "Next", OBJ_TYPE_REFERENCE );
		if ( objPtr1 )
		{
			tmpRef = objPtr1->GetRefPtr();
			objPtr1 = tmpRef->GetRefObj( OBJ_TYPE_DICTIONARY );
			if ( objPtr1 )
			{
				dictPtr = objPtr1->GetDictPtr();
			}
			pTmpItem->mpNext = pTmpItem->GetAllocator()->New<CPDF_OutlineItem>( pTmpItem->GetAllocator() );
			pTmpItem->mpNext->mpPrev = pTmpItem;
			pTmpItem = pTmpItem->mpNext;
			pTmpItem->mRefPtr = tmpRef;
			pTmpItem->mpParent = pCurItem;
			if ( bRoot )
			{
				pTmpItem->mpParent = nullptr;
			}
		}else{
			break;
		}
	}
	pCurItem->mpLast = pTmpItem;

	return count;
}

void CPDF_Outline::CleanChildTree( CPDF_OutlineItem * pFirst, CPDF_OutlineItem * pLast )
{
	if ( !pFirst || !pLast )
	{
		return;
	}

	CPDF_OutlineItem * pTmp = pFirst;
	CPDF_OutlineItem * pTarget = nullptr;
	while ( pTmp != pLast )
	{
		pTarget = pTmp;
		pTmp = pTmp->mpNext;
		if ( pTarget->mpFirst && pTarget->mpLast )
		{
			CleanChildTree( pTarget->mpFirst, pTarget->mpLast );
		}
		pTarget->GetAllocator()->Delete( pTarget );
	}
}

}//namespace