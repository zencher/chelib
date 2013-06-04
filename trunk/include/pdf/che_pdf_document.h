#ifndef _CHE_PDF_DOCUMENT_H_
#define _CHE_PDF_DOCUMENT_H_

#include "../che_base.h"
#include "che_pdf_file.h"
#include "che_pdf_pages.h"
#include "che_pdf_outline.h"
#include "che_pdf_fontmgr.h"


class CHE_PDF_Document : public CHE_Object
{
public:
	static CHE_PDF_Document *	CreateDocument( CHE_PDF_File * pPDFFile );

	~CHE_PDF_Document();

	PDF_VERSION					GetVersion() { return mpFile->GetPDFVersion(); }

	HE_DWORD					GetPageCount() const;

	CHE_PDF_PageTree *			GetPageTree() const { return mpPageTree; }

	CHE_PDF_Page *				GetPage( HE_DWORD index );

	CHE_PDF_FontMgr *			GetFontMgr() const;

	CHE_PDF_Outline *			GetOutline();

	HE_VOID						SetVersion( PDF_VERSION version ) { mpFile->SetPDFVersion( version ); }

	HE_BOOL						SetDocumentInfo( PDF_DOCUMENT_INFO infoType, const CHE_ByteString & str );
	
private:
	CHE_PDF_Document( CHE_PDF_File * mpFile, CHE_Allocator * pAllocator );

	HE_BOOL						ParsePageTree();
	HE_BOOL						ParseOutline();

	HE_VOID						CreateCatalogDict();

	CHE_PDF_File *				mpFile;
	CHE_PDF_PageTree *			mpPageTree;
	CHE_PDF_Outline *			mpOutline;
	CHE_PDF_FontMgr *			mpFontMgr;

	friend class CHE_Allocator;
};


#endif