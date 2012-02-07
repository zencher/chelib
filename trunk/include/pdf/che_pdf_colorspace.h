#ifndef _CHE_PDF_COLORSPACE_H_
#define _CHE_PDF_COLORSPACE_H_

#include "che_pdf_objects.h"

class CHE_PDF_Color : public CHE_Object
{
};

class CHE_PDF_ColorSpace : public CHE_Object
{
public:
	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mType(type) {}

	//CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, const CHE_ByteString & )

	PDF_COLORSPACE_TYPE GetType() { return mType; }

private:
	PDF_COLORSPACE_TYPE mType;
	CHE_PDF_Reference * mRef;
	CHE_ByteString mName;
};

#endif