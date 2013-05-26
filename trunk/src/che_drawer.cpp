#include "../include/che_drawer.h"

CHE_GraphicsDrawer::CHE_GraphicsDrawer( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight )
{
	m_DC = hDC;
	m_dwWidth = dibWidth;
	m_dwHeight = dibHeight;
	m_MemDC = CreateCompatibleDC( hDC );
	m_Bitmap = CreateCompatibleBitmap( m_DC, m_dwWidth, m_dwHeight );
	m_OldBitmap = SelectObject( m_MemDC, m_Bitmap );

	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );

	m_pGraphics  = new Gdiplus::Graphics( m_MemDC );
	m_pGraphics->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
	m_pGraphics->SetPageUnit( Gdiplus::UnitPixel );
	m_pPen = new Gdiplus::Pen( Gdiplus::Color( 255, 0, 0, 0 ), 1 );
	m_pBrush = new Gdiplus::SolidBrush( Gdiplus::Color( 255, 0, 0, 0 ) );

	mLineWidth = 1;
	mLineCap = LineCap_Butt;
	mLineJion = LineJoin_Miter;
	mDashPhase = 0;
}

CHE_GraphicsDrawer::~CHE_GraphicsDrawer()
{
	if ( m_pBrush )
	{
		delete m_pBrush;
		m_pBrush = NULL;
	}
	if ( m_pPen )
	{
		delete m_pPen;
		m_pPen = NULL;
	}
	if ( m_MemDC )
	{
		if ( m_OldBitmap )
		{
			SelectObject( m_MemDC, m_OldBitmap );
			m_OldBitmap = NULL;
		}
		if ( m_Bitmap )
		{
			DeleteObject( m_Bitmap );
			m_Bitmap = NULL;
		}
		DeleteDC( m_MemDC );
		m_MemDC = NULL;
	}
	if ( m_pGraphics )
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
}

HE_VOID	CHE_GraphicsDrawer::Resize( HE_DWORD dibWidth, HE_DWORD dibHeight )
{
	m_dwWidth = dibWidth;
	m_dwHeight = dibHeight;

	if ( m_MemDC )
	{
		if ( m_OldBitmap )
		{
			SelectObject( m_MemDC, m_OldBitmap );
			m_OldBitmap = NULL;
		}
		DeleteDC( m_MemDC );
		if ( m_Bitmap )
		{
			DeleteObject( m_Bitmap );
			m_Bitmap = NULL;
		}
	}
	m_MemDC = CreateCompatibleDC( m_DC );
	m_Bitmap = CreateCompatibleBitmap( m_DC, m_dwWidth, m_dwHeight );
	m_OldBitmap = SelectObject( m_MemDC, m_Bitmap );

	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );

	if ( m_pGraphics )
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}

	m_pGraphics  = new Gdiplus::Graphics( m_MemDC );
	m_pGraphics->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
	m_pGraphics->SetPageUnit( Gdiplus::UnitPixel );
}

HE_DWORD CHE_GraphicsDrawer::GetWidth() const
{
	return m_dwWidth;
}

HE_DWORD CHE_GraphicsDrawer::GetHeight() const
{
	return m_dwHeight;
}

HDC CHE_GraphicsDrawer::GetMemDC() const
{
	return m_MemDC;
}

HE_VOID	CHE_GraphicsDrawer::Clear()
{
	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );
}

HE_VOID CHE_GraphicsDrawer::MoveTo( HE_FLOAT x, HE_FLOAT y )
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	mCurX = x;
	mCurY = y;
}

HE_VOID CHE_GraphicsDrawer::LineTo( HE_FLOAT x, HE_FLOAT y )
{
	m_path.AddLine( mCurX, mCurY, x, y );
	mCurX = x;
	mCurY = y;
}

HE_VOID CHE_GraphicsDrawer::CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
{
	m_path.AddBezier( mCurX, mCurY, x1, y1, x2, y2, x3, y3 );
	mCurX = x3;
	mCurY = y3;
}

HE_VOID CHE_GraphicsDrawer::ClosePath()
{
	m_path.CloseFigure();
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
}
	

HE_VOID CHE_GraphicsDrawer::FillPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		m_pathToDraw.Reset();
	}
}

