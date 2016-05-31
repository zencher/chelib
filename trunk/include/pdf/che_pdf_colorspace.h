#ifndef _CHE_PDF_COLORSPACE_H_
#define _CHE_PDF_COLORSPACE_H_

#include "che_pdf_component.h"
#include "che_pdf_function.h"


enum PDF_COLORSPACE_TYPE
{
	COLORSPACE_DEVICE_GRAY			= 0x01,
	COLORSPACE_DEVICE_RGB			= 0x02,
	COLORSPACE_DEVICE_CMYK			= 0x03,
	COLORSPACE_CIEBASE_CALGRAY		= 0x04,
	COLORSPACE_CIEBASE_CALRGB		= 0x05,
	COLORSPACE_CIEBASE_CALLAB		= 0x06,
	COLORSPACE_CIEBASE_ICCBASED		= 0x07,
	COLORSPACE_SPECIAL_PATTERN		= 0x08,
	COLORSPACE_SPECIAL_INDEXED		= 0x09,
	COLORSPACE_SPECIAL_SEPARATION	= 0x0A,
	COLORSPACE_SPECIAL_DEVICEN		= 0x0B
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
class CHE_PDF_CS_CalGray;
class CHE_PDF_CS_CalRGB;
class CHE_PDF_CS_CalLab;
class CHE_PDF_CS_ICCBased;
class CHE_PDF_CS_Pattern;
class CHE_PDF_CS_Indexed;
class CHE_PDF_CS_Separation;
class CHE_PDF_CS_DeviceN;


class CHE_PDF_ColorSpacePtr : public CHE_PDF_ComponentPtr
{
public:
    CHE_PDF_ColorSpace * operator->() const;
};

class CHE_PDF_CS_CalGrayPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_CalGray * operator->() const;
};

class CHE_PDF_CS_CalRGBPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_CalRGB * operator->() const;
};

class CHE_PDF_CS_CalLabPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_CalLab * operator->() const;
};

class CHE_PDF_CS_ICCBasedPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_ICCBased * operator->() const;
};

class CHE_PDF_CS_PatternPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_Pattern * operator->() const;
};

class CHE_PDF_CS_IndexedPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_Indexed * operator->() const;
};

class CHE_PDF_CS_SeparationPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_Separation * operator->() const;
};

class CHE_PDF_CS_DeviceNPtr : public CHE_PDF_ColorSpacePtr
{
public:
    CHE_PDF_CS_DeviceN * operator->() const;
};


class CHE_PDF_ColorSpace : public CHE_PDF_Component
{
public:
	static CHE_PDF_ColorSpacePtr CreateDeviceGray(CHE_Allocator * pAllocator = NULL);

	static CHE_PDF_ColorSpacePtr CreateDeviceRGB(CHE_Allocator * pAllocator = NULL);

	static CHE_PDF_ColorSpacePtr CreateDeviceCMYK(CHE_Allocator * pAllocator = NULL);

	static CHE_PDF_ColorSpacePtr CreatePattern(CHE_Allocator * pAllocator = NULL);

	static CHE_PDF_ColorSpacePtr Create(const CHE_ByteString & str, CHE_Allocator * pAllocator = NULL);

	static CHE_PDF_ColorSpacePtr Create(const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL);
    
    static CHE_PDF_ColorSpacePtr Convert( const CHE_PDF_ComponentPtr & componetPtr );

	virtual ~CHE_PDF_ColorSpace();

	PDF_COLORSPACE_TYPE         GetColorSpaceType() const { return mColorSpaceType; }

	HE_UINT32                   GetComponentCount() const;
    
    CHE_PDF_CS_CalGrayPtr       GetCalGrayPtr() const;

    CHE_PDF_CS_CalRGBPtr        GetCalRGBPtr() const;
    
    CHE_PDF_CS_CalLabPtr        GetCalLabPtr() const;
    
    CHE_PDF_CS_ICCBasedPtr      GetICCBasedPtr() const;
    
    CHE_PDF_CS_IndexedPtr       GetIndexedPtr() const;
    
    CHE_PDF_CS_PatternPtr       GetPatternPtr() const;
    
    CHE_PDF_CS_SeparationPtr    GetSeparationPtr() const;
    
    CHE_PDF_CS_DeviceNPtr       GetDeviceNPtr() const;

#ifdef WIN32
	virtual HE_ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif

protected:
	CHE_PDF_ColorSpace(PDF_COLORSPACE_TYPE type, HE_UINT32 componentCount, CHE_Allocator * pAllocator = NULL);

	PDF_COLORSPACE_TYPE		mColorSpaceType;
	HE_UINT32				mComponentCount;

	friend class CHE_Allocator;
};


class CHE_PDF_CS_CalGray : public CHE_PDF_ColorSpace
{
public:
    HE_DOUBLE   mWhitePoint[3];
    HE_DOUBLE   mBlackPoint[3];
    HE_DOUBLE   mGamma;

private:
    CHE_PDF_CS_CalGray(CHE_Allocator * pAllocator = NULL);
    
    CHE_PDF_CS_CalGray(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_CalRGB : public CHE_PDF_ColorSpace
{
public:
    HE_DOUBLE   mWhitePoint[3];
    HE_DOUBLE   mBlackPoint[3];
    HE_DOUBLE	mGamma[3];
    HE_DOUBLE	mMatrix[9];
    
private:
    CHE_PDF_CS_CalRGB(CHE_Allocator * pAllocator = NULL);
    
    CHE_PDF_CS_CalRGB(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_CalLab : public CHE_PDF_ColorSpace
{
public:
    HE_DOUBLE				mWhitePoint[3];
    HE_DOUBLE				mBlackPoint[3];
    HE_DOUBLE				mRange[4];

#ifdef WIN32
	HE_ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_CalLab(CHE_Allocator * pAllocator = NULL);
    
    CHE_PDF_CS_CalLab(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_ICCBased : public CHE_PDF_ColorSpace
{
public:
    HE_DOUBLE				mRange[8];
    CHE_PDF_ColorSpacePtr   mAlternate;
    CHE_PDF_StreamAcc       mStmAcc;

#ifdef WIN32
	HE_ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_ICCBased(const CHE_PDF_StreamPtr & stream, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_Indexed : public CHE_PDF_ColorSpace
{
public:
    HE_ULONG				mIndexCount;
    HE_ULONG				mIndexTableSize;
    HE_LPBYTE				mpIndexTable;
    HE_LPBYTE               mpNewTable;
    HE_ULONG                mNewTableSize;
    CHE_PDF_ColorSpacePtr	mBaseColorSpace;

#ifdef WIN32
	HE_ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_Indexed(const CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_Pattern : public CHE_PDF_ColorSpace
{
public:
    CHE_PDF_ComponentPtr    mTiling;
    CHE_PDF_ColorSpacePtr   mUnderLyingColorspace;
    
private:
    CHE_PDF_CS_Pattern(CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_Separation : public CHE_PDF_ColorSpace
{
public:
    CHE_PDF_ColorSpacePtr   mBaseColorSpace;
    CHE_PDF_FunctionPtr		mFunction;

#ifdef WIN32
	HE_ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_Separation(CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_DeviceN : public CHE_PDF_ColorSpace
{
public:
    CHE_PDF_ColorSpacePtr   mBaseColorSpace;
    CHE_PDF_FunctionPtr		mFunction;

#ifdef WIN32
	HE_ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif

private:
    CHE_PDF_CS_DeviceN(CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = NULL);
    
    friend class CHE_Allocator;
};

#endif