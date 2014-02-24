#include "../../include/pdf/che_pdf_outline.h"

CHE_PDF_OutlineItem::CHE_PDF_OutlineItem( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mCount( 0 ), mpFirstChild( NULL ), mpLastChild( NULL ), mpPrevBrother( NULL ), mpNextBrother( NULL )
{
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

	CHE_PDF_ObjectPtr objPtr;
	objPtr = mpFile->GetObject( curRefInfo );
	if ( ! objPtr || objPtr->GetType() != OBJ_TYPE_DICTIONARY )
	{
		return FALSE;
	}

	CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
	if ( !dictPtr )
	{
		return FALSE;
	}

// 	objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
// 	if ( ! objPtr )
// 	{
// 		return FALSE;
// 	}
// 	CHE_PDF_NamePtr namePtr = objPtr->GetNamePtr();
// 	if ( namePtr->GetString() != "Outlines" )
// 	{
// 		return FALSE;
// 	}

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

	if ( GetOutlineItem( mpRootItem, refPtr ) )
	{
		BuildChildTree( mpRootItem, refPtr, firstRefPtr );
	}

	
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


CHE_PDF_OutlineItem * CHE_PDF_Outline::BuildBrotherTree( CHE_PDF_OutlineItem * pCurItem, CHE_PDF_ReferencePtr curRefPtr, CHE_PDF_ReferencePtr nextPtr )
{
	if ( pCurItem == NULL )
	{
		return NULL;
	}

	if ( ! nextPtr || ! curRefPtr )
	{
		return NULL;
	}

	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_ReferencePtr refPtr = nextPtr;
	CHE_PDF_DictionaryPtr dictPtr;
	CHE_PDF_OutlineItem * pNewItem = NULL;
	CHE_PDF_OutlineItem * pPreItem = pCurItem;

	while ( refPtr )
	{
		objPtr = mpFile->GetObject( refPtr->GetRefInfo() );
		if ( ! objPtr || objPtr->GetType() != OBJ_TYPE_DICTIONARY )
		{
			break;
		}
		dictPtr = objPtr->GetDictPtr();
		if ( ! dictPtr )
		{
			break;
		}

		pNewItem = GetAllocator()->New<CHE_PDF_OutlineItem>( GetAllocator() );
		if ( ! GetOutlineItem( pNewItem, refPtr ) )
		{
			objPtr = dictPtr->GetElement( "Next", OBJ_TYPE_REFERENCE );
			if ( objPtr )
			{
				refPtr = objPtr->GetRefPtr();
			}else{
				refPtr.Reset();
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
			BuildChildTree( pNewItem, refPtr, objPtr->GetRefPtr() );
		}

		objPtr = dictPtr->GetElement( "Next", OBJ_TYPE_REFERENCE );
		if ( objPtr )
		{
			refPtr = objPtr->GetRefPtr();
		}else{
			refPtr.Reset();
		}
		pPreItem = pNewItem;
	}

	return pNewItem;
}

HE_VOID CHE_PDF_Outline::BuildChildTree( CHE_PDF_OutlineItem * pCurItem, CHE_PDF_ReferencePtr curRefInfo, CHE_PDF_ReferencePtr firstPtr )
{
	if ( pCurItem == NULL )
	{
		return;
	}

	if ( !firstPtr )
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr = mpFile->GetObject( firstPtr->GetRefInfo() );
	if ( !objPtr || objPtr->GetType() != OBJ_TYPE_DICTIONARY )
	{
		return;
	}

	CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
	if ( !dictPtr )
	{
		return;
	}

	pCurItem->mpPrevBrother = NULL;
	pCurItem->mpNextBrother = NULL;
	pCurItem->mpFirstChild = NULL;
	pCurItem->mpLastChild = NULL;

	CHE_PDF_OutlineItem * pNewItem = pCurItem->GetAllocator()->New<CHE_PDF_OutlineItem>( pCurItem->GetAllocator() );
	if ( ! GetOutlineItem( pCurItem, curRefInfo ) )
	{
		GetAllocator()->Delete<CHE_PDF_OutlineItem>( pNewItem );
		pNewItem = NULL;
		return;
	}

	pCurItem->mpFirstChild = pNewItem;
	objPtr = dictPtr->GetElement( "First", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		BuildChildTree( pNewItem, firstPtr, objPtr->GetRefPtr() );
	}
	objPtr = dictPtr->GetElement( "Next", OBJ_TYPE_REFERENCE );
	if ( objPtr )
	{
		pNewItem = BuildBrotherTree( pNewItem, firstPtr, objPtr->GetRefPtr() );
	}

	pCurItem->mpLastChild = pNewItem;
}

HE_BOOL CHE_PDF_Outline::GetOutlineItem( CHE_PDF_OutlineItem * pItem, CHE_PDF_ReferencePtr refPtr )
{
	if ( pItem == NULL )
	{
		return FALSE;
	}
	if ( ! refPtr )
	{
		return FALSE;
	}

	CHE_PDF_ObjectPtr objPtr = mpFile->GetObject( refPtr->GetRefInfo() );
	if ( ! objPtr || objPtr->GetType() != OBJ_TYPE_DICTIONARY )
	{
		return FALSE;
	}

	CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
	if ( dictPtr )
	{
		CHE_PDF_ObjectPtr objPtr = dictPtr->GetElement( "Title", OBJ_TYPE_STRING );
		if ( objPtr )
		{
			pItem->mTitleStrPtr = objPtr->GetStringPtr()->GetString();
		}
	}

	return TRUE;
}
