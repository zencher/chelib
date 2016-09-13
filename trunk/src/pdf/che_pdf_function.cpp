#include "../../include/pdf/che_pdf_function.h"
#include "../../include/pdf/che_pdf_parser.h"

#include <math.h>
#include <float.h>

namespace chelib {

static inline float lerp(float x, float xmin, float xmax, float ymin, float ymax)
{
	if (xmin == xmax)
		return ymin;
	if (ymin == ymax)
		return ymin;
	return ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin);
}

static inline float fz_clamp(float f, float min, float max)
{
	return (f > min ? (f < max ? f : max) : min);
}

static inline int fz_clampi(int i, float min, float max)
{
	return (i > min ? (i < max ? i : max) : min);
}

#define DIV_BY_ZERO(a, b, min, max) (((a) < 0) ^ ((b) < 0) ? (min) : (max))

CPDF_Function * CPDF_FunctionPtr::operator->() const
{
	return (CPDF_Function*)mpCom;
}

CPDF_FunctionPtr CPDF_Function::Create( const CPDF_ObjectPtr & rootObjPtr, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_FunctionPtr ptr;

	if ( !rootObjPtr )
	{
		return ptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}

	CPDF_ObjectPtr objPtr;
	CPDF_DictionaryPtr dictPtr;
    CPDF_StreamPtr streamPtr;
	CPDF_Function * pTmp = nullptr;

	if ( rootObjPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = rootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			dictPtr = objPtr->GetDictPtr();
		}else{
			objPtr = rootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
			if ( objPtr )
			{
                streamPtr = objPtr->GetStreamPtr();
				dictPtr = streamPtr->GetDictPtr();
			}else{
				return ptr;
			}
		}
	}else if ( rootObjPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		dictPtr = rootObjPtr->GetDictPtr();
	}else if ( rootObjPtr->GetType() == OBJ_TYPE_STREAM )
	{
        streamPtr = rootObjPtr->GetStreamPtr();
		dictPtr = streamPtr->GetDictPtr();
	}else{
		return ptr;
	}

	objPtr = dictPtr->GetElement( "FunctionType", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		switch( objPtr->GetNumberPtr()->GetInteger() )
		{
		case 0:
			pTmp = pAllocator->New<CPDF_Function_Sampled>( rootObjPtr, pAllocator );
			break;
		case 2:
			pTmp = pAllocator->New<CPDF_Function_Exponential>( rootObjPtr, pAllocator );
			break;
		case 3:
			pTmp = pAllocator->New<CPDF_Function_Stitching>( rootObjPtr, pAllocator );
			break;
		case 4:
			pTmp = pAllocator->New<CPDF_Function_PostScript>( streamPtr, pAllocator );
			break;
		default:;
		}
	}

	if ( pTmp )
	{
		if ( !pTmp->IsError() )
		{
			ptr.Reset( pTmp );
		}
	}

	return ptr;
}

FLOAT CPDF_Function::GetDomianMin( uint32 index ) const
{
	if ( index < GetInputCount() && mpDomain )
	{
		return mpDomain[index * 2];
	}
	return 0;
}

FLOAT CPDF_Function::GetDomianMax( uint32 index ) const
{
	if ( index < GetInputCount() && mpDomain )
	{
		return mpDomain[index * 2 + 1];
	}
	return 0;
}

FLOAT CPDF_Function::GetRangeMin( uint32 index ) const
{
	if ( index < GetOutputCount() && mpRange )
	{
		return mpRange[index * 2];
	}
	return 0;
}

FLOAT CPDF_Function::GetRangeMax( uint32 index ) const
{
	if ( index < GetOutputCount() && mpRange )
	{
		return mpRange[index * 2 + 1];
	}
	return 0;
}

