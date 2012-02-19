#ifndef _CHE_PDF_COLORSPACE_H_
#define _CHE_PDF_COLORSPACE_H_

#include "che_pdf_objects.h"

class CHE_PDF_Color : public CHE_Object
{
public:
	CHE_PDF_Color( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}

	std::vector<HE_FLOAT> mConponents;

	CHE_PDF_Color * Clone() const
	{
		CHE_PDF_Color * pRet = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		pRet->mConponents = mConponents;
		return pRet;
	}
};

class CHE_PDF_ColorSpace : public CHE_Object
{
public:
	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mType(type), mpObj(NULL) {}

	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, const CHE_ByteString & resName, CHE_PDF_Object * pObj, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mType(type), mResName(resName), mpObj(pObj) {}

	~CHE_PDF_ColorSpace()
	{
		if ( mpObj )
		{
			mpObj->Release();
		}
	}

	PDF_COLORSPACE_TYPE GetType() const { return mType; }

	CHE_ByteString GetResName() const { return mResName; }

	CHE_Object * GetObj() const { return mpObj; }

	CHE_PDF_ColorSpace * Clone() const
	{
		CHE_PDF_ColorSpace * pRet = GetAllocator()->New<CHE_PDF_ColorSpace>( mType, GetAllocator() );
		pRet->mResName = mResName;
		if ( mpObj )
		{
			pRet->mpObj = mpObj->Clone();
		}
		return pRet;
	}

private:
	PDF_COLORSPACE_TYPE mType;
	CHE_PDF_Object * mpObj;
	CHE_ByteString mResName;
};

#endif