#include "../../include/che_pdf_document.h"
#include <cassert>

namespace chelib {

CPDF_Document * CPDF_Document::CreateDocument( CPDF_File * pPDFFile )
{
	if ( pPDFFile )
	{
		CAllocator * pAllocator = pPDFFile->GetAllocator();
		CPDF_Document * pDocument = pAllocator->New<CPDF_Document>( pPDFFile, pAllocator );
		if ( pDocument )
		{
			if ( ! pPDFFile->GetTrailerDict() )
			{
				pDocument->CreateCatalogDict();
			}
			pDocument->ParsePageTree();
			pDocument->ParseNameDict();
			pDocument->ParseOutline();
			pDocument->ParsePageLabels();
			return pDocument;
		}
	}
	return nullptr;
}

CPDF_Document::CPDF_Document( CPDF_File * pFile, CAllocator * pAllocator )
	: CObject(pAllocator), mpFile(pFile), mpPageTree(nullptr), mpNameDict(nullptr), mpOutline(nullptr), mpPageLabels(nullptr), mpComponentMgr(nullptr)
{
	mpComponentMgr = GetAllocator()->New<CPDF_ComponentMgr>( GetAllocator() );
}

CPDF_Document::~CPDF_Document()
{
	if ( mpPageTree )
	{
		mpPageTree->GetAllocator()->Delete( mpPageTree );
	}
	if ( mpNameDict )
	{
		mpNameDict->GetAllocator()->Delete( mpNameDict );
	}
	if ( mpOutline )
	{
		mpOutline->GetAllocator()->Delete( mpOutline );
	}
	if ( mpComponentMgr )
	{
		mpComponentMgr->GetAllocator()->Delete( mpComponentMgr );
	}
}

size_t CPDF_Document::GetPageCount() const
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPageCount();
	}
	return 0;
}

CPDF_Page * CPDF_Document::GetPage( size_t index )
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPage( index );
	}
	return nullptr;
}

CPDF_ComponentMgr * CPDF_Document::GetComponentMgr() const
{
	return mpComponentMgr;
}

CPDF_Outline * CPDF_Document::GetOutline()
{
    return mpOutline;
}

bool CPDF_Document::SetDocumentInfo( PDF_DOCUMENT_INFO infoType, const CByteString & str )
{
	if ( mpFile )
	{
		return mpFile->SetInfo( infoType, str );
	}
	return false;
}

bool CPDF_Document::ParsePageTree()
{
	CPDF_DictionaryPtr pDict = mpFile->GetRootDict();
	CPDF_ObjectPtr pObj;
	if ( pDict )
	{
		pObj = pDict->GetElement( "Pages", OBJ_TYPE_DICTIONARY );
		if ( pObj )
		{
			pDict = pObj->GetDictPtr();
		}
		if ( pDict )
		{
			mpPageTree = GetAllocator()->New<CPDF_PageTree>( pDict, mpFile, GetAllocator() );
			return TRUE;
		}
	}
	return false;
}

bool CPDF_Document::ParseNameDict()
{
	if ( mpFile )
	{
		CPDF_DictionaryPtr dictPtr = mpFile->GetRootDict();
		CPDF_ObjectPtr objPtr = dictPtr->GetElement( "Names", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			mpNameDict = GetAllocator()->New<CPDF_NameDict>( GetAllocator() );
			mpNameDict->Parse( objPtr->GetDictPtr() );
			return TRUE;
		}
	}
	return false;
}

bool CPDF_Document::ParseOutline()
{
	if ( mpFile )
	{
		CPDF_DictionaryPtr dictPtr = mpFile->GetRootDict();

		CPDF_ObjectPtr objPtr = dictPtr->GetElement( "Outlines", OBJ_TYPE_REFERENCE );

		if ( objPtr )
		{
			mpOutline = GetAllocator()->New<CPDF_Outline>( GetAllocator() );
			mpOutline->Parse( objPtr->GetRefPtr(), mpNameDict );
			return TRUE;
		}
	}
	return false;
}

bool CPDF_Document::ParsePageLabels()
{
	if ( mpFile )
	{
		CPDF_DictionaryPtr dictPtr = mpFile->GetRootDict();
		CPDF_ObjectPtr objPtr = dictPtr->GetElement("PageLabels", OBJ_TYPE_DICTIONARY);
		if (objPtr)
		{
			mpPageLabels = GetAllocator()->New<CPDF_PageLabels>(objPtr->GetDictPtr(), GetAllocator());
			return TRUE;
		}
	}
	return false;
}

void CPDF_Document::CreateCatalogDict()
{
	if ( mpFile == nullptr )
	{
		return;
	}
	mpFile->CreateCatalogDict();
}

}//namespace