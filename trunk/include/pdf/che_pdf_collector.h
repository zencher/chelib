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

	~CHE_PDF_Collector() { Clear(); }

	HE_ULONG GetCount() { return m_QuickReq.GetCount(); }

	HE_BOOL Add( const PDF_RefInfo & refInfo, const CHE_PDF_ObjectPtr & ObjPtr )
	{
		if ( ! ObjPtr )
		{
			return FALSE;
		}
        CHE_PDF_CollectorNode tmpNode( refInfo, ObjPtr );
		if ( m_QuickReq.Append( tmpNode ) )
		{
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

	HE_VOID Clear()
	{
		m_QuickReq.Clear();
	}

private:

	CHE_SkipList<CHE_PDF_CollectorNode>	m_QuickReq;
};

#endif