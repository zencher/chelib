#include <windows.h>
#include <GdiPlus.h>
#include "../../include/pdf/che_pdf_renderer.h"

using namespace Gdiplus;
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

CHE_PDF_Page * gpPage = NULL;
CHE_PDF_Document * gdoc = NULL;
unsigned int pageIndex = 0;

class IHE_PDF_GDIplusDraw : public IHE_PDF_DrawGraphics
{
public:
	IHE_PDF_GDIplusDraw( HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight, HE_DWORD posiX, HE_DWORD posiY, HE_FLOAT scale, CHE_Allocator * pAllocator = NULL );

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

	virtual HE_VOID Execute( CHE_PDF_OrderObject * pOrder );

	HE_VOID UpdateDC();

private:
	HDC m_DC;
	HDC m_MemDC;
	HBITMAP m_Bitmap;
	HE_DWORD m_dwWidth;
	HE_DWORD m_dwHeight;
	HE_DWORD m_dwPosiX;
	HE_DWORD m_dwPosiY;

	Graphics * m_pGraphics;
	Pen * m_pPen;
	Brush * m_pBrush;
	Region * m_pRegion;
	

	 HE_FLOAT m_a;
	 HE_FLOAT m_b;
	 HE_FLOAT m_c;
	 HE_FLOAT m_d;
	 HE_FLOAT m_e;
	 HE_FLOAT m_f;

	 CHE_PtrStack m_ClipStack;
};

IHE_PDF_GDIplusDraw::IHE_PDF_GDIplusDraw(	HDC hDC, HE_DWORD dibWidth, HE_DWORD dibHeight,  
											HE_DWORD posiX, HE_DWORD posiY, HE_FLOAT scale,
											CHE_Allocator * pAllocator/* = NULL*/ )
	: IHE_PDF_DrawGraphics( pAllocator ), m_ClipStack( pAllocator )
{
	m_DC = hDC;
	m_MemDC = CreateCompatibleDC( hDC );
	m_Bitmap = CreateCompatibleBitmap( m_DC, dibWidth , dibHeight );
 	SelectObject( m_MemDC, m_Bitmap );
 	RECT rt;
 	rt.top = 0;
 	rt.left = 0;
	rt.right = m_dwWidth = dibWidth;
 	rt.bottom = m_dwHeight = dibHeight;
 	FillRect( m_MemDC, &rt, HBRUSH(WHITE_BRUSH) );
 	
	m_pGraphics  = new Graphics( m_MemDC );
 	m_pGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
 	m_pGraphics->SetPageUnit( UnitPoint );
 	m_pPen = new Pen( Color( 255, 0, 0, 0 ), 1 );
 	m_pBrush = new SolidBrush( Color( 255, 0, 0, 0 ) );
 	m_pRegion = NULL;

	m_dwPosiX = posiX;
	m_dwPosiY = posiY;
 
 	m_a = 0/*1 * scale*/;
 	m_b = 0;
 	m_c = 0/*-1 * scale*/;
 	m_d = 0;
 	m_e = 0;
 	m_f = 0/*dibHeight * scale*/;
}

HE_VOID IHE_PDF_GDIplusDraw::UpdateDC()
{
	BitBlt( m_DC, m_dwPosiX, m_dwPosiY, m_dwWidth,  m_dwHeight, m_MemDC, 0, 0, SRCCOPY );
}

