#include "../../include/pdf/che_pdf_collector.h"

bool operator == ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 )
{
	if ( node1.m_ObjNum == node2.m_ObjNum )
	{
		return true;
	}
	return false;
}

bool operator > ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 )
{
	if ( node1.m_ObjNum > node2.m_ObjNum )
	{
		return true;
	}
	return false;
}

bool operator < (  const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2  )
{
	if ( node1.m_ObjNum < node2.m_ObjNum )
	{
		return true;
	}
	return false;
}

CHE_PDF_CollectorNode & CHE_PDF_CollectorNode::operator = ( const CHE_PDF_CollectorNode & node )
{
	m_ObjNum = node.m_ObjNum;
	m_InObj = node.m_InObj;
	return *this;
}