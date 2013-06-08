#include "../../include/pdf/che_pdf_document.h"
#include <cassert>

CHE_PDF_Document * CHE_PDF_Document::CreateDocument( CHE_PDF_File * pPDFFile )
{
	if ( pPDFFile )
	{
		CHE_Allocator * pAllocator = pPDFFile->GetAllocator();
		CHE_PDF_Document * pDocument = pAllocator->New<CHE_PDF_Document>( pPDFFile, pAllocator );
		if ( pDocument )
		{
			if ( ! pPDFFile->GetTrailerDict() )
			{
				pDocument->CreateCatalogDict();
			}

			pDocument->ParsePageTree();

			pDocument->ParseOutline();

			return pDocument;
		}
	}
	return NULL;
}

CHE_PDF_Document::CHE_PDF_Document( CHE_PDF_File * pFile, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpFile( pFile ), mpPageTree( NULL ), mpFontMgr( NULL )
{
	mpFontMgr = GetAllocator()->New<CHE_PDF_FontMgr>( GetAllocator() );
}

CHE_PDF_Document::~CHE_PDF_Document()
{
	if ( mpPageTree )
	{
		mpPageTree->GetAllocator()->Delete( mpPageTree );
	}
	if ( mpFontMgr )
	{
		mpFontMgr->GetAllocator()->Delete( mpFontMgr );
	}
}

HE_ULONG CHE_PDF_Document::GetPageCount() const
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPageCount();
	}
	return 0;
}

CHE_PDF_Page * CHE_PDF_Document::GetPage( HE_ULONG index )
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPage( index );
	}
	return NULL;
}

CHE_PDF_FontMgr * CHE_PDF_Document::GetFontMgr() const
{
	return mpFontMgr;
}

HE_BOOL CHE_PDF_Document::SetDocumentInfo( PDF_DOCUMENT_INFO infoType, const CHE_ByteString & str )
{
	if ( mpFile )
	{
		return mpFile->SetInfo( infoType, str );
	}
	return FALSE;
}

HE_BOOL CHE_PDF_Document::ParsePageTree()
{
	CHE_PDF_DictionaryPtr pDict = mpFile->GetRootDict();
	CHE_PDF_ObjectPtr pObj;
	if ( pDict )
	{
		pObj = pDict->GetElement( "Pages", OBJ_TYPE_DICTIONARY );
		if ( pObj )
		{
			pDict = pObj->GetDictPtr();
		}
		if ( pDict )
		{
			mpPageTree = GetAllocator()->New<CHE_PDF_PageTree>( pDict, mpFile, GetAllocator() );
			return TRUE;
		}
	}
	return FALSE;
}

HE_BOOL CHE_PDF_Document::ParseOutline()
{
	if ( mpFile )
	{
		CHE_PDF_DictionaryPtr dictPtr = mpFile->GetRootDict();

		CHE_PDF_ObjectPtr objPtr = dictPtr->GetElement( "Outlines", OBJ_TYPE_REFERENCE );

		if ( objPtr )
		{
			mpOutline = GetAllocator()->New<CHE_PDF_Outline>( mpFile, GetAllocator() );
			mpOutline->Parse( objPtr->GetRefPtr() );
			return TRUE;
		}
	}
	return FALSE;
}

HE_VOID CHE_PDF_Document::CreateCatalogDict()
{
	if ( mpFile == NULL )
	{
		return;
	}
	mpFile->CreateCatalogDict();
}