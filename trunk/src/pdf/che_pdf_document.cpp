#include "../../include/pdf/che_pdf_document.h"

CHE_PDF_Document * CHE_PDF_Document::CreateDocument( CHE_PDF_File * pPDFFile )
{
	if ( pPDFFile )
	{
		CHE_PDF_Document * pDocument = pPDFFile->GetAllocator()->New<CHE_PDF_Document>( pPDFFile, pPDFFile->GetAllocator() );
		if ( pDocument )
		{
			pDocument->ParsePageTree();
		}
		return pDocument;
	}
	return NULL;
}

CHE_PDF_Document::CHE_PDF_Document( CHE_PDF_File * pFile, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpPageTree(NULL), mpFile(pFile) {}

CHE_PDF_Document::~CHE_PDF_Document()
{
	if ( mpPageTree )
	{
		mpPageTree->GetAllocator()->Delete( mpPageTree );
	}
}

HE_DWORD CHE_PDF_Document::GetPageCount() const
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPageCount();
	}
	return 0;
}

CHE_PDF_Page * CHE_PDF_Document::GetPage( HE_DWORD index )
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPage( index );
	}
	return NULL;
}

HE_BOOL CHE_PDF_Document::ParsePageTree()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Dictionary * pDict = mpFile->GetRootDict();
	CHE_PDF_Object * pObj = NULL;
	if ( pDict )
	{
		pObj = pDict->GetElement( "Pages", OBJ_TYPE_DICTIONARY, objCollector );
		if ( pObj )
		{
			pDict = pObj->ToDict();
		}
		if ( pDict )
		{
			mpPageTree = GetAllocator()->New<CHE_PDF_PageTree>( pDict, mpFile, GetAllocator() );
			return TRUE;
		}
	}
	return FALSE;
}