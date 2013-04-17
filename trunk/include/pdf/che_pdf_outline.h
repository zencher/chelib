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

	/*CHE_PDF_OutlineItem( CHE_PDF_OutlineItem & item );*/

	/*~CHE_PDF_OutlineItem();*/

/*private:*/
	HE_DWORD				mCount;
	CHE_PDF_StringPtr		mTitleStrPtr;
	CHE_PDF_Destination		mDestination;

	CHE_PDF_OutlineItem *	mpFirstChild;
	CHE_PDF_OutlineItem *	mpLastChild;

	CHE_PDF_OutlineItem *	mpPrevBrother;
	CHE_PDF_OutlineItem *	mpNextBrother;
};

class CHE_PDF_Outline : public CHE_Object
{
public:
	CHE_PDF_Outline( CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL );

	/*CHE_PDF_Outline( const CHE_PDF_Outline & outline );*/

	~CHE_PDF_Outline();

	HE_BOOL Parse( const CHE_PDF_ReferencePtr & refPtr );

private:

	//HE_VOID BuildTree( CHE_PDF_OutlineItem * pCurItem, PDF_RefInfo curRefInfo, CHE_PDF_ReferencePtr firstPtr, CHE_PDF_ReferencePtr lastPtr );

	HE_BOOL GetOutlineItem( CHE_PDF_OutlineItem * pItem, CHE_PDF_ReferencePtr refPtr );

	HE_VOID BuildBrotherTree( CHE_PDF_OutlineItem * pCurItem, PDF_RefInfo curRefInfo, CHE_PDF_ReferencePtr nextPtr );

	HE_VOID BuildChildTree( CHE_PDF_OutlineItem * pCurItem, PDF_RefInfo curRefInfo, CHE_PDF_ReferencePtr firstPtr, CHE_PDF_ReferencePtr lastPtr );

private:
	CHE_PDF_File *			mpFile;

	CHE_PDF_OutlineItem *	mpRootItem;

	HE_DWORD				mCount;
};


#endif