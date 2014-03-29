#ifndef _CHE_PDF_COLORSPACE_H_
#define _CHE_PDF_COLORSPACE_H_

#include "che_pdf_component.h"
#include "che_pdf_pattern.h"
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
	CHE_PDF_Color( CHE_Allocator * pAllocator = NULL ) 
		: CHE_Object( pAllocator ), mIndex(0)
	{
		for ( HE_ULONG i = 0; i < 4; ++i )
		{
			mComponent[i] = 0.0f;
		}
	}

	CHE_PDF_Color * Clone() const
	{
		CHE_PDF_Color * pRet = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		for ( HE_ULONG i = 0; i < 4; ++i )
		{
			pRet->mComponent[i] = mComponent[i]; 
		}
		pRet->mIndex = mIndex;
		return pRet;
	}

	HE_VOID Push( HE_FLOAT val )
	{
		if ( mIndex < 4 )
		{
			mComponent[mIndex++] = val;
		}
	}

	HE_ULONG GetComponentCount() const
	{
		return mIndex;
	}

	HE_FLOAT GetComponent( HE_ULONG index ) const
	{
		if ( index < mIndex )
		{
			return mComponent[index];
		}else{
			return 0.0f;
		}
	}

	HE_VOID Clear()
	{
		mIndex = 0;
	}

private:
	HE_FLOAT				mComponent[4];
	HE_ULONG				mIndex;
};


class CHE_PDF_ColorSpace;

class CHE_PDF_ColorSpacePtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_ColorSpace * operator->() const { return (CHE_PDF_ColorSpace*)mpCom; }
};

class CHE_PDF_ColorSpace : public CHE_PDF_Component
{
public:
	static CHE_PDF_ColorSpacePtr CreateDeviceGray();

	static CHE_PDF_ColorSpacePtr CreateDeviceRGB();

	static CHE_PDF_ColorSpacePtr CreateDeviceCMYK();

	static CHE_PDF_ColorSpacePtr CreatePattern();

	static CHE_PDF_ColorSpacePtr Create( const CHE_ByteString & str, CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_ColorSpacePtr Create( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_ColorSpacePtr Convert( const CHE_PDF_ComponentPtr & componetPtr );

	~CHE_PDF_ColorSpace();

	PDF_COLORSPACE_TYPE		GetColorSpaceType() const { return mColorSpaceType; }

	HE_BYTE					GetComponentCount() const;

	HE_ARGB					GetARGBValue( const CHE_PDF_Color & color ) const;
    
    HE_DOUBLE				mWhitePoint[3];
	HE_DOUBLE				mBlackPoint[3];
	HE_DOUBLE				mGamma[3];
	HE_DOUBLE				mMatrix[9];
	HE_DOUBLE				mRange[8];
    
    CHE_PDF_StreamAcc       mStmAcc;
    
	//for indexed colorspace
	HE_ULONG				mIndexCount;
	HE_LPBYTE				mpIndexTable;
	HE_ULONG				mIndexTableSize;
    CHE_PDF_ColorSpacePtr	mBaseColorspace;
    
    CHE_PDF_TilingPtr GetTiling() const { return mpTiling; }
    
	HE_VOID	SetTiling( const CHE_PDF_TilingPtr & pTiling ) { mpTiling = pTiling; }

private:
	CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type );
	CHE_PDF_ColorSpace( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

	HE_ARGB					lab_to_rgb( const CHE_PDF_Color & color ) const;

	PDF_COLORSPACE_TYPE		mColorSpaceType;
	CHE_PDF_ObjectPtr		mObjPtr;

	HE_ULONG				mComponentCount;


	//for separation colorspace
	CHE_PDF_FunctionPtr		mFunction;
    
    CHE_PDF_TilingPtr		mpTiling;
	

	friend class CHE_Allocator;
};


#endif