CPDF_Function::CPDF_Function(const CPDF_ObjectPtr & root, CAllocator * pAllocator/*= nullptr*/)
	: CPDF_Component(COMPONENT_TYPE_Function, root, pAllocator), mFunctionType(FUNCTION_TYPE_SAMPLE),
	mInputCount(0), mOutputCount(0), mpDomain(nullptr), mpRange(nullptr)
{
	CPDF_ObjectPtr		objPtr;
	CPDF_DictionaryPtr	dictPtr;
	CPDF_StreamPtr		streamPtr;
	if (mRootObject->GetType() == OBJ_TYPE_REFERENCE)
	{
		objPtr = mRootObject->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
		if (objPtr)
		{
			dictPtr = objPtr->GetDictPtr();
		}else{
			objPtr = mRootObject->GetRefPtr()->GetRefObj(OBJ_TYPE_STREAM);
			if (objPtr)
			{
				streamPtr = objPtr->GetStreamPtr();
				dictPtr = streamPtr->GetDictPtr();
			}
		}
	}
	else if (mRootObject->GetType() == OBJ_TYPE_DICTIONARY)
	{
		dictPtr = mRootObject->GetDictPtr();
	}
	else if (mRootObject->GetType() == OBJ_TYPE_STREAM )
	{
		streamPtr = mRootObject->GetStreamPtr();
		dictPtr = streamPtr->GetDictPtr();
	}

	objPtr = dictPtr->GetElement("FunctionType", OBJ_TYPE_NUMBER);
	if (objPtr)
	{
		switch (objPtr->GetNumberPtr()->GetInteger())
		{
		case 0:
			mFunctionType = FUNCTION_TYPE_SAMPLE;
			break;
		case 2:
			mFunctionType = FUNCTION_TYPE_EXPONENTIAL;
			break;
		case 3:
			mFunctionType = FUNCTION_TYPE_STITCHING;
			break;
		case 4:
			mFunctionType = FUNCTION_TYPE_POSTSCRIPT;
			break;
		default:
			SetError(COMPONENT_ERROR_CONSTRUCTION);
			return;
		}
	}

	objPtr = dictPtr->GetElement("Domain", OBJ_TYPE_ARRAY);
	if (objPtr)
	{
		CPDF_ArrayPtr array = objPtr->GetArrayPtr();
		mInputCount = (uint32)array->GetCount() / 2;
		mpDomain = GetAllocator()->NewArray<FLOAT>(mInputCount * 2);
		for (size_t i = 0; i < array->GetCount(); ++i)
		{
			objPtr = array->GetElement(i, OBJ_TYPE_NUMBER);
			if (objPtr)
			{
				mpDomain[i] = objPtr->GetNumberPtr()->GetFloat();
			}
			else{
				mpDomain[i] = 0.0f;
			}
		}
	}
	else{
		SetError(COMPONENT_ERROR_CONSTRUCTION);
		return;
	}

	objPtr = dictPtr->GetElement("Range", OBJ_TYPE_ARRAY);
	if (objPtr)
	{
		CPDF_ArrayPtr array = objPtr->GetArrayPtr();
		mOutputCount = (uint32)array->GetCount() / 2;
		mpRange = GetAllocator()->NewArray<FLOAT>(mOutputCount * 2);
		for (size_t i = 0; i < array->GetCount(); ++i)
		{
			objPtr = array->GetElement(i, OBJ_TYPE_NUMBER);
			if (objPtr)
			{
				mpRange[i] = objPtr->GetNumberPtr()->GetFloat();
			}
			else{
				mpRange[i] = 0.0f;
			}
		}
	}else{
        /*mOutputCount = mInputCount;
        mpRange = GetAllocator()->NewArray<FLOAT>(mInputCount * 2);
        for (size_t i = 0; i < mInputCount * 2; ++i)
        {
            mpRange[i] = mpDomain[i];
        }*/
        //SetError(COMPONENT_ERROR_CONSTRUCTION);
		//return;
	}
}

class CPDF_StreamBitsRead
{
public:
    CPDF_StreamBitsRead();
    CPDF_StreamBitsRead(PBYTE pData, size_t size);
    
    void Attach(PBYTE pData, size_t size);
    void Detach();
    
    void Reset();
    
    uint32 GetBits(uint32 n);
    
    int32 GetByte();
    
private:
    uint32 mAvail;
    uint32 mBits;
    size_t mSize;
    PBYTE mpBytesBegin;
    PBYTE mpBytesEnd;
    PBYTE mpCurByte;
};

CPDF_StreamBitsRead::CPDF_StreamBitsRead()
: mAvail(0), mBits(0), mSize(0), mpBytesBegin(nullptr), mpBytesEnd(nullptr), mpCurByte(nullptr) {}

CPDF_StreamBitsRead::CPDF_StreamBitsRead(PBYTE pData, size_t size)
: mAvail(0), mBits(0), mSize(size), mpBytesBegin(pData), mpBytesEnd(pData+size), mpCurByte(pData) {}

void CPDF_StreamBitsRead::Attach(PBYTE pData, size_t size)
{
    if (pData && size)
    {
        Detach();
        mSize = size;
        mpCurByte = mpBytesBegin = pData;
        mpBytesEnd = pData + size;
    }
}

void CPDF_StreamBitsRead::Detach()
{
    mAvail = mBits = mSize = 0;
    mpCurByte = mpBytesBegin = mpBytesEnd = nullptr;
}

void CPDF_StreamBitsRead::Reset()
{
    mAvail = mBits = 0;
}

uint32 CPDF_StreamBitsRead::GetBits(uint32 n)
{
    uint32 x;
    if (n <= mAvail)
    {
        mAvail -= n;
        x = (mBits >> mAvail) & ((1 << n) - 1);
    }else{
        x = mBits & ((1 << mAvail) - 1);
        n -= mAvail;
        mAvail = 0;
        
        while (n > 8)
        {
            x = (x << 8) | GetByte();
            n -= 8;
        }
        
        if (n > 0)
        {
            mBits = GetByte();
            mAvail = 8 - n;
            x = (x << n) | (mBits >> mAvail);
        }
    }
    return x;
}

int32 CPDF_StreamBitsRead::GetByte()
{
    if (mpCurByte != mpBytesEnd)
    {
        return *mpCurByte++;
    }
    return EOF;
}

