#ifndef _CHE_PDF_NAMEDICT_H_
#define _CHE_PDF_NAMEDICT_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_objects.h"
#include "che_pdf_nametree.h"

class CHE_PDF_NameDict : public CHE_Object
{
public:
	CHE_PDF_NameDict(CHE_Allocator * pAllocator = NULL);

	~CHE_PDF_NameDict();

	HE_VOID Parse(const CHE_PDF_DictionaryPtr & dict);

	CHE_PDF_ObjectPtr GetDest(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetJavaScript(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetAnnotAppear(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetPage(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetTemplate(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetIDS(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetURL(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetEmbeddedFile(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetAlternatePresentation(const CHE_ByteString & name);

	CHE_PDF_ObjectPtr GetRendition(const CHE_ByteString & name);

private:
	CHE_PDF_NameTree *	mpDestNameTree;
	CHE_PDF_NameTree *	mpAPNameTree;
	CHE_PDF_NameTree *	mpJSNameTree;
	CHE_PDF_NameTree *	mpPagesNameTree;
	CHE_PDF_NameTree *	mpTemplatesNameTree;
	CHE_PDF_NameTree *	mpIDSNameTree;
	CHE_PDF_NameTree *	mpURLSNameTree;
	CHE_PDF_NameTree *	mpEFNameTree;
	CHE_PDF_NameTree *	mpAlPrNameTree;
	CHE_PDF_NameTree *	mpRendNameTree;
};

#endif