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
	wchar_t charCode;
    wchar_t ucs;
	uint32  gid;
	uint32  cid;
	int32   kerning;
	FLOAT   width;
	FLOAT   height;
};

class CHE_PDF_Text : public CHE_PDF_ContentObject
{									 
public:
	CHE_PDF_Text( CHE_Allocator * pAllocator = nullptr )
		: CHE_PDF_ContentObject( ContentType_Text, pAllocator ), mpLastKerning( 0 ) {}

	~CHE_PDF_Text(){}

	bool SetTextObject( const CHE_PDF_ObjectPtr & pObj );

	CHE_PDF_ObjectPtr GetTextObject() const { return mpObj; }

	CHE_PDF_ContentObject * Clone() const;

	CHE_Matrix GetTextMatrix() const;

	CHE_Matrix GetCharMatrix( size_t index ) const;

	CHE_Rect GetTextRect() const;

	CHE_Rect GetCharRect( size_t index ) const;

    //计算一个TJ、Tj指令中的所有内容所产生的偏移，包括头尾的kerning，因为头尾的kering在计算文本和字符串矩形的时候会被跳过
    FLOAT GetOffSet() const;

    std::vector<CHE_PDF_TextItem> mItems;

	CHE_PDF_Path * GetGraphPath( size_t index );

private:
	CHE_PDF_ObjectPtr	mpObj;
	FLOAT               mpLastKerning;
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
		FLOAT               value;
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
	CHE_PDF_Path( CHE_Allocator * pAllocator = nullptr )
		: CHE_PDF_ContentObject( ContentType_Path, pAllocator ) {}

	~CHE_PDF_Path();

	std::vector<CHE_PDF_PathItem> mItems;

	void SetPaintType( PDF_PATH_PAINT type ) { mType = type; }

	PDF_PATH_PAINT GetPaintType() const { return mType; }

	void SetFillMode( PDF_FILL_MODE mode ) { mFillMode = mode; }

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
	CHE_PDF_Mark( CHE_Allocator * pAllocator = nullptr )
		: CHE_PDF_ContentObject( ContentType_Mark, pAllocator ), mMarkType( Mark_MP ) {}

	~CHE_PDF_Mark() {}

	void SetMarkType( PDF_MARK_TYPE type ) { mMarkType = type; }

	PDF_MARK_TYPE GetMarkType() const { return mMarkType; }

	void SetTag( const CHE_ByteString & tag ) { mTag = tag; }

	CHE_ByteString GetTag() const { return mTag; }

	void SetProperty( const CHE_PDF_DictionaryPtr & pDict )
	{
		mpDictionary = pDict;
	}

	CHE_ByteString GetPropertyResName() const { return mDictName; }

	void SetPropertyResName( const CHE_ByteString & resName ) { mDictName = resName; }

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
	CHE_PDF_InlineImage( bool bMask, uint32 width, uint32 hight, uint32 bpc, PBYTE pBytes, size_t size, CHE_PDF_ObjectPtr objPtr,
                         CHE_PDF_ColorSpacePtr colorspace, GRAPHICS_STATE_RENDERINTENTS ri, CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_InlineImage();

	CHE_PDF_ContentObject * Clone() const;

	bool IsMask() const { return mbMask; }
	uint32 GetWidth() const { return mWidth; }
	uint32 GetHeight() const { return mHeight; }
	uint32 GetBpc() const { return mBpc; }
	CHE_PDF_ColorSpacePtr GetColorspace() const { return mColorspace; }
	CHE_PDF_ObjectPtr GetDecode() const { return mDecodeObjPtr; }
    GRAPHICS_STATE_RENDERINTENTS GetRenderIntent() const { return mRI; }

	PBYTE GetData() const { return mpData; }
	size_t GetDataSize() const { return mDataSize; }

#ifdef WIN32
	CHE_Bitmap * GetBitmap();
#endif

private:
	bool					mbMask;
	uint32                  mWidth;
	uint32                  mHeight;
	uint32                  mBpc;
	PBYTE                   mpData;
	size_t                  mDataSize;
	CHE_PDF_ColorSpacePtr	mColorspace;
	CHE_PDF_ObjectPtr		mDecodeObjPtr;
    GRAPHICS_STATE_RENDERINTENTS    mRI;
};

class CHE_PDF_ComponentRef : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_ComponentRef( const CHE_ByteString & name, const CHE_PDF_ComponentPtr & componentPtr, CHE_Allocator * pAllocator = nullptr )
		: CHE_PDF_ContentObject( ContentType_Component, pAllocator ), mName( name ), mComponentPtr( componentPtr ) {}

	CHE_ByteString			GetName() const { return mName; }
	CHE_PDF_ComponentPtr	GetComponentPtr() const { return mComponentPtr; }

	CHE_PDF_ContentObject * Clone() const;

private:
	CHE_ByteString			mName;
	CHE_PDF_ComponentPtr	mComponentPtr;
};

#endif