bool CPDF_Function_Exponential::Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output)
{
    if ( input.size() == 0 || input.size() < GetInputCount() )
    {
        return false;
    }
    
    FLOAT x = input[0];
    x = fz_clamp( x, GetDomianMin(0), GetDomianMax(0) );
    
    output.clear();
    
    FLOAT tmp1 = powf( x, GetOutputCount() );
    FLOAT tmp2 = 0.0f;
    
    for ( uint32 i = 0; i < GetOutputCount(); ++i )
    {
        tmp2 = mpC0[i] + tmp1 * ( mpC1[i] - mpC0[i] );
        if ( HasRange() )
        {
            tmp2 = fz_clamp( tmp2, GetRangeMin(i), GetRangeMax(i) );
        }
        output.push_back( tmp2 );
    }
    
    return TRUE;
    
    /*for (uint32_t i = 0; i < mInputCount; i++)
        for (uint32_t j = 0; j < m_nOrigOutputs; j++) {
            
           // output[i * m_nOrigOutputs + j] =
            FLOAT v =  m_pBeginValues[j] + (FLOAT)powf(input[i], m_Exponent) * (m_pEndValues[j] - m_pBeginValues[j]);
            output.push_back(v);
        }
    return TRUE;*/
}

CPDF_Function_Sampled::CPDF_Function_Sampled( const CPDF_ObjectPtr & rootObjPtr, CAllocator * pAllocator )
	: CPDF_Function( rootObjPtr, pAllocator ), mBps(1), mOrder(1), mpSize(nullptr), mpEncode(nullptr), mpDecode(nullptr)
{
	if ( mRootObject->GetType() != OBJ_TYPE_REFERENCE )
	{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	CPDF_ObjectPtr objPtr = mRootObject->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
	if ( !objPtr )
	{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	CPDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
	CPDF_DictionaryPtr dict = stmPtr->GetDictPtr();
	
	objPtr = dict->GetElement( "BitsPerSample", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		mBps = objPtr->GetNumberPtr()->GetInteger();
	}

	objPtr = dict->GetElement( "Order", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		mOrder = objPtr->GetNumberPtr()->GetInteger();
	}

	objPtr = dict->GetElement( "Size", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mpSize = GetAllocator()->NewArray<int32>( 2 * GetInputCount() );
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpSize[i] = objPtr->GetNumberPtr()->GetInteger();
			}else{
				mpSize[i] = 0;
			}
		}
	}

	objPtr = dict->GetElement( "Encode", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mpEncode = GetAllocator()->NewArray<FLOAT>( 2 * GetInputCount() );
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpEncode[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpEncode[i] = 0;
			}
		}
	}

	objPtr = dict->GetElement( "Decode", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mpDecode = GetAllocator()->NewArray<FLOAT>( 2 * GetOutputCount() );
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < 2 * GetOutputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpDecode[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpDecode[i] = 0;
			}
		}
	}

	CPDF_StreamAcc stmAcc;
    CPDF_StreamBitsRead stmBits;
	if ( stmAcc.Attach( stmPtr ) )
	{
        stmBits.Attach(stmAcc.GetData(), stmAcc.GetSize());
        size_t count = stmAcc.GetSize() * 8 / mBps;
        mSample.resize( count );
        FLOAT s = 0;
        uint32 x = 0;
        for (size_t i = 0; i < count; ++i)
        {
            switch (mBps)
            {
            case 1: s = stmBits.GetBits(1); break;
            case 2: s = stmBits.GetBits(2) / 3.0f; break;
            case 4: s = stmBits.GetBits(4) / 15.0f; break;
            case 8: s = stmBits.GetByte() / 255.0f; break;
            case 12: s = stmBits.GetBits(12) / 4095.0f; break;
            case 16:
                x = stmBits.GetByte() << 8;
                x |= stmBits.GetByte();
                s = x / 65535.0f;
                break;
            case 24:
                x = stmBits.GetByte() << 16;
                x |= stmBits.GetByte() << 8;
                x |= stmBits.GetByte();
                s = x / 16777215.0f;
                break;
            case 32:
                x = stmBits.GetByte() << 24;
                x |= stmBits.GetByte() << 16;
                x |= stmBits.GetByte() << 8;
                x |= stmBits.GetByte();
                s = x / 4294967295.0f;
                break;
            default:
                //assert();
                break;
            }
            mSample[i] = s;
        }

        stmBits.Detach();
        stmAcc.Detach();
	}
}

CPDF_Function_Sampled::~CPDF_Function_Sampled()
{
	if ( mpSize )
	{
		GetAllocator()->DeleteArray<int32>( mpSize );
		mpSize = nullptr;
	}
	if ( mpEncode )
	{
		GetAllocator()->DeleteArray<FLOAT>( mpEncode );
		mpEncode = nullptr;
	}
	if ( mpDecode )
	{
		GetAllocator()->DeleteArray<FLOAT>( mpDecode );
		mpDecode = nullptr;
	}
}

float CPDF_Function_Sampled::interpolate_sample( int *scale, int *e0, int *e1, float *efrac, int dim, int idx )
{
	float a, b;
	int idx0, idx1;

	idx0 = e0[dim] * scale[dim] + idx;
	idx1 = e1[dim] * scale[dim] + idx;

	if (dim == 0)
	{
		a = mSample[idx0];
		b = mSample[idx1];
	}
	else
	{
		a = interpolate_sample(scale, e0, e1, efrac, dim - 1, idx0);
		b = interpolate_sample(scale, e0, e1, efrac, dim - 1, idx1);
	}

	return a + (b - a) * efrac[dim];
}

int32 CPDF_Function_Sampled::GetSize( uint32 index ) const
{
	if ( index < GetInputCount() && mpSize )
	{
		return mpSize[index];
	}
	return 0;
}

