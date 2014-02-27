#include "../../include/pdf/che_pdf_function.h"
#include "../../include/pdf/che_pdf_parser.h"

#include <math.h>
#include <float.h>

#define A(a)		(a)
#define B(a,b)		(a | b<<8)
#define C(a,b,c)	(a | b<<8 | c<<16)
#define D(a,b,c,d)	(a | b<<8 | c<<16 | d<<24)

static HE_ULONG StringToDWORD( CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return 0;
	}
	HE_ULONG length = 4;
	if ( str.GetLength() < length )
	{
		length = str.GetLength();
	}
	HE_ULONG valRet = 0;
	for ( HE_ULONG i = length; i > 0; --i )
	{
		valRet = valRet<<8;
		valRet |= str.GetData()[i-1];
	}
	return valRet;
}

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
			pTmp = pAllocator->New<CHE_PDF_Function_PostScript>( rootObjPtr, pAllocator );
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


CHE_PDF_Function::CHE_PDF_Function( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Component(COMPONENT_TYPE_Function, rootObjPtr, pAllocator), mType(FUNCTION_TYPE_SAMPLE), 
	mInputCount(0), mOutputCount(0), mpDomain(NULL), mpRange(NULL), mbRange(FALSE)
{
	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr;
	if ( mRootObjPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = mRootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			dictPtr = objPtr->GetDictPtr();
		}
	}else if ( mRootObjPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		dictPtr = mRootObjPtr->GetDictPtr();
	}else{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	objPtr = dictPtr->GetElement( "FunctionType", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		switch( objPtr->GetNumberPtr()->GetInteger() )
		{
		case 0:
			mType = FUNCTION_TYPE_SAMPLE;
			break;
		case 2:
			mType = FUNCTION_TYPE_EXPONENTIAL;
			break;
		case 3:
			mType = FUNCTION_TYPE_STITCHING;
			break;
		case 4:
			mType = FUNCTION_TYPE_POSTSCRIPT;
			break;
		default:
			SetError( COMPONENT_ERROR_CONSTRUCTION );
			return;
 		}
	}

	objPtr = dictPtr->GetElement( "Domain", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CHE_PDF_ArrayPtr array = objPtr->GetArrayPtr();
		mInputCount = array->GetCount() / 2;
		mpDomain = GetAllocator()->NewArray<HE_FLOAT>( mInputCount * 2 );
		for ( HE_ULONG i = 0; i < array->GetCount(); ++i )
		{
			objPtr = array->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpDomain[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpDomain[i] = 0.0f;
			}
		}
	}else{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	objPtr = dictPtr->GetElement( "Range", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CHE_PDF_ArrayPtr array = objPtr->GetArrayPtr();
		mOutputCount = array->GetCount() / 2;
		mpRange = GetAllocator()->NewArray<HE_FLOAT>( mOutputCount * 2 );
		for ( HE_ULONG i = 0; i < array->GetCount(); ++i )
		{
			objPtr = array->GetElement( i, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mpRange[i] = objPtr->GetNumberPtr()->GetFloat();
			}else{
				mpRange[i] = 0.0f;
			}
		}
	}else{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}
}

CHE_PDF_Function_Sampled::CHE_PDF_Function_Sampled( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( rootObjPtr, pAllocator ), mBps(1), mOrder(1), mpSize(NULL), mpEncode(NULL), mpDecode(NULL), mpSample(NULL)
{
	if ( mRootObjPtr->GetType() != OBJ_TYPE_REFERENCE )
	{
		SetError( COMPONENT_ERROR_CONSTRUCTION );
		return;
	}

	CHE_PDF_ObjectPtr objPtr = mRootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
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
		mpDecode = GetAllocator()->NewArray<HE_FLOAT>( 2 * GetInputCount() );
		for ( HE_ULONG i = 0; i < arrayPtr->GetCount() && i < 2 * GetInputCount(); ++i )
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
	if ( index < GetInputCount() && mpDecode )
	{
		return mpDecode[index * 2];
	}
	return 0;
}

HE_FLOAT CHE_PDF_Function_Sampled::GetDecodeMax( HE_UINT32 index ) const
{
	if ( index < GetInputCount() && mpDecode )
	{
		return mpDecode[index * 2 + 1];
	}
	return 0;
}

HE_BOOL CHE_PDF_Function_Sampled::Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
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

	for ( HE_ULONG i = 0; i < GetInputCount(); ++i )
	{
		if ( GetInputCount() == 1 )
		{
			float a = mpSample[e0[0] * GetOutputCount() + i];
			float b = mpSample[e1[0] * GetOutputCount() + i];

			float ab = a + (b - a) * efrac[0];

			tmpValue = lerp( ab, 0, 1, GetDecodeMin( i ), GetDecodeMax( i ) );
			tmpValue = fz_clamp( tmpValue, GetRangeMin( i ), GetRangeMax( i ) );
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

	if ( mRootObjPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = mRootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			dictPtr = objPtr->GetDictPtr();
		}else{
			SetError( COMPONENT_ERROR_CONSTRUCTION );
			return;
		}
	}else if ( mRootObjPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		dictPtr = mRootObjPtr->GetDictPtr();
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

HE_BOOL CHE_PDF_Function_Exponential::Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
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
	
	for ( HE_ULONG i = 0; i < GetOutputCount(); ++i )
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

	if ( mRootObjPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = mRootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
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
					mFunctions[i] = tmpFunctionPtr;
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

HE_BOOL CHE_PDF_Function_Stitching::Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
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

CHE_PDF_Function_PostScript::CHE_PDF_Function_PostScript( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( rootObjPtr, pAllocator ), mbParsed( FALSE )
{
}

CHE_PDF_Function_PostScript::~CHE_PDF_Function_PostScript()
{
}

HE_BOOL CHE_PDF_Function_PostScript::Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
{
	if ( mbParsed == FALSE )
	{
		Parse();
	}

	//todo

	return TRUE;
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
		CHE_PDF_SyntaxParser syntaxParser( NULL, GetAllocator() );
		CHE_PDF_ParseWordDes wordDes( GetAllocator() );

		PSFuncItem psItem;

		if ( syntaxParser.InitParser( pRead ) )
		{
			while ( syntaxParser.GetWord( wordDes ) )
			{
				switch ( wordDes.type )
				{
				case PARSE_WORD_INTEGER:
					{
						psItem.mType = PSITEM_INT;
						psItem.mIntegerValue = wordDes.str.GetInteger();
						mCodes.push_back( psItem );
						break;
					}
				case PARSE_WORD_FLOAT:
					{
						psItem.mType = PSITEM_FLOAT;
						psItem.mFloatValue = wordDes.str.GetFloat();
						mCodes.push_back( psItem );
						break;
					}
				case PARSE_WORD_UNKNOWN:
					{
						switch ( StringToDWORD( wordDes.str ) )
						{
						case A( '{' ):
							{
								psItem.mType = PSITEM_BLOCK;
								mCodes.push_back( psItem );
								break;
							}
						case A( '}' ):
							{
								psItem.mType = PSITEM_BLOCK;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'a', 'b', 's' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_ABS;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'a', 'd', 'd' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_ADD;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'a', 't', 'a', 'n' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_ATAN;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'c', 'e', 'i', 'l' ): //ceiling
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_CEILING;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'c', 'o', 's' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_COS;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'c', 'v', 'i' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_CVI;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'd', 'i', 'v' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_DIV;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'e', 'x', 'p' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_EXP;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'f', 'l', 'o', 'o' ): //floor
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_FLOOR;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'i', 'd', 'i', 'v' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_IDIV;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'l', 'n' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_LN;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'l', 'o', 'g' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_LOG;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'm', 'o', 'd' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_MOD;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'm', 'u', 'l' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_MUL;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'n', 'e', 'g' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_NEG;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'r', 'o', 'u', 'n' ): //round
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_ROUND;
								mCodes.push_back( psItem );
								break;
							}
						case C( 's', 'i', 'n' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_SIN;
								mCodes.push_back( psItem );
								break;
							}
						case D( 's', 'q', 'r', 't' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_SQRT;
								mCodes.push_back( psItem );
								break;
							}
						case C( 's', 'u', 'b' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_SUB;
								mCodes.push_back( psItem );
								break;
							}
						case D( 't', 'r', 'u', 'n' ): //truncate
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_TRUNCATE;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'a', 'n', 'd' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_AND;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'b', 'i', 't', 's' ): //bitshift
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_BITSHIFT;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'e', 'q' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_EQ;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'f', 'a', 'l', 's' ): //false
							{
								psItem.mType = PSITEM_BOOL;
								psItem.mBoolValue = FALSE;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'g', 'e' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_GE;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'g', 't' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_GT;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'l', 'e' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_LE;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'l', 't' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_LT;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'n', 'e' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_NE;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'n', 'o', 't' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_NOT;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'o', 'r' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_OR;
								mCodes.push_back( psItem );
								break;
							}
						case D( 't', 'r', 'u', 'e' ):
							{
								psItem.mType = PSITEM_BOOL;
								psItem.mBoolValue = TRUE;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'x', 'o', 'r' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_XOR;
								mCodes.push_back( psItem );
								break;
							}
						case B( 'i', 'f' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_IF;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'i', 'f', 'e', 'l' ): //ifelse
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_IFELSE;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'c', 'o', 'p', 'y' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_COPY;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'd', 'u', 'p' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_DUP;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'e', 'x', 'c', 'h' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_EXCH;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'i', 'n', 'd', 'e' ): //index
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_INDEX;
								mCodes.push_back( psItem );
								break;
							}
						case C( 'p', 'o', 'p' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_POP;
								mCodes.push_back( psItem );
								break;
							}
						case D( 'r', 'o', 'l', 'l' ):
							{
								psItem.mType = PSITEM_OPERATOR;
								psItem.mOperator = PSOPERATOR_ROLL;
								mCodes.push_back( psItem );
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
			}
		}

		HE_DestoryIHERead( pRead );
	}

	mbParsed = TRUE;
}

HE_BOOL PSFuncStack::Execute( PSFUNCITEM_OPERATOR op )
{
	static PSFuncItem * pItem0 = NULL;
	static PSFuncItem * pItem1 = NULL;
	static PSFuncItem * pItem2 = NULL;
	static PSFuncItem * pItem3 = NULL;
	static PSFuncItem * pTaregtItem = NULL;
	switch( op )
	{
	case PSOPERATOR_ABS:
		{
			pItem0 = &mStack[mStackTopIndex];
			pTaregtItem = pItem0;
			if ( pItem0->mType == PSITEM_FLOAT )
			{
				pItem0->mFloatValue = fabsf( pItem0->mFloatValue );
			}else if ( pItem0->mType == PSITEM_INT )
			{
				pItem0->mIntegerValue = abs( pItem0->mIntegerValue );
			}
			return TRUE;
		}
	case PSOPERATOR_ADD:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			--mStackTopIndex;
			if ( pItem0->mType == PSITEM_INT )
			{
				HE_INT32 val = pItem0->mIntegerValue;
				if ( pItem1->mType == PSITEM_INT )
				{
					pTaregtItem->mIntegerValue += val;
				}else if ( pItem1->mType == PSITEM_FLOAT )
				{
					pTaregtItem->mFloatValue += val;
				}
			}else if ( pItem0->mType == PSITEM_FLOAT )
			{
				HE_FLOAT val = pItem0->mFloatValue;
				if ( pItem1->mType == PSITEM_INT )
				{
					pTaregtItem->mType = PSITEM_FLOAT;
					pTaregtItem->mFloatValue = pItem1->mIntegerValue + val;
				}else if ( pItem1->mType == PSITEM_FLOAT )
				{
					pTaregtItem->mFloatValue = pItem1->mFloatValue + val;
				}
			}
			return TRUE;
		}
	case PSOPERATOR_AND:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			--mStackTopIndex;
			if ( pItem0->mType == PSITEM_INT && pItem1->mType == PSITEM_INT )
			{
				pTaregtItem->mType = PSITEM_BOOL;
				pTaregtItem->mBoolValue = pItem0->mIntegerValue & pItem1->mIntegerValue;
			}else if ( pItem0->mType == PSITEM_BOOL && pItem1->mType == PSITEM_BOOL )
			{
				pTaregtItem->mBoolValue = pItem0->mBoolValue & pItem1->mBoolValue;
			}
			break;
		}
	case PSOPERATOR_ATAN:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			--mStackTopIndex;

			HE_FLOAT val = atan2f( pItem0->mFloatValue, pItem1->mFloatValue ) * 57.2957795;
			if ( val < 0 )
			{
				val += 360;
			}
			pTaregtItem->mFloatValue = val;
			break;
		}
	case PSOPERATOR_BITSHIFT:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			--mStackTopIndex;

			if ( pItem0->mIntegerValue > 0 && pItem0->mIntegerValue < 8 * sizeof (pItem0->mIntegerValue))
			{
				pTaregtItem->mIntegerValue = pItem1->mIntegerValue << pItem0->mIntegerValue;
				pTaregtItem->mType = PSITEM_INT;
			}
			else if ( pItem0->mIntegerValue < 0 && pItem0->mIntegerValue > -8 * (int)sizeof (pItem0->mIntegerValue) )
			{
				pTaregtItem->mIntegerValue = pItem0->mIntegerValue >> - pItem0->mIntegerValue;
				pTaregtItem->mType = PSITEM_INT;
			}
			else
			{
				pTaregtItem->mIntegerValue = pItem1->mIntegerValue;
				pTaregtItem->mType = PSITEM_INT;
			}
			break;
		}
	case PSOPERATOR_CEILING:
		{
			pTaregtItem = &mStack[mStackTopIndex];
			pTaregtItem->mFloatValue = ceilf(pTaregtItem->mFloatValue);
			break;
		}
	case PSOPERATOR_COPY:
		{
			pTaregtItem = &mStack[mStackTopIndex];
			--mStackTopIndex;

			pItem0 = &mStack[mStackTopIndex-pTaregtItem->mIntegerValue];
			pItem1 = &mStack[mStackTopIndex];

			for ( pTaregtItem = pItem0; pTaregtItem <= pItem1; ++pTaregtItem )
			{
				Push( *pTaregtItem );
			}
			break;
		}
	case PSOPERATOR_COS:
		{
			pTaregtItem = &mStack[mStackTopIndex];
			pTaregtItem->mFloatValue = cosf( pTaregtItem->mFloatValue / 57.2957795 );
			break;
		}
	case PSOPERATOR_CVI:
		{
			pTaregtItem = &mStack[mStackTopIndex];
			pTaregtItem->mIntegerValue = pTaregtItem->mIntegerValue;
			pTaregtItem->mType = PSITEM_INT;
			break;
		}
	case PSOPERATOR_CVR:
		{
			pTaregtItem = &mStack[mStackTopIndex];
			pTaregtItem->mFloatValue = pTaregtItem->mFloatValue;
			pTaregtItem->mType = PSITEM_FLOAT;
			break;
		}
	case PSOPERATOR_DIV:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			mStackTopIndex--;

			pTaregtItem->mType = PSITEM_FLOAT;
			if (fabsf(pItem0->mFloatValue) < FLT_EPSILON)
			{
				pTaregtItem->mFloatValue = pItem1->mFloatValue / pItem0->mFloatValue;
			}
			else
			{
				if ( pItem1->mFloatValue >= 0 )
				{
					pTaregtItem->mFloatValue = FLT_MAX;
				}else{
					pTaregtItem->mFloatValue = -FLT_MAX;
				}
			}
			break;
		}
	case PSOPERATOR_DUP:
		{
			pTaregtItem = &mStack[mStackTopIndex];
			Push( *pTaregtItem );
			break;
		}
	case PSOPERATOR_EQ:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			mStackTopIndex--;

			if ( pItem1->mType == PSITEM_BOOL )
			{
				pTaregtItem->mType = PSITEM_BOOL;
				pTaregtItem->mBoolValue = ( pItem0->mBoolValue == pItem1->mBoolValue );
			}
			else if ( pItem1->mType == PSITEM_INT )
			{
				pTaregtItem->mType = PSITEM_BOOL;
				pTaregtItem->mBoolValue = ( pItem0->mIntegerValue == pItem1->mIntegerValue );
			}
			else {
				pTaregtItem->mType = PSITEM_BOOL;
				pTaregtItem->mBoolValue = ( pItem0->mFloatValue == pItem1->mFloatValue );
			}
			break;
		}
	case PSOPERATOR_EXCH:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			PSFuncItem tmpVal = *pItem0;
			*pItem0 = *pItem1;
			*pItem1 = tmpVal;
			break;
		}
	case PSOPERATOR_EXP:
		{
			pItem0 = &mStack[mStackTopIndex];
			pItem1 = &mStack[mStackTopIndex-1];
			pTaregtItem = pItem1;
			mStackTopIndex--;
			pTaregtItem->mType = PSITEM_FLOAT;
			pTaregtItem->mFloatValue = powf( pItem0->mFloatValue, pItem1->mFloatValue );
			break;
		}
	case PSOPERATOR_FALSE:
		{
			PushBool( FALSE );
			break;
		}
	}
}

