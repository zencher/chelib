#include "../pdftext/pdftext.h"
#include <stdio.h>

int main( int argc, char * argv[] )
{
	int i = 0;
	char tmpStr[1024];
	PDFDocument doc = CHEPDF_OpenDocument( "e:\\zencher's housework1.pdf" ); //"e:\\W7ADD32EN006.pdf"
	if ( doc )
	{
		PDFPage page = CHEPDF_GetPage( doc, 0 );
		if ( page )
		{
			PDFPageContent content = CHEPDF_GetPageContent( page );
			if ( content )
			{
				PDFPageText text = CHEPDF_GetFirstPageText( content );
				while ( text )
				{
					PDFPosition posi = CHEPDF_GetTextPosition( text );
					wchar_t str[128];
					CHEPDF_GetTextUnicodes( text, str, 128 );
					PDFRect rect = CHEPDF_GetTextBox( text );

					PDFBitmap bitmap = CHEPDF_RenderPDFPageText( text, 10 );
					sprintf_s( tmpStr, "e:\\%d.bmp", i );
					++i;
					CHEPDF_SaveBitmapToFile( bitmap, tmpStr/*"e:\\text.bmp"*/ );
					int x = 0;
					
// 					for ( unsigned int i = 0; i < CHEPDF_GetTextLength( text ); ++i )
// 					{
// 						PDFPageChar textChar = CHEPDF_GetPageChar( text, i );
// 						if ( textChar )
// 						{
// 							PDFMatrix mtx;
// 							mtx = CHEPDF_GetCharMatirx( textChar );
// 							PDFRect rect = CHEPDF_GetCharBox( textChar );
// 							PDFBitmap bitmap = CHEPDF_RenderPDFPageChar( textChar, 10 );
// 							CHEPDF_SaveBitmapToFile( bitmap, "e:\\d.bmp" );
// 							int x = 0;
// 						}
// 					}
					text = CHEPDF_GetNextPageText( content );
				}
			}
		}
	}

	return 0;
}