FLOAT CPDF_Function_Sampled::GetEncodeMin( uint32 index ) const
{
	if ( index < GetInputCount() && mpEncode )
	{
		return mpEncode[index * 2];
	}
	return 0;
}

FLOAT CPDF_Function_Sampled::GetEncodeMax( uint32 index ) const
{
	if ( index < GetInputCount() && mpEncode )
	{
		return mpEncode[index * 2 + 1];
	}
	return 0;
}

FLOAT CPDF_Function_Sampled::GetDecodeMin( uint32 index ) const
{
	if ( index < GetOutputCount() && mpDecode )
	{
		return mpDecode[index * 2];
	}
	return 0;
}

FLOAT CPDF_Function_Sampled::GetDecodeMax( uint32 index ) const
{
	if ( index < GetOutputCount() && mpDecode )
	{
		return mpDecode[index * 2 + 1];
	}
	return 0;
}

bool CPDF_Function_Sampled::Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output)
{
	if ( input.size() == 0 || input.size() < GetInputCount() )
	{
		return false;
	}

	int32 e0[64], e1[64], scale[64];
	FLOAT efrac[64];
    FLOAT tmpValue;

	/* encode input coordinates */
	for ( uint32 i = 0; i < GetInputCount(); ++i )
	{
		tmpValue = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
		tmpValue = lerp( tmpValue, GetDomianMin( i ), GetDomianMax( i ), GetEncodeMin( i ), GetEncodeMax( i ) );
		tmpValue = fz_clamp( tmpValue, 0, GetSize( i ) - 1 );
		e0[i] = floorf(tmpValue);
		e1[i] = ceilf(tmpValue);
		efrac[i] = tmpValue - floorf(tmpValue);
	}

	scale[0] = GetOutputCount();
	for ( uint32 i = 1; i < GetInputCount(); ++i )
	{
		scale[i] = scale[i - 1] * GetSize( i );
	}

	for ( uint32 i = 0; i < GetOutputCount(); ++i )
	{
		if ( GetInputCount() == 1 )
		{
			float a = mSample[e0[0] * GetOutputCount() + i ];
			float b = mSample[e1[0] * GetOutputCount() + i ];
			float ab = a + (b - a) * efrac[0];
			tmpValue = lerp(ab, 0, 1, GetDecodeMin(i), GetDecodeMax(i) );
			tmpValue = fz_clamp(tmpValue, GetRangeMin(i), GetRangeMax(i) );
		}
		else if ( GetInputCount() == 2 )
		{
			int s0 = GetOutputCount();
			int s1 = s0 * GetSize( i );
			float a = mSample[e0[0] * s0 + e0[1] * s1 + i];
			float b = mSample[e1[0] * s0 + e0[1] * s1 + i];
			float c = mSample[e0[0] * s0 + e1[1] * s1 + i];
			float d = mSample[e1[0] * s0 + e1[1] * s1 + i];
			float ab = a + (b - a) * efrac[0];
			float cd = c + (d - c) * efrac[0];
			float abcd = ab + (cd - ab) * efrac[1];
			tmpValue = lerp(abcd, 0, 1, GetDecodeMin(i), GetDecodeMax(i) );
			tmpValue = fz_clamp(tmpValue, GetRangeMin(i), GetRangeMax(i) );
		}
		else
		{
			tmpValue = interpolate_sample(scale, e0, e1, efrac, GetInputCount() - 1, i);
			tmpValue = lerp(tmpValue, 0, 1, GetDecodeMin(i), GetDecodeMax(i) );
			tmpValue = fz_clamp(tmpValue, GetRangeMin(i), GetRangeMax(i) );
		}
		output.push_back(tmpValue);
	}
	return TRUE;
}

CPDF_Function_Exponential::CPDF_Function_Exponential( const CPDF_ObjectPtr & rootObjPtr, CAllocator * pAllocator )
	: CPDF_Function( rootObjPtr, pAllocator )
{
	CPDF_ObjectPtr objPtr;
	CPDF_DictionaryPtr dictPtr;

	if ( mRootObject->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = mRootObject->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			dictPtr = objPtr->GetDictPtr();
		}else{
			SetError( COMPONENT_ERROR_CONSTRUCTION );
			return;
		}
	}else if ( mRootObject->GetType() == OBJ_TYPE_DICTIONARY )
	{
		dictPtr = mRootObject->GetDictPtr();
	}else{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	if ( !dictPtr )
	{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}
    
    
    
    /*objPtr = dictPtr->GetElement("C0", OBJ_TYPE_ARRAY);
    if (mOutputCount == 0)
    {
        mOutputCount = 1;
        if (objPtr)
        {
            mOutputCount = objPtr->GetArrayPtr()->GetCount();
        }
    }
    
    CPDF_ArrayPtr arrayPtr0 = objPtr->GetArrayPtr();
    CPDF_ArrayPtr arrayPtr1;
    
    objPtr = dictPtr->GetElement("C1", OBJ_TYPE_ARRAY);
    if ( objPtr )
    {
        arrayPtr1 = objPtr->GetArrayPtr();
    }
    
    m_pBeginValues = GetAllocator()->NewArray<FLOAT>(2);
    m_pEndValues = GetAllocator()->NewArray<FLOAT>(2);
    
    for (uint32_t i = 0; i < mOutputCount; ++i) {
        m_pBeginValues[i] = arrayPtr0 ? arrayPtr0->GetElement(i, OBJ_TYPE_NUMBER)->GetNumberPtr()->GetFloat() : 0.0f;
        m_pEndValues[i] = arrayPtr1 ? arrayPtr1->GetElement(i, OBJ_TYPE_NUMBER)->GetNumberPtr()->GetFloat() : 1.0f;
    }
    
    m_Exponent = dictPtr->GetElement("N", OBJ_TYPE_NUMBER)->GetNumberPtr()->GetFloat();
    
    m_nOrigOutputs = mOutputCount;
    if (mOutputCount && mInputCount > INT_MAX / mOutputCount)
    {
        //error
    }
    mOutputCount *= mInputCount;*/

	objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		mN = objPtr->GetNumberPtr()->GetInteger();
	}

	objPtr = dictPtr->GetElement( "C0", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
        
        mOutputCount = (uint32)arrayPtr->GetCount();
		mpC0 = GetAllocator()->NewArray<FLOAT>( 2 * GetOutputCount() );
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < 2 * GetOutputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpC0[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpC0[i] = 0.0f;
			}
		}
	}

	objPtr = dictPtr->GetElement( "C1", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
        mOutputCount = (uint32)arrayPtr->GetCount();
		mpC1 = GetAllocator()->NewArray<FLOAT>( 2 * GetOutputCount() );
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < 2 * GetOutputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpC1[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpC1[i] = 1.0f;
			}
		}
	}
}

