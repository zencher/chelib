#ifndef _CHELIB_PDF_XOBJECT_H_
#define _CHELIB_PDF_XOBJECT_H_

#include "che_pdf_gstate.h"
#include "che_pdf_contentlist.h"
#include "che_pdf_colorspace.h"
#include "che_pdf_componentmgr.h"

namespace chelib {

class CPDF_ImageXObject;

class CPDF_ImageXObjectPtr : public CPDF_ComponentPtr
{
public:
	CPDF_ImageXObject * operator->() const { return (CPDF_ImageXObject*)mpCom; }
};

class CPDF_ImageXObject : public CPDF_Component
{
public:
	static CPDF_ImageXObjectPtr Create( const CPDF_ReferencePtr & refPtr, CAllocator * pAllocator = nullptr );

	static CPDF_ImageXObjectPtr Convert( const CPDF_ComponentPtr & componentPtr );

	~CPDF_ImageXObject();

	CPDF_StreamPtr       GetStreamPtr() { return mStmPtr; }
	size_t                GetWidth() const { return mWidth; }
	size_t                GetHeight() const { return mHeight; }
	size_t                GetBPC() const { return mBpc; }
	CPDF_ColorSpacePtr	GetColorspace() const { return mColorspace; }
	bool                 IsMask() const { return mbMask; }
	bool                 IsInterpolate() const { return mbInterpolate; }
    PBYTE               GetData();
    size_t                GetSize();
    
    CPDF_ArrayPtr        GetDecodeArray() const { return mDecodeArray; }
    
    CPDF_ArrayPtr		GetColorKeyMask() const { return mColorKeyMaskPtr; }
    CPDF_ImageXObjectPtr GetMaskImagePtr() const { return mMaskImagePtr; }
    CPDF_ImageXObjectPtr GetSoftMaskImagePtr() const { return mSoftMaskImagePtr; }
    
    GRAPHICS_STATE_RENDERINTENTS GetRI() const { return mRI; }

private:
 	CPDF_ImageXObject( const CPDF_ReferencePtr & refPtr, CAllocator * pAllocator = nullptr );
	
	size_t                mWidth;
	size_t                mHeight;
	size_t                mBpc;
    bool                 mbInterpolate;
    CPDF_StreamPtr       mStmPtr;
    CPDF_StreamAcc       mStmAcc;
	CPDF_ColorSpacePtr	mColorspace;
	
    bool					mbMask;
	CPDF_ArrayPtr        mColorKeyMaskPtr;
    CPDF_ImageXObjectPtr mMaskImagePtr;
    CPDF_ImageXObjectPtr mSoftMaskImagePtr;
    
    CPDF_ArrayPtr        mDecodeArray;
    
    GRAPHICS_STATE_RENDERINTENTS mRI;
    
	friend class CAllocator;
};


class CPDF_FormXObject;

class CPDF_FormXObjectPtr : public CPDF_ComponentPtr
{
public:
	CPDF_FormXObject * operator->() const { return (CPDF_FormXObject*)mpCom; }
};

class CPDF_FormXObject : public CPDF_Component
{
public:
	static CPDF_FormXObjectPtr Create( const CPDF_ReferencePtr & refPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );

	static CPDF_FormXObjectPtr Convert( const CPDF_ComponentPtr & componentPtr );

	CPDF_ContentObjectList & GetList() { return mList; }
    
    CMatrix GetMatrix() const { return mMatrix; }
    
    bool IsGroup() const { return mIsGroup; }

private:
	CPDF_FormXObject( const CPDF_ReferencePtr & refPtr, CAllocator * pAllocator = nullptr );

    CMatrix mMatrix;
    
	CPDF_ContentObjectList mList;
    
    bool mIsGroup;

	friend class CAllocator;
};

}//namespace
#endif