HE_BOOL CHE_PDF_Function_PostScript::RunCode( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )
{
	PSFuncStack stack;
	stack.Init();

	if ( mCodes.size() == 0 )
	{
		return FALSE;
	}

	HE_FLOAT tmpVal = 0.0f;
	for ( HE_ULONG i = 0; i < input.size(); ++i )
	{
		tmpVal = fz_clamp( input[i], GetDomianMin( i ), GetDomianMax( i ) );
		stack.PushFloat( tmpVal );
	}

	PSFuncItem * pItem = NULL;
	HE_ULONG codeSize = mCodes.size();
	for ( HE_ULONG codeIndex = 0; codeIndex < codeSize; ++codeIndex )
	{
		switch ( mCodes[codeIndex].mType )
		{
		case PSITEM_BOOL:
			{
				stack.PushBool( mCodes[codeIndex++].mBoolValue );
				break;
			}
		case PSITEM_INT:
			{
				stack.PushInteger( mCodes[codeIndex++].mIntegerValue );
				break;
			}
		case PSITEM_FLOAT:
			{
				stack.PushFloat( mCodes[codeIndex++].mFloatValue );
				break;
			}
		case PSITEM_BLOCK:
			{
				break;
			}
		case PSITEM_OPERATOR:
			{
				switch( mCodes[codeIndex].mOperator )
				{
				case PSOPERATOR_ABS:
				case PSOPERATOR_ADD:
				case PSOPERATOR_AND:
				case PSOPERATOR_ATAN:
				case PSOPERATOR_BITSHIFT:
				case PSOPERATOR_COPY:
				case PSOPERATOR_COS:
				case PSOPERATOR_DIV:
				case PSOPERATOR_DUP:
				case PSOPERATOR_EQ:
				case PSOPERATOR_EXCH:
				case PSOPERATOR_EXP:
				case PSOPERATOR_FALSE:
					{
						stack.Execute( mCodes[codeIndex].mOperator );
						break;
					}

// 				case PSOPERATOR_FLOOR:
// 					r1 = ps_pop_real(st);
// 					ps_push_real(st, floorf(r1));
// 					break;
// 
// 				case PSOPERATOR_GE:
// 					if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_bool(st, i1 >= i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_bool(st, r1 >= r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_GT:
// 					if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_bool(st, i1 > i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_bool(st, r1 > r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_IDIV:
// 					i2 = ps_pop_int(st);
// 					i1 = ps_pop_int(st);
// 					if (i2 != 0)
// 						ps_push_int(st, i1 / i2);
// 					else
// 						ps_push_int(st, DIV_BY_ZERO(i1, i2, INT_MIN, INT_MAX));
// 					break;
// 
// 				case PSOPERATOR_INDEX:
// 					ps_index(st, ps_pop_int(st));
// 					break;
// 
// 				case PSOPERATOR_LE:
// 					if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_bool(st, i1 <= i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_bool(st, r1 <= r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_LN:
// 					r1 = ps_pop_real(st);
// 					/* Bug 692941 - logf as separate statement */
// 					r2 = logf(r1);
// 					ps_push_real(st, r2);
// 					break;
// 
// 				case PSOPERATOR_LOG:
// 					r1 = ps_pop_real(st);
// 					ps_push_real(st, log10f(r1));
// 					break;
// 
// 				case PSOPERATOR_LT:
// 					if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_bool(st, i1 < i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_bool(st, r1 < r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_MOD:
// 					i2 = ps_pop_int(st);
// 					i1 = ps_pop_int(st);
// 					if (i2 != 0)
// 						ps_push_int(st, i1 % i2);
// 					else
// 						ps_push_int(st, DIV_BY_ZERO(i1, i2, INT_MIN, INT_MAX));
// 					break;
// 
// 				case PSOPERATOR_MUL:
// 					if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_int(st, i1 * i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_real(st, r1 * r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_NE:
// 					if (ps_is_type2(st, PS_BOOL)) {
// 						b2 = ps_pop_bool(st);
// 						b1 = ps_pop_bool(st);
// 						ps_push_bool(st, b1 != b2);
// 					}
// 					else if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_bool(st, i1 != i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_bool(st, r1 != r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_NEG:
// 					if (ps_is_type(st, PS_INT))
// 						ps_push_int(st, -ps_pop_int(st));
// 					else
// 						ps_push_real(st, -ps_pop_real(st));
// 					break;
// 
// 				case PSOPERATOR_NOT:
// 					if (ps_is_type(st, PS_BOOL))
// 						ps_push_bool(st, !ps_pop_bool(st));
// 					else
// 						ps_push_int(st, ~ps_pop_int(st));
// 					break;
// 
// 				case PSOPERATOR_OR:
// 					if (ps_is_type2(st, PS_BOOL)) {
// 						b2 = ps_pop_bool(st);
// 						b1 = ps_pop_bool(st);
// 						ps_push_bool(st, b1 || b2);
// 					}
// 					else {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_int(st, i1 | i2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_POP:
// 					if (!ps_underflow(st, 1))
// 						st->sp--;
// 					break;
// 
// 				case PSOPERATOR_ROLL:
// 					i2 = ps_pop_int(st);
// 					i1 = ps_pop_int(st);
// 					ps_roll(st, i1, i2);
// 					break;
// 
// 				case PSOPERATOR_ROUND:
// 					if (!ps_is_type(st, PS_INT)) {
// 						r1 = ps_pop_real(st);
// 						ps_push_real(st, (r1 >= 0) ? floorf(r1 + 0.5f) : ceilf(r1 - 0.5f));
// 					}
// 					break;
// 
// 				case PSOPERATOR_SIN:
// 					r1 = ps_pop_real(st);
// 					ps_push_real(st, sinf(r1/RADIAN));
// 					break;
// 
// 				case PSOPERATOR_SQRT:
// 					r1 = ps_pop_real(st);
// 					ps_push_real(st, sqrtf(r1));
// 					break;
// 
// 				case PSOPERATOR_SUB:
// 					if (ps_is_type2(st, PS_INT)) {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_int(st, i1 - i2);
// 					}
// 					else {
// 						r2 = ps_pop_real(st);
// 						r1 = ps_pop_real(st);
// 						ps_push_real(st, r1 - r2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_TRUE:
// 					ps_push_bool(st, 1);
// 					break;
// 
// 				case PSOPERATOR_TRUNCATE:
// 					if (!ps_is_type(st, PS_INT)) {
// 						r1 = ps_pop_real(st);
// 						ps_push_real(st, (r1 >= 0) ? floorf(r1) : ceilf(r1));
// 					}
// 					break;
// 
// 				case PSOPERATOR_XOR:
// 					if (ps_is_type2(st, PS_BOOL)) {
// 						b2 = ps_pop_bool(st);
// 						b1 = ps_pop_bool(st);
// 						ps_push_bool(st, b1 ^ b2);
// 					}
// 					else {
// 						i2 = ps_pop_int(st);
// 						i1 = ps_pop_int(st);
// 						ps_push_int(st, i1 ^ i2);
// 					}
// 					break;
// 
// 				case PSOPERATOR_IF:
// 					b1 = ps_pop_bool(st);
// 					if (b1)
// 						ps_run(ctx, code, st, code[pc + 1].u.block);
// 					pc = code[pc + 2].u.block;
// 					break;
// 
// 				case PSOPERATOR_IFELSE:
// 					b1 = ps_pop_bool(st);
// 					if (b1)
// 						ps_run(ctx, code, st, code[pc + 1].u.block);
// 					else
// 						ps_run(ctx, code, st, code[pc + 0].u.block);
// 					pc = code[pc + 2].u.block;
// 					break;
// 
// 				case PSOPERATOR_RETURN:
// 					return;

				default:break;;
				}
				break;
			}
		default:break;
		}
	}
	

// 	ps_run(ctx, func->u.p.code, &st, 0);
// 
// 	for (i = func->n - 1; i >= 0; i--)
// 	{
// 		x = ps_pop_real(&st);
// 		out[i] = fz_clamp(x, func->range[i][0], func->range[i][1]);
// 	}


}

