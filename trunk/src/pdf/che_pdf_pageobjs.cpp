#include "../../include/pdf/che_pdf_pageobjs.h"


HE_VOID CHE_PDF_ContentObject::Release()
{
	switch ( m_Type )
	{
	case CONTENTOBJ_INVALID:
		GetAllocator()->Delete<CHE_PDF_ContentObject>( this );
		break;
	case CONTENTOBJ_TEXT:
		GetAllocator()->Delete<CHE_PDF_TextObject>( (CHE_PDF_TextObject*)this );
		break;
	default:
		GetAllocator()->Delete<CHE_PDF_ContentObject>( this );
		break;
	}
}

HE_BOOL CHE_PDF_TextObject::AppendItem( CHE_PDF_TextObjectItem * pItem )
{
	if ( pItem )
	{
		return m_arrTextItem.Append( (HE_LPVOID)pItem );
	}
	return FALSE;
}

HE_VOID CHE_PDF_TextObject::Clear()
{
	CHE_PDF_TextObjectItem * pItem = NULL;
	for ( HE_DWORD i = 0; i < m_arrTextItem.GetCount(); i++ )
	{
		 pItem = (CHE_PDF_TextObjectItem *)m_arrTextItem.GetItem( i );
		 if ( pItem )
		 {
			 pItem->GetAllocator()->Delete<CHE_PDF_TextObjectItem>( pItem );
		 }
	}
	//m_arrTextItem.Clear();
}

CHE_PDF_TextObjectItem::~CHE_PDF_TextObjectItem()
{
	if ( m_pText )
	{
		m_pText->GetAllocator()->Delete<CHE_PDF_Object>( m_pText );
	}
}

HE_BOOL CHE_PDF_PathObject::AppendItem( CHE_GraphicsObject * pGraph )
{
	if ( pGraph )
	{
		return m_arrPathItem.Append( (HE_LPVOID)pGraph );
	}
	return FALSE;
}

HE_VOID CHE_PDF_PathObject::Clear()
{
	CHE_GraphicsObject * pItem = NULL;
	for ( HE_DWORD i = 0; i < m_arrPathItem.GetCount(); i++ )
	{
		pItem = (CHE_GraphicsObject *)m_arrPathItem.GetItem( i );
		if ( pItem )
		{
			pItem->GetAllocator()->Delete<CHE_GraphicsObject>( pItem );
		}
	}
	m_arrPathItem.Clear();
}