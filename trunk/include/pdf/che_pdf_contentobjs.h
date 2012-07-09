#ifndef _CHE_PDF_CONTENTOBJS_H_
#define _CHE_PDF_CONTENTOBJS_H_

#include "../che_base.h"
#include "che_pdf_graphicsstate.h"
#include "che_pdf_font.h"
#include "che_pdf_contentresmgr.h"
#include <vector>
#include <list>

enum PDF_CONTENTOBJ_TYPE
{
	ContentType_Text = 0,
	ContentType_Path = 1,
	ContentType_Mark = 2,
	ContentType_RefImage = 3,
	ContentType_InlineImage = 4,
	ContentType_Form = 5,
	ContentType_Shading = 6,
	//ContentType_PushGState = 7,
	//ContentType_PopGState = 8,
	//ContentType_TextBegin = 9,
	//ContentType_TextEnd = 10
};

class CHE_PDF_ContentObject : public CHE_Object
{
public:
	CHE_PDF_ContentObject( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpGState(NULL) {}
	virtual ~CHE_PDF_ContentObject()
	{
		if ( mpGState )
		{
			mpGState->GetAllocator()->Delete( mpGState );
		}
	}

	virtual PDF_CONTENTOBJ_TYPE GetType() const = 0;

	virtual CHE_PDF_ContentObject * Clone() const = 0;

	CHE_PDF_GState * GetGState() const { return mpGState; }
	HE_BOOL SetGState( CHE_PDF_GState * pGSatae );

	CHE_PDF_Matrix GetExtMatrix() { return mExtMatrixl; }
	HE_VOID	SetExtMatrix( const CHE_PDF_Matrix & matrx ) { mExtMatrixl = matrx; }

protected:
	CHE_PDF_GState *	mpGState;
	CHE_PDF_Matrix		mExtMatrixl;
};

typedef std::list<CHE_PDF_ContentObject*> ContentObjectList;

class CHE_PDF_ContentObjectList : public CHE_Object
{
public:
	CHE_PDF_ContentObjectList( const CHE_PDF_DictionaryPtr & pResDict = CHE_PDF_DictionaryPtr(), CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), mResMgr(pResDict, pAllocator) {}

	~CHE_PDF_ContentObjectList();

	bool Append( CHE_PDF_ContentObject * pObj );

	ContentObjectList::iterator Begin();

	ContentObjectList::iterator End();

	CHE_PDF_ContentObjectList * Clone();

	HE_VOID Clear();

	CHE_PDF_ContentResMgr & GetResMgr() { return mResMgr; }

private:
	ContentObjectList		mList;
	CHE_PDF_ContentResMgr	mResMgr;
};

class CHE_PDF_NamedContentObject : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_NamedContentObject( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: mName(name), CHE_PDF_ContentObject( pAllocator ) {}
	virtual ~CHE_PDF_NamedContentObject() {}

	bool SetName( const CHE_ByteString & name ) { mName = name; }

	CHE_ByteString GetName() const { return mName; }

protected:
	CHE_ByteString mName;
};

class CHE_PDF_TextItem
{
public:
	HE_INT32 cid;
	HE_INT32 ucs;
	HE_INT32 kerning;
	HE_FLOAT offsetX;
	HE_FLOAT offsetY;
};

class CHE_PDF_Text : public CHE_PDF_ContentObject
{									 
public:
	CHE_PDF_Text( CHE_Allocator * pAllocator = NULL  ) : CHE_PDF_ContentObject(pAllocator) {}

	~CHE_PDF_Text(){}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Text; }

	HE_BOOL SetTextObject( const CHE_PDF_ObjectPtr & pObj );

	CHE_PDF_ObjectPtr GetText() const { return mpObj; }

	CHE_PDF_ContentObject * Clone() const
	{
		CHE_PDF_Text * pTextRet = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
		pTextRet->mItems = mItems;
		pTextRet->mExtMatrixl = mExtMatrixl;
		if ( mpGState )
		{
			pTextRet->mpGState = mpGState->Clone();
		}
		if ( mpObj )
		{
			pTextRet->mpObj = mpObj->Clone();
		}
		return pTextRet;
	}

	std::vector<CHE_PDF_TextItem> mItems;

private:
	CHE_PDF_ObjectPtr mpObj;
};

enum PDF_PATHITEM_TYPE
{
	PathItem_MoveTo = 0,
	PathItem_LineTo = 1,
	PathItem_CurveTo = 2,
	PathItem_Rectangle = 3,
	PathItem_Close = 4
};

class CHE_PDF_PathItem
{
public:
	union{
		PDF_PATHITEM_TYPE type;
		HE_FLOAT value;
	};
};

enum PDF_PATH_PAINT
{
	Paint_Fill = 0,
	Paint_Stroke = 1,
	Paint_FillStroke = 2,
	Paint_None = 3
};

enum PDF_FILL_MODE
{
	Mode_Nonzero = 0,
	Mode_EvenOdd = 1
};

class CHE_PDF_Path : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_Path( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	~CHE_PDF_Path();

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Path; }

	std::vector<CHE_PDF_PathItem> mItems;

	HE_VOID SetPaintType( PDF_PATH_PAINT type ) { mType = type; }

	PDF_PATH_PAINT GetPaintType() const { return mType; }

	HE_VOID SetFillMode( PDF_FILL_MODE mode ) { mFillMode = mode; }

	PDF_FILL_MODE GetFillMode() const { return mFillMode; }

	CHE_PDF_ContentObject * Clone() const
	{
		CHE_PDF_Path * pTmpPath = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
		pTmpPath->mType = mType;
		pTmpPath->mFillMode = mFillMode;
		pTmpPath->mItems = mItems;
		pTmpPath->mpGState = NULL;
		if ( mpGState )
		{
			pTmpPath->mpGState = mpGState->Clone();
		}
		return pTmpPath;
	}

