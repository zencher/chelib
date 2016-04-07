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
				dictPtr = objPtr->GetStreamPtr()->GetDictPtr();
			}else{
				return ptr;
			}
		}
	}else if ( rootObjPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		dictPtr = rootObjPtr->GetDictPtr();
	}else if ( rootObjPtr->GetType() == OBJ_TYPE_STREAM )
	{
		dictPtr = rootObjPtr->GetStreamPtr()->GetDictPtr();
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
			pTmp = pAllocator->New<CHE_PDF_Function_PostScript>( rootObjPtr->GetStreamPtr(), pAllocator );
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
	mInputCount(0), mOutputCount(0), mpDomain(NULL), mpRange(NULL), mbRange(FALSE), mBitsPerSample(8)
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
		mInputCount = array->GetCount() / 2;
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
		mOutputCount = array->GetCount() / 2;
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
    
    objPtr = dictPtr->GetElement("mBitPerSample", OBJ_TYPE_NUMBER);
    if (objPtr)
    {
        mBitsPerSample = objPtr->GetNumberPtr()->GetInteger();
    }else{
        //todo
    }
}

CHE_PDF_Function_Sampled::CHE_PDF_Function_Sampled( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( rootObjPtr, pAllocator ), mBps(1), mOrder(1), mpSize(NULL), mpEncode(NULL), mpDecode(NULL), mpSample(NULL)
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
	if ( stmAcc.Attach( stmPtr ) )
	{
		mpSample = GetAllocator()->NewArray<HE_BYTE>( stmAcc.GetSize() );
		memcpy( mpSample, stmAcc.GetData(), stmAcc.GetSize() * sizeof(HE_BYTE) );
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
	if ( mpSample )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( mpSample );
		mpSample = NULL;
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
		a = mpSample[idx0];
		b = mpSample[idx1];
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

	int e0[64], e1[64], scale[64];
	float efrac[64];
	HE_FLOAT x;
	HE_FLOAT tmpValue;

	/* encode input coordinates */
	for ( HE_ULONG i = 0; i < GetInputCount(); ++i )
	{
		x = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
		x = lerp( x, GetDomianMin( i ), GetDomianMax( i ), GetEncodeMin( i ), GetEncodeMax( i ) );
		x = fz_clamp( x, 0, GetSize( i ) - 1 );
		e0[i] = floorf(x);
		e1[i] = ceilf(x);
		efrac[i] = x - floorf(x);
	}
    
    

	scale[0] = GetOutputCount();
	for ( HE_ULONG i = 1; i < GetInputCount(); ++i )
	{
		scale[i] = scale[i - 1] * GetSize( i );
	}

	for ( HE_ULONG i = 0; i < GetOutputCount(); ++i )
	{
		if ( GetInputCount() == 1 )
		{
			float a = mpSample[e0[0] * GetOutputCount() + i ];  //todo supprot all kind of bitspersample, not only bitspersample 8
			float b = mpSample[e1[0] * GetOutputCount() + i ];  //todo supprot all kind of bitspersample, not only bitspersample 8

			float ab = a + (b - a) * efrac[0];
            
            tmpValue = ab / ( pow(2, mBitsPerSample) - 1);
            tmpValue = lerp( tmpValue, GetRangeMin( i ), GetRangeMax( i ), GetDecodeMin( i ), GetDecodeMax( i ) );
			//tmpValue = lerp( ab, 0, 1, GetDecodeMin( i ), GetDecodeMax( i ) );
            //tmpValue = fz_clamp( tmpValue, GetDecodeMin( i ), GetDecodeMax( i ) );
			//tmpValue = fz_clamp( tmpValue, GetRangeMin( i ), GetRangeMax( i ) );
		}
		else if ( GetInputCount() == 2 )
		{
			int s0 = GetOutputCount();
			int s1 = s0 * GetSize( i );

			float a = mpSample[e0[0] * s0 + e0[1] * s1 + i];
			float b = mpSample[e1[0] * s0 + e0[1] * s1 + i];
			float c = mpSample[e0[0] * s0 + e1[1] * s1 + i];
			float d = mpSample[e1[0] * s0 + e1[1] * s1 + i];

			float ab = a + (b - a) * efrac[0];
			float cd = c + (d - c) * efrac[0];
			float abcd = ab + (cd - ab) * efrac[1];

			tmpValue = lerp( abcd, 0, 1, GetDecodeMin( i ), GetDecodeMax( i ) );
			tmpValue = fz_clamp( tmpValue, GetRangeMin( i ), GetRangeMax( i ) );
		}
		else
		{
			float x = interpolate_sample(scale, e0, e1, efrac, GetInputCount() - 1, i);
			tmpValue = lerp( x, 0, 1, GetDecodeMin( i ), GetDecodeMax( i ) );
			tmpValue = fz_clamp( tmpValue, GetRangeMin( i ), GetRangeMax( i ) );
		}
		output.push_back( tmpValue );
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
        
        mOutputCount = arrayPtr->GetCount();
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
        mOutputCount = arrayPtr->GetCount();
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
		mK = arrayPtr->GetCount();
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

	inputValue = fz_clamp( inputValue, GetDomianMin( 0 ), GetDomianMax( 0 ) );

	HE_ULONG i = 0;
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
	}else if ( i = mK - 1 )
	{
		low = mBounds[mK - 2];
		high = mBounds[0];
	}else{
		low = mBounds[i - 1];
		high = mBounds[i];
	}

	inputValue = lerp( inputValue, low, high, GetEncodeMin( i ), GetEncodeMax( i ) );

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
		IHE_Read *  pRead = HE_CreateMemBufRead( stmAcc.GetData(), stmAcc.GetSize(), GetAllocator() );
		CHE_PDF_SyntaxParser syntaxParser(NULL, GetAllocator());
		syntaxParser.InitParser(pRead);
		ParseImp(syntaxParser);
		HE_DestoryIHERead( pRead );
	}

	mbParsed = TRUE;
}

