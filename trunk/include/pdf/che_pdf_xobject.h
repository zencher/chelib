#ifndef _CHE_PDF_XOBJECT_H_
#define _CHE_PDF_XOBJECT_H_

#include "../che_bitmap.h"
#include "../che_graphics.h"
#include "che_pdf_contentlist.h"
#include "che_pdf_colorspace.h"
#include "che_pdf_componentmgr.h"


class CHE_PDF_ImageXObject;

class CHE_PDF_ImageXObjectPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_ImageXObject * operator->() const { return (CHE_PDF_ImageXObject*)mpCom; }
};

class CHE_PDF_ImageXObject : public CHE_PDF_Component
{
public:
	static CHE_PDF_ImageXObjectPtr Create( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_ImageXObjectPtr Convert( const CHE_PDF_ComponentPtr & componentPtr );

	~CHE_PDF_ImageXObject();

	CHE_PDF_StreamPtr       GetStreamPtr() { return mStmPtr; }
	HE_ULONG                GetWidth() const { return mWidth; }
	HE_ULONG                GetHeight() const { return mHeight; }
	HE_ULONG                GetBPC() const { return mBpc; }
	CHE_PDF_ColorSpacePtr	GetColorspace() const { return mColorspace; }
	HE_BOOL                 IsMask() const { return mbMask; }
	HE_BOOL                 IsInterpolate() const { return mbInterpolate; }
    HE_LPBYTE               GetData();
    HE_ULONG                GetSize();
    
    CHE_PDF_ArrayPtr        GetDecodeArray() const { return mDecodeArray; }
    
    CHE_PDF_ArrayPtr		GetColorKeyMask() const { return mColorKeyMaskPtr; }
    CHE_PDF_ImageXObjectPtr GetMaskImagePtr() const { return mMaskImagePtr; }
    CHE_PDF_ImageXObjectPtr GetSoftMaskImagePtr() const { return mSoftMaskImagePtr; }
    
    GRAPHICS_STATE_RENDERINTENTS GetRI() const { return mRI; }

private:
 	CHE_PDF_ImageXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = NULL );
	
	HE_ULONG                mWidth;
	HE_ULONG                mHeight;
	HE_ULONG                mBpc;
    HE_BOOL                 mbInterpolate;
    CHE_PDF_StreamPtr       mStmPtr;
    CHE_PDF_StreamAcc       mStmAcc;
	CHE_PDF_ColorSpacePtr	mColorspace;
	
    HE_BOOL					mbMask;
	CHE_PDF_ArrayPtr        mColorKeyMaskPtr;
    CHE_PDF_ImageXObjectPtr mMaskImagePtr;
    CHE_PDF_ImageXObjectPtr mSoftMaskImagePtr;
    
    CHE_PDF_ArrayPtr        mDecodeArray;
    
    GRAPHICS_STATE_RENDERINTENTS mRI;
    
	friend class CHE_Allocator;
};


class CHE_PDF_FormXObject;

class CHE_PDF_FormXObjectPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_FormXObject * operator->() const { return (CHE_PDF_FormXObject*)mpCom; }
};

class CHE_PDF_FormXObject : public CHE_PDF_Component
{
public:
	static CHE_PDF_FormXObjectPtr Create( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_FormXObjectPtr Convert( const CHE_PDF_ComponentPtr & componentPtr );

	CHE_PDF_ContentObjectList & GetList() { return mList; }
    
    CHE_Matrix GetMatrix() const { return mMatrix; }

private:
	CHE_PDF_FormXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = NULL );

    CHE_Matrix mMatrix;
    
	CHE_PDF_ContentObjectList mList;

	friend class CHE_Allocator;
};

#endif