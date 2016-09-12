#ifndef _CHELIB_PDF_CONTENTLIST_H_
#define _CHELIB_PDF_CONTENTLIST_H_

#include "pdf_contentresmgr.h"

#include <list>
#include <vector>

namespace chelib {

enum PDF_CONTENTOBJ_TYPE
{
	ContentType_Text		= 0,
	ContentType_Path		= 1,
	ContentType_Mark		= 2,
	ContentType_InlineImage = 3,
	ContentType_Component	= 4
};

class CPDF_GState;

class CPDF_ContentObject : public CObject
{
public:
	CPDF_ContentObject( PDF_CONTENTOBJ_TYPE type, CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mType(type), mpGState(nullptr) {}

	virtual ~CPDF_ContentObject();

	PDF_CONTENTOBJ_TYPE GetType() const { return mType; }

	virtual CPDF_ContentObject * Clone() const = 0;

	CPDF_GState * GetGState() const { return mpGState; }

	bool SetGState( CPDF_GState * pGSatae );

	// 	CMatrix GetExtMatrix() { return mExtMatrixl; }
	// 	void	SetExtMatrix( const CMatrix & matrx ) { mExtMatrixl = matrx; }

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
	//	CMatrix			mExtMatrixl;

	PDF_CONTENTOBJ_TYPE		mType;
	CPDF_GState *		mpGState;
};


typedef std::list<CPDF_ContentObject*> ContentObjectList;

class CPDF_ContentObjectList : public CObject
{
public:
	CPDF_ContentObjectList( CAllocator * pAllocator = nullptr )
      : CObject( pAllocator ), mResMgr(pAllocator) {}

	~CPDF_ContentObjectList();

	bool Append( CPDF_ContentObject * pObj );

	ContentObjectList::iterator Begin();

	ContentObjectList::iterator End();

	//CPDF_ContentObjectList * Clone();

	void Clear();

	CPDF_ContentResMgr & GetResMgr() { return mResMgr; }

	void SetType3BBox( const int32 type, std::vector<FLOAT> & param );
	bool GetType3BBox( int32 & type, std::vector<FLOAT> & param );

private:
	int32				mType3DType;
	std::vector<FLOAT>	mType3Param;
	ContentObjectList		mList;
	CPDF_ContentResMgr	mResMgr;
};

}//namespace

#endif