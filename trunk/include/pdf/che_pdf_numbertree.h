#ifndef _CHE_PDF_NUMBERTREE_H_
#define _CHE_PDF_NUMBERTREE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <map>

class CHE_PDF_NumberTree : public CHE_Object
{
public:
	CHE_PDF_NumberTree(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = nullptr)
		: CHE_Object(pAllocator), mDictPtr(dict) {}

	CHE_PDF_ObjectPtr GetObject( int32 num );

	void ParseAll() { Parse(mDictPtr); }

	CHE_PDF_ObjectPtr First(int32 & ret);

	CHE_PDF_ObjectPtr Next(int32 & ret);

private:
	bool Find( int32 num, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet );

	bool Parse(const CHE_PDF_DictionaryPtr & dict);

	CHE_PDF_DictionaryPtr							mDictPtr;
	std::map<int32,CHE_PDF_ObjectPtr>::iterator	mIt;
	std::map<int32,CHE_PDF_ObjectPtr>			mMap;
};

#endif