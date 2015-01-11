#ifndef _CHE_PDF_COMPONENT_H_
#define _CHE_PDF_COMPONENT_H_


#include "che_pdf_objects.h"


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


class CHE_PDF_Component : public CHE_Object
{
public:
	PDF_COMPONENT_TYPE		GetType() const { return mType; }

	CHE_PDF_ObjectPtr		GetRootObject() const;

	HE_VOID					Release();

	HE_BOOL					IsError() const { return (mError == COMPONENT_ERROR_NOERROR) ? FALSE : TRUE; }
	
protected:
    CHE_PDF_Component( PDF_COMPONENT_TYPE type, CHE_Allocator * pAllocator = NULL )
    : CHE_Object(pAllocator), mType(type), mError(COMPONENT_ERROR_NOERROR) {}
    
	CHE_PDF_Component( PDF_COMPONENT_TYPE type, const CHE_PDF_ObjectPtr & rootObject, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mType(type), mError(COMPONENT_ERROR_NOERROR), mRootObject(rootObject) {}

	HE_VOID					SetError(PDF_COMPONENT_ERROR error) { mError = error; }

	PDF_COMPONENT_TYPE		mType;
	PDF_COMPONENT_ERROR		mError;
	CHE_RefCount			mRefs;
	CHE_PDF_ObjectPtr		mRootObject;

	friend class CHE_Allocator;
	friend class CHE_PDF_ComponentPtr;
};


class CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_ComponentPtr() : mpCom( NULL ) {}

	CHE_PDF_ComponentPtr( const CHE_PDF_ComponentPtr & ptr );

	~CHE_PDF_ComponentPtr();

	CHE_PDF_ComponentPtr operator = ( const CHE_PDF_ComponentPtr & ptr );

	bool operator!() const { return mpCom ? FALSE : TRUE ; }

	operator HE_BOOL() const { return mpCom ? TRUE : FALSE ; }

	CHE_PDF_Component * operator->() const { return mpCom; }

	CHE_PDF_Component * GetPointer() const { return mpCom; } 

	HE_VOID	Reset( CHE_PDF_Component * pCom = NULL );

// 	HE_BOOL IsExtStateComponent() const;
// 
// 	HE_BOOL IsColorSpaceComponent() const;

protected:
	CHE_PDF_Component * mpCom;
};

#endif