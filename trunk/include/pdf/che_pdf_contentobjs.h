#ifndef _CHE_PDF_CONTENTOBJS_H_
#define _CHE_PDF_CONTENTOBJS_H_

#include "../che_base.h"
#include <string>
#include <vector>

enum PDF_CONTENTOBJ_TYPE
{
	ContentType_Text = 0,
	ContentType_Path = 1,
	ContentType_Mark = 2,
	ContentType_Image = 3,
	ContentType_Form = 4,
	ContentType_Shading = 5,
	ContentType_PushGState = 6,
	ContentType_PopGState = 7
};

class CHE_PDF_ContentObject : public CHE_Object
{
public:
	CHE_PDF_ContentObject( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}
	virtual ~CHE_PDF_ContentObject() {}

	virtual PDF_CONTENTOBJ_TYPE GetType() = 0;
};

class CHE_PDF_NamedContentObject : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_NamedContentObject( const std::string & name, CHE_Allocator * pAllocator = NULL )
		: mName(name), CHE_PDF_ContentObject( pAllocator ) {}
	virtual ~CHE_PDF_NamedContentObject() {}

protected:
	std::string mName;
};

class CHE_PDF_Text : public CHE_PDF_ContentObject
{									 
public:
	CHE_PDF_Text(  CHE_Allocator * pAllocator = NULL  ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_Text; }
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
		float value;
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

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_Path; }

	std::vector<CHE_PDF_PathItem> mItems;

	void SetPaintType( PDF_PATH_PAINT type ) { mType = type; }

	PDF_PATH_PAINT GetPaintType() { return mType; }

	void SetFillMode( PDF_FILL_MODE mode ) { mFillMode = mode; }

	PDF_FILL_MODE GetFillMode() { return mFillMode; }

private:
	PDF_PATH_PAINT mType;
	PDF_FILL_MODE mFillMode;
};

class CHE_PDF_Mark : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_Mark( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_Mark; }
};

class CHE_PDF_Image : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Image( const std::string & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_Image; }
};

class CHE_PDF_Form : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Form( const std::string & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_Form; }
};

class CHE_PDF_Shading : public CHE_PDF_NamedContentObject
{
public:
	CHE_PDF_Shading( const std::string & name, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_NamedContentObject( name, pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_Shading; }
};

class CHE_PDF_PushGState : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_PushGState( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_PushGState; }
};

class CHE_PDF_PopGState : public CHE_PDF_ContentObject
{
public:
	CHE_PDF_PopGState( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_ContentObject( pAllocator ) {}

	PDF_CONTENTOBJ_TYPE GetType() { return ContentType_PopGState; }
};


#endif