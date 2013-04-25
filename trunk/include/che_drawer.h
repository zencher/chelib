#ifndef _CHE_DRAWER_H_
#define _CHE_DRAWER_H_

#include "che_define.h"
#include "che_graphics.h"

//windows implementation
#include <Windows.h>
#include <GdiPlus.h>

class CHE_GraphicsDrawer
{
public:
	CHE_GraphicsDrawer( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight, HE_FLOAT scale )
	{
		m_DC = hDC;
		mScale = scale;
		m_dwWidth = dibWidth*scale;
		m_dwHeight = dibHeight*scale;
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
		Gdiplus::Matrix mtx( 96.0*mScale/72, 0, 0, -96.0*mScale/72, 0, m_dwHeight );
		m_pGraphics->SetTransform( &mtx );
		m_pPen = new Gdiplus::Pen( Gdiplus::Color( 255, 0, 0, 0 ), 1 );
		m_pBrush = new Gdiplus::SolidBrush( Gdiplus::Color( 255, 0, 0, 0 ) );

		mLineWidth = 1;
		mLineCap = LineCap_Butt;
		mLineJion = LineJoin_Miter;
		mDashPhase = 0;
	}

	~CHE_GraphicsDrawer()
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

	inline HE_VOID MoveTo( HE_FLOAT x, HE_FLOAT y )
	{
		if ( m_path.GetPointCount() > 0 )
		{
			m_pathToDraw.AddPath( &m_path, false );
			m_path.Reset();
		}
		mCurX = x;
		mCurY = y;
	}

	inline HE_VOID LineTo( HE_FLOAT x, HE_FLOAT y )
	{
		m_path.AddLine( mCurX, mCurY, x, y );
		mCurX = x;
		mCurY = y;
	}

	inline HE_VOID CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
	{
		m_path.AddBezier( mCurX, mCurY, x1, y1, x2, y2, x3, y3 );
		mCurX = x3;
		mCurY = y3;
	}

	inline HE_VOID ClosePath()
	{
		m_path.CloseFigure();
		if ( m_path.GetPointCount() > 0 )
		{
			m_pathToDraw.AddPath( &m_path, false );
		}
		m_path.Reset();
	}

	inline HE_VOID FillPath()
	{
		if ( m_path.GetPointCount() > 0 )
		{
			m_pathToDraw.AddPath( &m_path, false );
		}
		m_path.Reset();
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		m_pathToDraw.Reset();
	}

	inline HE_VOID StrokePath()
	{
		if ( m_path.GetPointCount() > 0 )
		{
			m_pathToDraw.AddPath( &m_path, false );
		}
		m_path.Reset();
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		m_pathToDraw.Reset();
	}

	inline HE_VOID FillStrokePath()
	{
		m_path.CloseFigure();
		if ( m_path.GetPointCount() > 0 )
		{
			m_pathToDraw.AddPath( &m_path, false );
		}
		m_path.Reset();
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		m_pathToDraw.Reset();
	}

	inline HE_VOID SetMatrix( const CHE_Matrix & matrix )
	{
		if ( m_pGraphics )
		{
			CHE_Matrix tmpMatrix( 96.0*mScale/72, 0, 0, -96.0*mScale/72, 0, m_dwHeight );
			CHE_Matrix t = matrix;
			t.Concat( tmpMatrix );
			Gdiplus::Matrix matrixToDev(	t.a, t.b, t.c, t.d, t.e, t.f );
			m_pGraphics->SetTransform( &matrixToDev );
		}
	}

	inline HE_VOID SetLineWidth( const HE_FLOAT & lineWidth )
	{
		if ( m_pPen )
		{
			m_pPen->SetWidth( lineWidth + 0.01 );
		}
		mLineWidth = lineWidth;
	}

	inline HE_VOID SetMiterLimit( const HE_FLOAT & miterLimit )
	{
		if ( m_pPen )
		{
			m_pPen->SetMiterLimit( miterLimit );
		}
	}

	inline HE_VOID SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
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

	inline HE_VOID SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
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

	inline HE_VOID SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
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

	inline HE_VOID SetFillColor( const HE_DWORD & color )
	{
		if ( m_pBrush )
		{
			((Gdiplus::SolidBrush*)m_pBrush)->SetColor( color );
		}
	}

	inline HE_VOID SetStrokeColor( const HE_DWORD & color )
	{
		if ( m_pPen )
		{
			m_pPen->SetColor( color );
		}
	}

	inline HE_FLOAT GetLineWidth() const { return mLineWidth; }
	inline GRAPHICS_STATE_LINECAP GetLineCap() const { return mLineCap; }
	inline GRAPHICS_STATE_LINEJOIN GetLineJion() const { return mLineJion; }
	inline HE_FLOAT GetDashPhase() const { return mDashPhase; }

	inline HDC		GetMemDC() { return m_MemDC; };
	inline HE_VOID	Clear()
	{
		RECT rt;
		rt.top = 0;
		rt.left = 0;
		rt.right = m_dwWidth;
		rt.bottom = m_dwHeight;
		FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );
	}

	inline HE_VOID	Resize( HE_DWORD dibWidth, HE_DWORD dibHeight, HE_FLOAT scale )
	{
		mScale = scale;
		m_dwWidth = dibWidth*scale;
		m_dwHeight = dibHeight*scale;

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
		Gdiplus::Matrix mtx( 96.0*mScale/72, 0, 0, -96.0*mScale/72, 0, m_dwHeight );
		m_pGraphics->SetTransform( &mtx );
		m_pPen = new Gdiplus::Pen( Gdiplus::Color( 255, 0, 0, 0 ), 1 );
		m_pBrush = new Gdiplus::SolidBrush( Gdiplus::Color( 255, 0, 0, 0 ) );

		mLineWidth = 1;
		mLineCap = LineCap_Butt;
		mLineJion = LineJoin_Miter;
		mDashPhase = 0;
	}

	inline HE_DWORD GetWidth() { return m_dwWidth; }
	inline HE_DWORD GetHeight() { return m_dwHeight; }

private:
	HDC							m_DC;
	HDC							m_MemDC;
	HBITMAP						m_Bitmap;
	HGDIOBJ						m_OldBitmap;
	HE_DWORD					m_dwWidth;
	HE_DWORD					m_dwHeight;
	Gdiplus::Graphics *			m_pGraphics;
	Gdiplus::Pen *				m_pPen;
	Gdiplus::Brush *			m_pBrush;
	Gdiplus::GraphicsPath		m_path;
	Gdiplus::GraphicsPath		m_pathToDraw;
	HE_FLOAT					mCurX;
	HE_FLOAT					mCurY;
	HE_FLOAT					mScale;
	HE_FLOAT					mLineWidth;
	GRAPHICS_STATE_LINECAP		mLineCap;
	GRAPHICS_STATE_LINEJOIN		mLineJion;
	HE_FLOAT					mDashPhase;
};


#endif