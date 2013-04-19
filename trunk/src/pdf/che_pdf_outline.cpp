#include "../../include/pdf/che_pdf_outline.h"

CHE_PDF_OutlineItem::CHE_PDF_OutlineItem( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mCount( 0 ), mpFirstChild( NULL ), mpLastChild( NULL ), mpPrevBrother( NULL ), mpNextBrother( NULL )
{
	mTitleStrPtr.reset();
}

CHE_PDF_Outline::CHE_PDF_Outline( CHE_PDF_File * pFile, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mpFile( pFile ), mpRootItem( NULL ), mCount( 0 ) {}

CHE_PDF_Outline::~CHE_PDF_Outline()
{
	//销毁目录树
}

HE_BOOL CHE_PDF_Outline::Parse( const CHE_PDF_ReferencePtr & refPtr )
{
	if ( !refPtr || !mpFile )
	{
		return FALSE;
	}

	if ( mpRootItem )
	{
		//已经解析好了
		return TRUE;
	}

	PDF_RefInfo curRefInfo = refPtr->GetRefInfo();

	CHE_PDF_DictionaryPtr dictPtr = refPtr->GetDictPtr();
	if ( !dictPtr )
	{
		return FALSE;
	}

	CHE_PDF_ObjectPtr objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
	if ( ! objPtr )
	{
		return FALSE;
	}
	CHE_PDF_NamePtr namePtr = objPtr->GetNamePtr();
	if ( namePtr->GetString() != "Outlines" )
	{
		return FALSE;
	}

	objPtr = dictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( !objPtr )
	{
		return FALSE;
	}
	CHE_PDF_NumberPtr numberPtr = objPtr->GetNumberPtr();
	mCount = numberPtr->GetInteger();

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

	//构造树

	mpRootItem = GetAllocator()->New<CHE_PDF_OutlineItem>( GetAllocator() );
	mpRootItem->mCount = mCount;
	mpRootItem->mpFirstChild = NULL;
	mpRootItem->mpLastChild = NULL;
	mpRootItem->mpNextBrother = NULL;
	mpRootItem->mpPrevBrother = NULL;

	BuildChildTree( mpRootItem, curRefInfo, firstRefPtr, lastRefPtr );
// 
// 	CHE_PDF_ReferencePtr refPtr = firstRefPtr;
// 
// 	while ( refPtr && refPtr->GetRefNum() != lastRefPtr->GetRefNum() && refPtr->GetGenNum() != lastRefPtr->GetGenNum() )
// 	{
// 		CHE_PDF_DictionaryPtr itemDict = refPtr->GetRefObj( OBJ_TYPE_DICTIONARY );
// 		if ( !itemDict )
// 		{
// 			break;
// 		}
// 
// 
// 
// 		refPtr = itemDict->GetElement( "Next", OBJ_TYPE_REFERENCE );
// 	}
	
	return TRUE;
}


HE_VOID CHE_PDF_Outline::BuildBrotherTree( CHE_PDF_OutlineItem * pCurItem, PDF_RefInfo curRefInfo, CHE_PDF_ReferencePtr nextPtr )
{
	if ( pCurItem == NULL )
	{
		return;
	}

	if ( ! nextPtr )
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_ReferencePtr refPtr = nextPtr;
	CHE_PDF_DictionaryPtr dictPtr;
	PDF_RefInfo refInfo;
	CHE_PDF_OutlineItem * pNewItem = NULL;
	CHE_PDF_OutlineItem * pPreItem = pCurItem;
	CHE_PDF_ReferencePtr firstPtr;
	CHE_PDF_ReferencePtr lastPtr;

	while ( refPtr )
	{
		dictPtr = refPtr->GetDictPtr();
		if ( ! dictPtr )
		{
			break;
		}

		refInfo = refPtr->GetRefInfo();

		pNewItem = GetAllocator()->New<CHE_PDF_OutlineItem>( GetAllocator() );
		if ( ! GetOutlineItem( pNewItem, refPtr ) )
		{
			objPtr = dictPtr->GetElement( "Next", OBJ_TYPE_REFERENCE );
			if ( objPtr )
			{
				refPtr = objPtr->GetRefPtr();
			}else{
				refPtr.reset();
			}
			GetAllocator()->Delete<CHE_PDF_OutlineItem>( pNewItem );
			pNewItem = NULL;
			continue;
		}

		pPreItem->mpNextBrother = pNewItem;
		pNewItem->mpPrevBrother = pPreItem;

		objPtr = dictPtr->GetElement( "First", OBJ_TYPE_REFERENCE );
		if ( objPtr )
		{
			firstPtr = objPtr->GetRefPtr();
		}else{
			firstPtr.reset();
		}
		objPtr = dictPtr->GetElement( "Last", OBJ_TYPE_REFERENCE );
		if ( objPtr )
		{
			lastPtr = objPtr->GetRefPtr();
		}else{
			lastPtr.reset();
		}

		if ( firstPtr && lastPtr )
		{
			BuildChildTree( pNewItem, refInfo, firstPtr, lastPtr );
		}
		objPtr = dictPtr->GetElement( "Next", OBJ_TYPE_REFERENCE );
		if ( objPtr )
		{
			refPtr = objPtr->GetRefPtr();
		}else{
			refPtr.reset();
		}
		pPreItem = pNewItem;
	}
}

HE_VOID CHE_PDF_Outline::BuildChildTree(	CHE_PDF_OutlineItem * pCurItem, PDF_RefInfo curRefInfo,
											CHE_PDF_ReferencePtr firstPtr, CHE_PDF_ReferencePtr lastPtr )
{
	if ( pCurItem == NULL )
	{
		return;
	}


}
