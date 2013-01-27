#ifndef _PDF_TEXT_DLL_H_
#define _PDF_TEXT_DLL_H_

typedef	int		PDFStatus;
typedef	void *	PDFDocument;
typedef	void *	PDFPage;
typedef	void *	PDFPageContent;
typedef	void *	PDFPageText;
typedef	void *	PDFPageChar;
typedef void *	PDFBitmap;

struct PDFPosition
{
	float x;
	float y;
};

struct PDFRect
{
	float left;
	float bottom;
	float width;
	float height;
};

struct PDFMatrix
{
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
};

PDFDocument			CHEPDF_OpenDocument( const char * pFilePath );
void				CHEPDF_CloseDocument( PDFDocument doc );

int					CHEPDF_GetPageCount( PDFDocument doc );
PDFPage				CHEPDF_GetPage( PDFDocument doc, unsigned int index );
void				CHEPDF_ClosePage( PDFPage page );
PDFRect				CHEPDF_GetPageBox( PDFPage page );

PDFPageContent		CHEPDF_GetPageContent( PDFPage page );
void				CHEPDF_ReleasePageContent( PDFPageContent content );

PDFPageText			CHEPDF_GetFirstPageText( PDFPageContent content );
PDFPageText			CHEPDF_GetNextPageText( PDFPageContent content );

PDFPosition			CHEPDF_GetTextPosition( PDFPageText text );
PDFMatrix			CHEPDF_GetTextMatrix( PDFPageText text );
unsigned int		CHEPDF_GetTextLength( PDFPageText text );
PDFStatus			CHEPDF_GetTextUnicodes( PDFPageText text, wchar_t * pBuf, unsigned int bufSize );
PDFRect				CHEPDF_GetTextBox( PDFPageText text );
PDFBitmap			CHEPDF_RenderText( PDFPageText text );

PDFPageChar			CHEPDF_GetPageChar( PDFPageText text, unsigned int index );
void				CHEPDF_ClosePageChar( PDFPageChar textChar );
PDFMatrix			CHEPDF_GetCharMatirx( PDFPageChar textChar );
PDFPosition			CHEPDF_GetCharPosition( PDFPageChar textChar );
PDFStatus			CHEPDF_GetCharUnicode( PDFPageChar textChar, wchar_t * UnicodeRet );
PDFRect				CHEPDF_GetCharBox( PDFPageChar textChar );
PDFBitmap			CHEPDF_RenderChar( PDFPageChar textChar );

void				CHEPDF_CloseBitmap( PDFBitmap bitmap );
unsigned int		CHEPDF_GetBitmapWidth( PDFBitmap bitmap );
unsigned int		CHEPDF_GetBitmapHeight( PDFBitmap bitmap );
PDFStatus			CHEPDF_SaveBitmapToFile( PDFBitmap bitmap, char * filePath );

#endif