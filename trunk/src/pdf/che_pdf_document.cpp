#include "../../include/pdf/che_pdf_document.h"
#include <cassert>

CHE_PDF_Document * CHE_PDF_Document::CreateDocument( CHE_PDF_File * pPDFFile )
{
	if ( pPDFFile )
	{
		CHE_PDF_Document * pDocument = pPDFFile->GetAllocator()->New<CHE_PDF_Document>( pPDFFile, pPDFFile->GetAllocator() );
		if ( pDocument )
		{
			if ( pPDFFile )
			{
				if ( ! pPDFFile->GetTrailerDict() )
				{
					pDocument->CreateCatalogDict();
				}
				pDocument->ParsePageTree();
			}
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

HE_BOOL CHE_PDF_Document::SetDocumentInfo( PDF_DOCUMENT_INFO infoType, const CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return FALSE;
	}
	if ( infoType > 8 || infoType < 0 )
	{
		return FALSE;
	}
	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_DictionaryPtr InfoDictPtr;

	ObjPtr = mpFile->GetTrailerDict()->GetElement( "Info", OBJ_TYPE_DICTIONARY );
	if ( ObjPtr )
	{
		InfoDictPtr = ObjPtr->GetDictPtr();
	}else{
		PDF_RefInfo refInfo = mpFile->CreateDictObject( InfoDictPtr );
		if ( InfoDictPtr )
		{
			mpFile->GetTrailerDict()->SetAtReference( "Info", refInfo.objNum, refInfo.genNum, mpFile );
		}
	}

	
	if ( ! InfoDictPtr )
	{
		return FALSE;
	}

	switch( infoType )
	{
	case DOCUMENT_INFO_TITLE:
		InfoDictPtr->SetAtString( "Title", str );
		break;
	case DOCUMENT_INFO_AUTHOR:
		InfoDictPtr->SetAtString( "Author", str );
		break;
	case DOCUMENT_INFO_SUBJECT:
		InfoDictPtr->SetAtString( "Subject", str );
		break;
	case DOCUMENT_INFO_KEYWORDS:
		InfoDictPtr->SetAtString( "Keywords", str );
		break;
	case DOCUMENT_INFO_CREATOR:
		InfoDictPtr->SetAtString( "Creator", str );
		break;
	case DOCUMENT_INFO_PRODUCER:
		InfoDictPtr->SetAtString( "Producer", str );
		break;
	case DOCUMENT_INFO_CREATIONDATE:
		InfoDictPtr->SetAtString( "CreationDate", str );
		break;
	case DOCUMENT_INFO_MODDATE:
		InfoDictPtr->SetAtString( "ModDate", str );
		break;
	case DOCUMENT_INFO_TRAPPED:
		InfoDictPtr->SetAtString( "Trapped", str );
		break;
	default:
		return FALSE;
	}
	return TRUE;
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

HE_VOID CHE_PDF_Document::CreateCatalogDict()
{
	if ( mpFile == NULL )
	{
		return;
	}
	mpFile->CreateCatalogDict();
}