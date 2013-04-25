#ifndef _CHE_PDF_COLORSPACE_H_
#define _CHE_PDF_COLORSPACE_H_

#include "che_pdf_objects.h"


// enum PDF_COLORSPACE_TYPE
// {
// 	COLORSPACE_DEVICE	= 0x00,
// 	COLORSPACE_CIEBASE	= 0x01,
// 	COLORSPACE_SPECIAL	= 0x02
// };


enum PDF_COLORSPACE_TYPE
{
	COLORSPACE_DEVICE_GRAY			= 0x01,
	COLORSPACE_DEVICE_RGB			= 0x02,
	COLORSPACE_DEVICE_CMYK			= 0x03,
	COLORSPACE_CIEBASE_CALGRAY		= 0x04,
	COLORSPACE_CIEBASE_CALRGB		= 0x05,
	COLORSPACE_CIEBASE_CALCMYK		= 0x06,
	COLORSPACE_CIEBASE_CALLAB		= 0x07,
	COLORSPACE_CIEBASE_ICCBASED		= 0x08,
	COLORSPACE_SPECIAL_PATTERN		= 0x09,
	COLORSPACE_SPECIAL_INDEXED		= 0x0A,
	COLORSPACE_SPECIAL_SEPARATION	= 0x0B,
	COLORSPACE_SPECIAL_DEVICEN		= 0x0C
};


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

	HE_DWORD GetArgb( CHE_PDF_Color & color );

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