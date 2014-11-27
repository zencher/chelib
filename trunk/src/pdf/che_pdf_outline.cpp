#include "../../include/pdf/che_pdf_outline.h"

CHE_PDF_OutlineItem::CHE_PDF_OutlineItem( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mCount( 0 ), mFlag( 0 ), mpDest( NULL ),
	mpParent( NULL ), mpPrev( NULL ),	mpNext( NULL ), mpFirst( NULL ), mpLast( NULL )
{
	mRGBColor[0] = 0;
	mRGBColor[1] = 0;
	mRGBColor[2] = 0;
}

CHE_PDF_Outline::CHE_PDF_Outline( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mCount( 0 ), mpFirst( NULL ), mpLast( NULL ), mpNameDict( NULL ) {}

CHE_PDF_Outline::~CHE_PDF_Outline()
{
	Clear();
}

HE_VOID	CHE_PDF_Outline::Clear()
{
	if ( mpFirst && mpLast )
	{
		CleanChildTree( mpFirst, mpLast );
	}
}

HE_BOOL CHE_PDF_Outline::Parse( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_NameDict * pNameDict /*= NULL*/ )
{
	mpNameDict = pNameDict;

	if ( !refPtr )
	{
		return FALSE;
	}

	if ( mpFirst && mpLast )
	{
		return TRUE;
	}

	CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_DICTIONARY );
	if ( ! objPtr )
	{
		return FALSE;
	}

	CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
	if ( ! dictPtr->CheckName( "Type", "Outlines", FALSE ) )
	{
		return FALSE;
	}

	objPtr = dictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( !objPtr )
	{
		return FALSE;
	}
	mCount = objPtr->GetNumberPtr()->GetInteger();

	CHE_PDF_ReferencePtr firstRefPtr, lastRefPtr;
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
		return FALSE;
	}

	mRefPtr = refPtr;
	CHE_PDF_OutlineItem * pTmp = GetAllocator()->New<CHE_PDF_OutlineItem>( GetAllocator() );
	mCount = BuildChildTree( pTmp, firstRefPtr, lastRefPtr, TRUE );
	mpFirst = pTmp->mpFirst;
	mpLast = pTmp->mpLast;
	GetAllocator()->Delete( pTmp );
	return TRUE;
}

HE_BOOL CHE_PDF_Outline::GetOutlineItem( CHE_PDF_OutlineItem * pItem, CHE_PDF_DictionaryPtr & dictPtr )
{
	if ( pItem == NULL )
	{
		return FALSE;
	}

	if ( dictPtr )
	{
		CHE_PDF_ObjectPtr objPtr = dictPtr->GetElement( "Title", OBJ_TYPE_STRING );
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
			CHE_PDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
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
				pItem->mpDest = GetAllocator()->New<CHE_PDF_Destination>( objPtr->GetArrayPtr(), GetAllocator() );
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
						pItem->mpDest = GetAllocator()->New<CHE_PDF_Destination>( objPtr->GetArrayPtr(), GetAllocator() );
					}
				}
			}
		}else{
			objPtr = dictPtr->GetElement( "A", OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				pItem->mAction = objPtr->GetDictPtr();
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

HE_UINT32 CHE_PDF_Outline::BuildChildTree( CHE_PDF_OutlineItem * pCurItem, CHE_PDF_ReferencePtr & firstRef, CHE_PDF_ReferencePtr & lastRef, HE_BOOL bRoot /*= FALSE*/ )
{
	if ( pCurItem == NULL )
	{
		return 0;
	}

	HE_UINT32 count = 0;
	CHE_PDF_ObjectPtr objPtr1;
	CHE_PDF_ObjectPtr objPtr2;
	objPtr1 = firstRef->GetRefObj( OBJ_TYPE_DICTIONARY );
	if ( !objPtr1 )
	{
		return 0;
	}

	CHE_PDF_ReferencePtr tmpRef = firstRef;
	CHE_PDF_DictionaryPtr dictPtr = objPtr1->GetDictPtr();

	CHE_PDF_OutlineItem * pTmpItem = GetAllocator()->New<CHE_PDF_OutlineItem>( GetAllocator() );
	pTmpItem->mRefPtr = tmpRef;
	pTmpItem->mpParent = pCurItem;
	if ( bRoot )
	{
		pTmpItem->mpParent = NULL;
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
			HE_UINT32 childCount = BuildChildTree( pTmpItem, objPtr1->GetRefPtr(), objPtr2->GetRefPtr() );
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
			pTmpItem->mpNext = pTmpItem->GetAllocator()->New<CHE_PDF_OutlineItem>( pTmpItem->GetAllocator() );
			pTmpItem->mpNext->mpPrev = pTmpItem;
			pTmpItem = pTmpItem->mpNext;
			pTmpItem->mRefPtr = tmpRef;
			pTmpItem->mpParent = pCurItem;
			if ( bRoot )
			{
				pTmpItem->mpParent = NULL;
			}
		}else{
			break;
		}
	}
	pCurItem->mpLast = pTmpItem;

	return count;
}

HE_VOID CHE_PDF_Outline::CleanChildTree( CHE_PDF_OutlineItem * pFirst, CHE_PDF_OutlineItem * pLast )
{
	if ( !pFirst || !pLast )
	{
		return;
	}

	CHE_PDF_OutlineItem * pTmp = pFirst;
	CHE_PDF_OutlineItem * pTarget = NULL;
	while ( pTmp != pLast )
	{
		pTarget = pTmp;
		pTmp = pTmp->mpNext;
		if ( pTarget->mpFirst && pTarget->mpLast )
		{
			CleanChildTree( pTarget->mpFirst, pTmp->mpLast );
		}
		pTarget->GetAllocator()->Delete( pTarget );
	}
}


