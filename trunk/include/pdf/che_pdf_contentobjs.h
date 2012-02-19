#ifndef _CHE_PDF_CONTENTOBJS_H_
#define _CHE_PDF_CONTENTOBJS_H_

#include "../che_base.h"
#include "che_pdf_graphicsstate.h"
#include "che_pdf_font.h"
#include <vector>
#include <list>

enum PDF_CONTENTOBJ_TYPE
{
	ContentType_Text = 0,
	ContentType_Path = 1,
	ContentType_Mark = 2,
	ContentType_Image = 3,
	ContentType_Form = 4,
	ContentType_Shading = 5,
	ContentType_PushGState = 6,
	ContentType_PopGState = 7,
	ContentType_TextBegin = 8,
	ContentType_TextEnd = 9
};

class CHE_PDF_ContentObject : public CHE_Object
{
public:
	CHE_PDF_ContentObject( CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mpGState(NULL) {}
	virtual ~CHE_PDF_ContentObject() {}

	virtual PDF_CONTENTOBJ_TYPE GetType() const = 0;

	virtual CHE_PDF_ContentObject * Clone() const = 0;

	CHE_PDF_GState * GetGState() const { return mpGState; }
	HE_BOOL SetGState( CHE_PDF_GState * pGSatae );

protected:
	CHE_PDF_GState * mpGState;
};

class CHE_PDF_NamedContentObject : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_NamedContentObject( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: mName(name), CHE_PDF_ContentObject( pAllocator ) {}
	virtual ~CHE_PDF_NamedContentObject() {}

protected:
	CHE_ByteString mName;
};

class CHE_PDF_TextItem
{
public:
	HE_INT32 cid;
	HE_INT32 kerning;
	HE_FLOAT offsetX;
	HE_FLOAT offsetY;
};

class CHE_PDF_Text : public CHE_PDF_ContentObject
{									 
public:
	CHE_PDF_Text( CHE_Allocator * pAllocator = NULL  ) : CHE_PDF_ContentObject(pAllocator), mpObj(NULL) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Text; }

	HE_BOOL SetTextObject( CHE_PDF_Object * pObj )
	{
		if ( pObj )
		{
			if ( mpObj )
			{
				mpObj->Release();
			}
			mpObj = pObj->Clone();
			return TRUE;
		}
		return FALSE;
	}

	CHE_PDF_Object * GetText() const { return mpObj; }

	CHE_PDF_ContentObject * Clone() const
	{
		CHE_PDF_Text * pTextRet = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
		pTextRet->mItems = mItems;
		if ( mpObj )
		{
			pTextRet->mpObj = mpObj->Clone();
		}
		return pTextRet;
	}

	std::vector<CHE_PDF_TextItem> mItems;

private:
	CHE_PDF_Object * mpObj;
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
	CHE_PDF_Mark( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject(pAllocator), mMarkType(Mark_MP), mpDictionary(NULL) {}

	~CHE_PDF_Mark()
	{
		if ( mpDictionary )
		{
			mpDictionary->Release();
		}
	}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Mark; }

	HE_VOID SetMarkType( PDF_MARK_TYPE type ) { mMarkType = type; }

	PDF_MARK_TYPE GetMarkType() const { return mMarkType; }

	HE_VOID SetTag( const CHE_ByteString & tag ) { mTag = tag; }

	CHE_ByteString GetTag() const { return mTag; }

	HE_VOID SetProperty( CHE_PDF_Dictionary * pDict )
	{
		if ( pDict )
		{
			if ( mpDictionary )
			{
				mpDictionary->Release();
			}
			mpDictionary = pDict->Clone();
		}
	}

	CHE_PDF_Dictionary * GetProperty() const { return mpDictionary; }

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
	CHE_PDF_Dictionary * mpDictionary;
};

class CHE_PDF_Image : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Image( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Image; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_Image>( mName, GetAllocator() ); }
};

class CHE_PDF_Form : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Form( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Form; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_Form>( mName, GetAllocator() ); }
};

class CHE_PDF_Shading : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Shading( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_Shading; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_Shading>( mName, GetAllocator() ); }
};

class CHE_PDF_PushGState : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_PushGState( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_PushGState; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_PushGState>( GetAllocator() ); }
};

class CHE_PDF_PopGState : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_PopGState( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_PopGState; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_PopGState>( GetAllocator() ); }
};

class CHE_PDF_TextBegin : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_TextBegin( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_TextBegin; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_TextBegin>( GetAllocator() ); }
};

class CHE_PDF_TextEnd : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_TextEnd( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() const { return ContentType_TextEnd; }

	CHE_PDF_ContentObject * Clone() const { return GetAllocator()->New<CHE_PDF_TextEnd>( GetAllocator() ); }
};

#endif