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

HE_VOID CHE_PDF_NameDict::Parse(CHE_PDF_DictionaryPtr & dict)
{
	if (!dict)
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr = dict->GetElement("Dests", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpDestNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpDestNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("AP", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpAPNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpAPNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("JavaScript", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpJSNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpJSNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("Pages", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpPagesNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpPagesNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("Templates", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpTemplatesNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpTemplatesNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("IDS", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpIDSNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpIDSNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("URLS", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpURLSNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpURLSNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("EmbeddedFile", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpEFNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpEFNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("AlternatePresentations", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpAlPrNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpAlPrNameTree->Parse(objPtr->GetRefPtr());
	}

	objPtr = dict->GetElement("Renditions", OBJ_TYPE_REFERENCE);
	if (objPtr)
	{
		mpRendNameTree = GetAllocator()->New<CHE_PDF_NameTree>(GetAllocator());
		mpRendNameTree->Parse(objPtr->GetRefPtr());
	}
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetDest(CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpDestNameTree)
	{
		objPtr = mpDestNameTree->GetObject(name);
	}
	return objPtr;
}

CHE_PDF_ObjectPtr CHE_PDF_NameDict::GetJavaScript(CHE_ByteString & name)
{
	CHE_PDF_ObjectPtr objPtr;
	if (mpDestNameTree)
	{
		objPtr = mpJSNameTree->GetObject(name);
	}
	return objPtr;
}