HE_VOID IHE_PDF_GDIplusDraw::DrawPath( CHE_PDF_PathObject * pPath )
{
	if ( pPath == NULL || m_pGraphics == NULL )
	{
		return;
	}
	CHE_PDF_GraphState * pGraphState = pPath->GetGraphState();

	m_pPen->SetWidth( pGraphState->GetLineWidth() + 0.001 );

	if ( pGraphState->GetLineCap() == 0 )
	{
		m_pPen->SetLineCap( LineCapFlat, LineCapFlat, DashCapFlat );
	}else if ( pGraphState->GetLineCap() == 1 )
	{
		m_pPen->SetLineCap( LineCapRound, LineCapRound, DashCapRound );
	}else if ( pGraphState->GetLineCap() == 2 )
	{
		m_pPen->SetLineCap( LineCapSquare, LineCapSquare, DashCapFlat );
	}

	if ( pGraphState->GetLineJoin() == 0 )
	{
		m_pPen->SetLineJoin( LineJoinMiter );
	}else if ( pGraphState->GetLineJoin() == 1 )
	{
		m_pPen->SetLineJoin( LineJoinRound );
	}else if ( pGraphState->GetLineJoin() == 2 )
	{
		m_pPen->SetLineJoin( LineJoinBevel );
	}
	
	m_pPen->SetMiterLimit( pGraphState->GetMiterLimit() );
	m_pPen->SetColor( pGraphState->GetStrokeColor()->GetARGB() );
	((SolidBrush*)m_pBrush)->SetColor( pGraphState->GetFillColor()->GetARGB() );

	if ( pGraphState->GetDashArraySize() == 1 )
	{
		REAL * dashArray = new REAL[2];
		dashArray[0] = dashArray[1] = pGraphState->GetDashArray()[0] / pGraphState->GetLineWidth();
		if ( pGraphState->GetLineCap() != 0 )
		{
			dashArray[0] += 0.99;
			dashArray[1] -= 0.99;
		}
		m_pPen->SetDashPattern( dashArray, 2 );
		if ( pGraphState->GetLineCap() == 2 )
		{
			m_pPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->GetLineWidth() + 0.5 );
		}else{
			m_pPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->GetLineWidth() );
		}	
		delete [] dashArray;
	}else if ( pGraphState->GetDashArraySize() > 1 )
	{
		if ( pGraphState->GetDashArraySize() % 2 == 1 )
		{
			REAL * dashArray = new REAL[pGraphState->GetDashArraySize()*2];
			for ( unsigned long i = 0; i < pGraphState->GetDashArraySize(); i++ )
			{
				dashArray[i] = pGraphState->GetDashArray()[i] / pGraphState->GetLineWidth();
			}
			for ( unsigned long j = pGraphState->GetDashArraySize(); j < pGraphState->GetDashArraySize() * 2; j++ )
			{
				dashArray[j] = dashArray[j-pGraphState->GetDashArraySize()];
			}
			if ( pGraphState->GetLineCap() != 0 )
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
			m_pPen->SetDashPattern( dashArray, pGraphState->GetDashArraySize() * 2 );
			if ( pGraphState->GetLineCap() == 2 )
			{
				m_pPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->GetLineWidth() + 0.5 );
			}else{
				m_pPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->GetLineWidth() );
			}
			delete [] dashArray;
		}else{
			REAL * dashArray = new REAL[pGraphState->GetDashArraySize()];
			for ( unsigned long i = 0; i < pGraphState->GetDashArraySize(); i++ )
			{
				dashArray[i] = pGraphState->GetDashArray()[i] / pGraphState->GetLineWidth();
			}
			if ( pGraphState->GetLineCap() != 0 )
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
			m_pPen->SetDashPattern( dashArray, pGraphState->GetDashArraySize() );
			if ( pGraphState->GetLineCap() == 2 )
			{
				m_pPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->GetLineWidth() + 0.5 );
			}else{
				m_pPen->SetDashOffset( pGraphState->GetDashPhase() / pGraphState->GetLineWidth() );
			}
			delete [] dashArray;
		}
	}else{
		m_pPen->SetDashStyle( DashStyleSolid );
		m_pPen->SetDashOffset( pGraphState->GetDashPhase() );
	}

	Matrix matrix(	pGraphState->GetMatrixA(), pGraphState->GetMatrixB(), pGraphState->GetMatrixC(),
					pGraphState->GetMatrixD(), pGraphState->GetMatrixE(), pGraphState->GetMatrixF() );
	Matrix matrixToDev(	m_a, m_b, m_c,
						m_d, m_e, m_f );
	m_pGraphics->SetTransform( &matrixToDev );
	if ( m_pRegion )
	{
		m_pGraphics->SetClip( m_pRegion );
	}else{
		m_pGraphics->ResetClip();
	}
	m_pGraphics->MultiplyTransform( &matrix );

	GraphicsPath GraphPath;
	HE_DWORD iGraphCount = pPath->GetItemCount();
	CHE_GraphicsObject * pGraphObj = NULL;
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

	if ( pPath->GetOperator() != PATH_OPERATOR_NOOP )
	{
		if ( pPath->GetOperator() == PATH_OPERATOR_STROKE )
		{
			m_pGraphics->DrawPath( m_pPen, &GraphPath );
		}else if ( pPath->GetOperator() == PATH_OPERATOR_FILL )
		{
			m_pGraphics->FillPath( m_pBrush, &GraphPath );
		}else{
			m_pGraphics->FillPath( m_pBrush, &GraphPath );
			m_pGraphics->DrawPath( m_pPen, &GraphPath );
		}	
	}

	if ( pPath->GetClip() == TRUE )
	{
		GraphPath.Transform( &matrix );
		if ( m_pRegion == NULL )
		{
			m_pRegion = new Region( &GraphPath );
		}else{
			Region tmpRegion( &GraphPath );
			m_pRegion->Intersect( &tmpRegion );
		}
	}
}