CPDF_Function_Exponential::~CPDF_Function_Exponential()
{
	/*if ( mpC0 )
	{
		GetAllocator()->DeleteArray<FLOAT>( mpC0 );
		mpC0 = nullptr;
	}
	if ( mpC1 )
	{
		GetAllocator()->DeleteArray<FLOAT>( mpC1 );
		mpC1 = nullptr;
	}*/
}

CPDF_Function_Stitching::CPDF_Function_Stitching( const CPDF_ObjectPtr & rootObjPtr, CAllocator * pAllocator )
	: CPDF_Function( rootObjPtr, pAllocator ), mK(0)
{
	CPDF_ObjectPtr objPtr;
	CPDF_DictionaryPtr dictPtr;

	if ( mRootObject->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = mRootObject->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			dictPtr = objPtr->GetDictPtr();
		}else{
			SetError( COMPONENT_ERROR_CONSTRUCTION );
			return;
		}
	}else if ( objPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		dictPtr = objPtr->GetDictPtr();
	}else if ( objPtr->GetType() == OBJ_TYPE_STREAM )
	{
		dictPtr = objPtr->GetStreamPtr()->GetDictPtr();
	}else{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	objPtr = dictPtr->GetElement( "Functions", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mK = (uint32)arrayPtr->GetCount();
		if ( mK > 0 )
		{
			CPDF_FunctionPtr tmpFunctionPtr;
			for ( size_t i = 0; i < mK; ++i )
			{
				objPtr = arrayPtr->GetElement( i, OBJ_TYPE_REFERENCE );
				if ( objPtr )
				{
					tmpFunctionPtr = CPDF_Function::Create( objPtr, GetAllocator() );
                    mFunctions.push_back( tmpFunctionPtr );
				}
			}
		}
	}

	objPtr = dictPtr->GetElement( "Encode", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mEncode.push_back( objPtr->GetNumberPtr()->GetFloat() );
			}else{
				mEncode.push_back( 0 );
			}
		}
	}

	objPtr = dictPtr->GetElement( "Bounds", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		for ( size_t i = 0; i < arrayPtr->GetCount() && i < GetInputCount() - 1; ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mBounds.push_back( objPtr->GetNumberPtr()->GetFloat() );
			}else{
				mBounds.push_back( 0 );
			}
		}
	}
}

CPDF_Function_Stitching::~CPDF_Function_Stitching()
{
}

FLOAT CPDF_Function_Stitching::GetEncodeMin( uint32 index ) const
{
	if ( index < GetInputCount() )
	{
		return mEncode[index * 2];
	}
	return 0;
}

FLOAT CPDF_Function_Stitching::GetEncodeMax( uint32 index ) const
{
	if ( index < GetInputCount() )
	{
		return mEncode[index * 2 + 1];
	}
	return 0;
}

bool CPDF_Function_Stitching::Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output)
{
	if ( input.size() == 0 )
	{
		return false;
	}

	FLOAT low, high;
	FLOAT inputValue = input[0];

	inputValue = fz_clamp(inputValue, GetDomianMin(0), GetDomianMax(0) );
    
    if (mBounds.size() == 0)
    {
        CPDF_FunctionPtr functionPtr = mFunctions[0];
        if ( functionPtr )
        {
            return functionPtr->Calculate( input, output );
        }
    }

	uint32 i = 0;
	for ( ; i < mK; ++i )
	{
		if ( inputValue < mBounds[i] )
		{
			break;
		}
	}

	if ( i == 0 && mK == 1 )
	{
		low = GetDomianMin( 0 );
		high = GetDomianMax( 0 );
	}else if ( i == 0 )
	{
		low = GetDomianMin( 0 );
		high = mBounds[0];
	}else if ( i == mK - 1 )
	{
		low = mBounds[mK - 2];
		high = mBounds[0];
	}else{
		low = mBounds[i - 1];
		high = mBounds[i];
	}

	inputValue = lerp(inputValue, low, high, GetEncodeMin(i), GetEncodeMax(i) );

	std::vector<FLOAT> newInput;
	newInput.push_back( inputValue );

	CPDF_FunctionPtr functionPtr = mFunctions[i];
	if ( functionPtr )
	{
		return functionPtr->Calculate( newInput, output );
	}

	return false;
}