HE_VOID CHE_PDF_Function_PostScript::ParseImp(CHE_PDF_SyntaxParser & syntaxParser)
{
	PSFuncItem psItem;
	CHE_PDF_ParseWordDes wordDes;
	HE_BOOL bRet = syntaxParser.GetWord(wordDes);
	
	while (bRet)
	{
		switch (wordDes.type)
		{
		case PARSE_WORD_INTEGER:
		{
			psItem.mType = PSITEM_INT;
			psItem.mIntegerValue = syntaxParser.GetInteger();
			mCodes.push_back(psItem);
			break;
		}
		case PARSE_WORD_FLOAT:
		{
			psItem.mType = PSITEM_FLOAT;
			psItem.mFloatValue = syntaxParser.GetFloat();
			mCodes.push_back(psItem);
			break;
		}
		case PARSE_WORD_BLOCK_B:
		{
			HE_UINT32 blockindex = 0, ifindex = -1, elseindex = -1;
			blockindex = mCodes.size();
			psItem.mType = PSITEM_BLOCK;
			psItem.mBlockIndex = blockindex + 4;
			mCodes.push_back(psItem);
			mCodes.push_back(psItem);
			mCodes.push_back(psItem);
			mCodes.push_back(psItem);
			ifindex = mCodes.size();
			ParseImp(syntaxParser);

			bRet = syntaxParser.GetWord(wordDes);
			if ( bRet )
			{
				if ( wordDes.type == PARSE_WORD_BLOCK_B )
				{
					elseindex = mCodes.size();
					ParseImp(syntaxParser);
					bRet = syntaxParser.GetWord(wordDes);
					if ( !bRet )
					{
						continue;
					}
				}

				if ( wordDes.type == PARSE_WORD_UNKNOWN )
				{
					CHE_ByteString str = syntaxParser.GetString();
					if ( str == "if" )
					{
						mCodes[blockindex].mType = PSITEM_OPERATOR;
						mCodes[blockindex].mOperator = PSOPERATOR_IF;
						mCodes[blockindex + 1].mType = PSITEM_BLOCK;
						mCodes[blockindex + 1].mBlockIndex = ifindex;
						mCodes[blockindex + 2].mType = PSITEM_BLOCK;
						mCodes[blockindex + 2].mBlockIndex = ifindex;
						mCodes[blockindex + 3].mType = PSITEM_BLOCK;
						mCodes[blockindex + 3].mBlockIndex = mCodes.size();
					}else if ( str == "ifelse" )
					{
						mCodes[blockindex].mType = PSITEM_OPERATOR;
						mCodes[blockindex].mOperator = PSOPERATOR_IFELSE;
						mCodes[blockindex + 1].mType = PSITEM_BLOCK;
						mCodes[blockindex + 1].mBlockIndex = ifindex;
						mCodes[blockindex + 2].mType = PSITEM_BLOCK;
						mCodes[blockindex + 2].mBlockIndex = elseindex;
						mCodes[blockindex + 3].mType = PSITEM_BLOCK;
						mCodes[blockindex + 3].mBlockIndex = mCodes.size();
					}else{
						continue;
					}
				}else{
					continue;
				}
			}

			break;
		}
		case PARSE_WORD_BLOCK_E:
		{
			psItem.mType = PSITEM_OPERATOR;
			psItem.mOperator = PSOPERATOR_RETURN;
			mCodes.push_back(psItem);
			return;
		}
		case PARSE_WORD_UNKNOWN:
		{
			switch (StringToDWORD(syntaxParser.GetString()))
			{
			case A('{'):
			{
				psItem.mType = PSITEM_BLOCK;
				mCodes.push_back(psItem);
				break;
			}
			case A('}'):
			{
				psItem.mType = PSITEM_BLOCK;
				mCodes.push_back(psItem);
				break;
			}
			case C('a', 'b', 's'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_ABS;
				mCodes.push_back(psItem);
				break;
			}
			case C('a', 'd', 'd'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_ADD;
				mCodes.push_back(psItem);
				break;
			}
			case D('a', 't', 'a', 'n'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_ATAN;
				mCodes.push_back(psItem);
				break;
			}
			case D('c', 'e', 'i', 'l'): //ceiling
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_CEILING;
				mCodes.push_back(psItem);
				break;
			}
			case C('c', 'o', 's'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_COS;
				mCodes.push_back(psItem);
				break;
			}
			case C('c', 'v', 'i'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_CVI;
				mCodes.push_back(psItem);
				break;
			}
			case C('d', 'i', 'v'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_DIV;
				mCodes.push_back(psItem);
				break;
			}
			case C('e', 'x', 'p'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_EXP;
				mCodes.push_back(psItem);
				break;
			}
			case D('f', 'l', 'o', 'o'): //floor
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_FLOOR;
				mCodes.push_back(psItem);
				break;
			}
			case D('i', 'd', 'i', 'v'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_IDIV;
				mCodes.push_back(psItem);
				break;
			}
			case B('l', 'n'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_LN;
				mCodes.push_back(psItem);
				break;
			}
			case C('l', 'o', 'g'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_LOG;
				mCodes.push_back(psItem);
				break;
			}
			case C('m', 'o', 'd'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_MOD;
				mCodes.push_back(psItem);
				break;
			}
			case C('m', 'u', 'l'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_MUL;
				mCodes.push_back(psItem);
				break;
			}
			case C('n', 'e', 'g'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_NEG;
				mCodes.push_back(psItem);
				break;
			}
			case D('r', 'o', 'u', 'n'): //round
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_ROUND;
				mCodes.push_back(psItem);
				break;
			}
			case C('s', 'i', 'n'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_SIN;
				mCodes.push_back(psItem);
				break;
			}
			case D('s', 'q', 'r', 't'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_SQRT;
				mCodes.push_back(psItem);
				break;
			}
			case C('s', 'u', 'b'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_SUB;
				mCodes.push_back(psItem);
				break;
			}
			case D('t', 'r', 'u', 'n'): //truncate
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_TRUNCATE;
				mCodes.push_back(psItem);
				break;
			}
			case C('a', 'n', 'd'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_AND;
				mCodes.push_back(psItem);
				break;
			}
			case D('b', 'i', 't', 's'): //bitshift
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_BITSHIFT;
				mCodes.push_back(psItem);
				break;
			}
			case B('e', 'q'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_EQ;
				mCodes.push_back(psItem);
				break;
			}
			case D('f', 'a', 'l', 's'): //false
			{
				psItem.mType = PSITEM_BOOL;
				psItem.mBoolValue = FALSE;
				mCodes.push_back(psItem);
				break;
			}
			case B('g', 'e'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_GE;
				mCodes.push_back(psItem);
				break;
			}
			case B('g', 't'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_GT;
				mCodes.push_back(psItem);
				break;
			}
			case B('l', 'e'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_LE;
				mCodes.push_back(psItem);
				break;
			}
			case B('l', 't'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_LT;
				mCodes.push_back(psItem);
				break;
			}
			case B('n', 'e'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_NE;
				mCodes.push_back(psItem);
				break;
			}
			case C('n', 'o', 't'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_NOT;
				mCodes.push_back(psItem);
				break;
			}
			case B('o', 'r'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_OR;
				mCodes.push_back(psItem);
				break;
			}
			case D('t', 'r', 'u', 'e'):
			{
				psItem.mType = PSITEM_BOOL;
				psItem.mBoolValue = TRUE;
				mCodes.push_back(psItem);
				break;
			}
			case C('x', 'o', 'r'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_XOR;
				mCodes.push_back(psItem);
				break;
			}
			case B('i', 'f'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_IF;
				mCodes.push_back(psItem);
				break;
			}
			case D('i', 'f', 'e', 'l'): //ifelse
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_IFELSE;
				mCodes.push_back(psItem);
				break;
			}
			case D('c', 'o', 'p', 'y'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_COPY;
				mCodes.push_back(psItem);
				break;
			}
			case C('d', 'u', 'p'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_DUP;
				mCodes.push_back(psItem);
				break;
			}
			case D('e', 'x', 'c', 'h'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_EXCH;
				mCodes.push_back(psItem);
				break;
			}
			case D('i', 'n', 'd', 'e'): //index
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_INDEX;
				mCodes.push_back(psItem);
				break;
			}
			case C('p', 'o', 'p'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_POP;
				mCodes.push_back(psItem);
				break;
			}
			case D('r', 'o', 'l', 'l'):
			{
				psItem.mType = PSITEM_OPERATOR;
				psItem.mOperator = PSOPERATOR_ROLL;
				mCodes.push_back(psItem);
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
		break;
		}

		bRet = syntaxParser.GetWord(wordDes);
	}
}

