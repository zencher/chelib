#ifndef _CHE_PDF_CONTENTRESMGR_H_
#define _CHE_PDF_CONTENTRESMGR_H_

#include "che_pdf_objects.h"
#include "che_pdf_component.h"

#include <unordered_map>

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
	CHE_PDF_ContentResMgr( CHE_Allocator * pAllocator = nullptr ) : CHE_Object(pAllocator) {}

	void SetDict( const CHE_PDF_DictionaryPtr & pDict ) { mpResDict = pDict; }

	CHE_PDF_DictionaryPtr GetDictPtr() const { return mpResDict; }

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, const CHE_PDF_ObjectPtr & pObj );

	CHE_ByteString CreateName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name, const CHE_PDF_ObjectPtr & pObj );

	bool	DeleteName( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_PDF_ObjectPtr GetResObj( PDF_CONTENTRES_TYPE type, const CHE_ByteString & name );

	CHE_PDF_ObjectPtr GetResObj( const CHE_ByteString & name );

	//for direct reference component
	CHE_PDF_ComponentPtr GetComponent( const CHE_ByteString & name );

	CHE_PDF_ComponentPtr GetComponent( const CHE_ByteString & name, PDF_COMPONENT_TYPE type );

	bool	PushComponent( const CHE_ByteString & name, const CHE_PDF_ComponentPtr & component );

private:
	CHE_PDF_DictionaryPtr GetSubDict( PDF_CONTENTRES_TYPE type );

	CHE_PDF_DictionaryPtr CreateSubDict( PDF_CONTENTRES_TYPE type );

	CHE_ByteString RequestName( const CHE_PDF_DictionaryPtr & pSubDict, const CHE_ByteString & name );

	CHE_PDF_DictionaryPtr									mpResDict;
	std::unordered_map<std::string,CHE_PDF_ComponentPtr>	mCompontentsMap;
};

#endif