CPDF_Function_PostScript::CPDF_Function_PostScript(const CPDF_StreamPtr & stm, CAllocator * pAllocator)
 : CPDF_Function(stm, pAllocator), mbParsed(false), mStmPtr(stm) {}

CPDF_Function_PostScript::~CPDF_Function_PostScript()
{
}

bool CPDF_Function_PostScript::Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output )
{
	if ( mbParsed == false )
	{
		Parse();
	}

	return RunCode( input, output );
}

void CPDF_Function_PostScript::Parse()
{
	if ( mbParsed != false && !mStmPtr )
	{
		return;
	}

	CPDF_StreamAcc stmAcc;
	if ( stmAcc.Attach( mStmPtr ) )
	{
        mbParsed = mPSEngine.Parse(stmAcc.GetData(), stmAcc.GetSize());
	}
}


bool CPDF_Function_PostScript::RunCode(const std::vector<FLOAT> & input, std::vector<FLOAT> & output )
{
    CPDF_PSEngine & PS = const_cast<CPDF_PSEngine&>(mPSEngine);
    FLOAT tmpVal = 0.0f;
    PS.Reset();
    for (uint32 i = 0; i < input.size(); i++)
    {
        tmpVal = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
        PS.Push(tmpVal);
    }
    PS.Execute();
    if (PS.GetStackSize() < GetOutputCount())
        return false;
    
    uint32 c = GetOutputCount();
    output.resize(c);
    for (uint32 i = 0; i < c; i++)
    {
        output[c - i - 1] = PS.Pop();
        output[c - i - 1] = fz_clamp(output[c - i - 1], GetRangeMin(c - i - 1), GetRangeMax(c - i - 1));
    }

    return TRUE;
}





bool CPDF_PSProc::Execute(CPDF_PSEngine* pEngine)
{
    for (size_t i = 0; i < m_Operators.size(); ++i)
    {
        const PDF_PSOP op = m_Operators[i]->GetOp();
        if (op == PSOP_PROC)
            continue;

        if (op == PSOP_CONST)
        {
            pEngine->Push(m_Operators[i]->GetFloatValue());
            continue;
        }

        if (op == PSOP_IF)
        {
            if (i == 0 || m_Operators[i - 1]->GetOp() != PSOP_PROC)
                return false;

            if (static_cast<int>(pEngine->Pop()))
                m_Operators[i - 1]->GetProc()->Execute(pEngine);
        } else if (op == PSOP_IFELSE) {
            if (i < 2 || m_Operators[i - 1]->GetOp() != PSOP_PROC ||
                m_Operators[i - 2]->GetOp() != PSOP_PROC) {
                    return false;
            }
            size_t offset = static_cast<int>(pEngine->Pop()) ? 2 : 1;
            m_Operators[i - offset]->GetProc()->Execute(pEngine);
        } else {
            pEngine->DoOperator(op);
        }
    }
    return TRUE;
}

CPDF_PSEngine::CPDF_PSEngine() { m_StackCount = 0;}

CPDF_PSEngine::~CPDF_PSEngine() {}

void CPDF_PSEngine::Push(FLOAT v)
{
    if (m_StackCount == PSENGINE_STACKSIZE)
    {
        return;
    }
    m_Stack[m_StackCount++] = v;
}

FLOAT CPDF_PSEngine::Pop()
{
    if (m_StackCount == 0)
    {
        return 0;
    }
    return m_Stack[--m_StackCount];
}

bool CPDF_PSEngine::Parse(const BYTE * str, size_t size) {

    bool bRet = false;
    IRead *  pRead = CreateMemBufIRead( str, size, GetDefaultAllocator() );
    CPDF_SyntaxParser syntaxParser(nullptr, GetDefaultAllocator());
    syntaxParser.InitParser(pRead);

    CPDF_ParseWordDes wordDes;
    if ( !syntaxParser.GetWord(wordDes) || wordDes.type != PARSE_WORD_BLOCK_B )
    {
        return false;
    }
    bRet = m_MainProc.Parse(&syntaxParser);
    DestoryIRead( pRead );
    return bRet;
}

