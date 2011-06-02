#include <windows.h>
#include <GdiPlus.h>
#include "../../include/pdf/che_pdf_renderer.h"

using namespace Gdiplus;
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

HDC gMemDc = NULL;
Graphics * gpGraphics = NULL;
Pen * gpPen = NULL;
Brush * gpBrush = NULL;

class IHE_PDF_GDIplusDraw : public IHE_PDF_DrawGraphics
{
public:
	IHE_PDF_GDIplusDraw( Graphics * pGraphics, CHE_Allocator * pAllocator = NULL )
		: IHE_PDF_DrawGraphics( pAllocator )
	{ m_pGrahpics = pGraphics; }

	virtual ~IHE_PDF_GDIplusDraw() {};

	virtual HE_VOID SetMatrixToDevice( HE_FLOAT a, HE_FLOAT b, HE_FLOAT c, HE_FLOAT d, HE_FLOAT e, HE_FLOAT f )
	{
		m_a = a; m_b = b; m_c = c; m_d = d; m_e = e; m_f = f;
	}

	virtual HE_FLOAT GetX( HE_FLOAT x, HE_FLOAT y )
	{
		return m_a * x + m_c * y + m_e;
	}
	virtual HE_FLOAT GetY( HE_FLOAT x, HE_FLOAT y )
	{
		return m_b * x + m_d * y + m_f;
	}

	virtual HE_VOID DrawPath( CHE_PDF_PathObject * pPath );

	virtual HE_VOID DrawText( CHE_PDF_TextObject * pText );

private:
	 Graphics * m_pGrahpics;

	 HE_FLOAT m_a;
	 HE_FLOAT m_b;
	 HE_FLOAT m_c;
	 HE_FLOAT m_d;
	 HE_FLOAT m_e;
	 HE_FLOAT m_f;
};

