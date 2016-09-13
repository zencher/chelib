#include "../../include/pdf/che_pdf_namedict.h"

namespace chelib {

CPDF_NameDict::CPDF_NameDict(CAllocator * pAllocator /*= nullptr*/)
  : CObject(pAllocator), mpDestNameTree(nullptr), mpAPNameTree(nullptr),
  mpJSNameTree(nullptr), mpPagesNameTree(nullptr), mpTemplatesNameTree(nullptr),
  mpIDSNameTree(nullptr), mpURLSNameTree(nullptr), mpEFNameTree(nullptr),
  mpAlPrNameTree(nullptr), mpRendNameTree(nullptr) {}

CPDF_NameDict::~CPDF_NameDict()
{
	if (mpDestNameTree)
	{
		mpDestNameTree->GetAllocator()->Delete(mpDestNameTree);
	}
	if (mpAPNameTree)
	{
		mpAPNameTree->GetAllocator()->Delete(mpAPNameTree);
	}
	if (mpJSNameTree)
	{
		mpJSNameTree->GetAllocator()->Delete(mpJSNameTree);
	}
	if (mpPagesNameTree)
	{
		mpPagesNameTree->GetAllocator()->Delete(mpPagesNameTree);
	}
	if (mpTemplatesNameTree)
	{
		mpTemplatesNameTree->GetAllocator()->Delete(mpTemplatesNameTree);
	}
	if (mpIDSNameTree)
	{
		mpIDSNameTree->GetAllocator()->Delete(mpIDSNameTree);
	}
	if (mpURLSNameTree)
	{
		mpURLSNameTree->GetAllocator()->Delete(mpURLSNameTree);
	}
	if (mpEFNameTree)
	{
		mpEFNameTree->GetAllocator()->Delete(mpEFNameTree);
	}
	if (mpAlPrNameTree)
	{
		mpAlPrNameTree->GetAllocator()->Delete(mpAlPrNameTree);
	}
	if (mpRendNameTree)
	{
		mpRendNameTree->GetAllocator()->Delete(mpRendNameTree);
	}
}

void CPDF_NameDict::Parse(const CPDF_DictionaryPtr & dict)
{
	if (!dict)
	{
		return;
	}

	CPDF_ObjectPtr objPtr = dict->GetElement("Dests", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpDestNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("AP", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpAPNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("JavaScript", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpJSNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("Pages", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpPagesNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("Templates", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpTemplatesNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("IDS", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpIDSNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("URLS", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpURLSNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("EmbeddedFile", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpEFNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("AlternatePresentations", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpAlPrNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("Renditions", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpRendNameTree = GetAllocator()->New<CPDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}
}

CPDF_ObjectPtr CPDF_NameDict::GetDest(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpDestNameTree)
	{
		objPtr = mpDestNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetJavaScript(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpJSNameTree)
	{
		objPtr = mpJSNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetAnnotAppear(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpAPNameTree)
	{
		objPtr = mpAPNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetPage(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpPagesNameTree)
	{
		objPtr = mpPagesNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetTemplate(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpTemplatesNameTree)
	{
		objPtr = mpTemplatesNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetIDS(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpIDSNameTree)
	{
		objPtr = mpIDSNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetURL(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpURLSNameTree)
	{
		objPtr = mpURLSNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetEmbeddedFile(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpEFNameTree)
	{
		objPtr = mpEFNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetAlternatePresentation(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpAlPrNameTree)
	{
		objPtr = mpAlPrNameTree->GetObject(name);
	}
	return objPtr;
}

CPDF_ObjectPtr CPDF_NameDict::GetRendition(const CByteString & name)
{
	CPDF_ObjectPtr objPtr;
	if (mpRendNameTree)
	{
		objPtr = mpRendNameTree->GetObject(name);
	}
	return objPtr;
}

}//namespace