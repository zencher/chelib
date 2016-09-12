#ifndef _CHELIB_PDF_DESTINATION_H_
#define _CHELIB_PDF_DESTINATION_H_

#include "../base.h"
#include "pdf_objects.h"
#include "pdf_component.h"

namespace chelib {

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

class CPDF_DestinationPtr;

class CPDF_Destination : public CPDF_Component
{
public:
	static CPDF_DestinationPtr Create( const CPDF_ArrayPtr & destArray,
                                       CAllocator * pAllocator = nullptr );

	PDF_DESTINATION_TYPE GetDestType() const { return mDestType; }
    
	FLOAT GetLeft() const { return mLeft; }
	FLOAT GetRight() const { return mRight; }
	FLOAT GetTop() const { return mTop; }
	FLOAT GetBottom() const { return mBottom; }
	FLOAT GetZoom() const { return mZoom; }

private:
	CPDF_Destination( const CPDF_ArrayPtr & arr, CAllocator * pAllocator = nullptr );

	PDF_DESTINATION_TYPE    mDestType;
	CPDF_ReferencePtr		mPageRef;
	uint32					mPageIndex;
	FLOAT					mLeft;
	FLOAT					mRight;
	FLOAT					mTop;
	FLOAT					mBottom;
	FLOAT					mZoom;

	friend class CAllocator;
};

class CPDF_DestinationPtr : public CPDF_ComponentPtr
{
public:
	CPDF_Destination * operator->() const { return (CPDF_Destination*)mpCom; }
};

}//namespace

#endif