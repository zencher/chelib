#include <windows.h>
#include <GdiPlus.h>

#include "../../include/pdf/che_pdf_graphics.h"
#include "../../include/pdf/che_pdf_renderer.h"
#include "../../include/pdf/che_pdf_contents.h"
#include "../../include/pdf/che_pdf_document.h"

using namespace Gdiplus;
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

IHE_Read *					gpFileRead = NULL;
CHE_PDF_File *				gpFile = NULL;
CHE_PDF_Document *			gpDocument = NULL;
CHE_PDF_PageTree *			gpPageTree = NULL;
CHE_PDF_Page *				gpPage = NULL;
unsigned int				gPageIndex = 0;

DWORD						gWndWidth = 0;
DWORD						gWndHeight = 0;

DWORD						gDragStartX = 0;
DWORD						gDragStartY = 0;

LONG						gTmpOffsetX = 0;
LONG						gTmpOffsetY = 0;
LONG						gOffsetX = 0;
LONG						gOffsetY = 0;

DWORD						gPageWidth = 0;
DWORD						gPageHeight = 0;

BOOL						gbMouseDown = FALSE;

class IHE_PDF_GDIplusDraw : public IHE_PDF_GraphicsDraw
{
public:
	IHE_PDF_GDIplusDraw( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight );

	virtual ~IHE_PDF_GDIplusDraw() {};

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

	//custom method
 	HE_VOID UpdateDC();
	HE_VOID Clear();

private:
	HDC m_DC;
	HDC m_MemDC;
	HBITMAP m_Bitmap;
	HE_DWORD m_dwWidth;
	HE_DWORD m_dwHeight;
	Graphics * m_pGraphics;
	Pen * m_pPen;
	Brush * m_pBrush;
	GraphicsPath m_path;
	GraphicsPath m_pathToDraw;
	HE_FLOAT mCurX;
	HE_FLOAT mCurY;
};

IHE_PDF_GDIplusDraw * gpDraw = NULL;

IHE_PDF_GDIplusDraw::IHE_PDF_GDIplusDraw( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight )
{
	m_DC = hDC;
	m_dwWidth = dibWidth;
	m_dwHeight = dibHeight;
	m_MemDC = CreateCompatibleDC( hDC );
	m_Bitmap = CreateCompatibleBitmap( m_DC, m_dwWidth , m_dwHeight );
 	SelectObject( m_MemDC, m_Bitmap );

	gWndHeight = dibHeight;
	gWndWidth = dibWidth;

	RECT rt;
	rt.top = 0;
	rt.left = 0;
	rt.right = m_dwWidth;
	rt.bottom = m_dwHeight;
 	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );
 	
	m_pGraphics  = new Graphics( m_MemDC );
 	m_pGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
 	m_pGraphics->SetPageUnit( UnitPixel );
	Matrix mtx( 96.0/72, 0, 0, -96.0/72, 0, m_dwHeight );
	m_pGraphics->SetTransform( &mtx );
 	m_pPen = new Pen( Color( 255, 0, 0, 0 ), 1 );
 	m_pBrush = new SolidBrush( Color( 255, 0, 0, 0 ) );
}

HE_VOID IHE_PDF_GDIplusDraw::UpdateDC()
{
	DWORD x = 0;
	DWORD y = 0;
	if ( gWndHeight > m_dwHeight )
	{
		y = ( gWndHeight - m_dwHeight ) / 2;
	}
	if ( gWndWidth > m_dwWidth )
	{
		x = ( gWndWidth - m_dwWidth ) / 2;
	}
	BitBlt( m_DC, x + gOffsetX + gTmpOffsetX, y + gOffsetY + gTmpOffsetY, m_dwWidth,  m_dwHeight, m_MemDC, 0, 0, SRCCOPY );
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
		CHE_PDF_Matrix tmpMatrix( 96.0/72, 0, 0, -96.0/72, 0, m_dwHeight );
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
		m_pPen->SetWidth( lineWidth );
	}
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
}

