#include "../include/che_graphics.h"

HE_VOID CHE_GraphicsObject::Release()
{
	switch ( m_type )
	{
	case GRAPHTYPE_INVALID:
		GetAllocator()->Delete<CHE_GraphicsObject>( this );
		break;
	case GRAPHTYPE_LINE:
		GetAllocator()->Delete<CHE_Line>( (CHE_Line*)this );
		break;
	case GRAPHTYPE_BCURVE:
		GetAllocator()->Delete<CHE_BCurve>( (CHE_BCurve*)this );
		break;
	case GRAPHTYPE_PATH:
		GetAllocator()->Delete<CHE_Path>( (CHE_Path*)this );
		break;
	default:
		GetAllocator()->Delete<CHE_GraphicsObject>( this );
		break;
	}
}

HE_VOID	CHE_Path::AddLine( CHE_Line & line )
{
	CHE_Line * pLine = GetAllocator()->New<CHE_Line>( line.m_fStartX, line.m_fStartY, line.m_fEndX, line.m_fEndY, GetAllocator() );
	m_objArray.Append( (HE_LPVOID)pLine );
}

HE_VOID CHE_Path::AddBCrave( CHE_BCurve & crave )
{
	CHE_BCurve * pCrave = GetAllocator()->New<CHE_BCurve>( crave.m_fX1, crave.m_fY1, crave.m_fX2, crave.m_fY2, crave.m_fX3, crave.m_fY3, crave.m_fX4, crave.m_fY4, GetAllocator() );
}

HE_VOID	CHE_Path::AddRectangle( CHE_Rectangle & rect )
{
	CHE_Rectangle * pRect = GetAllocator()->New<CHE_Rectangle>( rect.m_fLeftTopX, rect.m_fLeftTopY, rect.m_fWidth, rect.m_fHeight, GetAllocator() );
	m_objArray.Append( (HE_LPVOID)pRect );
}