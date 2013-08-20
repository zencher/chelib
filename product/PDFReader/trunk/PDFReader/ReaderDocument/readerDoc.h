#include "../../../../../trunk/include/pdf/che_pdf_file.h"
#include "../../../../../trunk/include/pdf/che_pdf_document.h"
#include "../../../../../trunk/include/che_drawer_windows.h"

class CReaderDocument : public CHE_Object
{
public:
	CReaderDocument( CHE_Allocator * pAllocator = NULL );
	~CReaderDocument();

	HE_BOOL	OpenFile( char * fileName );

protected:
	IHE_Read *				mpRead;
	CHE_PDF_File *			mpFile;
	CHE_PDF_Document *		mpDocument;
	CHE_PDF_PageTree *		mpPageTree;

	CHE_GraphicsDrawer *	mpThrumbDrawer;
	CHE_GraphicsDrawer *	mpPageDrawer;
	CHE_GraphicsDrawer *	mpHungDrawer;

private:

};