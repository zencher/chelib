#include <windows.h>
#include <GdiPlus.h>

#include "../../include/pdf/che_pdf_graphics.h"
#include "../../include/pdf/che_pdf_renderer.h"
#include "../../include/pdf/che_pdf_contents.h"
#include "../../include/pdf/che_pdf_document.h"

using namespace Gdiplus;
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );


//gAppState:
//0	init
//1 file opened
DWORD	gAppState = 0;

DWORD	gWndWidth = 0;
DWORD	gWndHeight = 0;
DWORD	gDragStartX = 0;
DWORD	gDragStartY = 0;
LONG	gTmpOffsetX = 0;
LONG	gTmpOffsetY = 0;
LONG	gOffsetX = 0;
LONG	gOffsetY = 0;
DWORD	gPageWidth = 0;
DWORD	gPageHeight = 0;
BOOL	gbMouseDown = FALSE;
float	gScale = 1;

IHE_Read *				gpFileRead = NULL;
CHE_PDF_File *			gpFile = NULL;
CHE_PDF_Document *		gpDocument = NULL;
CHE_PDF_PageTree *		gpPageTree = NULL;
CHE_PDF_Page *			gpPage = NULL;
unsigned int			gPageIndex = 0;


HE_DWORD parseTime = 0;
HE_DWORD renderTime = 0;
LARGE_INTEGER gFeq;
LARGE_INTEGER gBegin, gEnd;

HDC	gMemDC = NULL;
HBITMAP gBitmap = NULL;
HGDIOBJ gOldObject = NULL;

class IHE_PDF_GDIplusDraw : public IHE_PDF_GraphicsDraw
{
public:
	IHE_PDF_GDIplusDraw( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight, HE_FLOAT scale );

	virtual ~IHE_PDF_GDIplusDraw();

	virtual inline HE_VOID MoveTo( HE_FLOAT x, HE_FLOAT y );
	virtual inline HE_VOID LineTo( HE_FLOAT x, HE_FLOAT y );
	virtual inline HE_VOID CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 );
	virtual inline HE_VOID ClosePath();
	virtual inline HE_VOID FillPath();
	virtual inline HE_VOID StrokePath();
	virtual inline HE_VOID FillStrokePath();

	virtual	inline HE_VOID SetMatrix( const CHE_PDF_Matrix & matrix );
	virtual inline HE_VOID SetLineWidth( const HE_FLOAT & lineWidth );
	virtual inline HE_VOID SetMiterLimit( const HE_FLOAT & miterLimit );
	virtual inline HE_VOID SetLineCap( const PDF_GSTATE_LINECAP & lineCap );
	virtual inline HE_VOID SetLineJoin( const PDF_GSTATE_LINEJOIN & lineJion );
	virtual inline HE_VOID SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern );
	virtual inline HE_VOID SetFillColor( const HE_DWORD & color );
	virtual inline HE_VOID SetStrokeColor( const HE_DWORD & color );

	HE_FLOAT GetLineWidth() const { return mLineWidth; }
	PDF_GSTATE_LINECAP GetLineCap() const { return mLineCap; }
	PDF_GSTATE_LINEJOIN GetLineJion() const { return mLineJion; }
	HE_FLOAT GetDashPhase() const { return mDashPhase; }

	//custom method
	HDC GetMemDC() { return m_MemDC; };
 	HE_VOID UpdateDC();
	HE_VOID Clear();
	HE_VOID	Resize( HE_DWORD dibWidth, HE_DWORD dibHeight, HE_FLOAT scale );
	HE_DWORD GetWidth() { return m_dwWidth; }
	HE_DWORD GetHeight() { return m_dwHeight; }
// 	HE_VOID SetScale( HE_FLOAT scale )
// 	{
// 		Resize( m_dwWidth/mScale, m_dwHeight/mScale, scale );
// 	}
	//HE_FLOAT GetScale() const { return mScale; }

