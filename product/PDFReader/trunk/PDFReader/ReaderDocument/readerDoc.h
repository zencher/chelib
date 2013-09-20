#include "../../../../../trunk/include/pdf/che_pdf_file.h"
#include "../../../../../trunk/include/pdf/che_pdf_document.h"
#include "../../../../../trunk/include/pdf/che_pdf_contents.h"
#include "../../../../../trunk/include/pdf/che_pdf_renderer_windows.h"
#include "../../../../../trunk/include/che_drawer_windows.h"

#include <vector>

class CReaderDocument : public CHE_Object
{
public:
	CReaderDocument( CHE_Allocator * pAllocator = NULL );
	~CReaderDocument();

	HE_BOOL		OpenFile( char * fileName );

	HE_ULONG	GetPageCount();
	CHE_Rect	GetAllPageRect();
	CHE_Rect	GetPageRect( HE_ULONG index );

	HE_ULONG	GetPageIndex( HE_ULONG y );

	CHE_Bitmap* GetBitmap( HE_ULONG index );
	void		SetBitmap( HE_ULONG index, CHE_Bitmap * pBitmap );

	HE_BOOL	IsPageContentParse( HE_ULONG pageIndex );

	HE_BOOL ParseContentParse( HE_ULONG pageIndex );

	CHE_PDF_ContentObjectList * GetPageConent( HE_ULONG index );

protected:
	IHE_Read *				mpRead;
	CHE_PDF_File *			mpFile;
	CHE_PDF_Document *		mpDocument;
	CHE_PDF_PageTree *		mpPageTree;
	CHE_PDF_FontMgr *		mpFontMgr;

	std::vector<HE_BOOL>					mContentsFlag;
	std::vector<CHE_PDF_ContentObjectList*> mContents;
	std::vector<CHE_Rect>					mPageRects;
	CHE_Rect								mAllPageRect;

	std::vector<CHE_Bitmap*>				mBitmaps;

private:

};