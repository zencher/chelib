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
	CONTENTRES_FONT = 5
};

class CHE_PDF_ContentResMgr : public CHE_Object
{
public:
	CHE_PDF_ContentResMgr( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpResDict( CHE_PDF_Dictionary::Create(pAllocator) ) {}
	CHE_PDF_ContentResMgr( CHE_PDF_Dictionary * pResDict, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpResDict( pResDict ? pResDict->Clone() : CHE_PDF_Dictionary::Create(pAllocator) ) {}

	~CHE_PDF_ContentResMgr() {}

	HE_VOID SetDict( CHE_PDF_Dictionary * pDict ) { pDict ? ( mpResDict = pDict->Clone() ) : ( mpResDict = CHE_PDF_Dictionary::Create(GetAllocator()) ); }
	CHE_PDF_Dictionary * GetDict() { return mpResDict; }

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, CHE_PDF_Object * pObj );

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name, CHE_Object * pObj );

	HE_BOOL	DeleteName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_PDF_Object * GetResObj( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_PDF_Object * GetResObj( const CHE_ByteString & name );

private:
	CHE_PDF_Dictionary * GetSubDict( PDF_CONTENTRES_TYPE type );

	CHE_PDF_Dictionary * CreateSubDict( PDF_CONTENTRES_TYPE type );

	CHE_ByteString RequestName( CHE_PDF_Dictionary * pSubDict, const CHE_ByteString & name );

	CHE_PDF_Dictionary * mpResDict;
};

#endif