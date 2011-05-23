#include "../../include/pdf/che_pdf_graphics.h"

HE_VOID	CHE_Path::AddLine( CHE_Line & line )
{
	CHE_Line * pLine = new CHE_Line( line.m_lStartX, line.m_lStartY, line.m_lEndX, line.m_lEndY );
	m_objArray.Append( (HE_LPVOID)pLine );
}

HE_VOID	CHE_Path::AddRectangle( CHE_Rectangle & rect )
{
	CHE_Rectangle * pRect = new CHE_Rectangle( rect.m_lLeftTopX, rect.m_lLeftTopY, rect.m_dwWidth, rect.m_dwHeight );
	m_objArray.Append( (HE_LPVOID)pRect );
}