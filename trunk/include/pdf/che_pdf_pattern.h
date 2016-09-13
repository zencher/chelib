#ifndef _CHELIB_PDF_PATTERN_H_
#define _CHELIB_PDF_PATTERN_H_

#include "che_pdf_contentlist.h"
#include "che_pdf_contentlistbuilder.h"
#include "che_pdf_component.h"
#include "che_pdf_componentmgr.h"
#include "che_pdf_colorspace.h"
#include "che_pdf_function.h"

namespace chelib {

enum PDF_SHADING_TYPE
{
    SHADING_TYPE_FunctionBase = 1,
    SHADING_TYPE_Axial = 2,
    SHADING_TYPE_Radial = 3,
    SHADING_TYPE_FreeFormGouraudShadedTriangleMesh = 4,
    SHADING_TYPE_LatticeFormGouraudShadedTriangleMesh = 5,
    SHADING_TYPE_CoonsPatchMesh = 6,
    SHADING_TYPE_TensorProductPatchMesh = 7
};

class CPDF_Shading;
class CPDF_Shading_Axial;
class CPDF_Shading_Radial;

class CPDF_ShadingPtr : public CPDF_ComponentPtr
{
public:
	CPDF_Shading * operator->() const { return (CPDF_Shading*)mpCom; }
};

class CPDF_AxialShadingPtr : public CPDF_ShadingPtr
{
public:
    CPDF_Shading_Axial * operator->() const { return (CPDF_Shading_Axial*)mpCom; }
};

class CPDF_RadialShadingPtr : public CPDF_ShadingPtr
{
public:
    CPDF_Shading_Radial * operator->() const { return (CPDF_Shading_Radial*)mpCom; }
};

class CPDF_Shading : public CPDF_Component
{
public:
	static CPDF_ShadingPtr Create( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );
	static CPDF_ShadingPtr Convert( const CPDF_ComponentPtr & componentPtr );

    PDF_SHADING_TYPE GetShadingType() const { return mShadingType; }
    CPDF_ColorSpacePtr GetColorSpace() const { return mColorSpace; }
    CPDF_Color GetBackgroundColor() const { return mBackgroundColor; }
    
    CPDF_AxialShadingPtr GetAixalShadingPtr() const;
    CPDF_RadialShadingPtr GetRadialShadingPtr() const;
    
private:
	CPDF_Shading( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );
    PDF_SHADING_TYPE mShadingType;
    CPDF_ColorSpacePtr mColorSpace;
    CPDF_Color mBackgroundColor;

	friend class CAllocator;
    friend class CPDF_Shading_Axial;
    friend class CPDF_Shading_Radial;
};


class CPDF_Shading_Axial : public CPDF_Shading
{
public:
    CPDF_FunctionPtr GetFunction() const { return mFunction; }
    CPoint GetStartPoint() const { return mStart; }
    CPoint GetEndPoint() const { return mEnd; }
    bool GetStartExtend() const { return mStartExtend; }
    bool GetEndExtend() const { return mEndExtend; }

private:
    CPDF_Shading_Axial( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );
    
    CPDF_FunctionPtr mFunction;
    CPoint mStart;
    CPoint mEnd;
    bool mStartExtend;
    bool mEndExtend;
    
    friend class CAllocator;
};


class CPDF_Shading_Radial : public CPDF_Shading
{
public:
    CPDF_FunctionPtr GetFunction() const { return mFunction; }
    CPoint GetStartPoint() const { return mStart; }
    CPoint GetEndPoint() const { return mEnd; }
    FLOAT GetStartRadius() const { return mStartRadius; }
    FLOAT GetEndRadius() const { return mEndRadius; }
    bool GetStartExtend() const { return mStartExtend; }
    bool GetEndExtend() const { return mEndExtend; }
    
private:
    CPDF_Shading_Radial( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );
    
    CPDF_FunctionPtr mFunction;
    CPoint mStart;
    CPoint mEnd;
    bool mStartExtend;
    bool mEndExtend;
    FLOAT mStartRadius;
    FLOAT mEndRadius;
    
    friend class CAllocator;
};


class CPDF_Tiling;

class CPDF_TilingPtr : public CPDF_ComponentPtr
{
public:
	CPDF_Tiling * operator->() const { return (CPDF_Tiling*)mpCom; }
};

class CPDF_Tiling : public CPDF_Component
{
public:
	static CPDF_TilingPtr Create( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );
	static CPDF_TilingPtr Convert( const CPDF_ComponentPtr & componentPtr );
    
    uint32 GetTilingType() const { return mTilingType; }
    CMatrix GetMatrix() const { return mMatrix; }
    int32 GetXStep() const { return mXSetp; }
    int32 GetYStep() const { return mYSetp; }
    CRect GetBBox() const { return mBBox; }
    bool IsColored() const { return mbColored; }
    
    CPDF_ContentObjectList & GetList() { return mContentList; }
    
private:
	CPDF_Tiling( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator = nullptr );

private:
	bool						mbColored;
	uint32					mTilingType;
	int32					mXSetp;
	int32					mYSetp;
	CMatrix					mMatrix;
	CRect					mBBox;
	CPDF_ContentObjectList	mContentList;

	friend class CAllocator;
};

}//namespace

#endif