HE_VOID IHE_PDF_GDIplusDraw::DrawPath( CHE_PDF_PathObject * pPath )
{
	if ( pPath == NULL )
	{
		return;
	}
	CHE_PDF_GraphState * pGraphState = pPath->GetGraphState();




	gpPen->SetWidth( pGraphState->m_LineWidth + 0.001 );

	if ( pGraphState->m_LineCap == 0 )
	{
		gpPen->SetLineCap( LineCapFlat, LineCapFlat, DashCapFlat );
	}else if ( pGraphState->m_LineCap == 1 )
	{
		gpPen->SetLineCap( LineCapRound, LineCapRound, DashCapRound );
	}else if ( pGraphState->m_LineCap == 2 )
	{
		gpPen->SetLineCap( LineCapSquare, LineCapSquare, DashCapFlat );
	}

	if ( pGraphState->m_LineJoin == 0 )
	{
		gpPen->SetLineJoin( LineJoinMiter );
	}else if ( pGraphState->m_LineJoin == 1 )
	{
		gpPen->SetLineJoin( LineJoinRound );
	}else if ( pGraphState->m_LineJoin == 2 )
	{
		gpPen->SetLineJoin( LineJoinBevel );
	}
	
	gpPen->SetMiterLimit( pGraphState->m_MiterLimit );
	gpPen->SetColor( pGraphState->m_StrokeColor.GetARGB() );
	((SolidBrush*)gpBrush)->SetColor( pGraphState->m_FillColor.GetARGB() );

	if ( pGraphState->GetDashArraySize() == 1 )
	{
		REAL * dashArray = new REAL[2];
		dashArray[0] = dashArray[1] = pGraphState->GetDashArray()[0] / pGraphState->m_LineWidth;
		if ( pGraphState->m_LineCap != 0 )
		{
			dashArray[0] += 0.99;
			dashArray[1] -= 0.99;
		}
		gpPen->SetDashPattern( dashArray, 2 );
		if ( pGraphState->m_LineCap == 2 )
		{
			gpPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->m_LineWidth + 0.5 );
		}else{
			gpPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->m_LineWidth );
		}	
		delete [] dashArray;
	}else if ( pGraphState->GetDashArraySize() > 1 )
	{
		if ( pGraphState->GetDashArraySize() % 2 == 1 )
		{
			REAL * dashArray = new REAL[pGraphState->GetDashArraySize()*2];
			for ( unsigned long i = 0; i < pGraphState->GetDashArraySize(); i++ )
			{
				dashArray[i] = pGraphState->GetDashArray()[i] / pGraphState->m_LineWidth;
			}
			for ( unsigned long j = pGraphState->GetDashArraySize(); j < pGraphState->GetDashArraySize() * 2; j++ )
			{
				dashArray[j] = dashArray[j-pGraphState->GetDashArraySize()];
			}
			if ( pGraphState->m_LineCap != 0 )
			{
				int flag = 0;
				for ( unsigned long k = 0; k < pGraphState->GetDashArraySize() * 2; k++ )
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
			gpPen->SetDashPattern( dashArray, pGraphState->GetDashArraySize() * 2 );
			if ( pGraphState->m_LineCap == 2 )
			{
				gpPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->m_LineWidth + 0.5 );
			}else{
				gpPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->m_LineWidth );
			}
			delete [] dashArray;
		}else{
			REAL * dashArray = new REAL[pGraphState->GetDashArraySize()];
			for ( unsigned long i = 0; i < pGraphState->GetDashArraySize(); i++ )
			{
				dashArray[i] = pGraphState->GetDashArray()[i] / pGraphState->m_LineWidth;
			}
			if ( pGraphState->m_LineCap != 0 )
			{
				int flag = 0;
				for ( unsigned long i = 0; i < pGraphState->GetDashArraySize(); i++ )
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
			gpPen->SetDashPattern( dashArray, pGraphState->GetDashArraySize() );
			if ( pGraphState->m_LineCap == 2 )
			{
				gpPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->m_LineWidth + 0.5 );
			}else{
				gpPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->m_LineWidth );
			}
			delete [] dashArray;
		}
	}else{
		gpPen->SetDashPattern( NULL , 0 );
		gpPen->SetDashOffset( 0 );
	}

	gpGraphics->SetPageUnit( UnitPoint );

	Matrix matrix(	pGraphState->m_MatrixA, pGraphState->m_MatrixB, pGraphState->m_MatrixC,
					pGraphState->m_MatrixD, pGraphState->m_MatrixE, pGraphState->m_MatrixF );
	Matrix matrixToDev(	m_a, m_b, m_c,
						m_d, m_e, m_f );
	gpGraphics->SetTransform( &matrixToDev );
	gpGraphics->MultiplyTransform( &matrix );

	HE_DWORD iGraphCount = pPath->GetItemCount();
	CHE_GraphicsObject * pGraphObj = NULL;
	GraphicsPath GraphPath;
	for ( HE_DWORD i = 0; i < iGraphCount; i++ )
	{
		pGraphObj = pPath->GetItem( i );
		if ( pGraphObj->GetType() == GRAPHTYPE_LINE )
		{
			GraphicsPath tmpPath;
			tmpPath.AddLine(	((CHE_Line*)pGraphObj)->m_fStartX, ((CHE_Line*)pGraphObj)->m_fStartY, 
								((CHE_Line*)pGraphObj)->m_fEndX, ((CHE_Line*)pGraphObj)->m_fEndY );
			GraphPath.AddPath( &tmpPath, pGraphObj->IsConnect() );
		}else if ( pGraphObj->GetType() == GRAPHTYPE_BCURVE )
		{
			GraphicsPath tmpPath;
			tmpPath.AddBezier(	((CHE_BCurve*)pGraphObj)->m_fX1, ((CHE_BCurve*)pGraphObj)->m_fY1,
								((CHE_BCurve*)pGraphObj)->m_fX2, ((CHE_BCurve*)pGraphObj)->m_fY2,
								((CHE_BCurve*)pGraphObj)->m_fX3, ((CHE_BCurve*)pGraphObj)->m_fY3,
								((CHE_BCurve*)pGraphObj)->m_fX4, ((CHE_BCurve*)pGraphObj)->m_fY4 );
			GraphPath.AddPath( &tmpPath, pGraphObj->IsConnect() );
		}else if ( pGraphObj->GetType() == GRAPHTYPE_RECTANGLE ) {
			GraphicsPath tmpPath;
			Rect rt(	((CHE_Rectangle*)pGraphObj)->m_fLeftTopX, ((CHE_Rectangle*)pGraphObj)->m_fLeftTopY,
						((CHE_Rectangle*)pGraphObj)->m_fWidth, ((CHE_Rectangle*)pGraphObj)->m_fHeight );
			tmpPath.AddRectangle( rt );
			GraphPath.AddPath( &tmpPath, pGraphObj->IsConnect() );
		}
	}
	if ( pPath->GetFillMode() == PATH_FILL_MODE_EVERODD )
	{
		GraphPath.SetFillMode( FillModeAlternate );
	}else{
		GraphPath.SetFillMode( FillModeWinding );
	}

	if ( pPath->GetOperator() == PATH_OPERATOR_STROKE )
	{
		gpGraphics->DrawPath( gpPen, &GraphPath );
	}else if ( pPath->GetOperator() == PATH_OPERATOR_FILL )
	{
		gpGraphics->FillPath( gpBrush, &GraphPath );
	}else{
		gpGraphics->DrawPath( gpPen, &GraphPath );
		gpGraphics->FillPath( gpBrush, &GraphPath );
	}