HE_VOID inline IHE_PDF_GDIplusDraw::SetLineDash( const PDF_GSTATE_DASHPATTERN & dashPattern )
{
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
	
	wndclass.style			= CS_HREDRAW | CS_VREDRAW ;
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
	HDC		hdc ;
	PAINTSTRUCT	ps ;
	
	switch( message )
	{
	case WM_CREATE:
		{
			IHE_Read * gpFileRead = HE_CreateFileRead( "e:\\pdftestfiles\\穷爸爸富爸爸.pdf" );
			gpFile = GetDefaultAllocator()->New<CHE_PDF_File>( GetDefaultAllocator() );
			gpFile->Open( gpFileRead );
			gpDocument = CHE_PDF_Document::CreateDocument( gpFile );
			gpPageTree = gpDocument->GetPageTree();
			gpPage = gpPageTree->GetPage( 0 );
			gPageIndex = 0;

			if ( gpPage )
			{
				CHE_PDF_Rect rect = gpPage->GetMediaBox();
				gpDraw = new IHE_PDF_GDIplusDraw( GetWindowDC(hwnd), rect.width * 96 / 72, rect.height * 96 / 72 );
				CHE_PDF_ContentObjectList contentObjList;
				GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
				CHE_PDF_Renderer::Render( contentObjList, gpDraw );
			}

			break;
		}
	case WM_SIZE:
		{
			gWndWidth = LOWORD(lParam);
			gWndHeight = HIWORD(lParam);
			break;
		}
	case WM_PAINT:
		{
			hdc = BeginPaint( hwnd, &ps );
			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.bottom = gWndHeight;
			rect.right = gWndWidth;
			FillRect( hdc, &rect, HBRUSH(BLACK_BRUSH) );
			gpDraw->UpdateDC();
			EndPaint( hwnd, &ps );
			break ;
		}
	case WM_LBUTTONDOWN:
		{
			POINTS pt = MAKEPOINTS( lParam );
			gDragStartX = pt.x;
			gDragStartY = pt.y;
			GetCapture();
			gbMouseDown = TRUE;
			break;
		}
	case WM_MOUSEMOVE:
		{
			if ( gbMouseDown )
			{
				POINTS pt = MAKEPOINTS( lParam );
				gTmpOffsetX = pt.x - gDragStartX;
				gTmpOffsetY = pt.y - gDragStartY;
				if ( gWndWidth >= gPageWidth )
				{
					gTmpOffsetX = 0;
				}else{
					if ( gTmpOffsetX + gOffsetX > gPageWidth )
					{
						gTmpOffsetX = gPageWidth - gOffsetX;
					}
				}
				if ( gWndHeight >= gPageHeight )
				{
					gTmpOffsetY = 0;
				}else{
					if ( gTmpOffsetY + gOffsetY > gPageHeight )
					{
						gTmpOffsetY = gPageHeight - gOffsetY;
					}
				}
				if ( gTmpOffsetX > 0 || gTmpOffsetY > 0 )
				{
					InvalidateRect( hwnd, NULL, FALSE );
				}
			}
			break;
		}
	case WM_LBUTTONUP:
		{
			gOffsetX += gTmpOffsetX;
			gOffsetY += gTmpOffsetY;
			gTmpOffsetX = 0;
			gTmpOffsetY = 0;
			ReleaseCapture();
			gbMouseDown = FALSE;
			break;
		}
	case WM_KEYDOWN:
		{
			if ( wParam == 'o' )
			{
				int x = 0;
			}
			if ( gpPage )
			{
				gpPage->GetAllocator()->Delete( gpPage );
			}

			gpPage = gpPageTree->GetPage( ++gPageIndex );
			if ( gpPage )
			{
				gpDraw->Clear();
				CHE_PDF_ContentObjectList contentObjList;
				GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
				CHE_PDF_Renderer::Render( contentObjList, gpDraw );
			}
			InvalidateRect( hwnd, NULL, TRUE );
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