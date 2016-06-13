#include "../../include/pdf/che_pdf_function.h"
#include "../../include/pdf/che_pdf_parser.h"

#include <math.h>
#include <float.h>

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

CHE_PDF_Function * CHE_PDF_FunctionPtr::operator->() const
{
	return (CHE_PDF_Function*)mpCom;
}

CHE_PDF_FunctionPtr CHE_PDF_Function::Create( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_FunctionPtr ptr;

	if ( !rootObjPtr )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr;
    CHE_PDF_StreamPtr streamPtr;
	CHE_PDF_Function * pTmp = NULL;

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
			pTmp = pAllocator->New<CHE_PDF_Function_Sampled>( rootObjPtr, pAllocator );
			break;
		case 2:
			pTmp = pAllocator->New<CHE_PDF_Function_Exponential>( rootObjPtr, pAllocator );
			break;
		case 3:
			pTmp = pAllocator->New<CHE_PDF_Function_Stitching>( rootObjPtr, pAllocator );
			break;
		case 4:
			pTmp = pAllocator->New<CHE_PDF_Function_PostScript>( streamPtr, pAllocator );
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

HE_FLOAT CHE_PDF_Function::GetDomianMin( HE_UINT32 index ) const
{
	if ( index < GetInputCount() && mpDomain )
	{
		return mpDomain[index * 2];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function::GetDomianMax( HE_UINT32 index ) const
{
	if ( index < GetInputCount() && mpDomain )
	{
		return mpDomain[index * 2 + 1];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function::GetRangeMin( HE_UINT32 index ) const
{
	if ( index < GetOutputCount() && mpRange )
	{
		return mpRange[index * 2];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function::GetRangeMax( HE_UINT32 index ) const
{
	if ( index < GetOutputCount() && mpRange )
	{
		return mpRange[index * 2 + 1];
	}
	return 0;
}

CHE_PDF_Function::CHE_PDF_Function(const CHE_PDF_ObjectPtr & root, CHE_Allocator * pAllocator/*= NULL*/)
	: CHE_PDF_Component(COMPONENT_TYPE_Function, root, pAllocator), mFunctionType(FUNCTION_TYPE_SAMPLE),
	mInputCount(0), mOutputCount(0), mpDomain(NULL), mpRange(NULL)
{
	CHE_PDF_ObjectPtr		objPtr;
	CHE_PDF_DictionaryPtr	dictPtr;
	CHE_PDF_StreamPtr		streamPtr;
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
		CHE_PDF_ArrayPtr array = objPtr->GetArrayPtr();
		mInputCount = (HE_UINT32)array->GetCount() / 2;
		mpDomain = GetAllocator()->NewArray<HE_FLOAT>(mInputCount * 2);
		for (HE_ULONG i = 0; i < array->GetCount(); ++i)
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
		CHE_PDF_ArrayPtr array = objPtr->GetArrayPtr();
		mOutputCount = (HE_UINT32)array->GetCount() / 2;
		mpRange = GetAllocator()->NewArray<HE_FLOAT>(mOutputCount * 2);
		for (HE_ULONG i = 0; i < array->GetCount(); ++i)
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
        mpRange = GetAllocator()->NewArray<HE_FLOAT>(mInputCount * 2);
        for (HE_ULONG i = 0; i < mInputCount * 2; ++i)
        {
            mpRange[i] = mpDomain[i];
        }*/
        //SetError(COMPONENT_ERROR_CONSTRUCTION);
		//return;
	}
}

class CHE_PDF_StreamBitsRead
{
public:
    CHE_PDF_StreamBitsRead();
    CHE_PDF_StreamBitsRead(HE_LPBYTE pData, HE_ULONG size);
    
    HE_VOID Attach(HE_LPBYTE pData, HE_ULONG size);
    HE_VOID Detach();
    
    HE_VOID Reset();
    
    HE_UINT32 GetBits(HE_UINT32 n);
    
    HE_INT32 GetByte();
    
private:
    HE_UINT32 mAvail;
    HE_UINT32 mBits;
    HE_ULONG mSize;
    HE_LPBYTE mpBytesBegin;
    HE_LPBYTE mpBytesEnd;
    HE_LPBYTE mpCurByte;
};

CHE_PDF_StreamBitsRead::CHE_PDF_StreamBitsRead()
: mAvail(0), mBits(0), mSize(0), mpBytesBegin(NULL), mpBytesEnd(NULL), mpCurByte(NULL) {}

CHE_PDF_StreamBitsRead::CHE_PDF_StreamBitsRead(HE_LPBYTE pData, HE_ULONG size)
: mAvail(0), mBits(0), mSize(size), mpBytesBegin(pData), mpBytesEnd(pData+size), mpCurByte(pData) {}

HE_VOID CHE_PDF_StreamBitsRead::Attach(HE_LPBYTE pData, HE_ULONG size)
{
    if (pData && size)
    {
        Detach();
        mSize = size;
        mpCurByte = mpBytesBegin = pData;
        mpBytesEnd = pData + size;
    }
}

HE_VOID CHE_PDF_StreamBitsRead::Detach()
{
    mAvail = mBits = mSize = 0;
    mpCurByte = mpBytesBegin = mpBytesEnd = NULL;
}

HE_VOID CHE_PDF_StreamBitsRead::Reset()
{
    mAvail = mBits = 0;
}

HE_UINT32 CHE_PDF_StreamBitsRead::GetBits(HE_UINT32 n)
{
    HE_UINT32 x;
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

HE_INT32 CHE_PDF_StreamBitsRead::GetByte()
{
    if (mpCurByte != mpBytesEnd)
    {
        return *mpCurByte++;
    }
    return EOF;
}

HE_BOOL CHE_PDF_Function_Exponential::Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output)
{
    if ( input.size() == 0 || input.size() < GetInputCount() )
    {
        return FALSE;
    }
    
    HE_FLOAT x = input[0];
    x = fz_clamp( x, GetDomianMin(0), GetDomianMax(0) );
    
    output.clear();
    
    HE_FLOAT tmp1 = powf( x, GetOutputCount() );
    HE_FLOAT tmp2 = 0.0f;
    
    for ( HE_UINT32 i = 0; i < GetOutputCount(); ++i )
    {
        tmp2 = mpC0[i] + tmp1 * ( mpC1[i] - mpC0[i] );
        if ( HasRange() )
        {
            tmp2 = fz_clamp( tmp2, GetRangeMin(i), GetRangeMax(i) );
        }
        output.push_back( tmp2 );
    }
    
    return TRUE;
}

CHE_PDF_Function_Sampled::CHE_PDF_Function_Sampled( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( rootObjPtr, pAllocator ), mBps(1), mOrder(1), mpSize(NULL), mpEncode(NULL), mpDecode(NULL)
{
	if ( mRootObject->GetType() != OBJ_TYPE_REFERENCE )
	{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	CHE_PDF_ObjectPtr objPtr = mRootObject->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
	if ( !objPtr )
	{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
	CHE_PDF_DictionaryPtr dict = stmPtr->GetDictPtr();
	
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
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mpSize = GetAllocator()->NewArray<HE_INT32>( 2 * GetInputCount() );
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
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
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mpEncode = GetAllocator()->NewArray<HE_FLOAT>( 2 * GetInputCount() );
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
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
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mpDecode = GetAllocator()->NewArray<HE_FLOAT>( 2 * GetOutputCount() );
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetOutputCount(); ++i )
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

	CHE_PDF_StreamAcc stmAcc;
    CHE_PDF_StreamBitsRead stmBits;
	if ( stmAcc.Attach( stmPtr ) )
	{
        stmBits.Attach(stmAcc.GetData(), stmAcc.GetSize());
        HE_ULONG count = stmAcc.GetSize() * 8 / mBps;
        mSample.resize( count );
        HE_FLOAT s = 0;
        HE_UINT32 x = 0;
        for (HE_ULONG i = 0; i < count; ++i)
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

CHE_PDF_Function_Sampled::~CHE_PDF_Function_Sampled()
{
	if ( mpSize )
	{
		GetAllocator()->DeleteArray<HE_INT32>( mpSize );
		mpSize = NULL;
	}
	if ( mpEncode )
	{
		GetAllocator()->DeleteArray<HE_FLOAT>( mpEncode );
		mpEncode = NULL;
	}
	if ( mpDecode )
	{
		GetAllocator()->DeleteArray<HE_FLOAT>( mpDecode );
		mpDecode = NULL;
	}
}

float CHE_PDF_Function_Sampled::interpolate_sample( int *scale, int *e0, int *e1, float *efrac, int dim, int idx )
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

HE_INT32 CHE_PDF_Function_Sampled::GetSize( HE_UINT32 index ) const
{
	if ( index < GetInputCount() && mpSize )
	{
		return mpSize[index];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function_Sampled::GetEncodeMin( HE_UINT32 index ) const
{
	if ( index < GetInputCount() && mpEncode )
	{
		return mpEncode[index * 2];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function_Sampled::GetEncodeMax( HE_UINT32 index ) const
{
	if ( index < GetInputCount() && mpEncode )
	{
		return mpEncode[index * 2 + 1];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function_Sampled::GetDecodeMin( HE_UINT32 index ) const
{
	if ( index < GetOutputCount() && mpDecode )
	{
		return mpDecode[index * 2];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function_Sampled::GetDecodeMax( HE_UINT32 index ) const
{
	if ( index < GetOutputCount() && mpDecode )
	{
		return mpDecode[index * 2 + 1];
	}
	return 0;
}

HE_BOOL CHE_PDF_Function_Sampled::Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output)
{
	if ( input.size() == 0 || input.size() < GetInputCount() )
	{
		return FALSE;
	}

	HE_INT32 e0[64], e1[64], scale[64];
	HE_FLOAT efrac[64];
    HE_FLOAT tmpValue;

	/* encode input coordinates */
	for ( HE_UINT32 i = 0; i < GetInputCount(); ++i )
	{
		tmpValue = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
		tmpValue = lerp( tmpValue, GetDomianMin( i ), GetDomianMax( i ), GetEncodeMin( i ), GetEncodeMax( i ) );
		tmpValue = fz_clamp( tmpValue, 0, GetSize( i ) - 1 );
		e0[i] = floorf(tmpValue);
		e1[i] = ceilf(tmpValue);
		efrac[i] = tmpValue - floorf(tmpValue);
	}

	scale[0] = GetOutputCount();
	for ( HE_UINT32 i = 1; i < GetInputCount(); ++i )
	{
		scale[i] = scale[i - 1] * GetSize( i );
	}

	for ( HE_UINT32 i = 0; i < GetOutputCount(); ++i )
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

CHE_PDF_Function_Exponential::CHE_PDF_Function_Exponential( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( rootObjPtr, pAllocator )
{
	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr;

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

	objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		mN = objPtr->GetNumberPtr()->GetInteger();
	}

	objPtr = dictPtr->GetElement( "C0", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
        
        mOutputCount = (HE_UINT32)arrayPtr->GetCount();
		mpC0 = GetAllocator()->NewArray<HE_FLOAT>( 2 * GetOutputCount() );
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetOutputCount(); ++i )
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
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
        mOutputCount = (HE_UINT32)arrayPtr->GetCount();
		mpC1 = GetAllocator()->NewArray<HE_FLOAT>( 2 * GetOutputCount() );
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetOutputCount(); ++i )
		{
			objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpC1[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpC1[i] = 0.0f;
			}
		}
	}
}

CHE_PDF_Function_Exponential::~CHE_PDF_Function_Exponential()
{
	if ( mpC0 )
	{
		GetAllocator()->DeleteArray<HE_FLOAT>( mpC0 );
		mpC0 = NULL;
	}
	if ( mpC1 )
	{
		GetAllocator()->DeleteArray<HE_FLOAT>( mpC1 );
		mpC1 = NULL;
	}
}

CHE_PDF_Function_Stitching::CHE_PDF_Function_Stitching( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( rootObjPtr, pAllocator ), mK(0)
{
	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr;

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
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		mK = (HE_UINT32)arrayPtr->GetCount();
		if ( mK > 0 )
		{
			CHE_PDF_FunctionPtr tmpFunctionPtr;
			for ( HE_ULONG i = 0; i < mK; ++i )
			{
				objPtr = arrayPtr->GetElement( i, OBJ_TYPE_REFERENCE );
				if ( objPtr )
				{
					tmpFunctionPtr = CHE_PDF_Function::Create( objPtr, GetAllocator() );
                    mFunctions.push_back( tmpFunctionPtr );
				}
			}
		}
	}

	objPtr = dictPtr->GetElement( "Encode", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
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
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < GetInputCount() - 1; ++i )
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

CHE_PDF_Function_Stitching::~CHE_PDF_Function_Stitching()
{
}

HE_FLOAT CHE_PDF_Function_Stitching::GetEncodeMin( HE_UINT32 index ) const
{
	if ( index < GetInputCount() )
	{
		return mEncode[index * 2];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function_Stitching::GetEncodeMax( HE_UINT32 index ) const
{
	if ( index < GetInputCount() )
	{
		return mEncode[index * 2 + 1];
	}
	return 0;
}

HE_BOOL CHE_PDF_Function_Stitching::Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output)
{
	if ( input.size() == 0 )
	{
		return FALSE;
	}

	HE_FLOAT low, high;
	HE_FLOAT inputValue = input[0];

	inputValue = fz_clamp(inputValue, GetDomianMin(0), GetDomianMax(0) );

	HE_UINT32 i = 0;
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

	std::vector<HE_FLOAT> newInput;
	newInput.push_back( inputValue );

	CHE_PDF_FunctionPtr functionPtr = mFunctions[i];
	if ( functionPtr )
	{
		return functionPtr->Calculate( newInput, output );
	}

	return FALSE;
}

CHE_PDF_Function_PostScript::CHE_PDF_Function_PostScript(const CHE_PDF_StreamPtr & stm, CHE_Allocator * pAllocator)
 : CHE_PDF_Function(stm, pAllocator), mbParsed(FALSE), mStmPtr(stm) {}

CHE_PDF_Function_PostScript::~CHE_PDF_Function_PostScript()
{
}

HE_BOOL CHE_PDF_Function_PostScript::Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
{
	if ( mbParsed == FALSE )
	{
		Parse();
	}

	return RunCode( input, output );
}

HE_VOID CHE_PDF_Function_PostScript::Parse()
{
	if ( mbParsed != FALSE && !mStmPtr )
	{
		return;
	}

	CHE_PDF_StreamAcc stmAcc;
	if ( stmAcc.Attach( mStmPtr ) )
	{
        mbParsed = mPSEngine.Parse(stmAcc.GetData(), stmAcc.GetSize());
	}
}


HE_BOOL CHE_PDF_Function_PostScript::RunCode(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
{
    CHE_PDF_PSEngine & PS = const_cast<CHE_PDF_PSEngine&>(mPSEngine);
    HE_FLOAT tmpVal = 0.0f;
    PS.Reset();
    for (HE_UINT32 i = 0; i < input.size(); i++)
    {
        tmpVal = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
        PS.Push(tmpVal);
    }
    PS.Execute();
    if (PS.GetStackSize() < GetOutputCount())
        return FALSE;
    
    HE_UINT32 c = GetOutputCount();
    output.resize(c);
    for (HE_UINT32 i = 0; i < c; i++)
    {
        output[c - i - 1] = PS.Pop();
        output[c - i - 1] = fz_clamp(output[c - i - 1], GetRangeMin(c - i - 1), GetRangeMax(c - i - 1));
    }

    return TRUE;
}





HE_BOOL CHE_PDF_PSProc::Execute(CHE_PDF_PSEngine* pEngine)
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
                return FALSE;

            if (static_cast<int>(pEngine->Pop()))
                m_Operators[i - 1]->GetProc()->Execute(pEngine);
        } else if (op == PSOP_IFELSE) {
            if (i < 2 || m_Operators[i - 1]->GetOp() != PSOP_PROC ||
                m_Operators[i - 2]->GetOp() != PSOP_PROC) {
                    return FALSE;
            }
            size_t offset = static_cast<int>(pEngine->Pop()) ? 2 : 1;
            m_Operators[i - offset]->GetProc()->Execute(pEngine);
        } else {
            pEngine->DoOperator(op);
        }
    }
    return TRUE;
}

CHE_PDF_PSEngine::CHE_PDF_PSEngine() { m_StackCount = 0;}

CHE_PDF_PSEngine::~CHE_PDF_PSEngine() {}

void CHE_PDF_PSEngine::Push(HE_FLOAT v)
{
    if (m_StackCount == PSENGINE_STACKSIZE)
    {
        return;
    }
    m_Stack[m_StackCount++] = v;
}

HE_FLOAT CHE_PDF_PSEngine::Pop()
{
    if (m_StackCount == 0)
    {
        return 0;
    }
    return m_Stack[--m_StackCount];
}

HE_BOOL CHE_PDF_PSEngine::Parse(const HE_BYTE * str, HE_ULONG size) {

    HE_BOOL bRet = FALSE;
    IHE_Read *  pRead = HE_CreateMemBufRead( str, size, GetDefaultAllocator() );
    CHE_PDF_SyntaxParser syntaxParser(NULL, GetDefaultAllocator());
    syntaxParser.InitParser(pRead);

    CHE_PDF_ParseWordDes wordDes;
    if ( !syntaxParser.GetWord(wordDes) || wordDes.type != PARSE_WORD_BLOCK_B )
    {
        return FALSE;
    }
    bRet = m_MainProc.Parse(&syntaxParser);
    HE_DestoryIHERead( pRead );
    return bRet;
}

HE_BOOL CHE_PDF_PSProc::Parse(CHE_PDF_SyntaxParser* parser)
{
    CHE_ByteString word;
    CHE_PDF_ParseWordDes wordDes;
    while ( parser->GetWord(wordDes) )
    {
        switch (wordDes.type)
        {
        case PARSE_WORD_INTEGER:
            {
                std::unique_ptr<CHE_PDF_PSOP> op(new CHE_PDF_PSOP(parser->GetInteger()));
                m_Operators.push_back(std::move(op));
                break;
            }
        case PARSE_WORD_FLOAT:
            {
                std::unique_ptr<CHE_PDF_PSOP> op(new CHE_PDF_PSOP(parser->GetFloat()));
                m_Operators.push_back(std::move(op));
                break;
            }
        case PARSE_WORD_BLOCK_B:
            {
                std::unique_ptr<CHE_PDF_PSProc> proc(new CHE_PDF_PSProc);
                std::unique_ptr<CHE_PDF_PSOP> op(new CHE_PDF_PSOP(std::move(proc)));
                m_Operators.push_back(std::move(op));
                if (!m_Operators.back()->GetProc()->Parse(parser))
                {
                    return FALSE;
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
                    switch (StringToDWORD(word))
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
                    default: return FALSE;
                    }
                    std::unique_ptr<CHE_PDF_PSOP> psop(new CHE_PDF_PSOP(op));
                    m_Operators.push_back(std::move(psop));
                }else{
                    return FALSE;
                }
                break;
            }
        default:
            break;
        }
    }
    return TRUE;
}

HE_BOOL CHE_PDF_PSEngine::DoOperator(PDF_PSOP op)
{
    int i1, i2;
    HE_FLOAT d1, d2;
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
        Push((HE_FLOAT)fabsf(d1));
        break;
    case PSOP_CEILING:
        d1 = Pop();
        Push((HE_FLOAT)ceilf(d1));
        break;
    case PSOP_FLOOR:
        d1 = Pop();
        Push((HE_FLOAT)floor(d1));
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
        Push((HE_FLOAT)sqrt(d1));
        break;
    case PSOP_SIN:
        d1 = Pop();
        Push((HE_FLOAT)sin(d1 / RADIAN));
        break;
    case PSOP_COS:
        d1 = Pop();
        Push((HE_FLOAT)cos(d1 / RADIAN));
        break;
    case PSOP_ATAN:
        d2 = Pop();
        d1 = Pop();
        d1 = (HE_FLOAT)(atan2f(d1, d2) * RADIAN);
        if (d1 < 0) {
            d1 += 360;
        }
        Push(d1);
        break;
    case PSOP_EXP:
        d2 = Pop();
        d1 = Pop();
        Push((HE_FLOAT)powf(d1, d2));
        break;
    case PSOP_LN:
        d1 = Pop();
        Push((HE_FLOAT)logf(d1));
        break;
    case PSOP_LOG:
        d1 = Pop();
        Push((HE_FLOAT)log10f(d1));
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
                    HE_FLOAT first = m_Stack[m_StackCount - n];
                    for (int ii = 0; ii < n - 1; ii++)
                        m_Stack[m_StackCount - n + ii] = m_Stack[m_StackCount - n + ii + 1];
                    m_Stack[m_StackCount - 1] = first;
                }
            } else {
                for (int i = 0; i < j; i++) {
                    HE_FLOAT last = m_Stack[m_StackCount - 1];
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
    return TRUE;
}