#ifndef _CHE_PDF_XOBJECT_H_
#define _CHE_PDF_XOBJECT_H_

//#include "../che_bitmap.h"
//#include "../che_graphics.h"
#include "che_pdf_gstate.h"
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
	static CHE_PDF_ImageXObjectPtr Create( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = nullptr );

	static CHE_PDF_ImageXObjectPtr Convert( const CHE_PDF_ComponentPtr & componentPtr );

	~CHE_PDF_ImageXObject();

	CHE_PDF_StreamPtr       GetStreamPtr() { return mStmPtr; }
	size_t                GetWidth() const { return mWidth; }
	size_t                GetHeight() const { return mHeight; }
	size_t                GetBPC() const { return mBpc; }
	CHE_PDF_ColorSpacePtr	GetColorspace() const { return mColorspace; }
	bool                 IsMask() const { return mbMask; }
	bool                 IsInterpolate() const { return mbInterpolate; }
    PBYTE               GetData();
    size_t                GetSize();
    
    CHE_PDF_ArrayPtr        GetDecodeArray() const { return mDecodeArray; }
    
    CHE_PDF_ArrayPtr		GetColorKeyMask() const { return mColorKeyMaskPtr; }
    CHE_PDF_ImageXObjectPtr GetMaskImagePtr() const { return mMaskImagePtr; }
    CHE_PDF_ImageXObjectPtr GetSoftMaskImagePtr() const { return mSoftMaskImagePtr; }
    
    GRAPHICS_STATE_RENDERINTENTS GetRI() const { return mRI; }

private:
 	CHE_PDF_ImageXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = nullptr );
	
	size_t                mWidth;
	size_t                mHeight;
	size_t                mBpc;
    bool                 mbInterpolate;
    CHE_PDF_StreamPtr       mStmPtr;
    CHE_PDF_StreamAcc       mStmAcc;
	CHE_PDF_ColorSpacePtr	mColorspace;
	
    bool					mbMask;
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
	static CHE_PDF_FormXObjectPtr Create( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator = nullptr );

	static CHE_PDF_FormXObjectPtr Convert( const CHE_PDF_ComponentPtr & componentPtr );

	CHE_PDF_ContentObjectList & GetList() { return mList; }
    
    CHE_Matrix GetMatrix() const { return mMatrix; }
    
    bool IsGroup() const { return mIsGroup; }

private:
	CHE_PDF_FormXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = nullptr );

    CHE_Matrix mMatrix;
    
	CHE_PDF_ContentObjectList mList;
    
    bool mIsGroup;

	friend class CHE_Allocator;
};

#endif