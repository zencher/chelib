#ifndef _CHE_PDF_CONTENTRESMGR_H_
#define _CHE_PDF_CONTENTRESMGR_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

enum PDF_CONTENTRES_TYPE
{
	CONTENTRES_EXTGSTATE = 0,
	CONTENTRES_COLORSPACE = 1,
	CONTENTRES_PATTERN = 2,
	CONTENTRES_SHADING = 3,
	CONTENTRES_XOBJECT = 4,
	CONTENTRES_FONT = 5,
	CONTENTRES_PROPERTIES = 6
};

class CHE_PDF_ContentResMgr : public CHE_Object
{
public:
	CHE_PDF_ContentResMgr( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpResDict( CHE_PDF_Dictionary::Create(  pAllocator) ) {}
	
	CHE_PDF_ContentResMgr( const CHE_PDF_DictionaryPtr & pResDict, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpResDict( pResDict ) {}

	HE_VOID SetDict( const CHE_PDF_DictionaryPtr & pDict ) { mpResDict = pDict; }

	CHE_PDF_DictionaryPtr GetDictPtr() { return mpResDict; }

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, const CHE_PDF_ObjectPtr & pObj );

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name, const CHE_PDF_ObjectPtr & pObj );

	HE_BOOL	DeleteName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_PDF_ObjectPtr GetResObj( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_PDF_ObjectPtr GetResObj( const CHE_ByteString & name );

private:
	CHE_PDF_DictionaryPtr GetSubDict( PDF_CONTENTRES_TYPE type );

	CHE_PDF_DictionaryPtr CreateSubDict( PDF_CONTENTRES_TYPE type );

	CHE_ByteString RequestName( const CHE_PDF_DictionaryPtr & pSubDict, const CHE_ByteString & name );

	CHE_PDF_DictionaryPtr mpResDict;
};

#endif