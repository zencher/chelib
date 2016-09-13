#ifndef _CHELIB_PDF_CONTENTOBJS_H_
#define _CHELIB_PDF_CONTENTOBJS_H_

#include "pdf_gstate.h"
#include "pdf_contentlist.h"
#include "pdf_colorspace.h"

#include <list>

namespace chelib {

class CPDF_Path;

class CPDF_TextItem
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

class CPDF_Text : public CPDF_ContentObject
{									 
public:
	CPDF_Text( CAllocator * pAllocator = nullptr )
		: CPDF_ContentObject( ContentType_Text, pAllocator ), mpLastKerning( 0 ) {}

	~CPDF_Text(){}

	bool SetTextObject( const CPDF_ObjectPtr & pObj );

	CPDF_ObjectPtr GetTextObject() const { return mpObj; }

	CPDF_ContentObject * Clone() const;

	CMatrix GetTextMatrix() const;

	CMatrix GetCharMatrix( size_t index ) const;

	CRect GetTextRect() const;

	CRect GetCharRect( size_t index ) const;

    //计算一个TJ、Tj指令中的所有内容所产生的偏移，包括头尾的kerning，因为头尾的kering在计算文本和字符串矩形的时候会被跳过
    FLOAT GetOffSet() const;

    std::vector<CPDF_TextItem> mItems;

	CPDF_Path * GetGraphPath( size_t index );

private:
	CPDF_ObjectPtr	mpObj;
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

class CPDF_PathItem
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

class CPDF_Path : public CPDF_ContentObject
{
public:
	CPDF_Path( CAllocator * pAllocator = nullptr )
		: CPDF_ContentObject( ContentType_Path, pAllocator ) {}

	~CPDF_Path();

	std::vector<CPDF_PathItem> mItems;

	void SetPaintType( PDF_PATH_PAINT type ) { mType = type; }

	PDF_PATH_PAINT GetPaintType() const { return mType; }

	void SetFillMode( PDF_FILL_MODE mode ) { mFillMode = mode; }

	PDF_FILL_MODE GetFillMode() const { return mFillMode; }

	CPDF_ContentObject * Clone() const;
	
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

class CPDF_Mark : public CPDF_ContentObject
{
public:
	CPDF_Mark( CAllocator * pAllocator = nullptr )
		: CPDF_ContentObject( ContentType_Mark, pAllocator ), mMarkType( Mark_MP ) {}

	~CPDF_Mark() {}

	void SetMarkType( PDF_MARK_TYPE type ) { mMarkType = type; }

	PDF_MARK_TYPE GetMarkType() const { return mMarkType; }

	void SetTag( const CByteString & tag ) { mTag = tag; }

	CByteString GetTag() const { return mTag; }

	void SetProperty( const CPDF_DictionaryPtr & pDict )
	{
		mpDictionary = pDict;
	}

	CByteString GetPropertyResName() const { return mDictName; }

	void SetPropertyResName( const CByteString & resName ) { mDictName = resName; }

	CPDF_DictionaryPtr GetProperty() const { return mpDictionary; }

	CPDF_ContentObject * Clone() const
	{
		CPDF_Mark * pRet = GetAllocator()->New<CPDF_Mark>( GetAllocator() );
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
	CByteString mTag;
	CByteString mDictName;
	CPDF_DictionaryPtr mpDictionary;
};

class CPDF_InlineImage : public CPDF_ContentObject
{
public:
	CPDF_InlineImage( bool bMask, uint32 width, uint32 hight, uint32 bpc, PBYTE pBytes, size_t size, CPDF_ObjectPtr objPtr,
                         CPDF_ColorSpacePtr colorspace, GRAPHICS_STATE_RENDERINTENTS ri, CAllocator * pAllocator = nullptr );

	~CPDF_InlineImage();

	CPDF_ContentObject * Clone() const;

	bool IsMask() const { return mbMask; }
	uint32 GetWidth() const { return mWidth; }
	uint32 GetHeight() const { return mHeight; }
	uint32 GetBpc() const { return mBpc; }
	CPDF_ColorSpacePtr GetColorspace() const { return mColorspace; }
	CPDF_ObjectPtr GetDecode() const { return mDecodeObjPtr; }
    GRAPHICS_STATE_RENDERINTENTS GetRenderIntent() const { return mRI; }

	PBYTE GetData() const { return mpData; }
	size_t GetDataSize() const { return mDataSize; }

#ifdef WIN32
	CBitmap * GetBitmap();
#endif

private:
	bool					mbMask;
	uint32                  mWidth;
	uint32                  mHeight;
	uint32                  mBpc;
	PBYTE                   mpData;
	size_t                  mDataSize;
	CPDF_ColorSpacePtr	mColorspace;
	CPDF_ObjectPtr		mDecodeObjPtr;
    GRAPHICS_STATE_RENDERINTENTS    mRI;
};

class CPDF_ComponentRef : public CPDF_ContentObject
{
public:
	CPDF_ComponentRef( const CByteString & name, const CPDF_ComponentPtr & componentPtr, CAllocator * pAllocator = nullptr )
		: CPDF_ContentObject( ContentType_Component, pAllocator ), mName( name ), mComponentPtr( componentPtr ) {}

	CByteString			GetName() const { return mName; }
	CPDF_ComponentPtr	GetComponentPtr() const { return mComponentPtr; }

	CPDF_ContentObject * Clone() const;

private:
	CByteString			mName;
	CPDF_ComponentPtr	mComponentPtr;
};
    
}//namespace

#endif