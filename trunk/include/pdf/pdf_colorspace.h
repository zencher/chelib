#ifndef _CHELIB_PDF_COLORSPACE_H_
#define _CHELIB_PDF_COLORSPACE_H_

#include "pdf_component.h"
#include "pdf_function.h"

namespace chelib {

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


class CPDF_Color : public CObject
{
public:
	CPDF_Color( CAllocator * pAllocator = nullptr ) 
		: CObject( pAllocator ), mIndex(0)
	{
		for ( uint32 i = 0; i < 4; ++i )
		{
			mComponent[i] = 0.0f;
		}
	}
    
    CPDF_Color( const CPDF_Color & cs )
    : CObject( cs.GetAllocator() ), mIndex(0)
    {
        mComponent[0] = cs.mComponent[0];
        mComponent[1] = cs.mComponent[1];
        mComponent[2] = cs.mComponent[2];
        mComponent[3] = cs.mComponent[3];
        mIndex = cs.mIndex;
    }

	/*CPDF_Color * Clone() const
	{
		CPDF_Color * pRet = GetAllocator()->New<CPDF_Color>( GetAllocator() );
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
	FLOAT mComponent[4];
	uint32 mIndex;
};


class CPDF_ColorSpace;
class CPDF_CS_CalGray;
class CPDF_CS_CalRGB;
class CPDF_CS_CalLab;
class CPDF_CS_ICCBased;
class CPDF_CS_Pattern;
class CPDF_CS_Indexed;
class CPDF_CS_Separation;
class CPDF_CS_DeviceN;


class CPDF_ColorSpacePtr : public CPDF_ComponentPtr
{
public:
    CPDF_ColorSpace * operator->() const;
};

class CPDF_CS_CalGrayPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_CalGray * operator->() const;
};

class CPDF_CS_CalRGBPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_CalRGB * operator->() const;
};

class CPDF_CS_CalLabPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_CalLab * operator->() const;
};

class CPDF_CS_ICCBasedPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_ICCBased * operator->() const;
};

class CPDF_CS_PatternPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_Pattern * operator->() const;
};

class CPDF_CS_IndexedPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_Indexed * operator->() const;
};

class CPDF_CS_SeparationPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_Separation * operator->() const;
};

class CPDF_CS_DeviceNPtr : public CPDF_ColorSpacePtr
{
public:
    CPDF_CS_DeviceN * operator->() const;
};


class CPDF_ColorSpace : public CPDF_Component
{
public:
	static CPDF_ColorSpacePtr CreateDeviceGray( CAllocator * pAllocator = nullptr );
	static CPDF_ColorSpacePtr CreateDeviceRGB( CAllocator * pAllocator = nullptr );
	static CPDF_ColorSpacePtr CreateDeviceCMYK( CAllocator * pAllocator = nullptr );
	static CPDF_ColorSpacePtr CreatePattern(CAllocator * pAllocator = nullptr );
	static CPDF_ColorSpacePtr Create( const CByteString & str, CAllocator * pAllocator = nullptr );
	static CPDF_ColorSpacePtr Create( const CPDF_ObjectPtr & objPtr, CAllocator * pAllocator = nullptr );
    static CPDF_ColorSpacePtr Convert( const CPDF_ComponentPtr & componetPtr );

	virtual ~CPDF_ColorSpace();

	PDF_COLORSPACE_TYPE GetColorSpaceType() const { return mColorSpaceType; }

	uint32 GetComponentCount() const;
    
    CPDF_CS_CalGrayPtr GetCalGrayPtr() const;
    CPDF_CS_CalRGBPtr GetCalRGBPtr() const;
    CPDF_CS_CalLabPtr GetCalLabPtr() const;
    CPDF_CS_ICCBasedPtr GetICCBasedPtr() const;
    CPDF_CS_IndexedPtr GetIndexedPtr() const;
    CPDF_CS_PatternPtr GetPatternPtr() const;
    CPDF_CS_SeparationPtr GetSeparationPtr() const;
    CPDF_CS_DeviceNPtr GetDeviceNPtr() const;

#ifdef WIN32
	virtual ARGB GetARGBValue(const CPDF_Color & color) const;
#endif

protected:
	CPDF_ColorSpace( PDF_COLORSPACE_TYPE type, uint32 componentCount,
                     CAllocator * pAllocator = nullptr);

	PDF_COLORSPACE_TYPE mColorSpaceType;
	uint32 mComponentCount;

	friend class CAllocator;
};


class CPDF_CS_CalGray : public CPDF_ColorSpace
{
public:
    DOUBLE mWhitePoint[3];
    DOUBLE mBlackPoint[3];
    DOUBLE mGamma;

private:
    CPDF_CS_CalGray( CAllocator * pAllocator = nullptr );
    CPDF_CS_CalGray( const CPDF_DictionaryPtr & dict,
                     CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};


class CPDF_CS_CalRGB : public CPDF_ColorSpace
{
public:
    DOUBLE mWhitePoint[3];
    DOUBLE mBlackPoint[3];
    DOUBLE mGamma[3];
    DOUBLE mMatrix[9];
    
private:
    CPDF_CS_CalRGB( CAllocator * pAllocator = nullptr );
    CPDF_CS_CalRGB( const CPDF_DictionaryPtr & dict,
                    CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};


class CPDF_CS_CalLab : public CPDF_ColorSpace
{
public:
    DOUBLE mWhitePoint[3];
    DOUBLE mBlackPoint[3];
    DOUBLE mRange[4];

#ifdef WIN32
	ARGB GetARGBValue( const CPDF_Color & color ) const;
#endif
    
private:
    CPDF_CS_CalLab( CAllocator * pAllocator = nullptr );
    CPDF_CS_CalLab( const CPDF_DictionaryPtr & dict,
                    CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};


class CPDF_CS_ICCBased : public CPDF_ColorSpace
{
public:
    DOUBLE mRange[8];
    CPDF_ColorSpacePtr mAlternate;
    CPDF_StreamAcc mStmAcc;

#ifdef WIN32
	ARGB GetARGBValue( const CPDF_Color & color ) const;
#endif
    
private:
    CPDF_CS_ICCBased( const CPDF_StreamPtr & stream,
                      CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};


class CPDF_CS_Indexed : public CPDF_ColorSpace
{
public:
    size_t mIndexCount;
    size_t mIndexTableSize;
    PBYTE mpIndexTable;
    PBYTE mpNewTable;
    size_t mNewTableSize;
    CPDF_ColorSpacePtr mBaseColorSpace;

#ifdef WIN32
	ARGB GetARGBValue( const CPDF_Color & color ) const;
#endif
    
private:
    CPDF_CS_Indexed( const CPDF_ArrayPtr & array,
                     CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};


class CPDF_CS_Pattern : public CPDF_ColorSpace
{
public:
    CPDF_ComponentPtr mPattern;
    //CPDF_ComponentPtr mShading;
    CMatrix mPatternMatrix;
    CPDF_ColorSpacePtr mUnderLyingColorspace;
    
private:
    CPDF_CS_Pattern(CAllocator * pAllocator = nullptr);
    
    friend class CAllocator;
};


class CPDF_CS_Separation : public CPDF_ColorSpace
{
public:
    CPDF_ColorSpacePtr mBaseColorSpace;
    CPDF_FunctionPtr mFunction;

#ifdef WIN32
	ARGB GetARGBValue( const CPDF_Color & color ) const;
#endif
    
private:
    CPDF_CS_Separation( CPDF_ArrayPtr & array, CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};


class CPDF_CS_DeviceN : public CPDF_ColorSpace
{
public:
    CPDF_ColorSpacePtr mBaseColorSpace;
    CPDF_FunctionPtr mFunction;

#ifdef WIN32
	ARGB GetARGBValue( const CPDF_Color & color ) const;
#endif

private:
    CPDF_CS_DeviceN( CPDF_ArrayPtr & array, CAllocator * pAllocator = nullptr );
    
    friend class CAllocator;
};

}//namespace

#endif