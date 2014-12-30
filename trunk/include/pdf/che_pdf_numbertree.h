#ifndef _CHE_PDF_NUMBERTREE_H_
#define _CHE_PDF_NUMBERTREE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <unordered_map>

class CHE_PDF_NumberTree : public CHE_Object
{
public:
	CHE_PDF_NumberTree(CHE_Allocator * pAllocator = NULL) : CHE_Object(pAllocator) {};

	HE_VOID Parse( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ObjectPtr GetObject( HE_INT32 num );

private:
	HE_BOOL Find( HE_INT32 num, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet );

	CHE_PDF_ReferencePtr								mRefPtr;
	std::unordered_map<HE_INT32,CHE_PDF_ObjectPtr>		mMap;
};

#endif