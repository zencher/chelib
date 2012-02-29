#include "../../include/pdf/che_pdf_collector.h"

bool operator == ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 )
{
	if ( node1.m_RefInfo.objNum == node2.m_RefInfo.objNum && node1.m_RefInfo.genNum == node2.m_RefInfo.genNum )
	{
		return true;
	}
	return false;
}

bool operator > ( const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2 )
{
	if ( ( node1.m_RefInfo.objNum > node2.m_RefInfo.objNum ) ||
		 ( node1.m_RefInfo.objNum == node2.m_RefInfo.objNum && node1.m_RefInfo.genNum > node2.m_RefInfo.genNum ) )
	{
		return true;
	}
	return false;
}

bool operator < (  const CHE_PDF_CollectorNode & node1, const CHE_PDF_CollectorNode & node2  )
{
	if ( ( node1.m_RefInfo.objNum < node2.m_RefInfo.objNum ) ||
		 ( node1.m_RefInfo.objNum == node2.m_RefInfo.objNum && node1.m_RefInfo.genNum < node2.m_RefInfo.genNum ) )
	{
		return true;
	}
	return false;
}

CHE_PDF_CollectorNode & CHE_PDF_CollectorNode::operator = ( const CHE_PDF_CollectorNode & node )
{
	m_RefInfo.objNum = node.m_RefInfo.objNum;
	m_RefInfo.genNum = node.m_RefInfo.genNum;
	m_InObj = node.m_InObj;
	return *this;
}