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
		: CHE_Object(pAllocator), mType(type) {}

	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, const CHE_ByteString & resName, const CHE_PDF_ObjectPtr & pObj, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mType(type), mResName(resName), mpObj(pObj) {}

	PDF_COLORSPACE_TYPE GetType() const { return mType; }

	CHE_ByteString GetResName() const { return mResName; }

	CHE_PDF_ObjectPtr GetObj() const { return mpObj; }

	HE_BOOL IsDeviceColorSpace();

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
	CHE_PDF_ObjectPtr mpObj;
	CHE_ByteString mResName;
};

CHE_PDF_ColorSpace * GetColorSpace( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL );

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_ObjectPtr & pObj, CHE_Allocator * pAllocator = NULL );

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_NamePtr & pName, CHE_Allocator * pAllocator = NULL );

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_ArrayPtr & pArray, CHE_Allocator * pAllocator = NULL );

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_ReferencePtr & pRef, CHE_Allocator * pAllocator = NULL );

CHE_ByteString GetColorSpaceName( CHE_PDF_ColorSpace * pColorSpace );

#endif