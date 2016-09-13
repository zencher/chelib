#ifndef _CHELIB_PDF_NAMEDICT_H_
#define _CHELIB_PDF_NAMEDICT_H_

#include "che_pdf_object.h"
#include "che_pdf_nametree.h"

namespace chelib {

class CPDF_NameDict : public CObject
{
public:
	CPDF_NameDict(CAllocator * pAllocator = nullptr);
	~CPDF_NameDict();

	void Parse(const CPDF_DictionaryPtr & dict);

	CPDF_ObjectPtr GetDest(const CByteString & name);
	CPDF_ObjectPtr GetJavaScript(const CByteString & name);
	CPDF_ObjectPtr GetAnnotAppear(const CByteString & name);
	CPDF_ObjectPtr GetPage(const CByteString & name);
	CPDF_ObjectPtr GetTemplate(const CByteString & name);
	CPDF_ObjectPtr GetIDS(const CByteString & name);
	CPDF_ObjectPtr GetURL(const CByteString & name);
	CPDF_ObjectPtr GetEmbeddedFile(const CByteString & name);
	CPDF_ObjectPtr GetAlternatePresentation(const CByteString & name);
	CPDF_ObjectPtr GetRendition(const CByteString & name);

private:
	CPDF_NameTree *	mpDestNameTree;
	CPDF_NameTree *	mpAPNameTree;
	CPDF_NameTree *	mpJSNameTree;
	CPDF_NameTree *	mpPagesNameTree;
	CPDF_NameTree *	mpTemplatesNameTree;
	CPDF_NameTree *	mpIDSNameTree;
	CPDF_NameTree *	mpURLSNameTree;
	CPDF_NameTree *	mpEFNameTree;
	CPDF_NameTree *	mpAlPrNameTree;
	CPDF_NameTree *	mpRendNameTree;
};

}//namespace

#endif