bool CPDF_PSProc::Parse(CPDF_SyntaxParser* parser)
{
    CByteString word;
    CPDF_ParseWordDes wordDes;
    while ( parser->GetWord(wordDes) )
    {
        switch (wordDes.type)
        {
        case PARSE_WORD_INTEGER:
            {
                std::unique_ptr<CPDF_PSOP> op(new CPDF_PSOP(parser->GetInteger()));
                m_Operators.push_back(std::move(op));
                break;
            }
        case PARSE_WORD_FLOAT:
            {
                std::unique_ptr<CPDF_PSOP> op(new CPDF_PSOP(parser->GetFloat()));
                m_Operators.push_back(std::move(op));
                break;
            }
        case PARSE_WORD_BLOCK_B:
            {
                std::unique_ptr<CPDF_PSProc> proc(new CPDF_PSProc);
                std::unique_ptr<CPDF_PSOP> op(new CPDF_PSOP(std::move(proc)));
                m_Operators.push_back(std::move(op));
                if (!m_Operators.back()->GetProc()->Parse(parser))
                {
                    return false;
                }
                break;
            }
        case PARSE_WORD_BLOCK_E:
            {
                return TRUE;
            }
        case PARSE_WORD_UNKNOWN:
            {
                PDF_PSOP op;
                parser->GetString(word);
                if (word.GetLength())
                {
                    switch (StringToUINT32(word))
                    {
                    case C('a', 'b', 's'): op = PSOP_ABS; break;
                    case C('a', 'd', 'd'): op = PSOP_ADD; break;
                    case D('a', 't', 'a', 'n'): op = PSOP_ATAN; break;
                    case D('c', 'e', 'i', 'l'): op = PSOP_CEILING; break;
                    case C('c', 'o', 's'): op = PSOP_COS; break;
                    case C('c', 'v', 'r'): op = PSOP_CVR; break;
                    case C('c', 'v', 'i'): op = PSOP_CVI; break;
                    case C('d', 'i', 'v'): op = PSOP_DIV; break;
                    case C('e', 'x', 'p'): op = PSOP_EXP; break;
                    case D('f', 'l', 'o', 'o'): op = PSOP_FLOOR; break;
                    case D('i', 'd', 'i', 'v'): op = PSOP_IDIV; break;
                    case B('l', 'n'): op = PSOP_LN; break;
                    case C('l', 'o', 'g'): op = PSOP_LOG; break;
                    case C('m', 'o', 'd'): op = PSOP_MOD; break;
                    case C('m', 'u', 'l'): op = PSOP_MUL; break;
                    case C('n', 'e', 'g'): op = PSOP_NEG; break;
                    case D('r', 'o', 'u', 'n'): op = PSOP_ROUND; break;
                    case C('s', 'i', 'n'): op = PSOP_SIN; break;
                    case D('s', 'q', 'r', 't'): op = PSOP_SQRT; break;
                    case C('s', 'u', 'b'): op = PSOP_SUB; break;
                    case D('t', 'r', 'u', 'n'): op = PSOP_TRUNCATE; break;
                    case C('a', 'n', 'd'): op = PSOP_AND; break;
                    case D('b', 'i', 't', 's'): op = PSOP_BITSHIFT; break;
                    case B('e', 'q'): op = PSOP_EQ; break;
                    case D('f', 'a', 'l', 's'): op = PSOP_FALSE; break;
                    case D('t', 'r', 'u', 'e'): op = PSOP_TRUE; break;
                    case B('g', 'e'): op = PSOP_GE; break;
                    case B('g', 't'): op = PSOP_GT; break;
                    case B('l', 'e'): op = PSOP_LE; break;
                    case B('l', 't'): op = PSOP_LT; break;
                    case B('n', 'e'): op = PSOP_NE; break;
                    case C('n', 'o', 't'): op = PSOP_NOT; break;
                    case B('o', 'r'): op = PSOP_OR; break;
                    case C('x', 'o', 'r'): op = PSOP_XOR; break;
                    case B('i', 'f'): op = PSOP_IF; break;
                    case D('i', 'f', 'e', 'l'): op = PSOP_IFELSE; break;
                    case D('c', 'o', 'p', 'y'): op = PSOP_COPY; break;
                    case C('d', 'u', 'p'): op = PSOP_DUP; break;
                    case D('e', 'x', 'c', 'h'): op = PSOP_EXCH; break;
                    case D('i', 'n', 'd', 'e'): op = PSOP_INDEX; break;
                    case C('p', 'o', 'p'): op = PSOP_POP; break;
                    case D('r', 'o', 'l', 'l'): op = PSOP_ROLL; break;
                    default: return false;
                    }
                    std::unique_ptr<CPDF_PSOP> psop(new CPDF_PSOP(op));
                    m_Operators.push_back(std::move(psop));
                }else{
                    return false;
                }
                break;
            }
        default:
            break;
        }
    }
    return TRUE;
}

