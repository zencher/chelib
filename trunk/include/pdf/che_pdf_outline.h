#ifndef _CHE_PDF_OUTLINE_H_
#define _CHE_PDF_OUTLINE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_file.h"
#include "che_pdf_destination.h"


class CHE_PDF_OutlineItem : public CHE_Object
{
public:
	CHE_PDF_OutlineItem( CHE_Allocator * pAllocator = NULL );

	HE_UINT32				mCount;
	HE_UINT32				mFlag;
	HE_FLOAT				mColor[3];
	CHE_ByteString			mTitleStrPtr;
	CHE_PDF_Destination		mDestination;

	CHE_PDF_OutlineItem *	mpParent;
	CHE_PDF_OutlineItem *	mpPrevBrother;
	CHE_PDF_OutlineItem *	mpNextBrother;
	CHE_PDF_OutlineItem *	mpFirstChild;
	CHE_PDF_OutlineItem *	mpLastChild;
};

class CHE_PDF_Outline : public CHE_Object
{
public:
	CHE_PDF_Outline( CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_Outline();

	HE_BOOL Parse( const CHE_PDF_ReferencePtr & refPtr );

private:
	HE_BOOL GetOutlineItem( CHE_PDF_OutlineItem * pItem, CHE_PDF_ReferencePtr refPtr );

	CHE_PDF_OutlineItem * ParseSbling( CHE_PDF_OutlineItem * pCurItem, CHE_PDF_ReferencePtr curRefPtr, CHE_PDF_ReferencePtr nextPtr );

	HE_VOID ParseChild( CHE_PDF_OutlineItem * pCurItem, CHE_PDF_ReferencePtr curRefPtr, CHE_PDF_ReferencePtr firstPtr );

private:
	CHE_PDF_File *			mpFile;
	CHE_PDF_OutlineItem *	mpRootItem;
};


#endif