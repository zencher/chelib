#ifndef _CHE_PDF_DOCUMENT_H_
#define _CHE_PDF_DOCUMENT_H_

#include "../che_base.h"
#include "che_pdf_file.h"
#include "che_pdf_pages.h"

class CHE_PDF_Document : public CHE_Object
{
public:
	static CHE_PDF_Document *	CreateDocument( CHE_PDF_File * pPDFFile );

	~CHE_PDF_Document();

	HE_DWORD					GetPageCount() const;

	CHE_PDF_PageTree *			GetPageTree() { return mpPageTree; }

	CHE_PDF_Page *				GetPage( HE_DWORD index );

	HE_VOID						NewPage( HE_DWORD width, HE_DWORD height );

private:
	CHE_PDF_Document( CHE_PDF_File * mpFile, CHE_Allocator * pAllocator );

	HE_BOOL						ParsePageTree();

	HE_VOID						CreateCatalogDict();

	CHE_PDF_File *				mpFile;

	CHE_PDF_PageTree *			mpPageTree;

	friend class CHE_Allocator;
};


#endif