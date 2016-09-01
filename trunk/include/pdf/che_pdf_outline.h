#ifndef _CHE_PDF_OUTLINE_H_
#define _CHE_PDF_OUTLINE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_file.h"
#include "che_pdf_destination.h"
#include "che_pdf_nametree.h"
#include "che_pdf_namedict.h"
#include "che_pdf_action.h"

class CHE_PDF_Outline;

class CHE_PDF_OutlineItem : public CHE_Object
{
public:
	CHE_PDF_ReferencePtr	mRefPtr;

	int32				mCount;
	size_t				mFlag;
	FLOAT				mRGBColor[3];
	CHE_ByteString			mTitle;

	CHE_PDF_ObjectPtr		mDestObj;
	CHE_PDF_ObjectPtr		mActionObj;
	CHE_PDF_ActionPtr		mAction;
	CHE_PDF_DestinationPtr	mDest;
	CHE_PDF_DictionaryPtr	mSE;

	CHE_PDF_OutlineItem *	mpParent;
	CHE_PDF_OutlineItem *	mpPrev;
	CHE_PDF_OutlineItem *	mpNext;
	CHE_PDF_OutlineItem *	mpFirst;
	CHE_PDF_OutlineItem *	mpLast;

private:
	CHE_PDF_OutlineItem( CHE_Allocator * pAllocator = nullptr );

	friend class CHE_Allocator;
	friend class CHE_PDF_Outline;
};

class CHE_PDF_Outline : public CHE_Object
{
public:
	CHE_PDF_Outline( CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_Outline();

	bool Parse( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_NameDict * pNameTree /*= nullptr*/ );

	void	Clear();

	CHE_PDF_OutlineItem * First() const { return mpFirst; }

	CHE_PDF_OutlineItem * Last() const { return mpLast; }

private:
	bool GetOutlineItem( CHE_PDF_OutlineItem * pItem, const CHE_PDF_DictionaryPtr & dictPtr );

	uint32 BuildChildTree( CHE_PDF_OutlineItem * pCurItem, const CHE_PDF_ReferencePtr & firstRef, const CHE_PDF_ReferencePtr & lastRef, bool bRoot = false );

	void CleanChildTree( CHE_PDF_OutlineItem * pFirst, CHE_PDF_OutlineItem * pLast );

private:
	CHE_PDF_ReferencePtr	mRefPtr;
	size_t				mCount;
	CHE_PDF_OutlineItem *	mpFirst;
	CHE_PDF_OutlineItem *	mpLast;
	CHE_PDF_NameDict	*	mpNameDict;
	
};


#endif