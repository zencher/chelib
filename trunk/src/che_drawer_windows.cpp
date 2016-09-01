#include "../include/che_drawer_windows.h"
#include "../Include/che_bitmap.h"

CHE_GraphicsDrawer::CHE_GraphicsDrawer( HDC hDC, size_t dibWidth, size_t dibHeight )
{
	m_DC = hDC;
	m_dwWidth = dibWidth;
	m_dwHeight = dibHeight;
	m_MemDC = CreateCompatibleDC( m_DC );
	m_Bitmap = CreateCompatibleBitmap( m_DC, m_dwWidth, m_dwHeight );
	m_OldBitmap = SelectObject( m_MemDC, m_Bitmap );

	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );

	mStrokeAlpha = 1;
	mFillAlpha = 1;

	mCurX = 0;
	mCurY = 0;
	mBeginX = 0;
	mBeginY = 0;

	mLineWidth = 1;
	mLineCap = LineCap_Butt;
	mLineJion = LineJoin_Miter;
	mDashPhase = 0;

	m_pGraphics  = new Gdiplus::Graphics( m_MemDC );
	m_pGraphics->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
	m_pGraphics->SetPageUnit( Gdiplus::UnitPixel );
	m_pPen = new Gdiplus::Pen( Gdiplus::Color( mStrokeAlpha * 255, 0, 0, 0 ), 1 );
	m_pBrush = new Gdiplus::SolidBrush( Gdiplus::Color( 255, 0, 0, 0 ) );
}

CHE_GraphicsDrawer::~CHE_GraphicsDrawer()
{
	if ( m_pBrush )
	{
		delete m_pBrush;
		m_pBrush = nullptr;
	}
	if ( m_pPen )
	{
		delete m_pPen;
		m_pPen = nullptr;
	}
	if ( m_MemDC )
	{
		if ( m_OldBitmap )
		{
			SelectObject( m_MemDC, m_OldBitmap );
			m_OldBitmap = nullptr;
		}
		if ( m_Bitmap )
		{
			DeleteObject( m_Bitmap );
			m_Bitmap = nullptr;
		}
		DeleteDC( m_MemDC );
		m_MemDC = nullptr;
	}
	if ( m_pGraphics )
	{
		delete m_pGraphics;
		m_pGraphics = nullptr;
	}
}

void	CHE_GraphicsDrawer::Resize( size_t dibWidth, size_t dibHeight )
{
	m_dwWidth = dibWidth;
	m_dwHeight = dibHeight;

	if ( m_MemDC )
	{
		if ( m_OldBitmap )
		{
			SelectObject( m_MemDC, m_OldBitmap );
			m_OldBitmap = nullptr;
		}
		DeleteDC( m_MemDC );
		if ( m_Bitmap )
		{
			DeleteObject( m_Bitmap );
			m_Bitmap = nullptr;
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
		m_pGraphics = nullptr;
	}

	m_pGraphics  = new Gdiplus::Graphics( m_MemDC );
	m_pGraphics->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
	m_pGraphics->SetPageUnit( Gdiplus::UnitPixel );
}

size_t CHE_GraphicsDrawer::GetWidth() const
{
	return m_dwWidth;
}

size_t CHE_GraphicsDrawer::GetHeight() const
{
	return m_dwHeight;
}

HDC CHE_GraphicsDrawer::GetMemDC() const
{
	return m_MemDC;
}

bool CHE_GraphicsDrawer::GetBitmap( CHE_Bitmap & bitmap )
{
	if ( m_Bitmap )
	{
		BITMAP info;
		GetObjectW( m_Bitmap, sizeof(BITMAP), &info );
		bitmap.Create( m_dwWidth, m_dwHeight, (HE_BITMAP_DEPTH)info.bmBitsPixel, BITMAP_DIRECTION_DOWN );
		BYTE * pBytes = (PBYTE)bitmap.GetBuffer();
		GetBitmapBits( m_Bitmap, info.bmWidthBytes * info.bmHeight, pBytes );
		return TRUE;
	}
	return false;
}

void	CHE_GraphicsDrawer::Clear()
{
	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );
}

void CHE_GraphicsDrawer::MoveTo( FLOAT x, FLOAT y )
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	mBeginX = mCurX = x;
	mBeginY = mCurY = y;
}

