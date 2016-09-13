#ifndef _CHELIB_PDF_NAMETREE_H_
#define _CHELIB_PDF_NAMETREE_H_

#include "pdf_object.h"

#include <string>
#include <unordered_map>

namespace chelib {

class CPDF_NameTree : public CObject
{
public:
	CPDF_NameTree( CPDF_DictionaryPtr dictPtr, CAllocator * pAllocator = nullptr )
      : CObject( pAllocator ), mDictPtr( dictPtr ) {};

	CPDF_ObjectPtr GetObject( const CByteString & name );

private:
	bool Find( const std::string & name, const CPDF_DictionaryPtr & dict, CPDF_ObjectPtr & objRet );

	CPDF_DictionaryPtr								mDictPtr;
	std::unordered_map<std::string,CPDF_ObjectPtr>	mMap;
};

}//namespace

#endif