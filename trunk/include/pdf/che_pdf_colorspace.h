#ifndef _CHE_PDF_COLORSPACE_H_
#define _CHE_PDF_COLORSPACE_H_

#include "che_pdf_objects.h"
#include "che_pdf_function.h"

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
	static CHE_PDF_ColorSpace * Create( const CHE_ByteString & name, CHE_Allocator * pAllocator = NULL );
	static CHE_PDF_ColorSpace *	Create( const CHE_PDF_ObjectPtr & pObj, CHE_Allocator * pAllocator = NULL );
	static CHE_PDF_ColorSpace *	Create( const CHE_PDF_NamePtr & pName, CHE_Allocator * pAllocator = NULL );
	static CHE_PDF_ColorSpace *	Create( const CHE_PDF_ArrayPtr & pArray, CHE_Allocator * pAllocator = NULL );
	static CHE_PDF_ColorSpace *	Create( const CHE_PDF_ReferencePtr & pRef, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, CHE_Allocator * pAllocator = NULL );
	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, const CHE_ByteString & resName, const CHE_PDF_ObjectPtr & pObj, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_ColorSpace();

	PDF_COLORSPACE_TYPE		GetType() const		{ return mType; }

	CHE_ByteString			GetResName() const	{ return mResName; }

	CHE_ByteString			GetName() const;

	CHE_PDF_ObjectPtr		GetObj() const		{ return mpObj; }

	CHE_PDF_ColorSpace *	Clone() const;

	HE_BOOL					IsDeviceColorSpace() const;

	HE_BYTE					GetComponentCount() const;

	HE_ARGB					GetARGBValue( CHE_PDF_Color & color ) const;

private:
	HE_ARGB					lab_to_rgb( CHE_PDF_Color & color ) const;

	PDF_COLORSPACE_TYPE		mType;
	CHE_PDF_ObjectPtr		mpObj;
	CHE_ByteString			mResName;

	CHE_PDF_ColorSpace *	mpBaseColorspace;

	//for index color space
	HE_ULONG				mIndexCount;
	HE_LPBYTE				mpIndexTable;
	HE_ULONG				mIndexTableSize;

	//for seperation
	CHE_ByteString			mName;
	CHE_PDF_Function *		mpFunction;

	//for deviceN

};


#endif