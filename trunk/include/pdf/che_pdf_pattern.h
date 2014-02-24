#ifndef _CHE_PDF_PATTERN_H_
#define _CHE_PDF_PATTERN_H_

#include "che_pdf_contentlist.h"
#include "che_pdf_contentlistbuilder.h"
#include "che_pdf_component.h"
#include "che_pdf_componentmgr.h"


class CHE_PDF_Shading;

class CHE_PDF_ShadingPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_Shading * operator->() const { return (CHE_PDF_Shading*)mpCom; }
};

class CHE_PDF_Shading : public CHE_PDF_Component
{
public:
	static CHE_PDF_ShadingPtr Create( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_ShadingPtr Convert( const CHE_PDF_ComponentPtr & componentPtr );

private:
	CHE_PDF_Shading( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator = NULL );

	friend class CHE_Allocator;
};


class CHE_PDF_Tiling;

class CHE_PDF_TilingPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_Tiling * operator->() const { return (CHE_PDF_Tiling*)mpCom; }
};

class CHE_PDF_Tiling : public CHE_PDF_Component
{
public:
	static CHE_PDF_TilingPtr Create( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_TilingPtr Convert( const CHE_PDF_ComponentPtr & componentPtr );

private:
	CHE_PDF_Tiling( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_ContentObjectList & GetList() { return mContentList; }

private:
	HE_BOOL						mbColored;
	HE_UINT32					mTilingType;
	HE_INT32					mXSetp;
	HE_INT32					mYSetp;
	CHE_Matrix					mMatrix;
	CHE_Rect					mBBox;
	CHE_PDF_ContentObjectList	mContentList;

	friend class CHE_Allocator;
};


#endif