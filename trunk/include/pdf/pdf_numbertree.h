#ifndef _CHELIB_PDF_NUMBERTREE_H_
#define _CHELIB_PDF_NUMBERTREE_H_

#include "../base.h"
#include "pdf_objects.h"

#include <map>

namespace chelib {

class CPDF_NumberTree : public CObject
{
public:
	CPDF_NumberTree(const CPDF_DictionaryPtr & dict, CAllocator * pAllocator = nullptr)
		: CObject(pAllocator), mDictPtr(dict) {}

	CPDF_ObjectPtr GetObject( int32 num );

	void ParseAll() { Parse(mDictPtr); }

	CPDF_ObjectPtr First(int32 & ret);
	CPDF_ObjectPtr Next(int32 & ret);

private:
	bool Find( int32 num, const CPDF_DictionaryPtr & dict, CPDF_ObjectPtr & objRet );

	bool Parse(const CPDF_DictionaryPtr & dict);

	CPDF_DictionaryPtr                          mDictPtr;
	std::map<int32,CPDF_ObjectPtr>::iterator    mIt;
	std::map<int32,CPDF_ObjectPtr>              mMap;
};

}//namespace

#endif