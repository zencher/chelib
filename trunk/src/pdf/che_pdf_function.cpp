#include "../../include/pdf/che_pdf_function.h"
#include "../../include/pdf/che_pdf_parser.h"

#define A(a)		(a)
#define B(a,b)		(a | b<<8)
#define C(a,b,c)	(a | b<<8 | c<<16)
#define D(a,b,c,d)	(a | b<<8 | c<<16 | d<<24)

HE_ULONG StringToDWORD( CHE_ByteString & str )
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

CHE_PDF_Function * CHE_PDF_Function::Create( CHE_PDF_ReferencePtr refPtr, CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( !refPtr )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	CHE_PDF_Function * pTmp = NULL;
	CHE_PDF_DictionaryPtr dictPtr;
	CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_DICTIONARY );
	if ( objPtr )
	{
		dictPtr = objPtr->GetDictPtr();
		objPtr = dictPtr->GetElement( "FunctionType", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			switch( objPtr->GetNumberPtr()->GetInteger() )
			{
			case 2:
				pTmp = pAllocator->New<CHE_PDF_Function_Exponential>( dictPtr, pAllocator );
				break;
			case 3:
				pTmp = pAllocator->New<CHE_PDF_Function_Stitching>( dictPtr, pAllocator );
				break;
			default:;
			}
		}
	}else{
		objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
		if ( objPtr )
		{
			CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
			dictPtr = stmPtr->GetDictPtr();
			if ( dictPtr )
			{
				objPtr = dictPtr->GetElement( "FunctionType", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					;
					switch( objPtr->GetNumberPtr()->GetInteger() )
					{
					case 0:
						pTmp = pAllocator->New<CHE_PDF_Function_Sampled>( stmPtr, pAllocator );
						break;
					case 4:
						pTmp = pAllocator->New<CHE_PDF_Function_PostScript>( stmPtr, pAllocator );
						break;
					default:;
					}
				}
			}
		}
	}
	return pTmp;
}

HE_VOID CHE_PDF_Function::Destroy( CHE_PDF_Function * pFunction )
{
	if ( pFunction )
	{
		pFunction->GetAllocator()->Delete<CHE_PDF_Function>( pFunction );
	}
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


CHE_PDF_Function::CHE_PDF_Function( CHE_PDF_DictionaryPtr dict, CHE_Allocator * pAllocator )
	: CHE_Object(pAllocator), mType(FUNCTION_TYPE_SAMPLE), mInputCount(0), mOutputCount(0),
	mpDomain(NULL), mpRange(NULL), mbRange(FALSE)
{
	CHE_PDF_ObjectPtr objPtr = dict->GetElement( "FunctionType", OBJ_TYPE_NUMBER );
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
 		}
	}

	objPtr = dict->GetElement( "Domain", OBJ_TYPE_ARRAY );
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
	}

	objPtr = dict->GetElement( "Range", OBJ_TYPE_ARRAY );
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
	}
}

CHE_PDF_Function_Sampled::CHE_PDF_Function_Sampled( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( stmPtr->GetDictPtr(), pAllocator ), mBps(1), mOrder(1), mpSize(NULL), mpEncode(NULL), mpDecode(NULL), mpSample(NULL)
{
	CHE_PDF_DictionaryPtr dict = stmPtr->GetDictPtr();
	CHE_PDF_ObjectPtr objPtr = dict->GetElement( "BitsPerSample", OBJ_TYPE_NUMBER );
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

CHE_PDF_Function_Exponential::CHE_PDF_Function_Exponential( CHE_PDF_DictionaryPtr dictPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( dictPtr, pAllocator )
{
	CHE_PDF_ObjectPtr objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
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

CHE_PDF_Function_Stitching::CHE_PDF_Function_Stitching( CHE_PDF_DictionaryPtr dictPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( dictPtr, pAllocator ), mK(0), mpBounds(NULL), mpEncode(NULL)
{
	//todo
}

CHE_PDF_Function_Stitching::~CHE_PDF_Function_Stitching()
{
	//todo
}

CHE_PDF_Function_PostScript::CHE_PDF_Function_PostScript( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator )
	: CHE_PDF_Function( stmPtr->GetDictPtr(), pAllocator )
{
	CHE_PDF_StreamAcc stmAcc;
	if ( stmAcc.Attach( stmPtr ) )
	{
		IHE_Read *  pRead = HE_CreateMemBufRead( stmAcc.GetData(), stmAcc.GetSize(), pAllocator );
		CHE_PDF_SyntaxParser syntaxParser( NULL, pAllocator );
		CHE_PDF_ParseWordDes wordDes( pAllocator );
		if ( syntaxParser.InitParser( pRead ) )
		{
			while ( syntaxParser.GetWord( wordDes ) )
			{
				switch ( wordDes.type )
				{
				case PARSE_WORD_INTEGER:
					wordDes.str.GetInteger();
					break;
				case PARSE_WORD_FLOAT:
					wordDes.str.GetFloat();
				case PARSE_WORD_UNKNOWN:
					{
						switch ( StringToDWORD( wordDes.str ) )
						{
						case C( 'a', 'b', 's' ):
						case C( 'a', 'd', 'd' ):
						case D( 'a', 't', 'a', 'n' ):
						case D( 'c', 'e', 'i', 'l' ): //ceiling
						case C( 'c', 'o', 's' ):
						case C( 'c', 'v', 'i' ):
						case C( 'd', 'i', 'v' ):
						case C( 'e', 'x', 'p' ):
						case D( 'f', 'l', 'o', 'o' ): //floor
						case D( 'i', 'd', 'i', 'v' ):
						case B( 'l', 'n' ):
						case C( 'l', 'o', 'g' ):
						case C( 'm', 'o', 'd' ):
						case C( 'm', 'u', 'l' ):
						case C( 'n', 'e', 'g' ):
						case D( 'r', 'o', 'n', 'n' ): //round
						case C( 's', 'i', 'n' ):
						case D( 's', 'q', 'r', 't' ):
						case C( 's', 'u', 'b' ):
						case D( 't', 'r', 'u', 'n' ): //truncate
						case C( 'a', 'n', 'd' ):
						case D( 'b', 'i', 't', 's' ): //bitshift
						case B( 'e', 'q' ):
						case D( 'f', 'a', 'l', 's' ): //false
						case B( 'g', 'e' ):
						case B( 'g', 't' ):
						case B( 'l', 'e' ):
						case B( 'l', 't' ):
						case B( 'n', 'e' ):
						case C( 'n', 'o', 't' ):
						case B( 'o', 'r' ):
						case D( 't', 'r', 'u', 'e' ):
						case C( 'x', 'o', 'r' ):
						case B( 'i', 'f' ):
						case D( 'i', 'f', 'e', 'l' ): //ifelse
						case D( 'c', 'o', 'p', 'y' ):
						case C( 'd', 'u', 'p' ):
						case D( 'e', 'x', 'c', 'h' ):
						case D( 'i', 'n', 'd', 'e' ): //index
						case C( 'p', 'o', 'p' ):
						case D( 'r', 'o', 'l', 'l' ):
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
	}
}

CHE_PDF_Function_PostScript::~CHE_PDF_Function_PostScript()
{
}


