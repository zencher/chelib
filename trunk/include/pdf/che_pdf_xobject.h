#ifndef _CHE_PDF_XOBJECT_H_
#define _CHE_PDF_XOBJECT_H_

#include "../che_bitmap.h"
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

	CHE_PDF_ReferencePtr    GetRef() { return mRefPtr; }
	CHE_PDF_StreamPtr       GetStreamPtr() { return mStmPtr; }
	HE_ULONG                GetWidth() const { return mWidth; }
	HE_ULONG                GetHeight() const { return mHeight; }
	HE_ULONG                GetBPC() const { return mBpc; }
	CHE_PDF_ColorSpacePtr	GetColorspace() const { return mColorspace; }
	HE_BOOL                 IsMask() const { return mbMask; }
	HE_BOOL                 IsInterpolate() const { return mbInterpolate; }

//	CHE_Bitmap *			GetBitmap();

private:
	CHE_PDF_ImageXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = NULL );

// 	CHE_Bitmap *			StreamToBitmap();
// 	CHE_Bitmap *			JPXStreamToBitmap( HE_LPBYTE pData, HE_ULONG size );
// 	CHE_Bitmap *			JpegStreamToBitmap( HE_LPBYTE data, HE_ULONG size );
// 	CHE_Bitmap *			JBig2StreamToBitmap( HE_LPBYTE data, HE_ULONG size, HE_LPBYTE globals = NULL, HE_ULONG globalsSize = 0 );
// 	CHE_Bitmap *			GetStencilMaskingBitmap( HE_LPBYTE pData, HE_ULONG size );
// 	CHE_Bitmap *			GetExplicitMaskingBitmap( CHE_Bitmap * pBitmapOrig, CHE_PDF_StreamPtr & stmPtr );

	CHE_PDF_ReferencePtr    mRefPtr;
	CHE_PDF_StreamPtr       mStmPtr;

	HE_ULONG                mWidth;
	HE_ULONG                mHeight;
	HE_ULONG                mBpc;
	CHE_PDF_ColorSpacePtr	mColorspace;
	HE_BOOL                 mbInterpolate;
	HE_BOOL					mbMask;
	HE_BYTE					mMaskDecode;
	CHE_PDF_ObjectPtr		mMaskPtr;
	CHE_Bitmap *			mpBitmapCache;

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

private:
	CHE_PDF_FormXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_ContentObjectList mList;

	friend class CHE_Allocator;
};

#endif