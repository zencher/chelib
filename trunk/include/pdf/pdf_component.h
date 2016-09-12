#ifndef _CHELIB_PDF_COMPONENT_H_
#define _CHELIB_PDF_COMPONENT_H_

#include "pdf_objects.h"

namespace chelib {

enum PDF_COMPONENT_TYPE
{
	COMPONENT_TYPE_ExtGState	= 0x00,
	COMPONENT_TYPE_ColorSpace	= 0x01,
	COMPONENT_TYPE_Tiling		= 0x02,
	COMPONENT_TYPE_Shading		= 0x03,
	COMPONENT_TYPE_FormXObject	= 0x04,
	COMPONENT_TYPE_ImageXObject	= 0x05,
	COMPONENT_TYPE_Font			= 0x06,
	COMPONENT_TYPE_ProcSet		= 0x07,
	COMPONENT_TYPE_Properties	= 0x08,
	COMPONENT_TYPE_Function		= 0x09,
	COMPONENT_TYPE_Action		= 0x10,
	COMPONENT_TYPE_Appearance	= 0x11,
	COMPONENT_TYPE_Destination	= 0x12
};


enum PDF_COMPONENT_ERROR
{
	COMPONENT_ERROR_NOERROR,
	COMPONENT_ERROR_NOTCOMPONENT,
	COMPONENT_ERROR_CONSTRUCTION
};


class CPDF_Component : public CObject
{
public:
	PDF_COMPONENT_TYPE		GetType() const { return mType; }

	CPDF_ObjectPtr		GetRootObject() const;

	void					Release();

	bool					IsError() const { return (mError == COMPONENT_ERROR_NOERROR) ? false : TRUE; }
	
protected:
    CPDF_Component( PDF_COMPONENT_TYPE type, CAllocator * pAllocator = nullptr )
    : CObject(pAllocator), mType(type), mError(COMPONENT_ERROR_NOERROR) {}
    
	CPDF_Component( PDF_COMPONENT_TYPE type, const CPDF_ObjectPtr & rootObject, CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mType(type), mError(COMPONENT_ERROR_NOERROR), mRootObject(rootObject) {}

	void					SetError(PDF_COMPONENT_ERROR error) { mError = error; }

	PDF_COMPONENT_TYPE		mType;
	PDF_COMPONENT_ERROR		mError;
	CRefCount			mRefs;
	CPDF_ObjectPtr		mRootObject;

	friend class CAllocator;
	friend class CPDF_ComponentPtr;
};


class CPDF_ComponentPtr
{
public:
	CPDF_ComponentPtr() : mpCom( nullptr ) {}

	CPDF_ComponentPtr( const CPDF_ComponentPtr & ptr );

	~CPDF_ComponentPtr();

	CPDF_ComponentPtr operator = ( const CPDF_ComponentPtr & ptr );

	bool operator!() const { return mpCom ? false : TRUE ; }

	operator bool() const { return mpCom ? TRUE : false ; }

	CPDF_Component * operator->() const { return mpCom; }

	CPDF_Component * GetPointer() const { return mpCom; } 

	void	Reset( CPDF_Component * pCom = nullptr );

// 	bool IsExtStateComponent() const;
// 
// 	bool IsColorSpaceComponent() const;

protected:
	CPDF_Component * mpCom;
};

}//namespace

#endif