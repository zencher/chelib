#include "../../include/pdf/che_pdf_namedict.h"

CHE_PDF_NameDict::CHE_PDF_NameDict(CHE_Allocator * pAllocator /*= NULL*/)
: CHE_Object(pAllocator), mpDestNameTree(NULL), mpAPNameTree(NULL),
mpJSNameTree(NULL), mpPagesNameTree(NULL), mpTemplatesNameTree(NULL),
mpIDSNameTree(NULL), mpURLSNameTree(NULL), mpEFNameTree(NULL),
mpAlPrNameTree(NULL), mpRendNameTree(NULL) {}

CHE_PDF_NameDict::~CHE_PDF_NameDict()
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

HE_VOID CHE_PDF_NameDict::Parse(const CHE_PDF_DictionaryPtr & dict)
{
	if (!dict)
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr = dict->GetElement("Dests", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpDestNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("AP", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpAPNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("JavaScript", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpJSNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("Pages", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpPagesNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("Templates", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpTemplatesNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("IDS", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpIDSNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("URLS", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpURLSNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("EmbeddedFile", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpEFNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("AlternatePresentations", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpAlPrNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}

	objPtr = dict->GetElement("Renditions", OBJ_TYPE_DICTIONARY);
	if (objPtr)
	{
		mpRendNameTree = GetAllocator()->New<CHE_PDF_NameTree>(objPtr->GetDictPtr(), GetAllocator());
	}
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetDest(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpDestNameTree)
	{
		objPtr = mpDestNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetJavaScript(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpJSNameTree)
	{
		objPtr = mpJSNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetAnnotAppear(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpAPNameTree)
	{
		objPtr = mpAPNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetPage(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpPagesNameTree)
	{
		objPtr = mpPagesNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetTemplate(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpTemplatesNameTree)
	{
		objPtr = mpTemplatesNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetIDS(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpIDSNameTree)
	{
		objPtr = mpIDSNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetURL(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpURLSNameTree)
	{
		objPtr = mpURLSNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetEmbeddedFile(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpEFNameTree)
	{
		objPtr = mpEFNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetAlternatePresentation(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpAlPrNameTree)
	{
		objPtr = mpAlPrNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetRendition(const CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpRendNameTree)
	{
		objPtr = mpRendNameTree->GetObject(name);
	}
	return objPtr;
}