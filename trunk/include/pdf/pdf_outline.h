#ifndef _CHELIB_PDF_OUTLINE_H_
#define _CHELIB_PDF_OUTLINE_H_

#include "../base.h"
#include "pdf_objects.h"
#include "pdf_file.h"
#include "pdf_destination.h"
#include "pdf_nametree.h"
#include "pdf_namedict.h"
#include "pdf_action.h"

namespace chelib {

class CPDF_Outline;

class CPDF_OutlineItem : public CObject
{
public:
	CPDF_ReferencePtr   mRefPtr;

	int32               mCount;
	size_t              mFlag;
	FLOAT               mRGBColor[3];
	CByteString         mTitle;

	CPDF_ObjectPtr      mDestObj;
	CPDF_ObjectPtr      mActionObj;
	CPDF_ActionPtr		mAction;
	CPDF_DestinationPtr	mDest;
	CPDF_DictionaryPtr	mSE;

	CPDF_OutlineItem *  mpParent;
	CPDF_OutlineItem *  mpPrev;
	CPDF_OutlineItem *  mpNext;
	CPDF_OutlineItem *  mpFirst;
	CPDF_OutlineItem *  mpLast;

private:
	CPDF_OutlineItem( CAllocator * pAllocator = nullptr );

	friend class CAllocator;
	friend class CPDF_Outline;
};

class CPDF_Outline : public CObject
{
public:
	CPDF_Outline( CAllocator * pAllocator = nullptr );
	~CPDF_Outline();

	bool Parse( const CPDF_ReferencePtr & refPtr, CPDF_NameDict * pNameTree );

	void Clear();

	CPDF_OutlineItem * First() const { return mpFirst; }
	CPDF_OutlineItem * Last() const { return mpLast; }

private:
	bool GetOutlineItem( CPDF_OutlineItem * pItem, const CPDF_DictionaryPtr & dictPtr );

	uint32 BuildChildTree( CPDF_OutlineItem * pCurItem, const CPDF_ReferencePtr & firstRef,
                           const CPDF_ReferencePtr & lastRef, bool bRoot = false );

	void CleanChildTree( CPDF_OutlineItem * pFirst, CPDF_OutlineItem * pLast );

private:
	CPDF_ReferencePtr	mRefPtr;
	size_t              mCount;
	CPDF_OutlineItem *	mpFirst;
	CPDF_OutlineItem *	mpLast;
	CPDF_NameDict	*	mpNameDict;
};

}//namespace

#endif