private:
	PDF_PATH_PAINT mType;
	PDF_FILL_MODE mFillMode;
};

enum PDF_MARK_TYPE
{
	Mark_MP = 0,
	Mark_DP = 1,
	Mark_BMC = 2,
	Mark_BDC = 3,
	Mark_EMC = 4
};

class CHE_PDF_Mark : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_Mark( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject(pAllocator), mMarkType(Mark_MP) {}

	~CHE_PDF_Mark() {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Mark; }

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


class CHE_PDF_RefImage : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_RefImage( const CHE_ByteString & name, const CHE_PDF_ReferencePtr & pRef, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ), mpRef(pRef) {}

	~CHE_PDF_RefImage() {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_RefImage; }

	CHE_PDF_ContentObject * Clone() const
	{
		return GetAllocator()->New<CHE_PDF_RefImage>( mName, mpRef, GetAllocator() );
	}

	CHE_PDF_ReferencePtr GetRef() { return mpRef; }

private:
	CHE_PDF_ReferencePtr mpRef;
};

class CHE_PDF_InlineImage : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_InlineImage(	HE_BOOL bMask, HE_DWORD width, HE_DWORD hight, HE_DWORD bps, HE_LPBYTE pBytes, HE_DWORD size,
							CHE_PDF_ObjectPtr objPtr = CHE_PDF_ObjectPtr(), CHE_PDF_ColorSpace * pColorspace = NULL, 
							CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ), mbMask( bMask ),
							mWidth( width ), mHeight( hight ), mBitps( bps ), mpData( NULL), mDataSize( 0 ),
							mDecodeObjPtr( objPtr ), mpColorspace( pColorspace  )
	{
		if ( pBytes )
		{
			mDataSize = size;
			mpData = GetAllocator()->NewArray<HE_BYTE>( mDataSize );
			memcpy( mpData, pBytes, mDataSize );
		}
	}

	~CHE_PDF_InlineImage()
	{
		if ( mpData )
		{
			GetAllocator()->DeleteArray( mpData );
		}
		if ( mpColorspace )
		{
			mpColorspace->GetAllocator()->Delete( mpColorspace );
		}
	}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_InlineImage; }

	CHE_PDF_ContentObject * Clone() const
	{
		CHE_PDF_ColorSpace * pTmpColorSpace = NULL;
		if ( mpColorspace )
		{
			pTmpColorSpace = mpColorspace->Clone();
		}
		CHE_PDF_ObjectPtr objPtr;
		if ( mDecodeObjPtr )
		{
			objPtr = mDecodeObjPtr->Clone();
		}
		return GetAllocator()->New<CHE_PDF_InlineImage>( mbMask, mWidth, mHeight, mBitps, mpData, mDataSize, objPtr, pTmpColorSpace, GetAllocator() );
	}

	HE_BOOL	IsMask() const { return mbMask; }
	HE_DWORD GetWidth() const { return mWidth; }
	HE_DWORD GetHight() const { return mHeight; }
	HE_DWORD GetBitps() const { return mBitps; }
	CHE_PDF_ColorSpace * GetColorspace() const { return mpColorspace; }
	CHE_PDF_ObjectPtr GetDecode() const { return mDecodeObjPtr; }

	HE_LPBYTE GetData() const { return mpData; }
	HE_DWORD GetDataSize() const { return mDataSize; }

private:
	HE_BOOL				mbMask;
	HE_DWORD			mWidth;
	HE_DWORD			mHeight;
	HE_DWORD			mBitps;
	HE_LPBYTE			mpData;
	HE_DWORD			mDataSize;
	CHE_PDF_ColorSpace*	mpColorspace;
	CHE_PDF_ObjectPtr	mDecodeObjPtr;
};

class CHE_PDF_Form : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Form( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}
	
	~CHE_PDF_Form();

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Form; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_Form>( mName, GetAllocator() ); }

	CHE_PDF_ContentObjectList & GetList() { return mList; }

private:
	CHE_PDF_ContentObjectList mList;
};

class CHE_PDF_Shading : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Shading( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Shading; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_Shading>( mName, GetAllocator() ); }
};

// class CHE_PDF_PushGState : public CHE_PDF_ContentObject
// {
// public:
// 	CHE_PDF_PushGState( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}
// 
// 	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_PushGState; }
// 
// 	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_PushGState>( GetAllocator() ); }
// };

// class CHE_PDF_PopGState : public CHE_PDF_ContentObject
// {
// public:
// 	CHE_PDF_PopGState( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}
// 
// 	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_PopGState; }
// 
// 	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_PopGState>( GetAllocator() ); }
// };

// class CHE_PDF_TextBegin : public CHE_PDF_ContentObject
// {
// public:
// 	CHE_PDF_TextBegin( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}
// 
// 	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_TextBegin; }
// 
// 	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_TextBegin>( GetAllocator() ); }
// };

// class CHE_PDF_TextEnd : public CHE_PDF_ContentObject
// {
// public:
// 	CHE_PDF_TextEnd( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}
// 
// 	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_TextEnd; }
// 
// 	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_TextEnd>( GetAllocator() ); }
// };

#endif