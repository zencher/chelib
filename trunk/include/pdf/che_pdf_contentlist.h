#ifndef _CHE_PDF_CONTENTLIST_H_
#define _CHE_PDF_CONTENTLIST_H_

#include <list>
#include <vector>

#include "che_pdf_contentresmgr.h"


enum PDF_CONTENTOBJ_TYPE
{
	ContentType_Text		= 0,
	ContentType_Path		= 1,
	ContentType_Mark		= 2,
	ContentType_InlineImage = 3,
	ContentType_Component	= 4
};

class CHE_PDF_GState;

class CHE_PDF_ContentObject : public CHE_Object
{
public:
	CHE_PDF_ContentObject( PDF_CONTENTOBJ_TYPE type, CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mType(type), mpGState(nullptr) {}

	virtual ~CHE_PDF_ContentObject();

	PDF_CONTENTOBJ_TYPE GetType() const { return mType; }

	virtual CHE_PDF_ContentObject * Clone() const = 0;

	CHE_PDF_GState * GetGState() const { return mpGState; }

	bool SetGState( CHE_PDF_GState * pGSatae );

	// 	CHE_Matrix GetExtMatrix() { return mExtMatrixl; }
	// 	void	SetExtMatrix( const CHE_Matrix & matrx ) { mExtMatrixl = matrx; }

	// 	bool	IsModified() const
	// 	{
	// 		return (mFlag & CONTENTOBJ_FLAG_MODIFIED);
	// 	}
protected:
	// 	void	SetFlag( size_t flag )
	// 	{
	// 		mFlag = flag;
	// 	}
	// 
	// 	void CombineFlag( size_t flag )
	// 	{
	// 		mFlag |= flag;
	// 	}

protected:
	//	size_t			mFlag;
	//	CHE_Matrix			mExtMatrixl;

	PDF_CONTENTOBJ_TYPE		mType;
	CHE_PDF_GState *		mpGState;
};


typedef std::list<CHE_PDF_ContentObject*> ContentObjectList;

class CHE_PDF_ContentObjectList : public CHE_Object
{
public:
	CHE_PDF_ContentObjectList( CHE_Allocator * pAllocator = nullptr )
		: CHE_Object( pAllocator ), mResMgr(pAllocator) {}

	~CHE_PDF_ContentObjectList();

	bool Append( CHE_PDF_ContentObject * pObj );

	ContentObjectList::iterator Begin();

	ContentObjectList::iterator End();

	//CHE_PDF_ContentObjectList * Clone();

	void Clear();

	CHE_PDF_ContentResMgr & GetResMgr() { return mResMgr; }

	void SetType3BBox( const int32 type, std::vector<FLOAT> & param );
	bool GetType3BBox( int32 & type, std::vector<FLOAT> & param );

private:
	int32				mType3DType;
	std::vector<FLOAT>	mType3Param;
	ContentObjectList		mList;
	CHE_PDF_ContentResMgr	mResMgr;
};

#endif