HE_VOID IHE_PDF_GDIplusDraw::DrawText( CHE_PDF_TextObject * pText )
{
	if ( pText == NULL )
	{
		return;
	}
	Region * pTmpRegion = NULL;
	if ( m_pRegion != NULL )
	{
		pTmpRegion = m_pRegion->Clone();
	}
	CHE_PDF_TextObjectItem * pTextItemObj = NULL;
	for ( HE_DWORD i = 0; i < pText->GetItemCount(); i++ )
	{
		pTextItemObj = pText->GetItem( i );
		if ( pTextItemObj == NULL )
		{
			continue;
		}
		CHE_PDF_GraphState * pGraphState = pTextItemObj->GetGraphState();
		if ( pGraphState == NULL )
		{
			continue;
		}

		Matrix matrix(	pGraphState->GetMatrixA(), pGraphState->GetMatrixB(), pGraphState->GetMatrixC(),
			pGraphState->GetMatrixD(), pGraphState->GetMatrixE(), pGraphState->GetMatrixF() );
		Matrix matrixT( pTextItemObj->GetMatrixA(), pTextItemObj->GetMatrixB(), pTextItemObj->GetMatrixC(),
						pTextItemObj->GetMatrixD(), pTextItemObj->GetMatrixE(), pTextItemObj->GetMatrixF() );
		Matrix matrixToDev(	m_a, m_b, m_c, m_d, m_e, m_f );
		Matrix mScale( pTextItemObj->GetScale() / 100.0, 0, 0, 1, 0, 0 );

		m_pGraphics->SetTransform( &matrixToDev );
		if ( m_pRegion )
		{
			m_pGraphics->SetClip( m_pRegion );
		}else{
			m_pGraphics->ResetClip();
		}

		FontFamily  fontFamily( L"Î¢ÈíÑÅºÚ" );
		HE_FLOAT	x = pTextItemObj->GetPosiX() + pTextItemObj->GetSize() * m_a / 6;
		HE_FLOAT	y = pTextItemObj->GetPosiY() + pTextItemObj->GetRise();// ( 
						//pTextItemObj->GetSize() * 1.0 / fontFamily.GetEmHeight(FontStyleRegular) * fontFamily.GetCellAscent(FontStyleRegular) );
		PointF      pointF( x, y );
		SolidBrush  solidBrush( pGraphState->GetFillColor()->GetARGB() );
		Pen			pen( pGraphState->GetStrokeColor()->GetARGB() );
		pen.SetWidth( pGraphState->GetLineWidth() + 0.001 );
		m_pGraphics->SetTextRenderingHint( TextRenderingHintAntiAliasGridFit );

		pointF.X = pointF.X / ( pTextItemObj->GetScale() / 100.0 );

		//pTextItemObj->SetSize( pTextItemObj->GetSize() * m_a );

		if ( pTextItemObj->GetString()->GetLength() > 0 )
		{
			GraphicsPath strPath;

			switch( pTextItemObj->GetRender() )
			{ 
			case 0: 
			case 4:
				{
					strPath.AddString( pTextItemObj->GetString()->GetData(), pTextItemObj->GetString()->GetLength(), &fontFamily, FontStyleRegular,
						pTextItemObj->GetSize() * 1.0, pointF, NULL );

					Matrix matrixR( 1, 0, 0, -1, 0, 0 );
					matrixT.Multiply( &matrix );
					matrixT.Multiply( &mScale );
					matrixT.Multiply( &matrixR );
					Matrix mts;
					mts.Translate( 0, y * -2 - pTextItemObj->GetSize() );
					matrixT.Multiply( &mts );
					strPath.Transform( &matrixT );

					m_pGraphics->FillPath( &solidBrush, &strPath );
					break;
				}
			case 1:
			case 5:
				{
					strPath.AddString( pTextItemObj->GetString()->GetData(), pTextItemObj->GetString()->GetLength(), &fontFamily, FontStyleRegular,
						pTextItemObj->GetSize() * 1.0, pointF, NULL );

					Matrix matrixR( 1, 0, 0, -1, 0, 0 );
					matrixT.Multiply( &matrix );
					matrixT.Multiply( &mScale );
					matrixT.Multiply( &matrixR );
					Matrix mts;

					mts.Translate( 0, y * - 2 - pTextItemObj->GetSize() );
					matrixT.Multiply( &mts );
					strPath.Transform( &matrixT );

					m_pGraphics->DrawPath( &pen, &strPath );
					break;
				}
			case 2:
			case 6:
				{
					strPath.AddString( pTextItemObj->GetString()->GetData(), pTextItemObj->GetString()->GetLength(), &fontFamily, FontStyleRegular,
						pTextItemObj->GetSize() * 1.0, pointF, NULL );

					Matrix matrixR( 1, 0, 0, -1, 0, 0 );
					matrixT.Multiply( &matrix );
					matrixT.Multiply( &mScale );
					matrixT.Multiply( &matrixR );
					Matrix mts;
					mts.Translate( 0, y * -2 - pTextItemObj->GetSize() );
					matrixT.Multiply( &mts );
					strPath.Transform( &matrixT );

					m_pGraphics->FillPath( &solidBrush, &strPath );
					m_pGraphics->DrawPath( &pen, &strPath );
					break;
				}
			case 3:
			case 7:
				{
					break;
				}
			default:
				break;
			}

			switch ( pTextItemObj->GetRender() )
			{
			case 4:
			case 5:
			case 6:
			case 7:
				{
					if ( pTmpRegion == NULL )
					{
						pTmpRegion = new Region( &strPath );
					}else{
						Region tmpRegion( &strPath );
						pTmpRegion->Union( &tmpRegion );
					}
					break;
				}
			default:
				break;
			}
		}
	}

	if ( m_pRegion )
	{
		delete m_pRegion;
	}
	m_pRegion = pTmpRegion;
}