#define RADIAN 57.2957795

HE_BOOL PSFuncStack::Execute( PSFUNCITEM_OPERATOR op )
{
	static PSFuncItem item0, item1, target;
	switch( op )
	{
	case PSOPERATOR_ABS:
		{
			if ( PopItem(target) )
			{
				if (target.mType == PSITEM_INT)
				{
					target.mIntegerValue = abs(target.mIntegerValue);
					Push(target);
					return true;
				}else if ( target.mType == PSITEM_FLOAT )
				{
					target.mFloatValue = fabsf(target.mIntegerValue);
					Push(target);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_ADD:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if (item0.mType == PSITEM_INT && item1.mType == PSITEM_INT)
				{
					item0.mIntegerValue += item1.mIntegerValue;
					Push(item0);
					return true;
				}else{
					HE_FLOAT val1 = 0, val2 = 0;
					target.mType = PSITEM_FLOAT;
					if ( item0.mType == PSITEM_INT)
					{
						val1 = item0.mIntegerValue;
					}else{
						val1 = item0.mFloatValue;
					}
					if ( item1.mType == PSITEM_INT )
					{
						val2 = item1.mIntegerValue;
					}else{
						val2 = item1.mFloatValue;
					}
					target.mFloatValue = val1 + val2;
					Push(target);
					return true;
				}
				return true;
			}
			return false;
		}
	case PSOPERATOR_AND:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if (item0.mType == PSITEM_INT && item1.mType == PSITEM_INT)
				{
					target.mType = PSITEM_BOOL;
					target.mBoolValue = item0.mIntegerValue & item1.mIntegerValue;
					Push(target);
					return true;
				}
				else if (item0.mType == PSITEM_BOOL && item1.mType == PSITEM_BOOL)
				{
					target.mType = PSITEM_BOOL;
					target.mBoolValue = item0.mBoolValue & item1.mBoolValue;
					Push(target);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_ATAN:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == PSITEM_FLOAT && item1.mType == PSITEM_FLOAT)
				{
					target.mType = PSITEM_FLOAT;
					target.mFloatValue = atan2f(item0.mFloatValue, item1.mFloatValue) * RADIAN;
					if (target.mFloatValue < 0)
					{
						target.mFloatValue += 360;
					}
					Push(target);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_BITSHIFT:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == PSITEM_INT && item1.mType == PSITEM_INT)
				{
					target.mType = PSITEM_INT;
					if ( item0.mIntegerValue > 0 && item0.mIntegerValue < 8 * sizeof(item0.mIntegerValue) )
					{
						target.mIntegerValue = item1.mIntegerValue << item0.mIntegerValue;
					}else if ( item0.mIntegerValue < 0 && item0.mIntegerValue > (-8 * sizeof(item0.mIntegerValue)) )
					{
						target.mIntegerValue = item1.mIntegerValue >> -item0.mIntegerValue;
					}else{
						target.mIntegerValue = item1.mIntegerValue;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_CEILING:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				target.mType = PSITEM_FLOAT;
				target.mFloatValue = ceilf(item0.mFloatValue);
				Push(target);
				return true;
			}
			return false;
		}
	case PSOPERATOR_COPY:
		{
			HE_INT32 val;
			if ( PopInteger(val) )
			{
				vector<PSFuncItem> tmp;
				while (val--)
				{
					PopItem(item0);
					tmp.push_back(item0);
				}
				for (HE_INT32 l = tmp.size() - 1; l >= 0; --l)
				{
					Push(tmp[l]);
				}
				for (HE_INT32 l = tmp.size() - 1; l >= 0; --l)
				{
					Push(tmp[l]);
				}
				return true;
			}
			return false;
		}
	case PSOPERATOR_COS:
		{
			HE_FLOAT val = 0;
			if (PopFloat(val))
			{
				target.mType = PSITEM_FLOAT;
				target.mFloatValue = cosf(val / 57.2957795);
				Push(target);
				return true;
			}
			return false;
		}
	case PSOPERATOR_CVI:
		{
			if ( PopItem (item0) )
			{
				target.mType = PSITEM_INT;
				target.mIntegerValue = item0.mIntegerValue;
				Push(target);
				return true;
			}
			return false;
		}
	case PSOPERATOR_CVR:
		{
			if (PopItem(item0))
			{
				target.mType = PSITEM_FLOAT;
				target.mFloatValue = item0.mFloatValue;
				Push(target);
				return true;
			}
			return false;
		}
	case PSOPERATOR_DIV:
		{
			if (PopItem(item0) && PopItem(item1) && item0.mType == PSITEM_FLOAT && item1.mType == PSITEM_FLOAT)
			{
				if (fabsf(item0.mFloatValue) < FLT_EPSILON)
				{
					target.mType = PSITEM_FLOAT;
					target.mFloatValue = item1.mFloatValue / item0.mFloatValue;
				}
				else{
					if (item1.mFloatValue >= 0)
					{
						target.mFloatValue = FLT_MAX;
					}
					else{
						target.mFloatValue = -FLT_MAX;
					}
				}
				return true;
			}
			return false;
		}
	case PSOPERATOR_DUP:
		{
			if ( PopItem (item0) )
			{
				Push(item0);
				Push(item0);
				return true;
			}
			return false;
		}
	case PSOPERATOR_EQ:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if ( item0.mType == item1.mType )
				{
					if (item0.mType == PSITEM_BOOL)
					{
						PushBool(item0.mBoolValue == item1.mBoolValue);
					}
					else if (item0.mType == PSITEM_FLOAT)
					{
						PushBool(fabsf(item0.mFloatValue - item1.mFloatValue) <= FLT_EPSILON);
					}else if (item0.mType == PSITEM_INT)
					{
						PushBool(item0.mIntegerValue == item1.mIntegerValue);
					}else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_EXCH:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				Push(item1);
				Push(item0);
				return true;
			}
			return false;
		}
	case PSOPERATOR_EXP:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == PSITEM_FLOAT && item1.mType == PSITEM_FLOAT)
				{
					target.mType = PSITEM_FLOAT;
					target.mFloatValue = powf(item0.mFloatValue, item1.mFloatValue);
					Push(target);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_FALSE:
		{
			PushBool( false );
			break;
		}
	case PSOPERATOR_FLOOR:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				PushFloat(floorf(val));
				return true;
			}
			return false;
		}
	case PSOPERATOR_GE:
		{
			if ( PopItem (item0) && PopItem (item1) )
			{
				if ( item0.mType == item1.mType )
				{
					if ( item0.mType == PSITEM_INT )
					{
						PushBool(item1.mIntegerValue >= item0.mIntegerValue);
					}else if ( item0.mType == PSITEM_FLOAT )
					{
						PushBool(item1.mFloatValue >= item0.mFloatValue);
					}else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_GT:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == item1.mType)
				{
					if (item0.mType == PSITEM_INT)
					{
						PushBool(item1.mIntegerValue > item0.mIntegerValue);
					}
					else if (item0.mType == PSITEM_FLOAT)
					{
						PushBool(item1.mFloatValue > item0.mFloatValue);
					}
					else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_IDIV:
		{
			if (PopItem(item0) && PopItem(item1) && item0.mType == PSITEM_INT && item1.mType == PSITEM_INT)
			{
				if ( item0.mIntegerValue != 0 )
				{
					PushInteger(item1.mIntegerValue / item0.mIntegerValue);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_INDEX:
		{
			HE_INT32 index = 0;
			if (PopInteger(index))
			{
				vector<PSFuncItem> vec;
				for (HE_INT32 i = 0; i <= index; ++i)
				{
					if (PopItem(item0))
					{
						vec.push_back(item0);
					}
				}
				if (vec.size() == index+1)
				{
					for (HE_INT32 i = index; i >= 0; --i)
					{
						Push(vec[i]);
					}
					Push(vec[0]);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_LE:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == item1.mType)
				{
					if (item0.mType == PSITEM_INT)
					{
						PushBool(item1.mIntegerValue <= item0.mIntegerValue);
					}
					else if (item0.mType == PSITEM_FLOAT)
					{
						PushBool(item1.mFloatValue <= item0.mFloatValue);
					}
					else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_LT:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == item1.mType)
				{
					if (item0.mType == PSITEM_INT)
					{
						PushBool(item1.mIntegerValue < item0.mIntegerValue);
					}
					else if (item0.mType == PSITEM_FLOAT)
					{
						PushBool(item1.mFloatValue < item0.mFloatValue);
					}
					else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_LN:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				val = logf(val);
				PushFloat(val);
				return true;
			}
			return false;
		}
	case PSOPERATOR_LOG:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				val = log10f(val);
				PushFloat(val);
				return true;
			}
			return false;
		}
	case PSOPERATOR_MOD:
		{
			HE_INT32 val1 = 0, val2 = 0;
			if (PopInteger(val1) && PopInteger(val2))
			{
				if (val1 != 0)
				{
					PushInteger(val2 % val1);
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_MUL:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if ( item0.mType == item1.mType )
				{
					if ( item0.mType == PSITEM_INT )
					{
						PushInteger(item1.mIntegerValue * item0.mIntegerValue);
					}else if ( item0.mType == PSITEM_FLOAT )
					{
						PushFloat(item1.mFloatValue * item0.mFloatValue);
					}else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_NE:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if ( item0.mType == item1.mType )
				{
					if ( item0.mType == PSITEM_BOOL )
					{
						PushBool(item0.mBoolValue != item1.mBoolValue);
					}else if ( item0.mType == PSITEM_INT )
					{
						PushBool(item0.mIntegerValue != item1.mIntegerValue);
					}else if ( item0.mType == PSITEM_FLOAT )
					{
						PushBool(fabsf(item0.mFloatValue - item1.mFloatValue) > FLT_EPSILON);
					}else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_NEG:
		{
			if ( PopItem(item0) )
			{
				if ( item0.mType == PSITEM_INT )
				{
					PushInteger(-item0.mIntegerValue);
				}else if ( item0.mType == PSITEM_FLOAT )
				{
					PushFloat(-item0.mFloatValue);
				}else{
					return false;
				}
				return true;
			}
			return false;
		}
	case PSOPERATOR_NOT:
		{
			if ( PopItem(item0) )
			{
				if ( item0.mType == PSITEM_BOOL )
				{
					PushBool(!item0.mBoolValue);
				}else if ( item0.mType == PSITEM_INT )
				{
					PushInteger(~item0.mIntegerValue);
				}else{
					return false;
				}
				return true;
			}
			return false;
		}
	case PSOPERATOR_OR:
		{
			if (PopItem(item0) && PopItem(item1))
			{
				if (item0.mType == item1.mType)
				{
					if (item0.mType == PSITEM_BOOL)
					{
						PushBool(item0.mBoolValue || item1.mBoolValue);
					}
					else if (item0.mType == PSITEM_INT)
					{
						PushInteger(item0.mIntegerValue | item1.mIntegerValue);
					}
					else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_POP:
		{
			return PopItem(item0);			   
		}
	case PSOPERATOR_ROLL:
		{
			HE_INT32 n = 0, j = 0;
			if ( PopInteger(j) && PopInteger(n) )
			{
				vector<PSFuncItem> vec;
				for (HE_INT32 i = 0; i < n; ++i)
				{
					if ( PopItem(item0) )
					{
						vec.push_back(item0);
					}
				}
				if ( vec.size() == n )
				{
					if ( j > 0 )
					{
						j %= n;
					}else{
						j = -j % n;
						if ( j != 0 )
						{
							j = n - j;
						}
					}
					for ( HE_INT32 i = 0; i < j; i++ )
					{
						item0 = vec[0];
						memmove( &vec[0], &vec[1], (n-1)*sizeof(PSFuncItem) );
						vec[n-1] = item0;
					}
					for ( HE_INT32 i = n - 1; i >= 0; --i )
					{
						Push(vec[i]);
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_ROUND:
		{
			if ( PopItem(item0) )
			{
				if ( item0.mType == PSITEM_FLOAT )
				{
					PushFloat((item0.mFloatValue >= 0) ? floorf(item0.mFloatValue + 0.5f) : ceilf(item0.mFloatValue - 0.5f));
				}else if ( item0.mType == PSITEM_INT )
				{
					PushInteger(item0.mIntegerValue);
				}else{
					return false;
				}
				return true;
			}
			return false;
		}
	case PSOPERATOR_SIN:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				PushFloat(sinf(val / RADIAN));
				return true;
			}
			return false;
		}
	case PSOPERATOR_SQRT:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				PushFloat(sqrtf(val));
				return true;
			}
			return false;
		}
	case PSOPERATOR_SUB:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if ( item0.mType == item1.mType )
				{
					if ( item0.mType == PSITEM_INT )
					{
						PushInteger(item1.mIntegerValue - item0.mIntegerValue);
					}else if ( item0.mType == PSITEM_FLOAT )
					{
						PushFloat(item1.mFloatValue - item0.mFloatValue);
					}else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	case PSOPERATOR_TRUE:
		{
			PushBool(true);
			return true;
		}
	case PSOPERATOR_TRUNCATE:
		{
			HE_FLOAT val = 0;
			if ( PopFloat(val) )
			{
				PushFloat((val >= 0) ? floorf(val) : ceilf(val));
				return true;
			}
			return false;
		}
	case PSOPERATOR_XOR:
		{
			if ( PopItem(item0) && PopItem(item1) )
			{
				if ( item0.mType == item1.mType )
				{
					if ( item0.mType == PSITEM_BOOL )
					{
						PushBool(item0.mBoolValue ^ item1.mBoolValue);
					}else if (item0.mType == PSITEM_INT)
					{
						PushInteger(item1.mIntegerValue ^ item0.mIntegerValue);
					}else{
						return false;
					}
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

HE_BOOL CHE_PDF_Function_PostScript::RunCode(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
{
	PSFuncStack stack;
	stack.Init();

	if ( mCodes.size() == 0 )
	{
		return false;
	}

	HE_UINT32 c = GetInputCount();
	HE_FLOAT tmpVal = 0.0f;
	for ( HE_UINT32 i = 0; i < input.size() && i < c; ++i )
	{
		tmpVal = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
		stack.PushFloat( tmpVal );
	}

	if ( RunCodeImp(stack, 0) )
	{
		HE_UINT32 i = 0, c = GetOutputCount();
		while ( stack.PopFloat(tmpVal) && i < c )
		{
			tmpVal = fz_clamp(tmpVal, GetRangeMin(i), GetRangeMax(i));
			output.push_back(tmpVal);
			++i;
		}
		return true;
	}
	return false;
}

HE_BOOL CHE_PDF_Function_PostScript::RunCodeImp(PSFuncStack & stack, HE_ULONG codeIndex)
{
	PSFuncItem * pItem = NULL;
	HE_ULONG codeSize = mCodes.size();
	for ( ; codeIndex < codeSize; )
	{
		switch (mCodes[codeIndex].mType)
		{
		case PSITEM_BOOL:
			{
				stack.PushBool(mCodes[codeIndex].mBoolValue);
				break;
			}
		case PSITEM_INT:
			{
				stack.PushInteger(mCodes[codeIndex].mIntegerValue);
				break;
			}
		case PSITEM_FLOAT:
			{
				stack.PushFloat(mCodes[codeIndex].mFloatValue);
				break;
			}
		case PSITEM_BLOCK:
			{
				codeIndex = mCodes[codeIndex].mBlockIndex;
				continue;
			}
		case PSITEM_OPERATOR:
			{
				switch (mCodes[codeIndex].mOperator)
				{
				case PSOPERATOR_IF:
					{
						HE_BOOL bVal = false;
						if (stack.PopBool(bVal))
						{
							if (bVal)
							{
								if ( !RunCodeImp(stack, mCodes[codeIndex + 1].mBlockIndex) )
								{
									return false;
								}
							}
							codeIndex = mCodes[codeIndex + 3].mBlockIndex;
							continue;
						}
						break;
					}
				case PSOPERATOR_IFELSE:
					{
						HE_BOOL bVal = false;
						if (stack.PopBool(bVal))
						{
							if (bVal)
							{
								if ( !RunCodeImp(stack, mCodes[codeIndex + 1].mBlockIndex) )
								{
									return false;
								}
							}else{
								if ( !RunCodeImp(stack, mCodes[codeIndex + 2].mBlockIndex) )
								{
									return false;
								}
							}
							codeIndex = mCodes[codeIndex + 3].mBlockIndex;
							continue;
						}
						break;
					}
				case PSOPERATOR_RETURN:
					{
						return true;
					}
				default:
					{
						if ( !stack.Execute(mCodes[codeIndex].mOperator) )
						{
							return false;
						}
					}
				}
				break;
			}
		default:break;
		}
		++codeIndex;
	}
	return true;
}



