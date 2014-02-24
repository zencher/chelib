#include "../../include/pdf/che_pdf_componentmgr.h"

class CHE_PDF_ComponentMgrItem
{
public:
	CHE_PDF_ComponentPtr mComponentPtr;
};

CHE_PDF_ComponentMgr::CHE_PDF_ComponentMgr( CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_Object(pAllocator), mFontMgr(pAllocator), mComponentsMap(pAllocator)
{
	int x = 0;
}

CHE_PDF_ComponentMgr::~CHE_PDF_ComponentMgr()
{
	int x= 0;
}

CHE_PDF_Font * CHE_PDF_ComponentMgr::LoadFont( const CHE_PDF_ReferencePtr & refPtr )
{
	return mFontMgr.LoadFont( refPtr );
}

CHE_PDF_ComponentPtr CHE_PDF_ComponentMgr::GetComponent( const CHE_PDF_ReferencePtr & refPtr )
{
	CHE_PDF_ComponentPtr ptr;
	if ( refPtr )
	{
		CHE_PDF_ComponentMgrItem * pItem = (CHE_PDF_ComponentMgrItem *) mComponentsMap.GetItem( refPtr->GetRefInfo().objNum );
		if ( pItem )
		{
			ptr = pItem->mComponentPtr;
		}
	}
	return ptr;
}

CHE_PDF_ComponentPtr CHE_PDF_ComponentMgr::GetComponent( const CHE_PDF_ReferencePtr & refPtr, PDF_COMPONENT_TYPE type )
{
	CHE_PDF_ComponentPtr ptr;
	if ( refPtr )
	{
		CHE_PDF_ComponentMgrItem * pItem = (CHE_PDF_ComponentMgrItem *) mComponentsMap.GetItem( refPtr->GetRefInfo().objNum );
		if ( pItem )
		{
			if ( pItem->mComponentPtr && pItem->mComponentPtr->GetType() == type )
			{
				ptr = pItem->mComponentPtr;
			}
		}
	}
	return ptr;
}

HE_BOOL	CHE_PDF_ComponentMgr::PushComponent( const CHE_PDF_ReferencePtr & refPtr, const CHE_PDF_ComponentPtr & component )
{
	if ( refPtr && component )
	{
		CHE_PDF_ComponentMgrItem * pItem = GetAllocator()->New<CHE_PDF_ComponentMgrItem>();
		pItem->mComponentPtr = component;
		mComponentsMap.Append( refPtr->GetGenNum(), pItem );
		return TRUE;
	}
	return FALSE;
}