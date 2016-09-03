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
	CHE_PDF_Color( CHE_Allocator * pAllocator = nullptr ) 
		: CHE_Object( pAllocator ), mIndex(0)
	{
		for ( uint32 i = 0; i < 4; ++i )
		{
			mComponent[i] = 0.0f;
		}
	}
    
    CHE_PDF_Color( const CHE_PDF_Color & cs )
    : CHE_Object( cs.GetAllocator() ), mIndex(0)
    {
        mComponent[0] = cs.mComponent[0];
        mComponent[1] = cs.mComponent[1];
        mComponent[2] = cs.mComponent[2];
        mComponent[3] = cs.mComponent[3];
        mIndex = cs.mIndex;
    }

	/*CHE_PDF_Color * Clone() const
	{
		CHE_PDF_Color * pRet = GetAllocator()->New<CHE_PDF_Color>( GetAllocator() );
		for ( size_t i = 0; i < 4; ++i )
		{
			pRet->mComponent[i] = mComponent[i]; 
		}
		pRet->mIndex = mIndex;
		return pRet;
	}*/

	void Push( FLOAT val )
	{
		if ( mIndex < 4 )
		{
			mComponent[mIndex++] = val;
		}
	}

	uint32 GetComponentCount() const
	{
		return mIndex;
	}

	FLOAT GetComponent( uint32 index ) const
	{
		if ( index < mIndex )
		{
			return mComponent[index];
		}else{
			return 0.0f;
		}
	}

	void Clear()
	{
		mIndex = 0;
	}

private:
	FLOAT				mComponent[4];
	uint32				mIndex;
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
	static CHE_PDF_ColorSpacePtr CreateDeviceGray(CHE_Allocator * pAllocator = nullptr);

	static CHE_PDF_ColorSpacePtr CreateDeviceRGB(CHE_Allocator * pAllocator = nullptr);

	static CHE_PDF_ColorSpacePtr CreateDeviceCMYK(CHE_Allocator * pAllocator = nullptr);

	static CHE_PDF_ColorSpacePtr CreatePattern(CHE_Allocator * pAllocator = nullptr);

	static CHE_PDF_ColorSpacePtr Create(const CHE_ByteString & str, CHE_Allocator * pAllocator = nullptr);

	static CHE_PDF_ColorSpacePtr Create(const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = nullptr);
    
    static CHE_PDF_ColorSpacePtr Convert( const CHE_PDF_ComponentPtr & componetPtr );

	virtual ~CHE_PDF_ColorSpace();

	PDF_COLORSPACE_TYPE         GetColorSpaceType() const { return mColorSpaceType; }

	uint32                   GetComponentCount() const;
    
    CHE_PDF_CS_CalGrayPtr       GetCalGrayPtr() const;

    CHE_PDF_CS_CalRGBPtr        GetCalRGBPtr() const;
    
    CHE_PDF_CS_CalLabPtr        GetCalLabPtr() const;
    
    CHE_PDF_CS_ICCBasedPtr      GetICCBasedPtr() const;
    
    CHE_PDF_CS_IndexedPtr       GetIndexedPtr() const;
    
    CHE_PDF_CS_PatternPtr       GetPatternPtr() const;
    
    CHE_PDF_CS_SeparationPtr    GetSeparationPtr() const;
    
    CHE_PDF_CS_DeviceNPtr       GetDeviceNPtr() const;

#ifdef WIN32
	virtual ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif

protected:
	CHE_PDF_ColorSpace(PDF_COLORSPACE_TYPE type, uint32 componentCount, CHE_Allocator * pAllocator = nullptr);

	PDF_COLORSPACE_TYPE		mColorSpaceType;
	uint32				mComponentCount;

	friend class CHE_Allocator;
};


class CHE_PDF_CS_CalGray : public CHE_PDF_ColorSpace
{
public:
    DOUBLE   mWhitePoint[3];
    DOUBLE   mBlackPoint[3];
    DOUBLE   mGamma;

private:
    CHE_PDF_CS_CalGray(CHE_Allocator * pAllocator = nullptr);
    
    CHE_PDF_CS_CalGray(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_CalRGB : public CHE_PDF_ColorSpace
{
public:
    DOUBLE   mWhitePoint[3];
    DOUBLE   mBlackPoint[3];
    DOUBLE	mGamma[3];
    DOUBLE	mMatrix[9];
    
private:
    CHE_PDF_CS_CalRGB(CHE_Allocator * pAllocator = nullptr);
    
    CHE_PDF_CS_CalRGB(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_CalLab : public CHE_PDF_ColorSpace
{
public:
    DOUBLE				mWhitePoint[3];
    DOUBLE				mBlackPoint[3];
    DOUBLE				mRange[4];

#ifdef WIN32
	ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_CalLab(CHE_Allocator * pAllocator = nullptr);
    
    CHE_PDF_CS_CalLab(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_ICCBased : public CHE_PDF_ColorSpace
{
public:
    DOUBLE				mRange[8];
    CHE_PDF_ColorSpacePtr   mAlternate;
    CHE_PDF_StreamAcc       mStmAcc;

#ifdef WIN32
	ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_ICCBased(const CHE_PDF_StreamPtr & stream, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_Indexed : public CHE_PDF_ColorSpace
{
public:
    size_t				mIndexCount;
    size_t				mIndexTableSize;
    PBYTE				mpIndexTable;
    PBYTE               mpNewTable;
    size_t                mNewTableSize;
    CHE_PDF_ColorSpacePtr	mBaseColorSpace;

#ifdef WIN32
	ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_Indexed(const CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_Pattern : public CHE_PDF_ColorSpace
{
public:
    CHE_PDF_ComponentPtr    mPattern;
    //CHE_PDF_ComponentPtr    mShading;
    CHE_Matrix              mPatternMatrix;
    CHE_PDF_ColorSpacePtr   mUnderLyingColorspace;
    
private:
    CHE_PDF_CS_Pattern(CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_Separation : public CHE_PDF_ColorSpace
{
public:
    CHE_PDF_ColorSpacePtr   mBaseColorSpace;
    CHE_PDF_FunctionPtr		mFunction;

#ifdef WIN32
	ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif
    
private:
    CHE_PDF_CS_Separation(CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};


class CHE_PDF_CS_DeviceN : public CHE_PDF_ColorSpace
{
public:
    CHE_PDF_ColorSpacePtr   mBaseColorSpace;
    CHE_PDF_FunctionPtr		mFunction;

#ifdef WIN32
	ARGB	GetARGBValue(const CHE_PDF_Color & color) const;
#endif

private:
    CHE_PDF_CS_DeviceN(CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator = nullptr);
    
    friend class CHE_Allocator;
};

#endif