private:
	HDC m_DC;
	HDC m_MemDC;
	HBITMAP m_Bitmap;
	HGDIOBJ m_OldBitmap;
	HE_DWORD m_dwWidth;
	HE_DWORD m_dwHeight;
	Graphics * m_pGraphics;
	Pen * m_pPen;
	Brush * m_pBrush;
	GraphicsPath m_path;
	GraphicsPath m_pathToDraw;
	HE_FLOAT mCurX;
	HE_FLOAT mCurY;
	HE_FLOAT mScale;

	HE_FLOAT mLineWidth;
	PDF_GSTATE_LINECAP mLineCap;
	PDF_GSTATE_LINEJOIN mLineJion;
	HE_FLOAT mDashPhase;
};

IHE_PDF_GDIplusDraw * gpDraw = NULL;

IHE_PDF_GDIplusDraw::IHE_PDF_GDIplusDraw( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight, HE_FLOAT scale )
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
 	
	m_pGraphics  = new Graphics( m_MemDC );
 	m_pGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
 	m_pGraphics->SetPageUnit( UnitPixel );
	Matrix mtx( 96.0*mScale/72, 0, 0, -96.0*mScale/72, 0, m_dwHeight );
	m_pGraphics->SetTransform( &mtx );
 	m_pPen = new Pen( Color( 255, 0, 0, 0 ), 1 );
 	m_pBrush = new SolidBrush( Color( 255, 0, 0, 0 ) );

	mLineWidth = 1;
	mLineCap = LineCap_Butt;
	mLineJion = LineJoin_Miter;
	mDashPhase = 0;
}

IHE_PDF_GDIplusDraw::~IHE_PDF_GDIplusDraw()
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

HE_VOID IHE_PDF_GDIplusDraw::UpdateDC()
{
// 	DWORD x = 0;
// 	DWORD y = 0;
// 	if ( gWndHeight > m_dwHeight*mScale )
// 	{
// 		y = ( gWndHeight - m_dwHeight*mScale ) / 2;
// 	}
// 	if ( gWndWidth > m_dwWidth*mScale )
// 	{
// 		x = ( gWndWidth - m_dwWidth*mScale ) / 2;
// 	}
// 	BitBlt( m_DC, x + gOffsetX + gTmpOffsetX, y + gOffsetY + gTmpOffsetY, m_dwWidth*mScale,  m_dwHeight*mScale, m_MemDC, 0, 0, SRCCOPY );
}

HE_VOID IHE_PDF_GDIplusDraw::Clear()
{
	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );
}

HE_VOID	IHE_PDF_GDIplusDraw::Resize( HE_DWORD dibWidth, HE_DWORD dibHeight, HE_FLOAT scale )
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

	m_pGraphics  = new Graphics( m_MemDC );
	m_pGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	m_pGraphics->SetPageUnit( UnitPixel );
	Matrix mtx( 96.0*mScale/72, 0, 0, -96.0*mScale/72, 0, m_dwHeight );
	m_pGraphics->SetTransform( &mtx );
	m_pPen = new Pen( Color( 255, 0, 0, 0 ), 1 );
	m_pBrush = new SolidBrush( Color( 255, 0, 0, 0 ) );

	mLineWidth = 1;
	mLineCap = LineCap_Butt;
	mLineJion = LineJoin_Miter;
	mDashPhase = 0;
}

HE_VOID inline IHE_PDF_GDIplusDraw::MoveTo( HE_FLOAT x, HE_FLOAT y )
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
		m_path.Reset();
	}
	mCurX = x;
	mCurY = y;
}

HE_VOID inline IHE_PDF_GDIplusDraw::LineTo( HE_FLOAT x, HE_FLOAT y )
{
	m_path.AddLine( mCurX, mCurY, x, y );
	mCurX = x;
	mCurY = y;
}

HE_VOID inline IHE_PDF_GDIplusDraw::CurveTo( HE_FLOAT x1, HE_FLOAT y1, HE_FLOAT x2, HE_FLOAT y2, HE_FLOAT x3, HE_FLOAT y3 )
{
	m_path.AddBezier( mCurX, mCurY, x1, y1, x2, y2, x3, y3 );
	mCurX = x3;
	mCurY = y3;
}

