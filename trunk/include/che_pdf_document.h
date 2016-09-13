#ifndef _CHELIB_PDF_DOCUMENT_H_
#define _CHELIB_PDF_DOCUMENT_H_

#include "che_pdf_file.h"
#include "che_pdf_page.h"
#include "che_pdf_outline.h"
#include "che_pdf_nametree.h"
#include "che_pdf_pagelabel.h"
#include "che_pdf_componentmgr.h"

namespace chelib {

class CPDF_Document : public CObject
{
public:
	static CPDF_Document * CreateDocument( CPDF_File * pPDFFile );

	~CPDF_Document();

	PDF_VERSION GetVersion() { return mpFile->GetPDFVersion(); }
    void SetVersion( PDF_VERSION version ) { mpFile->SetPDFVersion( version ); }
    
    bool SetDocumentInfo( PDF_DOCUMENT_INFO infoType, const CByteString & str );

	size_t GetPageCount() const;
	CPDF_PageTree * GetPageTree() const { return mpPageTree; }
	CPDF_Page * GetPage( size_t index );

	CPDF_ComponentMgr * GetComponentMgr() const;

	CPDF_Outline * GetOutline();

private:
	CPDF_Document( CPDF_File * mpFile, CAllocator * pAllocator );

	bool ParsePageTree();
	bool ParseNameDict();
	bool ParseOutline();
	bool ParsePageLabels();

	void CreateCatalogDict();

	CPDF_File *				mpFile;
	CPDF_PageTree *			mpPageTree;
	CPDF_NameDict *			mpNameDict;
	CPDF_Outline *			mpOutline;
	CPDF_PageLabels *		mpPageLabels;
	CPDF_ComponentMgr *		mpComponentMgr;

	friend class CAllocator;
};

}//namespace

#endif