HE_VOID CHE_GraphicsDrawer::StrokePath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		m_pathToDraw.Reset();
	}
}

HE_VOID CHE_GraphicsDrawer::FillStrokePath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		m_pathToDraw.Reset();
	}
}

HE_VOID CHE_GraphicsDrawer::ClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		Gdiplus::Region reg( &m_pathToDraw );
		m_pGraphics->SetClip( &reg, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}
}

HE_VOID	CHE_GraphicsDrawer::FillClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		Gdiplus::Region reg( &m_pathToDraw );
		m_pGraphics->SetClip( &reg, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}
}

HE_VOID	CHE_GraphicsDrawer::StrokeClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		Gdiplus::Region reg( &m_pathToDraw );
		m_pGraphics->SetClip( &reg, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}
}

HE_VOID	CHE_GraphicsDrawer::FillStrokeClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		Gdiplus::Region reg( &m_pathToDraw );
		m_pGraphics->SetClip( &reg, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}	
}

HE_VOID	CHE_GraphicsDrawer::ResetClip()
{
	m_pGraphics->ResetClip();
}

HE_VOID CHE_GraphicsDrawer::SetMatrix( const CHE_Matrix & matrix )
{
	if ( m_pGraphics )
	{
		CHE_Matrix t = matrix;
		t.Concat( mExtMatrix );
		Gdiplus::Matrix matrixToDev( t.a, t.b, t.c, t.d, t.e, t.f );
		m_pGraphics->SetTransform( &matrixToDev );
	}
}

HE_VOID CHE_GraphicsDrawer::SetExtMatrix( const CHE_Matrix & matrix )
{
	mExtMatrix = matrix;
}

HE_VOID CHE_GraphicsDrawer::SetLineWidth( const HE_FLOAT & lineWidth )
{
	if ( m_pPen )
	{
		m_pPen->SetWidth( lineWidth + 0.01 );
	}
	mLineWidth = lineWidth;
}

HE_VOID CHE_GraphicsDrawer::SetMiterLimit( const HE_FLOAT & miterLimit )
{
	if ( m_pPen )
	{
		m_pPen->SetMiterLimit( miterLimit );
	}
}

HE_VOID CHE_GraphicsDrawer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
{
	if ( m_pPen )
	{
		switch ( lineCap )
		{
		case LineCap_Butt:
			m_pPen->SetLineCap( Gdiplus::LineCapFlat, Gdiplus::LineCapFlat, Gdiplus::DashCapFlat );
			break;
		case LineCap_Round:
			m_pPen->SetLineCap( Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound );
			break;
		case LineCap_Square:
			m_pPen->SetLineCap( Gdiplus::LineCapSquare, Gdiplus::LineCapSquare, Gdiplus::DashCapFlat );
			break;
		default:
			break;
		}
	}
	mLineCap = lineCap;
}

HE_VOID CHE_GraphicsDrawer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
{
	if ( m_pPen )
	{
		switch ( lineJion )
		{
		case LineJoin_Miter:
			m_pPen->SetLineJoin( Gdiplus::LineJoinMiter );
			break;
		case LineJoin_Round:
			m_pPen->SetLineJoin( Gdiplus::LineJoinRound );
			break;
		case LineJoin_Bevel:
			m_pPen->SetLineJoin( Gdiplus::LineJoinBevel );
			break;
		default:
			break;
		}
	}
	mLineJion = lineJion;
}

