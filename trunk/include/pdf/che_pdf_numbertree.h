#ifndef _CHE_PDF_NUMBERTREE_H_
#define _CHE_PDF_NUMBERTREE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <map>

class CHE_PDF_NumberTree : public CHE_Object
{
public:
	CHE_PDF_NumberTree(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = NULL)
		: CHE_Object(pAllocator), mDictPtr(dict) {}

	CHE_PDF_ObjectPtr GetObject( HE_INT32 num );

	HE_VOID ParseAll() { Parse(mDictPtr); }

	CHE_PDF_ObjectPtr First(HE_INT32 & ret);

	CHE_PDF_ObjectPtr Next(HE_INT32 & ret);

private:
	HE_BOOL Find( HE_INT32 num, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet );

	HE_BOOL Parse(const CHE_PDF_DictionaryPtr & dict);

	CHE_PDF_DictionaryPtr							mDictPtr;
	std::map<HE_INT32,CHE_PDF_ObjectPtr>::iterator	mIt;
	std::map<HE_INT32,CHE_PDF_ObjectPtr>			mMap;
};

#endif