HE_VOID inline IHE_PDF_GDIplusDraw::ClosePath()
{
 	m_path.CloseFigure();
 	if ( m_path.GetPointCount() > 0 )
 	{
 		m_pathToDraw.AddPath( &m_path, false );
 	}
	m_path.Reset();
}

HE_VOID inline IHE_PDF_GDIplusDraw::FillPath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
	}
	m_path.Reset();
	m_pGraphics->FillPath( m_pBrush, &m_pathToDraw );
	m_pathToDraw.Reset();
}

HE_VOID inline IHE_PDF_GDIplusDraw::StrokePath()
{
	if ( m_path.GetPointCount() > 0 )
	{
		m_pathToDraw.AddPath( &m_path, false );
	}
	m_path.Reset();
	m_pGraphics->DrawPath( m_pPen, &m_pathToDraw );
	m_pathToDraw.Reset();
}

HE_VOID inline IHE_PDF_GDIplusDraw::FillStrokePath()
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

HE_VOID inline IHE_PDF_GDIplusDraw::SetMatrix( const CHE_PDF_Matrix & matrix )
{
	if ( m_pGraphics )
	{
		CHE_PDF_Matrix tmpMatrix( 96.0*mScale/72, 0, 0, -96.0*mScale/72, 0, m_dwHeight );
		CHE_PDF_Matrix t = matrix;
		t.Concat( tmpMatrix );
		Matrix matrixToDev(	t.a, t.b, t.c, t.d, t.e, t.f );
		m_pGraphics->SetTransform( &matrixToDev );
	}
}

HE_VOID	inline IHE_PDF_GDIplusDraw::SetLineWidth( const HE_FLOAT & lineWidth )
{
	if ( m_pPen )
	{
		m_pPen->SetWidth( lineWidth + 0.01 );
	}
	mLineWidth = lineWidth;
}

HE_VOID inline IHE_PDF_GDIplusDraw::SetMiterLimit( const HE_FLOAT & miterLimit )
{
	if ( m_pPen )
	{
		m_pPen->SetMiterLimit( miterLimit );
	}
}

HE_VOID inline IHE_PDF_GDIplusDraw::SetLineCap( const PDF_GSTATE_LINECAP & lineCap )
{
	if ( m_pPen )
	{
		switch ( lineCap )
		{
		case LineCap_Butt:
			m_pPen->SetLineCap( LineCapFlat, LineCapFlat, DashCapFlat );
			break;
		case LineCap_Round:
			m_pPen->SetLineCap( LineCapRound, LineCapRound, DashCapRound );
			break;
		case LineCap_Square:
			m_pPen->SetLineCap( LineCapSquare, LineCapSquare, DashCapFlat );
			break;
		default:
			break;
		}
	}
	mLineCap = lineCap;
}

HE_VOID inline IHE_PDF_GDIplusDraw::SetLineJoin( const PDF_GSTATE_LINEJOIN & lineJion )
{
	if ( m_pPen )
	{
		switch ( lineJion )
		{
		case LineJoin_Miter:
			m_pPen->SetLineJoin( LineJoinMiter );
			break;
		case LineJoin_Round:
			m_pPen->SetLineJoin( LineJoinRound );
			break;
		case LineJoin_Bevel:
			m_pPen->SetLineJoin( LineJoinBevel );
			break;
		default:
			break;
		}
	}
	mLineJion = lineJion;
}

HE_VOID inline IHE_PDF_GDIplusDraw::SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern )
{
	if ( dashPattern.dashArray.size() == 1 )
	{
		REAL * dashArray = new REAL[2];
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
			REAL * dashArray = new REAL[dashPattern.dashArray.size()*2];
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
			REAL * dashArray = new REAL[dashPattern.dashArray.size()];
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
		m_pPen->SetDashStyle( DashStyleSolid );
		m_pPen->SetDashOffset( dashPattern.dashPhase );
	}


}

HE_VOID inline IHE_PDF_GDIplusDraw::SetFillColor( const HE_DWORD & color )
{
	if ( m_pBrush )
	{
		((SolidBrush*)m_pBrush)->SetColor( color );
	}
}

