#include "../pdftext/pdftext.h"

int main( int argc, char * argv[] )
{
	PDFDocument doc = CHEPDF_OpenDocument( "e:\\zencher's housework.pdf" );
	if ( doc )
	{
		PDFPage page = CHEPDF_GetPage( doc, 0 );
		if ( page )
		{
			PDFPageContent content = CHEPDF_GetPageContent( page );
			if ( content )
			{
				PDFPageText text = CHEPDF_GetFirstPageText( content );
				if ( text )
				{
					Position posi = CHEPDF_GetTextPosition( text );
					wchar_t str[128];
					CHEPDF_GetTextUnicodes( text, str, 128 );
					int x = 0;
				}
			}
		}
	}

	return 0;
}

