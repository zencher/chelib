#ifndef _CHE_PDF_DESTINATION_H_
#define _CHE_PDF_DESTINATION_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_component.h"

enum PDF_DESTINATION_TYPE
{
	DESTINATION_NONE,
	DESTINATION_PAGE_XYZ,
	DESTINATION_PAGE_FIT,
	DESTINATION_PAGE_FITH,
	DESTINATION_PAGE_FITV,
	DESTINATION_PAGE_FITR,
	DESTINATION_PAGE_FITB,
	DESTINATION_PAGE_FITBH,
	DESTINATION_PAGE_FITBV
};

class CHE_PDF_DestinationPtr;

class CHE_PDF_Destination : public CHE_PDF_Component
{
public:
	static CHE_PDF_DestinationPtr Create(const CHE_PDF_ArrayPtr & destArray, CHE_Allocator * pAllocator = NULL);

	PDF_DESTINATION_TYPE		GetDestType() const { return mDestType; }
	HE_FLOAT					GetLeft() const { return mLeft; }
	HE_FLOAT					GetRight() const { return mRight; }
	HE_FLOAT					GetTop() const { return mTop; }
	HE_FLOAT					GetBottom() const { return mBottom; }
	HE_FLOAT					GetZoom() const { return mZoom; }

private:
	CHE_PDF_Destination(const CHE_PDF_ArrayPtr & arr, CHE_Allocator * pAllocator = NULL);

	PDF_DESTINATION_TYPE		mDestType;
	CHE_PDF_ReferencePtr		mPageRef;
	HE_UINT32					mPageIndex;
	HE_FLOAT					mLeft;
	HE_FLOAT					mRight;
	HE_FLOAT					mTop;
	HE_FLOAT					mBottom;
	HE_FLOAT					mZoom;

	friend class CHE_Allocator;
};

class CHE_PDF_DestinationPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_Destination * operator->() const { return (CHE_PDF_Destination*)mpCom; }
};

#endif