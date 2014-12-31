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
	CHE_PDF_ContentObject( PDF_CONTENTOBJ_TYPE type, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mType(type), mpGState(NULL) {}

	virtual ~CHE_PDF_ContentObject();

	PDF_CONTENTOBJ_TYPE GetType() const { return mType; }

	virtual CHE_PDF_ContentObject * Clone() const = 0;

	CHE_PDF_GState * GetGState() const { return mpGState; }

	HE_BOOL SetGState( CHE_PDF_GState * pGSatae );

	// 	CHE_Matrix GetExtMatrix() { return mExtMatrixl; }
	// 	HE_VOID	SetExtMatrix( const CHE_Matrix & matrx ) { mExtMatrixl = matrx; }

	// 	HE_BOOL	IsModified() const
	// 	{
	// 		return (mFlag & CONTENTOBJ_FLAG_MODIFIED);
	// 	}
protected:
	// 	HE_VOID	SetFlag( HE_ULONG flag )
	// 	{
	// 		mFlag = flag;
	// 	}
	// 
	// 	HE_VOID CombineFlag( HE_ULONG flag )
	// 	{
	// 		mFlag |= flag;
	// 	}

protected:
	//	HE_ULONG			mFlag;
	//	CHE_Matrix			mExtMatrixl;

	PDF_CONTENTOBJ_TYPE		mType;
	CHE_PDF_GState *		mpGState;
};


typedef std::list<CHE_PDF_ContentObject*> ContentObjectList;

class CHE_PDF_ContentObjectList : public CHE_Object
{
public:
	CHE_PDF_ContentObjectList( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), mResMgr(pAllocator) {}

	~CHE_PDF_ContentObjectList();

	bool Append( CHE_PDF_ContentObject * pObj );

	ContentObjectList::iterator Begin();

	ContentObjectList::iterator End();

	//CHE_PDF_ContentObjectList * Clone();

	HE_VOID Clear();

	CHE_PDF_ContentResMgr & GetResMgr() { return mResMgr; }

	HE_VOID SetType3BBox( const HE_INT32 type, std::vector<HE_FLOAT> & param );
	HE_BOOL GetType3BBox( HE_INT32 & type, std::vector<HE_FLOAT> & param );

private:
	HE_INT32				mType3DType;
	std::vector<HE_FLOAT>	mType3Param;
	ContentObjectList		mList;
	CHE_PDF_ContentResMgr	mResMgr;
};

#endif