HE_VOID CHE_GraphicsDrawer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
{
	if ( dashPattern.dashArray.size() == 1 )
	{
		Gdiplus::REAL * dashArray = new Gdiplus::REAL[2];
		dashArray[0] = dashArray[1] = dashPattern.dashArray[0] / GetLineWidth();
		if ( GetLineCap() != LineCap_Butt )
		{
			dashArray[0] += 0.99;
			dashArray[1] -= 0.99;
		}
		m_pPen->SetDashPattern( dashArray, 2 );
		if ( GetLineCap() == LineCap_Square )
		{
			m_pPen->SetDashOffset( GetDashPhase() / GetLineWidth() + 0.5 );
		}else{
			m_pPen->SetDashOffset( GetDashPhase() / GetLineWidth() );
		}	
		delete [] dashArray;
	}else if ( dashPattern.dashArray.size() > 1 )
	{
		if ( dashPattern.dashArray.size() % 2 == 1 )
		{
			Gdiplus::REAL * dashArray = new Gdiplus::REAL[dashPattern.dashArray.size()*2];
			for ( unsigned long i = 0; i < dashPattern.dashArray.size(); i++ )
			{
				dashArray[i] = dashPattern.dashArray[i] / GetLineWidth();
			}
			for ( unsigned long j = dashPattern.dashArray.size(); j < dashPattern.dashArray.size() * 2; j++ )
			{
				dashArray[j] = dashArray[j-dashPattern.dashArray.size()];
			}
			if ( GetLineCap() != LineCap_Butt )
			{
				int flag = 0;
				for ( unsigned long k = 0; k < dashPattern.dashArray.size() * 2; k++ )
				{
					if ( flag == 0 )
					{
						dashArray[k] += 0.99;
						flag = 1;
					}else{
						dashArray[k] -= 0.99;
						flag = 0;
					}
				}
			}
			m_pPen->SetDashPattern( dashArray, dashPattern.dashArray.size() * 2 );
			if ( GetLineCap() == LineCap_Square )
			{
				m_pPen->SetDashOffset( GetDashPhase() / GetLineWidth() + 0.5 );
			}else{
				m_pPen->SetDashOffset( GetDashPhase() / GetLineWidth() );
			}
			delete [] dashArray;
		}else{
			Gdiplus::REAL * dashArray = new Gdiplus::REAL[dashPattern.dashArray.size()];
			for ( unsigned long i = 0; i < dashPattern.dashArray.size(); i++ )
			{
				dashArray[i] = dashPattern.dashArray[i] / GetLineWidth();
			}
			if ( GetLineCap() != LineCap_Butt )
			{
				int flag = 0;
				for ( unsigned long i = 0; i < dashPattern.dashArray.size(); i++ )
				{
					if ( flag == 0 )
					{
						dashArray[i] += 0.99;
						flag = 1;
					}else{
						dashArray[i] -= 0.99;
						flag = 0;
					}
				}
			}
			m_pPen->SetDashPattern( dashArray, dashPattern.dashArray.size() );
			if ( GetLineCap() == 2 )
			{
				m_pPen->SetDashOffset( GetDashPhase() / GetLineWidth() + 0.5 );
			}else{
				m_pPen->SetDashOffset( GetDashPhase() / GetLineWidth() );
			}
			delete [] dashArray;
		}
	}else{
		m_pPen->SetDashStyle( Gdiplus::DashStyleSolid );
		m_pPen->SetDashOffset( dashPattern.dashPhase );
	}
}

HE_VOID CHE_GraphicsDrawer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
{
	if ( mode == FillMode_EvenOdd )
	{
		m_path.SetFillMode( Gdiplus::FillModeAlternate );
		m_pathToDraw.SetFillMode( Gdiplus::FillModeAlternate );
	}else{
		m_path.SetFillMode( Gdiplus::FillModeWinding );
		m_pathToDraw.SetFillMode( Gdiplus::FillModeWinding );
	}
}

HE_VOID CHE_GraphicsDrawer::SetFillColor( const HE_DWORD & color )
{
	if ( m_pBrush )
	{
		((Gdiplus::SolidBrush*)m_pBrush)->SetColor( color );
	}
}

HE_VOID CHE_GraphicsDrawer::SetStrokeColor( const HE_DWORD & color )
{
	if ( m_pPen )
	{
		m_pPen->SetColor( color );
	}
}

HE_FLOAT CHE_GraphicsDrawer::GetLineWidth() const
{
	return mLineWidth;
}

GRAPHICS_STATE_LINECAP CHE_GraphicsDrawer::GetLineCap() const
{
	return mLineCap;
}

GRAPHICS_STATE_LINEJOIN CHE_GraphicsDrawer::GetLineJion() const
{
	return mLineJion;
}

HE_FLOAT CHE_GraphicsDrawer::GetDashPhase() const
{
	return mDashPhase;
}