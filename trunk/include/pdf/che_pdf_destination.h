#ifndef _CHE_PDF_DESTINATION_H_
#define _CHE_PDF_DESTINATION_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

enum PDF_DESTINATION_TYPE
{
	DESTINATION_PAGE_XYZ,
	DESTINATION_PAGE_FIT,
	DESTINATION_PAGE_FITH,
	DESTINATION_PAGE_FITV,
	DESTINATION_PAGE_FITR,
	DESTINATION_PAGE_FITB,
	DESTINATION_PAGE_FITBH,
	DESTINATION_PAGE_FITBV
};

class CHE_PDF_Destination : public CHE_Object
{
public:
	CHE_PDF_Destination( CHE_Allocator * pAllocator = NULL );

	PDF_DESTINATION_TYPE		GetType() const { return mType; }
	HE_VOID						SetType( PDF_DESTINATION_TYPE type ) { mType = type; }

	HE_FLOAT					GetLeft() const { return mLeft; }
	HE_FLOAT					GetRight() const { return mRight; }
	HE_FLOAT					GetTop() const { return mTop; }
	HE_FLOAT					GetBottom() const { return mBottom; }
	HE_FLOAT					GetZoom() const { return mZoom; }

	HE_VOID						SetLeft( HE_FLOAT left ) { mLeft = left; }
	HE_VOID						SetRight( HE_FLOAT right ) { mRight = right; }
	HE_VOID						SetTop( HE_FLOAT top ) { mTop = top; }
	HE_VOID						SetBottom( HE_FLOAT bottom ) { mBottom = bottom; }
	HE_VOID						SetZoom( HE_FLOAT zoom ) { mZoom = zoom; }

private:
	PDF_DESTINATION_TYPE		mType;

	HE_FLOAT					mLeft;
	HE_FLOAT					mRight;
	HE_FLOAT					mTop;
	HE_FLOAT					mBottom;
	HE_FLOAT					mZoom;
};

#endif