void CHE_GraphicsDrawer::LineTo( FLOAT x, FLOAT y )
{
	m_path.AddLine( mCurX, mCurY, x, y );
	mCurX = x;
	mCurY = y;
}

void CHE_GraphicsDrawer::CurveTo( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3 )
{
	m_path.AddBezier( mCurX, mCurY, x1, y1, x2, y2, x3, y3 );
	mCurX = x3;
	mCurY = y3;
}

void CHE_GraphicsDrawer::ClosePath()
{
	m_path.CloseFigure();
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
		mCurX = mBeginX;
		mCurY = mBeginY;
	}
}

void CHE_GraphicsDrawer::FillPath()
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

void CHE_GraphicsDrawer::StrokePath()
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

void CHE_GraphicsDrawer::FillStrokePath()
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

void CHE_GraphicsDrawer::ClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->SetClip( &m_pathToDraw, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}
}

void	CHE_GraphicsDrawer::FillClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
		m_pGraphics->SetClip( &m_pathToDraw, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}
}

void	CHE_GraphicsDrawer::StrokeClipPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	if ( m_pathToDraw.GetPointCount() > 0 )
	{
		m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
		m_pGraphics->SetClip( &m_pathToDraw, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}
}

void	CHE_GraphicsDrawer::FillStrokeClipPath()
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
		m_pGraphics->SetClip( &m_pathToDraw, Gdiplus::CombineModeIntersect );
		m_pathToDraw.Reset();
	}	
}

void	CHE_GraphicsDrawer::ResetClip()
{
	m_pGraphics->ResetClip();
}

void CHE_GraphicsDrawer::SetMatrix( const CHE_Matrix & matrix )
{
	if ( m_pGraphics )
	{
		CHE_Matrix t = matrix;
		t.Concat( mExtMatrix );
		Gdiplus::Matrix matrixToDev( t.a, t.b, t.c, t.d, t.e, t.f );
		m_pGraphics->SetTransform( &matrixToDev );
	}
}

void CHE_GraphicsDrawer::SetExtMatrix( const CHE_Matrix & matrix )
{
	mExtMatrix = matrix;
}

void CHE_GraphicsDrawer::SetLineWidth( const FLOAT & lineWidth )
{
	if ( m_pPen )
	{
		m_pPen->SetWidth( lineWidth );
	}
	mLineWidth = lineWidth;
}

void CHE_GraphicsDrawer::SetMiterLimit( const FLOAT & miterLimit )
{
	if ( m_pPen )
	{
		m_pPen->SetMiterLimit( miterLimit );
	}
}

void CHE_GraphicsDrawer::SetLineCap( const GRAPHICS_STATE_LINECAP & lineCap )
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

void CHE_GraphicsDrawer::SetLineJoin( const GRAPHICS_STATE_LINEJOIN & lineJion )
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

void CHE_GraphicsDrawer::SetLineDash( const GRAPHICS_STATE_DASHPATTERN & dashPattern )
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

void CHE_GraphicsDrawer::SetFillMode( GRAPHICS_STATE_FILLMODE mode )
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

void CHE_GraphicsDrawer::SetFillAlpha( FLOAT & alpha )
{
	mFillAlpha = alpha;
}

void	CHE_GraphicsDrawer::SetStrokeAlpha( FLOAT & alpha )
{
	mStrokeAlpha = alpha;
}

void CHE_GraphicsDrawer::SetFillColor( const size_t & color )
{
	if ( m_pBrush )
	{
		((Gdiplus::SolidBrush*)m_pBrush)->SetColor( color );
	}
}

void CHE_GraphicsDrawer::SetStrokeColor( const size_t & color )
{
	if ( m_pPen )
	{
		m_pPen->SetColor( color );
	}
}

FLOAT CHE_GraphicsDrawer::GetLineWidth() const
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

FLOAT CHE_GraphicsDrawer::GetDashPhase() const
{
	return mDashPhase;
}

