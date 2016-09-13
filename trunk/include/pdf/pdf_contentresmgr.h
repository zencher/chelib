#ifndef _CHELIB_PDF_CONTENTRESMGR_H_
#define _CHELIB_PDF_CONTENTRESMGR_H_

#include "pdf_component.h"

#include <unordered_map>

namespace chelib {

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

class CPDF_ContentResMgr : public CObject
{
public:
	CPDF_ContentResMgr( CAllocator * pAllocator = nullptr ) : CObject(pAllocator) {}

	void SetDict( const CPDF_DictionaryPtr & pDict ) { mpResDict = pDict; }

	CPDF_DictionaryPtr GetDictPtr() const { return mpResDict; }

	CByteString CreateName( PDF_CONTENTRES_TYPE type, const CPDF_ObjectPtr & pObj );

	CByteString CreateName( PDF_CONTENTRES_TYPE type, const CByteString & name, const CPDF_ObjectPtr & pObj );

	bool	DeleteName( PDF_CONTENTRES_TYPE type, const CByteString & name );

	CPDF_ObjectPtr GetResObj( PDF_CONTENTRES_TYPE type, const CByteString & name );

	CPDF_ObjectPtr GetResObj( const CByteString & name );

	//for direct reference component
	CPDF_ComponentPtr GetComponent( const CByteString & name );

	CPDF_ComponentPtr GetComponent( const CByteString & name, PDF_COMPONENT_TYPE type );

	bool	PushComponent( const CByteString & name, const CPDF_ComponentPtr & component );

private:
	CPDF_DictionaryPtr GetSubDict( PDF_CONTENTRES_TYPE type );

	CPDF_DictionaryPtr CreateSubDict( PDF_CONTENTRES_TYPE type );

	CByteString RequestName( const CPDF_DictionaryPtr & pSubDict, const CByteString & name );

	CPDF_DictionaryPtr									mpResDict;
	std::unordered_map<std::string,CPDF_ComponentPtr>	mCompontentsMap;
};

}//namespace

#endif