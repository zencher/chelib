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

	HE_BOOL					SetCompatibleDC( HDC hdc );

	CHE_GraphicsDrawer *	GetPageDrawer();
	CHE_GraphicsDrawer *	GetThumbnailDrawer();
	CHE_GraphicsDrawer *	GetHugeDrawer();

	HE_VOID StartParsePageContentThread();

	HE_VOID StartThumbnailRenderThread();

	HE_VOID StartPageRenderThread();

	HE_BOOL	IsPageContentParse( HE_ULONG pageIndex );

	HE_BOOL ParseContentParse( HE_ULONG pageIndex );

	HE_BOOL RenderPage( HE_ULONG pageIndex );

protected:
	IHE_Read *				mpRead;
	CHE_PDF_File *			mpFile;
	CHE_PDF_Document *		mpDocument;
	CHE_PDF_PageTree *		mpPageTree;
	CHE_PDF_FontMgr *		mpFontMgr;

	HDC						mCompatibleDC;
	CHE_GraphicsDrawer *	mpThumbnailDrawer;
	CHE_GraphicsDrawer *	mpPageDrawer;
	CHE_GraphicsDrawer *	mpHugeDrawer;

	std::vector<HE_BOOL>					mContentsFlag;
	std::vector<CHE_PDF_ContentObjectList*> mContents;
	std::vector<CHE_Rect>					mPageRects;
	CHE_Rect								mAllPageRect;

	std::vector<CHE_Bitmap*>				mBitmaps;


private:

};