// 	FontFamily  fontFamily( L"Times New Roman" ); 
// 	Font        font( &fontFamily, 20, FontStyleRegular, UnitPoint ); 
// 	PointF      pointF( 400.0f, 400.0f ); 
// 	SolidBrush  solidBrush( Color(255, 0, 0, 0) );
// 
// 	Matrix ma(	1, 0, 0, 1, 0, 0 );
// 	gpGraphics->SetTransform( &ma );
// 
// 
// 
// 	gpGraphics->DrawString( L"Hello", -1, &font, pointF, &solidBrush ); 
}

HE_VOID IHE_PDF_GDIplusDraw::DrawText( CHE_PDF_TextObject * pText )
{
// 	FontFamily  fontFamily( L"Times New Roman" ); 
// 	Font        font( &fontFamily, 24, FontStyleRegular, UnitPoint ); 
// 	PointF      pointF( 400.0f, 400.0f ); 
// 	SolidBrush  solidBrush( Color(255, 0, 0, 255) );
// 
// 	gpGraphics->DrawString( L"Hello", -1, &font, pointF, &solidBrush ); 
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow )
{
	MSG		msg ;
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
			HDC hdc = GetDC( hwnd );
			gMemDc =  CreateCompatibleDC( hdc );
			HBITMAP hBitmap = CreateCompatibleBitmap( hdc, 612 * 96 / 72, 792 * 96 / 72 );//A
			SelectObject( gMemDc, hBitmap );//B
			RECT rt;
			rt.top = 0;
			rt.left = 0;
			rt.bottom = 792 * 96 / 72 ;
			rt.right = 612 * 96 / 72 ;
			FillRect( gMemDc, &rt, HBRUSH(WHITE_BRUSH) );
			gpGraphics = new Graphics( gMemDc );
			ReleaseDC( hwnd, hdc );
			gpGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
			gpGraphics->SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
			gpGraphics->SetPageUnit( UnitPoint );
			gpPen = new Pen( Color( 255, 0, 0, 0 ), 1 );
			gpBrush = new SolidBrush( Color( 255, 0, 0, 0 ) );
			IHE_PDF_DrawGraphics * pGraphDraw = new IHE_PDF_GDIplusDraw( gpGraphics );
			pGraphDraw->SetMatrixToDevice( 1, 0, 0, -1, 0, 792 );/*ToDev*/

			IHE_Read * pIHE_Read  = HE_CreateFileRead( "c:\\demo4.pdf" );
			CHE_PDF_Document doc;
			doc.Load( pIHE_Read );
			CHE_PDF_Page * pPage = doc.GetPage( 0 );
			CHE_PDF_Renderer renderer;
			renderer.Render( pPage, pGraphDraw );
			break;
		}
	case WM_PAINT:
		{
		hdc = BeginPaint( hwnd, &ps );
		BitBlt( hdc, 0, 0, 612 * 96 / 72, 792 * 96 / 72, gMemDc, 0, 0, SRCCOPY );
		EndPaint( hwnd, &ps );
		break ;
		}
	case WM_LBUTTONDOWN:
		{
// 		Pen* pen = new Pen(Color(255, 255, 0, 0), 1 ); ;
// 		pen->SetEndCap( LineCapRound );
// 		gpGraphics->SetSmoothingMode( SmoothingModeAntiAlias ); 
// 		GraphicsPath path; 
// 		path.AddRectangle( Rect( 10, 10, 200, 200 ) );
// 		path.AddRectangle( Rect( 30, 30, 150, 150 ) );
// 		path.AddRectangle( Rect( 50, 50, 120, 120 ) );
// 		SolidBrush brush(Color(255, 255, 0, 0));
// 		path.SetFillMode( FillModeAlternate );
// 		gpGraphics->FillPath( &brush, &path); 
// 		gpGraphics->DrawPath( pen, &path);
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