#ifndef _CHE_PDF_COLLECTOR_H_
#define _CHE_PDF_COLLECTOR_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"

class CHE_PDF_CollectorNode
{
public:
	CHE_PDF_CollectorNode() : m_ObjNum( 0 ), m_InObj( NULL ) {}
	CHE_PDF_CollectorNode( HE_DWORD objNum, CHE_PDF_IndirectObject * pInObj ) : m_ObjNum( objNum ), m_InObj( pInObj ) {}
	CHE_PDF_CollectorNode & operator = ( const CHE_PDF_CollectorNode & node );

	HE_DWORD m_ObjNum;
	CHE_PDF_IndirectObject * m_InObj;
};

bool operator == ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 );
bool operator >  ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 );
bool operator <  ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 );

class CHE_PDF_Collector : public CHE_Object
{
public:
	CHE_PDF_Collector( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ), m_QuickReq( pAllocator ) {}

	~CHE_PDF_Collector() { Clear(); }

	HE_DWORD GetCount() { return m_QuickReq.GetCount(); }

	HE_BOOL Add( CHE_PDF_IndirectObject * pInObj )
	{
		if ( pInObj == NULL )
		{
			return FALSE;
		}
		if ( m_QuickReq.Append( CHE_PDF_CollectorNode( pInObj->GetObjNum(), pInObj ) )  )
		{
			m_QuickGet.push_back( pInObj );
			return TRUE;
		}
		return FALSE;
	}

// 	HE_BOOL Add( CHE_PDF_Object * pObj )
// 	{
// 		if ( pObj == NULL )
// 		{
// 			return FALSE;
// 		}
// 		if ( m_QuickReq.Append( CHE_PDF_CollectorNode( pObj->GetObjNum(), pObj ) )  )
// 		{
// 			m_QuickGet.push_back( pObj );
// 			return TRUE;
// 		}
// 		return FALSE;
// 	}

	HE_BOOL IsExist( HE_DWORD objNum )
	{
		return m_QuickReq.IsExist( CHE_PDF_CollectorNode( objNum, NULL ) );
	}

	CHE_PDF_IndirectObject * GetInObj( HE_DWORD objNum )
	{
		CHE_PDF_CollectorNode node( objNum, NULL );
		if ( m_QuickReq.Find( node ) )
		{
			return node.m_InObj;
		}
		return NULL; 
	}

// 	CHE_PDF_Object * GetObj( HE_DWORD objNum )
// 	{
// 		CHE_PDF_CollectorNode node( objNum, NULL );
// 		if ( m_QuickReq.Find( node ) )
// 		{
// 			return node.m_Obj;
// 		}
// 		return NULL; 
// 	}

	CHE_PDF_IndirectObject * GetObjByIndex( HE_DWORD index )
	{
		if ( index < m_QuickReq.GetCount() )
		{
			return m_QuickGet[index];
		}
		return NULL;
	}

// 	CHE_PDF_Object * GetObjByIndex( HE_DWORD index )
// 	{
// 		if ( index < m_QuickReq.GetCount() )
// 		{
// 			return m_QuickGet[index];
// 		}
// 		return NULL;
// 	}

	HE_VOID Clear()
	{
		m_QuickGet.clear();
		m_QuickReq.Clear();
	}

	HE_VOID ReleaseObj()
	{
		HE_DWORD count = m_QuickReq.GetCount();
		CHE_PDF_IndirectObject * pInObj = NULL;
		for ( HE_DWORD i = 0; i < count; i++ )
		{
			pInObj = m_QuickGet[i];
			if ( pInObj )
			{
				pInObj->Release();
			}
		}
	}

private:
	std::vector<CHE_PDF_IndirectObject*> m_QuickGet;
	CHE_SkipList<CHE_PDF_CollectorNode>	m_QuickReq;
};

#endif