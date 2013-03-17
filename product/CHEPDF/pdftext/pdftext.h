#ifndef _PDF_TEXT_DLL_H_
#define _PDF_TEXT_DLL_H_

typedef	int		PDFStatus;
typedef	void *	PDFDocument;
typedef	void *	PDFPage;
typedef	void *	PDFPageContent;
typedef	void *	PDFPageText;
typedef	void *	PDFPageChar;
typedef void *	PDFBitmap;
typedef void *	PDFPageWordSet;
typedef void *	PDFPageWord;

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

#define				PDF_STATUS_OK				0
#define				PDF_STATUS_ERROR			-1
#define				PDF_STATUS_PARAM_ERR		-2
#define				PDF_STATUS_NOTEXT_ERR		-3
#define				PDF_STATUS_BUFFER_TOO_SMALL	-4

#define				PDF_WORD_NOTSYMBOLIC		0
#define				PDF_WORD_SYMBOLIC			1

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
PDFBitmap			CHEPDF_RenderText( PDFPageText text, float sclae = 1 );

PDFPageChar			CHEPDF_GetPageChar( PDFPageText text, unsigned int index );
void				CHEPDF_ClosePageChar( PDFPageChar textChar );
PDFMatrix			CHEPDF_GetCharMatirx( PDFPageChar textChar );
PDFPosition			CHEPDF_GetCharPosition( PDFPageChar textChar );
PDFStatus			CHEPDF_GetCharUnicode( PDFPageChar textChar, wchar_t * UnicodeRet );
PDFRect				CHEPDF_GetCharBox( PDFPageChar textChar );
PDFBitmap			CHEPDF_RenderChar( PDFPageChar textChar, float sclae = 1 );

void				CHEPDF_CloseBitmap( PDFBitmap bitmap );
unsigned int		CHEPDF_GetBitmapWidth( PDFBitmap bitmap );
unsigned int		CHEPDF_GetBitmapHeight( PDFBitmap bitmap );
PDFStatus			CHEPDF_SaveBitmapToFile( PDFBitmap bitmap, char * filePath );

PDFPageWordSet		CHEPDF_GetPageWordSet( PDFPageContent content );
PDFPageWord			CHEPDF_GetFirstPageWord( PDFPageWordSet wordset );
PDFPageWord			CHEPDF_GetNextPageWord( PDFPageWordSet wordset );
void				CHEPDF_ReleasePageWordSet( PDFPageWordSet wordset );

PDFRect				CHEPDF_GetWordBox( PDFPageWord word );
PDFStatus			CHEPDF_IsWordSymbolic( PDFPageWord word );
unsigned int		CHEPDF_GetWordLength( PDFPageWord word );
PDFStatus			CHEPDF_GetWordUnicodes( PDFPageWord word, wchar_t * pBuf, unsigned int bufSize );
PDFBitmap			CHEPDF_RenderWord( PDFPageWord word, float sclae = 1 );



#endif