HE_VOID IHE_PDF_GDIplusDraw::Execute( CHE_PDF_OrderObject * pOrder )
{
	if ( pOrder == NULL )
	{
		return;
	}else{
		if ( pOrder->GetOrder() == ORDER_CLIPRESET )
		{
			if ( m_pRegion )
			{
				delete m_pRegion;
				m_pRegion = NULL;
			}
		}else if ( pOrder->GetOrder() == ORDER_CLIPPOP )
		{
			if ( m_pRegion )
			{
				delete m_pRegion;
				m_pRegion = NULL;
			}
			if ( m_ClipStack.Pop( (HE_VOID**)&m_pRegion ) == FALSE )
			{
				m_pRegion = NULL;
			}
		}else if ( pOrder->GetOrder() == ORDER_CLIPPUSH )
		{
			if ( m_pRegion == NULL )
			{
				m_ClipStack.Push( m_pRegion );
			}else{
				Region * tmpRegion = m_pRegion->Clone();
				m_ClipStack.Push( tmpRegion );
			}
		}
	}
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
			gdoc = new CHE_PDF_Document;
			IHE_Read * pIHE_Read  = HE_CreateFileRead( "e:\\chelibpdf\\demo2.pdf" );
			gdoc->Load( pIHE_Read );
			gpPage = gdoc->GetPage( 0 );
			break;
		}
	case WM_PAINT:
		{
			CHE_PDF_Renderer renderer;
			hdc = BeginPaint( hwnd, &ps );
			IHE_PDF_GDIplusDraw	tmpDraw( hdc,  1.0 * gpPage->GetPageWidth() * 96 /72, 1.0 * gpPage->GetPageHeight() * 96 / 72, 0, 0, 1 );
			tmpDraw.SetMatrixToDevice( 1 * 1.0, 0, 0, -1 * 1.0, 0, gpPage->GetPageHeight() * 1.0 );
			renderer.Render( gpPage, &tmpDraw );
			tmpDraw.UpdateDC();
			
			EndPaint( hwnd, &ps );
			break ;
		}
	case WM_LBUTTONDOWN:
		{
			gpPage = gdoc->GetPage( ++pageIndex );
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