// void CHE_GraphicsDrawer::DrawBitmap( size_t width, size_t height, BYTE bpc, PBYTE pData, size_t size )
// {
// 	BITMAPINFO bitmapInfo;
// 	bitmapInfo.bmiHeader.biSize = 40;
// 	bitmapInfo.bmiHeader.biWidth = (int32)width;
// 	size_t lHeight = height;
// 	bitmapInfo.bmiHeader.biHeight = (int32)(-lHeight);
// 	bitmapInfo.bmiHeader.biPlanes = 1;
// 	bitmapInfo.bmiHeader.biBitCount = bpc;
// 	bitmapInfo.bmiHeader.biCompression = 0;
// 	bitmapInfo.bmiHeader.biSizeImage = (uint32)( size );
// 	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
// 	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
// 	bitmapInfo.bmiHeader.biClrUsed = 0;
// 	bitmapInfo.bmiHeader.biClrImportant = 0;
// 
// 	Gdiplus::Bitmap bitmap( &bitmapInfo, pData );
// 	Gdiplus::CachedBitmap cacheBitmap( &bitmap, m_pGraphics );
// 
// 	m_pGraphics->DrawCachedBitmap( cacheBitmap, 0, 1, 1, -1 );
// }

void	CHE_GraphicsDrawer::DrawImage( ImageTypeSupport imgType, PBYTE data, size_t size )
{
	if ( m_pGraphics )
	{
		HGLOBAL global = ::GlobalAlloc( GMEM_MOVEABLE, size );
		if( global )
		{
			void * dest = ::GlobalLock( global );
			if( dest )
			{
				memcpy( dest, data, size );
				::GlobalUnlock( global );

				IStream * stream = nullptr;
				if( ::CreateStreamOnHGlobal( global, TRUE, &stream ) != S_OK )
				{
					::GlobalFree( global );
					return;
				}

				Gdiplus::Image *image = Gdiplus::Image::FromStream( stream );
				if ( image )
				{
					m_pGraphics->DrawImage( image, 0, 1, 1, -1 );

					stream->Release();
				}else{
					::GlobalFree( global );
				}
			}
		}		
	}
}

void	CHE_GraphicsDrawer::DrawBitmap( CHE_Bitmap * pBitmap )
{
	if ( pBitmap == nullptr )
	{
		return;
	}
	if ( m_pGraphics )
	{
		Gdiplus::ColorMatrix colorMatrix = {	1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
												0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
												0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
												0.0f, 0.0f, 0.0f, mFillAlpha, 0.0f,
												0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		Gdiplus::ImageAttributes imageAtt;
		imageAtt.SetColorMatrix( &colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap );

		switch ( pBitmap->Depth() )
		{
		case BITMAP_DEPTH_32BPP:
			{
				Gdiplus::Bitmap bitmap( pBitmap->Width(), pBitmap->Height(), pBitmap->Pitch(), PixelFormat32bppARGB, (BYTE*)pBitmap->GetBuffer() );
				m_pGraphics->DrawImage( &bitmap, Gdiplus::Rect( 0.0f, 1.0f, 1.0f, -1.0f), 0, 0, pBitmap->Width(), pBitmap->Height(), Gdiplus::UnitPixel, &imageAtt  );
				break;
			}
		case BITMAP_DEPTH_24BPP:
			{
				Gdiplus::Bitmap bitmap( pBitmap->Width(), pBitmap->Height(), pBitmap->Pitch(), PixelFormat24bppRGB, (BYTE*)pBitmap->GetBuffer() );
				m_pGraphics->DrawImage( &bitmap, Gdiplus::Rect( 0.0f, 1.0f, 1.0f, -1.0f), 0, 0, pBitmap->Width(), pBitmap->Height(), Gdiplus::UnitPixel, &imageAtt  );
				break;
			}
		case BITMAP_DEPTH_1BPP:
			{
				Gdiplus::Bitmap bitmap( pBitmap->Width(), pBitmap->Height(), pBitmap->Pitch(), PixelFormat1bppIndexed, (BYTE*)pBitmap->GetBuffer() );
				m_pGraphics->DrawImage( &bitmap, Gdiplus::Rect( 0.0f, 1.0f, 1.0f, -1.0f), 0, 0, pBitmap->Width(), pBitmap->Height(), Gdiplus::UnitPixel, &imageAtt  );
				break;
			}
		default:
			break;
		}
	}
}