#ifndef _CHE_PDF_COLLECTOR_H_
#define _CHE_PDF_COLLECTOR_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"

class CHE_PDF_CollectorNode
{
public:
	CHE_PDF_CollectorNode() { m_RefInfo.objNum = 0; m_RefInfo.genNum = 0; }
	CHE_PDF_CollectorNode( PDF_RefInfo refInfo, const CHE_PDF_ObjectPtr & ObjPtr )
		: mObjPtr( ObjPtr ) { m_RefInfo.objNum = refInfo.objNum; m_RefInfo.genNum = refInfo.genNum; }
	CHE_PDF_CollectorNode & operator = ( const CHE_PDF_CollectorNode & node );

	PDF_RefInfo m_RefInfo;
	CHE_PDF_ObjectPtr mObjPtr;
};

bool operator == ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 );
bool operator >  ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 );
bool operator <  ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 );

class CHE_PDF_Collector : public CHE_Object
{
public:
	CHE_PDF_Collector( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ), m_QuickReq( pAllocator ) {}

	~CHE_PDF_Collector() { /*Clear();*/ }

	HE_DWORD GetCount() { return m_QuickReq.GetCount(); }

	HE_BOOL Add( const PDF_RefInfo & refInfo, const CHE_PDF_ObjectPtr & ObjPtr )
	{
		if ( ! ObjPtr )
		{
			return FALSE;
		}
// 		PDF_RefInfo refInfo;
// 		refInfo.objNum = pInObj->GetObjNum();
// 		refInfo.genNum = pInObj->GetGenNum();
		if ( m_QuickReq.Append( CHE_PDF_CollectorNode( refInfo, ObjPtr ) )  )
		{
			//m_QuickGet.push_back( pInObj );
			return TRUE;
		}
		return FALSE;
	}

	HE_BOOL IsExist( PDF_RefInfo refInfo )
	{
		return m_QuickReq.IsExist( CHE_PDF_CollectorNode( refInfo, CHE_PDF_ObjectPtr() ) );
	}

	CHE_PDF_ObjectPtr GetObj( PDF_RefInfo refInfo )
	{
		CHE_PDF_CollectorNode node( refInfo, CHE_PDF_ObjectPtr() );
		if ( m_QuickReq.Find( node ) )
		{
			return node.mObjPtr;
		}
		return CHE_PDF_ObjectPtr(); 
	}

// 	CHE_PDF_IndirectObject * GetObjByIndex( HE_DWORD index )
// 	{
// 		if ( index < m_QuickReq.GetCount() )
// 		{
// 			return m_QuickGet[index];
// 		}
// 		return NULL;
// 	}

	HE_VOID Clear()
	{
		//m_QuickGet.clear();
		m_QuickReq.Clear();
	}

// 	HE_VOID ReleaseObj()
// 	{
// 		HE_DWORD count = m_QuickReq.GetCount();
// 		CHE_PDF_IndirectObject * pInObj = NULL;
// 		for ( HE_DWORD i = 0; i < count; i++ )
// 		{
// 			pInObj = m_QuickGet[i];
// 			if ( pInObj )
// 			{
// 				pInObj->Release();
// 			}
// 		}
// 	}

private:
	//std::vector<CHE_PDF_IndirectObject*> m_QuickGet;
	CHE_SkipList<CHE_PDF_CollectorNode>	m_QuickReq;
};

#endif