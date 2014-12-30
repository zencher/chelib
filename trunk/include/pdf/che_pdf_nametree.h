#ifndef _CHE_PDF_NAMETREE_H_
#define _CHE_PDF_NAMETREE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <string>
#include <unordered_map>

class CHE_PDF_NameTree : public CHE_Object
{
public:
	CHE_PDF_NameTree( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};

	HE_VOID Parse( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ObjectPtr GetObject( const CHE_ByteString & name );

private:
	HE_BOOL Find( const string & name, const CHE_PDF_DictionaryPtr & dict, CHE_PDF_ObjectPtr & objRet );

	CHE_PDF_ReferencePtr								mRefPtr;
	std::unordered_map<std::string,CHE_PDF_ObjectPtr>	mMap;
};

#endif