bool CPDF_PSEngine::DoOperator(PDF_PSOP op)
{
    int i1, i2;
    FLOAT d1, d2;
    switch (op) {
    case PSOP_ADD:
        d1 = Pop();
        d2 = Pop();
        Push(d1 + d2);
        break;
    case PSOP_SUB:
        d2 = Pop();
        d1 = Pop();
        Push(d1 - d2);
        break;
    case PSOP_MUL:
        d1 = Pop();
        d2 = Pop();
        Push(d1 * d2);
        break;
    case PSOP_DIV:
        d2 = Pop();
        d1 = Pop();
        Push(d1 / d2);
        break;
    case PSOP_IDIV:
        i2 = (int)Pop();
        i1 = (int)Pop();
        Push(i1 / i2);
        break;
    case PSOP_MOD:
        i2 = (int)Pop();
        i1 = (int)Pop();
        Push(i1 % i2);
        break;
    case PSOP_NEG:
        d1 = Pop();
        Push(-d1);
        break;
    case PSOP_ABS:
        d1 = Pop();
        Push((FLOAT)fabsf(d1));
        break;
    case PSOP_CEILING:
        d1 = Pop();
        Push((FLOAT)ceilf(d1));
        break;
    case PSOP_FLOOR:
        d1 = Pop();
        Push((FLOAT)floor(d1));
        break;
    case PSOP_ROUND:
        d1 = Pop();
        Push((d1 >= 0) ? floorf(d1 + 0.5f) : ceilf(d1 - 0.5f));
        break;
    case PSOP_TRUNCATE:
        i1 = (int)Pop();
        Push(i1);
        break;
    case PSOP_SQRT:
        d1 = Pop();
        Push((FLOAT)sqrt(d1));
        break;
    case PSOP_SIN:
        d1 = Pop();
        Push((FLOAT)sin(d1 / RADIAN));
        break;
    case PSOP_COS:
        d1 = Pop();
        Push((FLOAT)cos(d1 / RADIAN));
        break;
    case PSOP_ATAN:
        d2 = Pop();
        d1 = Pop();
        d1 = (FLOAT)(atan2f(d1, d2) * RADIAN);
        if (d1 < 0) {
            d1 += 360;
        }
        Push(d1);
        break;
    case PSOP_EXP:
        d2 = Pop();
        d1 = Pop();
        Push((FLOAT)powf(d1, d2));
        break;
    case PSOP_LN:
        d1 = Pop();
        Push((FLOAT)logf(d1));
        break;
    case PSOP_LOG:
        d1 = Pop();
        Push((FLOAT)log10f(d1));
        break;
    case PSOP_CVI:
        i1 = (int)Pop();
        Push(i1);
        break;
    case PSOP_CVR:
        break;
    case PSOP_EQ:
        d2 = Pop();
        d1 = Pop();
        Push((int)(d1 == d2));
        break;
    case PSOP_NE:
        d2 = Pop();
        d1 = Pop();
        Push((int)(d1 != d2));
        break;
    case PSOP_GT:
        d2 = Pop();
        d1 = Pop();
        Push((int)(d1 > d2));
        break;
    case PSOP_GE:
        d2 = Pop();
        d1 = Pop();
        Push((int)(d1 >= d2));
        break;
    case PSOP_LT:
        d2 = Pop();
        d1 = Pop();
        Push((int)(d1 < d2));
        break;
    case PSOP_LE:
        d2 = Pop();
        d1 = Pop();
        Push((int)(d1 <= d2));
        break;
    case PSOP_AND:
        i1 = (int)Pop();
        i2 = (int)Pop();
        Push(i1 & i2);
        break;
    case PSOP_OR:
        i1 = (int)Pop();
        i2 = (int)Pop();
        Push(i1 | i2);
        break;
    case PSOP_XOR:
        i1 = (int)Pop();
        i2 = (int)Pop();
        Push(i1 ^ i2);
        break;
    case PSOP_NOT:
        i1 = (int)Pop();
        Push((int)!i1);
        break;
    case PSOP_BITSHIFT:
        {
            int shift = (int)Pop();
            int i = (int)Pop();
            if (shift > 0) {
                Push(i << shift);
            } else {
                Push(i >> -shift);
            }
            break;
        }
    case PSOP_TRUE:
        Push(1);
        break;
    case PSOP_FALSE:
        Push(0);
        break;
    case PSOP_POP:
        Pop();
        break;
    case PSOP_EXCH:
        d2 = Pop();
        d1 = Pop();
        Push(d2);
        Push(d1);
        break;
    case PSOP_DUP:
        d1 = Pop();
        Push(d1);
        Push(d1);
        break;
    case PSOP_COPY:
        {
            int n = static_cast<int>(Pop());
            if (n < 0 || m_StackCount + n > PSENGINE_STACKSIZE ||
                n > static_cast<int>(m_StackCount))
                break;
            for (int i = 0; i < n; i++)
                m_Stack[m_StackCount + i] = m_Stack[m_StackCount + i - n];
            m_StackCount += n;
            break;
        }
    case PSOP_INDEX:
        {
            int n = static_cast<int>(Pop());
            if (n < 0 || n >= static_cast<int>(m_StackCount))
                break;
            Push(m_Stack[m_StackCount - n - 1]);
            break;
        }
    case PSOP_ROLL: 
        {
            int j = static_cast<int>(Pop());
            int n = static_cast<int>(Pop());
            if (m_StackCount == 0)
                break;
            if (n < 0 || n > static_cast<int>(m_StackCount))
                break;
            if (j < 0) {
                for (int i = 0; i < -j; i++) {
                    FLOAT first = m_Stack[m_StackCount - n];
                    for (int ii = 0; ii < n - 1; ii++)
                        m_Stack[m_StackCount - n + ii] = m_Stack[m_StackCount - n + ii + 1];
                    m_Stack[m_StackCount - 1] = first;
                }
            } else {
                for (int i = 0; i < j; i++) {
                    FLOAT last = m_Stack[m_StackCount - 1];
                    int ii;
                    for (ii = 0; ii < n - 1; ii++)
                        m_Stack[m_StackCount - ii - 1] = m_Stack[m_StackCount - ii - 2];
                    m_Stack[m_StackCount - ii - 1] = last;
                }
            }
            break;
        }
    default:
        break;
    }
    return true;
}

}//namespace