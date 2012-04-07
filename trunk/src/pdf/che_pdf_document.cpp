#include "../../include/pdf/che_pdf_document.h"

CHE_PDF_Document * CHE_PDF_Document::CreateDocument( CHE_PDF_File * pPDFFile )
{
	if ( pPDFFile )
	{
		CHE_PDF_Document * pDocument = pPDFFile->GetAllocator()->New<CHE_PDF_Document>( pPDFFile, pPDFFile->GetAllocator() );
		if ( pDocument )
		{
			if ( pPDFFile )
			{
				if ( pPDFFile->GetTrailerDict() )
				{
					pDocument->ParsePageTree();
				}else{
					pDocument->CreateCatalogDict();
				}		 
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

HE_VOID CHE_PDF_Document::NewPage( HE_DWORD width, HE_DWORD height )
{
// 	if ( mpFile == NULL )
// 	{
// 		return;
// 	}
// 	CHE_PDF_ObjectPtr pObj;
// 	CHE_PDF_DictionaryPtr pPagesDict;
// 	pObj = mpFile->GetTrailerDict()->GetElement( "Root", OBJ_TYPE_DICTIONARY );
// 	if ( pObj )
// 	{
// 		pObj = pObj->GetDictPtr()->GetElement( "Catalog", OBJ_TYPE_DICTIONARY );
// 	}
// 	if ( pObj )
// 	{
// 		pObj = pObj->GetDictPtr()->GetElement( "Pages", OBJ_TYPE_DICTIONARY );
// 	}
// 	if ( pObj )
// 	{
// 		pPagesDict = pObj->GetDictPtr();
// 	}
// 	if ( pPagesDict )
// 	{
// 		CHE_PDF_ArrayPtr pKidsArray;
// 		CHE_PDF_ReferencePtr pKidsRef;
// 		pKidsArray = pPageDict->GetElement( "Kids", OBJ_TYPE_ARRAY )->GetArrayPtr();
// 		if ( pKidsArray )
// 		{
// 		}
// 		if ( IsPdfReference( pObj ) )
// 		{
// 			pKidsRef = pObj->GetReference();
// 			pObj = pKidsRef->GetRefObj();
// 			pKidsArray = pObj->GetArrayPtr();
// 		}else if ( IsPdfArray( pObj ) )
// 		{
// 			pKidsArray = pObj->GetArrayPtr();
// 		}
// 		if ( pKidsArray )
// 		{
// 			CHE_PDF_IndirectObject * pInObj = mpFile->CreateInObj_Dict();
// 			CHE_PDF_Reference * pRef = CHE_PDF_Reference::Create( pInObj->GetObjNum(), pInObj->GetGenNum(), mpFile, GetAllocator() );
// 			pKidsArray->Append( pRef );
// 
// 			CHE_PDF_Number * pCountNumber = NULL;
// 			CHE_PDF_Reference * pCountRef = NULL;
// 			pObj = pPageDict->GetElement( "Count" );
// 			if ( IsPdfReference( pObj ) )
// 			{
// 				pCountRef = pObj->GetReference();
// 			}else if ( IsPdfNumber( pObj ) )
// 			{
// 				pCountNumber = pObj->GetNumberPtr();
// 			}
// 			if ( pCountNumber )
// 			{
// 				pCountNumber->SetValue( (HE_INT32)( pCountNumber->GetInteger() + 1 ) );
// 			}
// 
// 
// 			CHE_PDF_Dictionary * pDict = pInObj->GetObj()->GetDictPtr();
// 			if ( pDict )
// 			{
// 				pDict->SetAtName( "Type", "Page" );
// 				CHE_PDF_Array * pArray = CHE_PDF_Array::Create( GetAllocator() );
// 				CHE_PDF_Number * pNumber = CHE_PDF_Number::Create( (HE_INT32)(0), GetAllocator() );
// 				pArray->Append( pNumber );
// 				pNumber = CHE_PDF_Number::Create( (HE_INT32)(0), GetAllocator() );
// 				pArray->Append( pNumber );
// 				pNumber = CHE_PDF_Number::Create( (HE_INT32)(width), GetAllocator() );
// 				pArray->Append( pNumber );
// 				pNumber = CHE_PDF_Number::Create( (HE_INT32)(height), GetAllocator() );
// 				pArray->Append( pNumber );
// 				pDict->SetAtArray( "MediaBox", pArray );
// 			}
// 		}
// 	}
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