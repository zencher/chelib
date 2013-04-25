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
CHE_GraphicsDrawer * gpDrawer = NULL;

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

			if ( gpDrawer )
			{
				DWORD x = 0;
				DWORD y = 0;
				if ( gWndHeight > gpDrawer->GetHeight() )
				{
					y = ( gWndHeight - gpDrawer->GetHeight() ) / 2;
				}
				if ( gWndWidth > gpDrawer->GetWidth() )
				{
					x = ( gWndWidth - gpDrawer->GetWidth() ) / 2;
				}
				BitBlt( gMemDC, x + gOffsetX + gTmpOffsetX, y + gOffsetY + gTmpOffsetY, gpDrawer->GetWidth(),  gpDrawer->GetHeight(), gpDrawer->GetMemDC(), 0, 0, SRCCOPY );
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
						CHE_Rect rect = gpPage->GetMediaBox();
						gPageWidth = rect.width * 96 / 72;
						gPageHeight =  rect.height * 96 / 72;
						gpDrawer = new CHE_GraphicsDrawer( GetDC(hwnd), gPageWidth, gPageHeight, gScale );
						CHE_PDF_ContentObjectList contentObjList;
						QueryPerformanceCounter( &gBegin );
						GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
						QueryPerformanceCounter( &gEnd );
						parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

						QueryPerformanceCounter( &gBegin );
						CHE_PDF_Renderer::Render( contentObjList, *gpDrawer );
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
						CHE_Rect rect = gpPage->GetMediaBox();
						gPageWidth = rect.width * 96 / 72;
						gPageHeight =  rect.height * 96 / 72;
						gpDrawer->Resize( gPageWidth, gPageHeight, gScale );
						CHE_PDF_ContentObjectList contentObjList;
						QueryPerformanceCounter( &gBegin );
						GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
						QueryPerformanceCounter( &gEnd );
						parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

						QueryPerformanceCounter( &gBegin );
						CHE_PDF_Renderer::Render( contentObjList, *gpDrawer );
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
						CHE_Rect rect = gpPage->GetMediaBox();
						gPageWidth = rect.width * 96 / 72;
						gPageHeight =  rect.height * 96 / 72;
						gpDrawer->Resize( gPageWidth, gPageHeight, gScale );
						CHE_PDF_ContentObjectList contentObjList;
						QueryPerformanceCounter( &gBegin );
						GetPageContent( gpPage->GetPageDict(), &contentObjList, gpDocument->GetFontMgr() );
						QueryPerformanceCounter( &gEnd );
						parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

						QueryPerformanceCounter( &gBegin );
						CHE_PDF_Renderer::Render( contentObjList, *gpDrawer );
						QueryPerformanceCounter( &gEnd );
						renderTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );
					}
					InvalidateRect( hwnd, NULL, FALSE );
				}
			}else if ( wParam == 'z' || wParam == 'Z' )
			{
				gScale+=0.5;
				gpDrawer->Resize( gPageWidth, gPageHeight, gScale );
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
					CHE_PDF_Renderer::Render( contentObjList, *gpDrawer );
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
				gpDrawer->Resize( gPageWidth, gPageHeight, gScale );
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
					CHE_PDF_Renderer::Render( contentObjList, *gpDrawer );
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