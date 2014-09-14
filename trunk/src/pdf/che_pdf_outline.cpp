#include "../../include/pdf/che_pdf_outline.h"

CHE_PDF_OutlineItem::CHE_PDF_OutlineItem( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mCount( 0 ), mpParent(NULL), mpPrevBrother( NULL ), mpNextBrother( NULL ),
	mpFirstChild( NULL ), mpLastChild( NULL ) {}

CHE_PDF_Outline::CHE_PDF_Outline( CHE_PDF_File * pFile, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mpFile( pFile ), mpRootItem( NULL ) {}

CHE_PDF_Outline::~CHE_PDF_Outline()
{
	//Ïú»ÙÄ¿Â¼Ê÷
}

HE_BOOL CHE_PDF_Outline::Parse( const CHE_PDF_ReferencePtr & refPtr )
{
	if ( !refPtr || !mpFile )
	{
		return FALSE;
	}

	if ( mpRootItem )
	{
		return TRUE;
	}

	PDF_RefInfo curRefInfo = refPtr->GetRefInfo();
	CHE_PDF_ObjectPtr objPtr = mpFile->GetObject( curRefInfo );
	if ( ! objPtr || objPtr->GetType() != OBJ_TYPE_DICTIONARY )
	{
		return FALSE;
	}

	CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();

//	/Type/Outlines is optional!
// 	objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
// 	if ( ! objPtr )
// 	{
// 		return FALSE;
// 	}
// 	if ( objPtr->GetNamePtr()->GetString() != "Outlines" )
// 	{
// 		return FALSE;
// 	}

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
		return FALSE;
	}

	objPtr = dictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( !objPtr )
	{
		return FALSE;
	}

	mpRootItem = GetAllocator()->New<CHE_PDF_OutlineItem>( GetAllocator() );
	mpRootItem->mpParent = NULL;
	mpRootItem->mpNextBrother = NULL;
	mpRootItem->mpPrevBrother = NULL;
	mpRootItem->mpFirstChild = NULL;
	mpRootItem->mpLastChild = NULL;
	mpRootItem->mCount = objPtr->GetNumberPtr()->GetInteger();
	BuildChildTree( mpRootItem, refPtr, firstRefPtr );
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
		pNewItem->mpParent = pCurItem->mpParent;
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

	pCurItem->mpFirstChild = NULL;
	pCurItem->mpLastChild = NULL;

	CHE_PDF_OutlineItem * pNewItem = pCurItem->GetAllocator()->New<CHE_PDF_OutlineItem>( pCurItem->GetAllocator() );
	pNewItem->mpParent = pCurItem;

	if ( ! GetOutlineItem( pNewItem, firstPtr ) )
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
		objPtr = dictPtr->GetElement( "Title", OBJ_TYPE_STRING );
		if ( objPtr )
		{
			pItem->mTitleStrPtr = objPtr->GetStringPtr()->GetString();
		}
	}

	objPtr = dictPtr->GetElement( "Count", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		pItem->mCount = objPtr->GetNumberPtr()->GetInteger();
	}

	return TRUE;
}