HE_VOID inline IHE_PDF_GDIplusDraw::SetStrokeColor( const HE_DWORD & color )
{
	if ( m_pPen )
	{
		m_pPen->SetColor( color );
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow )
{
	MSG			msg ;
	HWND		hwnd ;
	WNDCLASS	wndclass ;

	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	QueryPerformanceFrequency( &gFeq );
	
	wndclass.style			= 0/*CS_HREDRAW | CS_VREDRAW */;
	wndclass.lpfnWndProc	= WndProc ;
	wndclass.cbClsExtra		= 0 ;
	wndclass.cbWndExtra		= 0 ;
	wndclass.hInstance		= hInstance ;
	wndclass.hIcon			= LoadIcon( NULL, IDI_APPLICATION ) ;
	wndclass.hCursor		= LoadCursor( NULL, IDC_ARROW ) ;
	wndclass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH ) ;
	wndclass.lpszMenuName	= NULL ;
	wndclass.lpszClassName	= TEXT("BaseWndClass") ;
	
	if( !RegisterClass( &wndclass ) )
	{
		MessageBox( NULL, TEXT("Registe Windows Class Fail."), TEXT("Error"), MB_ICONERROR ) ;
		return 0 ;
	}
	
	hwnd = CreateWindow (	TEXT("BaseWndClass"),
							TEXT ("pdfrender"),
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							NULL,
							NULL,
							hInstance,
							NULL ) ;
	
	ShowWindow( hwnd, nCmdShow ) ;
	UpdateWindow( hwnd ) ;
	
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return 0 ;
}


LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
		{
			HDC hDC = GetDC( hwnd );
			if ( hDC )
			{
				gMemDC = CreateCompatibleDC( hDC );
				if ( gMemDC )
				{
					gBitmap = CreateCompatibleBitmap( hDC, gWndWidth, gWndHeight );
					gOldObject = SelectObject( gMemDC, gBitmap );
				}
			}
			ReleaseDC( hwnd, hDC );
			break;
		}
	case WM_SIZE:
		{
			HDC hDC = GetDC( hwnd );
			gWndWidth = LOWORD(lParam);
			gWndHeight = HIWORD(lParam);
			if ( gOldObject )
			{
				SelectObject( gMemDC, gOldObject );
			}
			if ( gBitmap )
			{
				DeleteObject( gBitmap );
			}
			gBitmap = CreateCompatibleBitmap( hDC, gWndWidth, gWndHeight );
			gOldObject = SelectObject( gMemDC, gBitmap );
			ReleaseDC( hwnd, hDC );
			InvalidateRect( hwnd, NULL, FALSE );
			break;
		}
	case WM_PAINT:
		{
			HDC hdc = NULL;
			PAINTSTRUCT ps;
			hdc = BeginPaint( hwnd, &ps );
			RECT rc;
			rc.left = 0;
			rc.top = 0;
			rc.bottom = gWndHeight;
			rc.right = gWndWidth;
			FillRect( gMemDC, &rc, HBRUSH(LTGRAY_BRUSH) );

			if ( gpDraw )
			{
				DWORD x = 0;
				DWORD y = 0;
				if ( gWndHeight > gpDraw->GetHeight() )
				{
					y = ( gWndHeight - gpDraw->GetHeight() ) / 2;
				}
				if ( gWndWidth > gpDraw->GetWidth() )
				{
					x = ( gWndWidth - gpDraw->GetWidth() ) / 2;
				}
				BitBlt( gMemDC, x + gOffsetX + gTmpOffsetX, y + gOffsetY + gTmpOffsetY, gpDraw->GetWidth(),  gpDraw->GetHeight(), gpDraw->GetMemDC(), 0, 0, SRCCOPY );
			}
			BitBlt( hdc, 0, 0, gWndWidth, gWndHeight, gMemDC, 0, 0, SRCCOPY );

			char tmpStr[128];
			sprintf( tmpStr, "parse : %d ms", parseTime );
			TextOut( hdc, 0, 0, tmpStr, strlen( tmpStr ) );

			sprintf( tmpStr, "render : %d ms", renderTime );
			TextOut( hdc, 0, 20, tmpStr, strlen( tmpStr ) );

			EndPaint( hwnd, &ps );
			break ;
		}
	case WM_LBUTTONDOWN:
		{
			if ( gAppState == 1 )
			{
				POINTS pt = MAKEPOINTS( lParam );
				gDragStartX = pt.x;
				gDragStartY = pt.y;
				GetCapture();
				gbMouseDown = TRUE;
			}
			break;
		}
	case WM_MOUSEMOVE:
		{
			if ( gAppState == 1 )
			{
				if ( gbMouseDown )
				{
					POINTS pt = MAKEPOINTS( lParam );
					gTmpOffsetX = pt.x - gDragStartX;
					gTmpOffsetY = pt.y - gDragStartY;
// 					if ( gWndWidth >= gPageWidth )
// 					{
// 						gTmpOffsetX = 0;
// 					}else{
// 						if ( gTmpOffsetX + gOffsetX > gPageWidth )
// 						{
// 							gTmpOffsetX = gPageWidth - gOffsetX;
// 						}
// 					}
// 					if ( gWndHeight >= gPageHeight )
// 					{
// 						gTmpOffsetY = 0;
// 					}else{
// 						if ( gTmpOffsetY + gOffsetY > gPageHeight )
// 						{
// 							gTmpOffsetY = gPageHeight - gOffsetY;
// 						}
// 					}
					if ( gTmpOffsetX > 0 || gTmpOffsetY > 0 )
					{
						InvalidateRect( hwnd, NULL, FALSE );
					}
				}
			}
			break;
		}
	case WM_LBUTTONUP:
		{
			if ( gAppState == 1 )
			{
				gOffsetX += gTmpOffsetX;
				gOffsetY += gTmpOffsetY;
				gTmpOffsetX = 0;
				gTmpOffsetY = 0;
				ReleaseCapture();
				gbMouseDown = FALSE;
			}
			break;
		}
	case WM_CHAR:
		{
			if ( wParam == 'o' || wParam == 'O' )
			{
				static char fileName[1024], fileTitleName[1024];
				static OPENFILENAME ofn;
				ofn.lStructSize		= sizeof ( OPENFILENAME );
				ofn.hwndOwner		= hwnd;
				ofn.lpstrFilter		= "PDF Files(*.pdf)\0*.pdf\0\0";
				ofn.lpstrCustomFilter = NULL;
				ofn.nMaxCustFilter	= 0;
				ofn.nFilterIndex	= 1;
				ofn.lpstrFile		= fileName;
				ofn.nMaxFile		= MAX_PATH;
				ofn.lpstrFileTitle	= fileTitleName;
				ofn.nMaxFileTitle	= MAX_PATH;
				ofn.Flags			= OFN_NOCHANGEDIR ;
				ofn.nFileOffset		= 16 ;
				ofn.nFileExtension	= 0 ;
				ofn.lCustData		= NULL ;
				ofn.lpfnHook		= NULL ;
				ofn.lpTemplateName	= NULL ;

				if ( GetOpenFileName( &ofn ) )
				{
					if ( gAppState == 1 )
					{
						gAppState = 0;
					}

					gpFileRead = HE_CreateFileRead( fileName );
					gpFile = GetDefaultAllocator()->New<CHE_PDF_File>( GetDefaultAllocator() );
					gpFile->Open( gpFileRead );
					gpDocument = CHE_PDF_Document::CreateDocument( gpFile );
					gpPageTree = gpDocument->GetPageTree();
					gpPage = gpPageTree->GetPage( 0 );
					gPageIndex = 0;
					if ( gpPage )
					{
						CHE_PDF_Rect rect = gpPage->GetMediaBox();
						gPageWidth = rect.width * 96 / 72;
						gPageHeight =  rect.height * 96 / 72;
						gpDraw = new IHE_PDF_GDIplusDraw( GetDC(hwnd), gPageWidth, gPageHeight, gScale );
						CHE_PDF_ContentObjectList contentObjList;
						QueryPerformanceCounter( &gBegin );
						GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
						QueryPerformanceCounter( &gEnd );
						parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

						QueryPerformanceCounter( &gBegin );
						CHE_PDF_Renderer::Render( contentObjList, gpDraw );
						QueryPerformanceCounter( &gEnd );
						renderTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );
					}
					gAppState = 1;
					InvalidateRect( hwnd, NULL, FALSE );
					break;
				}
			}else if ( wParam == 'c' || wParam == 'C' )
			{
			}else if ( wParam == '.' )
			{
				if ( gAppState == 1 )
				{
					if ( gpPage )
					{
						gpPage->GetAllocator()->Delete( gpPage );
					}
					gpPage = gpPageTree->GetPage( ++gPageIndex );
					if ( gpPage )
					{
						CHE_PDF_Rect rect = gpPage->GetMediaBox();
						gPageWidth = rect.width * 96 / 72;
						gPageHeight =  rect.height * 96 / 72;
						gpDraw->Resize( gPageWidth, gPageHeight, gScale );
						CHE_PDF_ContentObjectList contentObjList;
						QueryPerformanceCounter( &gBegin );
						GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
						QueryPerformanceCounter( &gEnd );
						parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

						QueryPerformanceCounter( &gBegin );
						CHE_PDF_Renderer::Render( contentObjList, gpDraw );
						QueryPerformanceCounter( &gEnd );
						renderTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );
					}
					InvalidateRect( hwnd, NULL, FALSE );
				}
			}else if ( wParam == ',' )
			{
				if ( gAppState == 1 )
				{
					if ( gpPage )
					{
						gpPage->GetAllocator()->Delete( gpPage );
					}
					gpPage = gpPageTree->GetPage( --gPageIndex );
					if ( gpPage )
					{
						CHE_PDF_Rect rect = gpPage->GetMediaBox();
						gPageWidth = rect.width * 96 / 72;
						gPageHeight =  rect.height * 96 / 72;
						gpDraw->Resize( gPageWidth, gPageHeight, gScale );
						CHE_PDF_ContentObjectList contentObjList;
						QueryPerformanceCounter( &gBegin );
						GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
						QueryPerformanceCounter( &gEnd );
						parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

						QueryPerformanceCounter( &gBegin );
						CHE_PDF_Renderer::Render( contentObjList, gpDraw );
						QueryPerformanceCounter( &gEnd );
						renderTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );
					}
					InvalidateRect( hwnd, NULL, FALSE );
				}
			}else if ( wParam == 'z' || wParam == 'Z' )
			{
				gScale+=0.5;
				gpDraw->Resize( gPageWidth, gPageHeight, gScale );
				if ( gpPage )
				{
					gpPage->GetAllocator()->Delete( gpPage );
				}
				gpPage = gpPageTree->GetPage( gPageIndex );
				if ( gpPage )
				{
					CHE_PDF_ContentObjectList contentObjList;
					QueryPerformanceCounter( &gBegin );
					GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
					QueryPerformanceCounter( &gEnd );
					parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

					QueryPerformanceCounter( &gBegin );
					CHE_PDF_Renderer::Render( contentObjList, gpDraw );
					QueryPerformanceCounter( &gEnd );
					renderTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );
				}
				InvalidateRect( hwnd, NULL, TRUE );
			}else if ( wParam == 'x' || wParam == 'X' )
			{
				if ( gScale - 0.5 >= 0 )
				{
					gScale -= 0.5;
				}
				gpDraw->Resize( gPageWidth, gPageHeight, gScale );
				if ( gpPage )
				{
					gpPage->GetAllocator()->Delete( gpPage );
				}
				gpPage = gpPageTree->GetPage( gPageIndex );
				if ( gpPage )
				{
					CHE_PDF_ContentObjectList contentObjList;
					QueryPerformanceCounter( &gBegin );
					GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
					QueryPerformanceCounter( &gEnd );
					parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

					QueryPerformanceCounter( &gBegin );
					CHE_PDF_Renderer::Render( contentObjList, gpDraw );
					QueryPerformanceCounter( &gEnd );
					renderTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );
				}
				InvalidateRect( hwnd, NULL, FALSE );
			}
			
			break;
		}
	case WM_DESTROY:
		PostQuitMessage( 0 ) ;
		break ;
		
	default :
		return DefWindowProc( hwnd, message, wParam, lParam ) ;
	}
	return 0 ;
}