// typedef struct _ps_stack
// {
// 	CHE_PDF_Function_PostScriptItem stack[100];
// 	int sp;
// }ps_stack;

// 

// static void ps_init_stack(ps_stack *st)
// {
// 	memset(st->stack, 0, sizeof(st->stack));
// 	st->sp = 0;
// }
// 
// #define nelem(x) (sizeof(x)/sizeof((x)[0]))
// 
// static inline int ps_overflow(ps_stack *st, int n)
// {
// 	return n < 0 || st->sp + n >= nelem(st->stack);
// }
// 
// static inline int ps_underflow(ps_stack *st, int n)
// {
// 	return n < 0 || st->sp - n < 0;
// }
// 
// static inline int ps_is_type(ps_stack *st, int t)
// {
// 	return !ps_underflow(st, 1) && st->stack[st->sp - 1].mType == t;
// }
// 
// static inline int ps_is_type2(ps_stack *st, int t)
// {
// 	return !ps_underflow(st, 2) && st->stack[st->sp - 1].mType == t && st->stack[st->sp - 2].mType == t;
// }
// 
// static void ps_push_bool(ps_stack *st, HE_BOOL b)
// {
// 	if (!ps_overflow(st, 1))
// 	{
// 		st->stack[st->sp].mType = PSITEM_BOOL;
// 		st->stack[st->sp].mBoolValue = b;
// 		st->sp++;
// 	}
// }
// 
// static void
// ps_push_int(ps_stack *st, HE_INT32 n)
// {
// 	if (!ps_overflow(st, 1))
// 	{
// 		st->stack[st->sp].mType = PSITEM_INT;
// 		st->stack[st->sp].mIntegerValue = n;
// 		st->sp++;
// 	}
// }
// 
// static void
// ps_push_real(ps_stack *st, HE_FLOAT n)
// {
// 	if (!ps_overflow(st, 1))
// 	{
// 		st->stack[st->sp].mType = PSITEM_FLOAT;
// 		if (_isnan(n))
// 		{
// 			/* Push 1.0, as it's a small known value that won't
// 			 * cause a divide by 0. Same reason as in fz_atof. */
// 			n = 1.0;
// 		}
// 		st->stack[st->sp].u.f = fz_clamp(n, -FLT_MAX, FLT_MAX);
// 		st->sp++;
// 	}
// }
// 
// static int
// ps_pop_bool(ps_stack *st)
// {
// 	if (!ps_underflow(st, 1))
// 	{
// 		if (ps_is_type(st, PS_BOOL))
// 			return st->stack[--st->sp].u.b;
// 	}
// 	return 0;
// }
// 
// static int
// ps_pop_int(ps_stack *st)
// {
// 	if (!ps_underflow(st, 1))
// 	{
// 		if (ps_is_type(st, PS_INT))
// 			return st->stack[--st->sp].u.i;
// 		if (ps_is_type(st, PS_REAL))
// 			return st->stack[--st->sp].u.f;
// 	}
// 	return 0;
// }
// 
// static float
// ps_pop_real(ps_stack *st)
// {
// 	if (!ps_underflow(st, 1))
// 	{
// 		if (ps_is_type(st, PS_INT))
// 			return st->stack[--st->sp].u.i;
// 		if (ps_is_type(st, PS_REAL))
// 			return st->stack[--st->sp].u.f;
// 	}
// 	return 0;
// }
// 
// static void
// ps_copy(ps_stack *st, int n)
// {
// 	if (!ps_underflow(st, n) && !ps_overflow(st, n))
// 	{
// 		memcpy(st->stack + st->sp, st->stack + st->sp - n, n * sizeof(psobj));
// 		st->sp += n;
// 	}
// }
// 
// static void
// ps_roll(ps_stack *st, int n, int j)
// {
// 	psobj tmp;
// 	int i;
// 
// 	if (ps_underflow(st, n) || j == 0 || n == 0)
// 		return;
// 
// 	if (j >= 0)
// 	{
// 		j %= n;
// 	}
// 	else
// 	{
// 		j = -j % n;
// 		if (j != 0)
// 			j = n - j;
// 	}
// 
// 	for (i = 0; i < j; i++)
// 	{
// 		tmp = st->stack[st->sp - 1];
// 		memmove(st->stack + st->sp - n + 1, st->stack + st->sp - n, n * sizeof(psobj));
// 		st->stack[st->sp - n] = tmp;
// 	}
// }
// 
// static void
// ps_index(ps_stack *st, int n)
// {
// 	if (!ps_overflow(st, 1) && !ps_underflow(st, n))
// 	{
// 		st->stack[st->sp] = st->stack[st->sp - n - 1];
// 		st->sp++;
// 	}
// }

// 

// HE_BOOL CHE_PDF_Function_PostScript::RunCode( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output )

// {

// 

// }


