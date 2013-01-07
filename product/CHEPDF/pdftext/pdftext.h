#ifndef _PDF_TEXT_DLL_H_
#define _PDF_TEXT_DLL_H_

typedef	void *	PDFDocument;
typedef	void *	PDFPage;
typedef	void *	PDFPageContent;
typedef	void *	PDFPageText;
//typedef	void *	PDFPageChar;
typedef	int		PDFStatus;

PDFDocument			CHEPDF_OpenDocument( const char * pFilePath );
void				CHEPDF_CloseDocument( PDFDocument doc );

int					CHEPDF_GetPageCount( PDFDocument doc );
PDFPage				CHEPDF_GetPage( PDFDocument doc, unsigned int index );
void				CHEPDF_ClosePage( PDFPage page );
//float				CHEPDF_GetPageBox( PDFPage page );

PDFPageContent		CHEPDF_GetPageContent( PDFPage page );
void				CHEPDF_ReleasePageContent( PDFPageContent content );

PDFPageText			CHEPDF_GetFirstPageText( PDFPageContent content );
PDFPageText			CHEPDF_GetNextPageText( PDFPageContent content );

// struct Matrix
// {
// 	float a;
// 	float b;
// 	float c;
// 	float d;
// 	float e;
// 	float f;
// };
// 
// Matrix				CHEPDF_GetTextMatrix( PDFPageText text );

struct Position
{
	float x;
	float y;
};

Position			CHEPDF_GetTextPosition( PDFPageText text );

unsigned int		CHEPDF_GetTextLength( PDFPageText text );

PDFStatus			CHEPDF_GetTextUnicodes( PDFPageText text, wchar_t * pBuf, unsigned int bufSize );

//PDFPageChar			CHEPDF_GetTextChar( PDFPageText text, unsigned int index );





#endif