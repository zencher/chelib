#ifndef _CHE_PDF_CONTENTOBJS_H_
#define _CHE_PDF_CONTENTOBJS_H_

#include <list>

#include "../che_bitmap.h"
/*#include "che_pdf_gstate.h"*/
#include "che_pdf_contentlist.h"
//#include "che_pdf_contentresmgr.h"
//#include "che_pdf_components.h"
#include "che_pdf_colorspace.h"

//#define CONTENTOBJ_FLAG_MODIFIED	0x00000001



class CHE_PDF_Path;

class CHE_PDF_TextItem
{
public:
	HE_WCHAR charCode;
	HE_ULONG gid;
	HE_ULONG cid;
	HE_WCHAR ucs;
	HE_INT32 kerning;
	HE_FLOAT width;
	HE_FLOAT height;
};

class CHE_PDF_Text : public CHE_PDF_ContentObject
{									 
public:
	CHE_PDF_Text( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_ContentObject( ContentType_Text, pAllocator ), mpLastKerning( 0 ) {}

	~CHE_PDF_Text(){}

	HE_BOOL SetTextObject( const CHE_PDF_ObjectPtr & pObj );

	CHE_PDF_ObjectPtr GetTextObject() const { return mpObj; }

	CHE_PDF_ContentObject * Clone() const;

	CHE_Matrix GetTextMatrix() const;

	CHE_Matrix GetCharMatrix( HE_ULONG index ) const;

	CHE_Rect GetTextRect() const;

	CHE_Rect GetCharRect( HE_ULONG index ) const;

	//计算一个TJ、Tj指令中的所有内容所产生的偏移，包括头尾的kerning，因为头尾的kering在计算文本和字符串矩形的时候会被跳过
	HE_FLOAT GetOffSet() const;


	std::vector<CHE_PDF_TextItem> mItems;

	CHE_PDF_Path * GetGraphPath( HE_ULONG index );

private:
	CHE_PDF_ObjectPtr	mpObj;
	HE_FLOAT			mpLastKerning;
};



enum PDF_PATHITEM_TYPE
{
	PathItem_MoveTo		= 0,
	PathItem_LineTo		= 1,
	PathItem_CurveTo	= 2,
	PathItem_Rectangle	= 3,
	PathItem_Close		= 4
};

class CHE_PDF_PathItem
{
public:
	union
	{
		PDF_PATHITEM_TYPE	type;
		HE_FLOAT			value;
	};
};

enum PDF_PATH_PAINT
{
	Paint_Fill			= 0,
	Paint_Stroke		= 1,
	Paint_FillStroke	= 2,
	Paint_None			= 3
};

enum PDF_FILL_MODE
{
	Mode_Nonzero		= 0,
	Mode_EvenOdd		= 1
};

class CHE_PDF_Path : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_Path( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_ContentObject( ContentType_Path, pAllocator ) {}

	~CHE_PDF_Path();

	std::vector<CHE_PDF_PathItem> mItems;

	HE_VOID SetPaintType( PDF_PATH_PAINT type ) { mType = type; }

	PDF_PATH_PAINT GetPaintType() const { return mType; }

	HE_VOID SetFillMode( PDF_FILL_MODE mode ) { mFillMode = mode; }

	PDF_FILL_MODE GetFillMode() const { return mFillMode; }

	CHE_PDF_ContentObject * Clone() const;
	
private:
	PDF_PATH_PAINT mType;
	PDF_FILL_MODE mFillMode;
};

enum PDF_MARK_TYPE
{
	Mark_MP		= 0,
	Mark_DP		= 1,
	Mark_BMC	= 2,
	Mark_BDC	= 3,
	Mark_EMC	= 4
};

class CHE_PDF_Mark : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_Mark( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_ContentObject( ContentType_Mark, pAllocator ), mMarkType( Mark_MP ) {}

	~CHE_PDF_Mark() {}

	HE_VOID SetMarkType( PDF_MARK_TYPE type ) { mMarkType = type; }

	PDF_MARK_TYPE GetMarkType() const { return mMarkType; }

	HE_VOID SetTag( const CHE_ByteString & tag ) { mTag = tag; }

	CHE_ByteString GetTag() const { return mTag; }

	HE_VOID SetProperty( const CHE_PDF_DictionaryPtr & pDict )
	{
		mpDictionary = pDict;
	}

	CHE_ByteString GetPropertyResName() const { return mDictName; }

	HE_VOID SetPropertyResName( const CHE_ByteString & resName ) { mDictName = resName; }

	CHE_PDF_DictionaryPtr GetProperty() const { return mpDictionary; }

	CHE_PDF_ContentObject * Clone() const
	{
		CHE_PDF_Mark * pRet = GetAllocator()->New<CHE_PDF_Mark>( GetAllocator() );
		pRet->mMarkType = mMarkType;
		pRet->mTag = mTag;
		if ( mpDictionary )
		{
			pRet->mpDictionary = mpDictionary->Clone();
		}
		return pRet;
	}
private:
	PDF_MARK_TYPE mMarkType;
	CHE_ByteString mTag;
	CHE_ByteString mDictName;
	CHE_PDF_DictionaryPtr mpDictionary;
};

class CHE_PDF_InlineImage : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_InlineImage(	HE_BOOL bMask, HE_ULONG width, HE_ULONG hight, HE_ULONG bpc, HE_LPBYTE pBytes, HE_ULONG size, CHE_PDF_ObjectPtr objPtr,
                            CHE_PDF_ColorSpacePtr colorspace, GRAPHICS_STATE_RENDERINTENTS ri, CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_InlineImage();

	CHE_PDF_ContentObject * Clone() const;

	HE_BOOL	IsMask() const { return mbMask; }
	HE_ULONG GetWidth() const { return mWidth; }
	HE_ULONG GetHeight() const { return mHeight; }
	HE_ULONG GetBpc() const { return mBpc; }
	CHE_PDF_ColorSpacePtr GetColorspace() const { return mColorspace; }
	CHE_PDF_ObjectPtr GetDecode() const { return mDecodeObjPtr; }
    GRAPHICS_STATE_RENDERINTENTS GetRenderIntent() const { return mRI; }

	HE_LPBYTE GetData() const { return mpData; }
	HE_ULONG GetDataSize() const { return mDataSize; }

#ifdef _WIN_32_
	CHE_Bitmap * GetBitmap();
#endif

private:
	HE_BOOL					mbMask;
	HE_ULONG				mWidth;
	HE_ULONG				mHeight;
	HE_ULONG				mBpc;
	HE_LPBYTE				mpData;
	HE_ULONG				mDataSize;
	CHE_PDF_ColorSpacePtr	mColorspace;
	CHE_PDF_ObjectPtr		mDecodeObjPtr;
    GRAPHICS_STATE_RENDERINTENTS    mRI;
};

class CHE_PDF_ComponentRef : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_ComponentRef( const CHE_ByteString & name, const CHE_PDF_ComponentPtr & componentPtr, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_ContentObject( ContentType_Component, pAllocator ), mName( name ), mComponentPtr( componentPtr ) {}

	CHE_ByteString			GetName() const { return mName; }
	CHE_PDF_ComponentPtr	GetComponentPtr() const { return mComponentPtr; }

	CHE_PDF_ContentObject * Clone() const;

private:
	CHE_ByteString			mName;
	CHE_PDF_ComponentPtr	mComponentPtr;
};

#endif