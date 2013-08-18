#include "../../include/pdf/che_pdf_colorspace.h"
#include <math.h>

static inline float fung(float x)
{
    if (x >= 6.0f / 29.0f)
        return x * x * x;
    return (108.0f / 841.0f) * (x - (4.0f / 29.0f));
}

static inline float fz_clamp(float f, float min , float max)
{
    return (f > min ? ( f < max ? f : max) : min );
}

HE_ARGB CHE_PDF_ColorSpace::lab_to_rgb( CHE_PDF_Color & color ) const
{
	HE_ARGB valRet = 0xFF000000;
	if ( color.GetComponentCount() >= 3 )
	{
		HE_FLOAT lab[3];
		HE_FLOAT range[6] = { -100, 100, -100, 100, -100, 100 };
		for ( HE_BYTE i = 0 ; i < 3; ++i )
		{
			lab[i] = color.GetComponent( i );
		}

		if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
			CHE_PDF_ObjectPtr objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
				objPtr = dictPtr->GetElement( "Range", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					arrayPtr = objPtr->GetArrayPtr();
					if ( arrayPtr->GetCount() >= 4 )
					{
						for ( HE_ULONG i = 0 ; i < 4; ++i )
						{
							objPtr = arrayPtr->GetElement( i+2, OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								range[i+2] = objPtr->GetNumberPtr()->GetInteger();
							}
						}
					}
				}
			}
		}

		lab[0] = lab[0] * ( range[1] - range[0]) + fabs( range[0] );
		lab[1] = lab[1] * ( range[3] - range[2]) + fabs( range[2] );
		lab[2] = lab[2] * ( range[5] - range[4]) + fabs( range[4] );

		HE_FLOAT lstar , astar, bstar, l , m, n, x , y, z, r , g, b;
		lstar = lab [0];
		astar = lab [1];
		bstar = lab [2];
		m = (lstar + 16) / 116;
		l = m + astar / 500;
		n = m - bstar / 200;
		x = fung (l);
		y = fung (m);
		z = fung (n);
		r = (3.240449f * x + -1.537136f * y + -0.498531f * z) * 0.830026f;
		g = (-0.969265f * x + 1.876011f * y + 0.041556f * z) * 1.05452f;
		b = (0.055643f * x + -0.204026f * y + 1.057229f * z) * 1.1003f;

		
		HE_BYTE tmpByte1 = sqrtf (fz_clamp( r, 0, 1)) * 255;
		HE_BYTE tmpByte2 = sqrtf (fz_clamp( g, 0, 1)) * 255;
		HE_BYTE tmpByte3 = sqrtf (fz_clamp( b, 0, 1)) * 255;
		valRet = valRet + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
	}
	return valRet;
}

CHE_PDF_ColorSpace * CHE_PDF_ColorSpace::Create( const CHE_ByteString & name, CHE_Allocator * pAllocator )
{
	CHE_PDF_ColorSpace * pColorSpace = NULL;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	if ( name == "DeviceGray" || name == "G" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_DEVICE_GRAY, pAllocator );
	}else if ( name == "DeviceRGB" || name == "RGB" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_DEVICE_RGB, pAllocator );
	}else if ( name == "DeviceCMYK" || name == "CMYK"  )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_DEVICE_CMYK, pAllocator );
	}else if ( name == "Pattern" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_PATTERN, pAllocator );
	}
	return pColorSpace;
}

CHE_PDF_ColorSpace * CHE_PDF_ColorSpace::Create( const CHE_PDF_ObjectPtr & ObjPtr, CHE_Allocator * pAllocator )
{
	if ( ! ObjPtr )
	{
		return NULL;
	}
	switch ( ObjPtr->GetType() )
	{
	case OBJ_TYPE_NAME:
		return CHE_PDF_ColorSpace::Create( ObjPtr->GetNamePtr(), pAllocator );
	case OBJ_TYPE_ARRAY:
		return CHE_PDF_ColorSpace::Create( ObjPtr->GetArrayPtr(), pAllocator );
	case OBJ_TYPE_REFERENCE:
		return CHE_PDF_ColorSpace::Create( ObjPtr->GetRefPtr(), pAllocator );
	default:;
	}

	return NULL;
}

CHE_PDF_ColorSpace * CHE_PDF_ColorSpace::Create( const CHE_PDF_NamePtr & pName, CHE_Allocator * pAllocator )
{
	if ( !pName )
	{
		return NULL;
	}
	return CHE_PDF_ColorSpace::Create( pName->GetString(), pAllocator );
}

CHE_PDF_ColorSpace * CHE_PDF_ColorSpace::Create( const CHE_PDF_ArrayPtr & pArray, CHE_Allocator * pAllocator )
{
	if ( !pArray )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	if ( pArray->GetCount() == 0 )
	{
		return NULL;
	}
	CHE_PDF_ObjectPtr pObj = pArray->GetElement( 0, OBJ_TYPE_NAME );
	if ( !pObj || pObj->GetType() != OBJ_TYPE_NAME )
	{
		return NULL;
	}
	CHE_ByteString name = pObj->GetNamePtr()->GetString();
	CHE_PDF_ColorSpace * pColorSpace = NULL;
	if ( name == "CalGray" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALGRAY, "", pArray, pAllocator );
	}else if ( name == "CalRGB" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALRGB, "", pArray, pAllocator );
	}else if ( name == "CalCMYK" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALCMYK, "", pArray, pAllocator );
	}else if ( name == "ICCBased" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_ICCBASED, "", pArray, pAllocator );
	}else if ( name == "Lab" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALLAB, "", pArray, pAllocator );
	}else if ( name == "Indexed" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_INDEXED, "", pArray, pAllocator );
	}else if ( name == "Separation" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_SEPARATION, "", pArray, pAllocator );
	}else if ( name == "DeviceN" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_DEVICEN, "", pArray, pAllocator );
	}
	return pColorSpace;
}

CHE_PDF_ColorSpace * CHE_PDF_ColorSpace::Create( const CHE_PDF_ReferencePtr & pRef, CHE_Allocator * pAllocator )
{
	if ( !pRef )
	{
		return NULL;
	}
	CHE_PDF_ObjectPtr pObj = pRef->GetRefObj();
	if ( !pObj )
	{
		return NULL;
	}
	switch ( pObj->GetType() )
	{
	case OBJ_TYPE_NAME:
		{
			return CHE_PDF_ColorSpace::Create( pObj->GetNamePtr(), pAllocator );
		}
	case OBJ_TYPE_ARRAY:
		{
			return CHE_PDF_ColorSpace::Create( pObj->GetArrayPtr(), pAllocator );
		}
	default:break;
	}
	return NULL;
}

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mType(type), mResName(pAllocator), mComponentCount(0), mpBaseColorspace(NULL),
	mIndexCount(0), mpIndexTable(NULL), mIndexTableSize(0), mName(pAllocator), mpFunction(NULL)
{
	switch ( mType )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			mComponentCount = 1;
			break;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
	case COLORSPACE_CIEBASE_CALLAB:
		{
			mComponentCount = 3;
			break;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
		{
			mComponentCount = 4;
			break;
		}
	default:;
	}
}

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, const CHE_ByteString & resName, 
	const CHE_PDF_ObjectPtr & pObj, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mType(type), mResName(resName), mComponentCount(0), mpObj(pObj), mpBaseColorspace(NULL), 
	mIndexCount(0), mpIndexTable(NULL), mIndexTableSize(0), mName(pAllocator), mpFunction(NULL)
{
	switch ( mType )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			mComponentCount = 1;
			break;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
	case COLORSPACE_CIEBASE_CALLAB:
		{
			mComponentCount = 3;
			break;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
		{
			mComponentCount = 4;
			break;
		}
	case COLORSPACE_CIEBASE_ICCBASED:
		{
			if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
				if ( arrayPtr->GetCount() >= 2 )
				{
					CHE_PDF_ObjectPtr objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_STREAM );
					if ( objPtr )
					{
						CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
						CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
						if ( dictPtr )
						{
							objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								mComponentCount = objPtr->GetNumberPtr()->GetInteger();;
							}
						}
					}
				}
			}
			break;
		}
	case COLORSPACE_SPECIAL_INDEXED:
		{
			CHE_PDF_ObjectPtr objPtr;
			CHE_PDF_ArrayPtr arrayPtr;
			if ( pObj )
			{
				if ( pObj->GetType() == OBJ_TYPE_ARRAY )
				{
					arrayPtr = pObj->GetArrayPtr();
				}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					objPtr = pObj->GetRefPtr()->GetRefObj( OBJ_TYPE_ARRAY );
					if ( objPtr )
					{
						arrayPtr = objPtr->GetArrayPtr();
					}
				}
			}
			if ( arrayPtr && arrayPtr->GetCount() >= 4 )
			{
				objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NAME );
				if ( objPtr )
				{
					if ( objPtr->GetNamePtr()->GetString() != "Indexed" )
					{
						return;
					}
				}
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					mpBaseColorspace = CHE_PDF_ColorSpace::Create( objPtr->GetArrayPtr(), GetAllocator() );
				}else{ 
					objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NAME );
					if ( objPtr )
					{
						mpBaseColorspace = CHE_PDF_ColorSpace::Create( objPtr->GetNamePtr()->GetString(), GetAllocator() );
					}
				}
				if ( mpBaseColorspace == NULL )
				{
					return;
				}
				objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mIndexCount = objPtr->GetNumberPtr()->GetInteger();
				}else{
					//todo clear colorspace
					return;
				}
				objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_STREAM );
				if ( objPtr )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( objPtr->GetStreamPtr() ) )
					{
						mpIndexTable = GetAllocator()->NewArray<HE_BYTE>( stmAcc.GetSize() );
						memcpy( mpIndexTable, stmAcc.GetData(), stmAcc.GetSize() );
						mIndexTableSize = stmAcc.GetSize();
						stmAcc.Detach();
					}
				}else{
					objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_STRING );
					CHE_ByteString str = objPtr->GetStringPtr()->GetString();
					mpIndexTable = GetAllocator()->NewArray<HE_BYTE>( str.GetLength() );
					memcpy( mpIndexTable, str.GetData(), str.GetLength() );
					mIndexTableSize = str.GetLength();
				}
			}
			break;
		}
	case COLORSPACE_SPECIAL_PATTERN:
		{
			mComponentCount = 0;
			break;
		}
	case COLORSPACE_SPECIAL_SEPARATION:
		{
			mComponentCount = 1;

			CHE_PDF_ObjectPtr objPtr;
			CHE_PDF_ArrayPtr arrayPtr;
			if ( pObj )
			{
				if ( pObj->GetType() == OBJ_TYPE_ARRAY )
				{
					arrayPtr = pObj->GetArrayPtr();
				}else if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
				{
					objPtr = pObj->GetRefPtr()->GetRefObj( OBJ_TYPE_ARRAY );
					if ( objPtr )
					{
						arrayPtr = objPtr->GetArrayPtr();
					}
				}
			}
			if ( arrayPtr && arrayPtr->GetCount() >= 4 )
			{
				objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NAME );
				if ( objPtr )
				{
					if ( objPtr->GetNamePtr()->GetString() != "Separation" )
					{
						return;
					}
				}
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NAME );
				if ( objPtr )
				{
					mName = objPtr->GetNamePtr()->GetString();
				}else{
					return;
				}
				objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					mpBaseColorspace = CHE_PDF_ColorSpace::Create( objPtr->GetArrayPtr(), GetAllocator() );
				}else{ 
					objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NAME );
					if ( objPtr )
					{
						mpBaseColorspace = CHE_PDF_ColorSpace::Create( objPtr->GetNamePtr()->GetString(), GetAllocator() );
					}
				}
				if ( mpBaseColorspace == NULL )
				{
					return;
				}

				objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_DICTIONARY );
				if ( objPtr )
				{
					mpFunction = CHE_PDF_Function::Create( objPtr->GetDictPtr(), GetAllocator() );
				}else{
					objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_REFERENCE );
					if ( objPtr )
					{
						mpFunction = CHE_PDF_Function::Create( objPtr->GetRefPtr(), GetAllocator() );
					}
				}
			}
			break;
		}
	case COLORSPACE_SPECIAL_DEVICEN:
		{
			mComponentCount = 0;
			break;
		}
	default:;
	}
}

CHE_PDF_ColorSpace::~CHE_PDF_ColorSpace()
{
	if ( mpBaseColorspace )
	{
		mpBaseColorspace->GetAllocator()->Delete( mpBaseColorspace );
		mpBaseColorspace = NULL;
	}
	if ( mpIndexTable )
	{
		GetAllocator()->Delete( mpIndexTable );
		mpIndexTable = NULL;
	}
// 	if ( mpFunction )
// 	{
// 		GetAllocator()->Delete( mpFunction );
// 	}
}

CHE_PDF_ColorSpace * CHE_PDF_ColorSpace::Clone() const
{
	CHE_PDF_ColorSpace * pRet = GetAllocator()->New<CHE_PDF_ColorSpace>( mType, GetAllocator() );
	pRet->mResName = mResName;
	if ( mpObj )
	{
		pRet->mpObj = mpObj->Clone();
	}

	if ( mpBaseColorspace )
	{
		pRet->mpBaseColorspace = mpBaseColorspace->Clone();
		pRet->mIndexCount = mIndexTableSize;
	}
	if ( mpIndexTable && mIndexTableSize )
	{
		pRet->mpIndexTable = pRet->GetAllocator()->NewArray<HE_BYTE>( mIndexTableSize );
		memcpy( pRet->mpIndexTable, mpIndexTable, mIndexTableSize );
		pRet->mIndexTableSize = mIndexTableSize;
	}
	if ( mpFunction )
	{
		//todo
		pRet->mpFunction = mpFunction;
	}
	pRet->mComponentCount = mComponentCount;

	return pRet;
}

HE_BOOL CHE_PDF_ColorSpace::IsDeviceColorSpace() const
{
	switch ( GetType() )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_DEVICE_CMYK:
		return TRUE;
	default:
		break;
	}

	return FALSE;;
}

CHE_ByteString CHE_PDF_ColorSpace::GetName() const
{
	switch ( mType )
	{
	case COLORSPACE_DEVICE_GRAY:
		return "DeviceGray";
	case COLORSPACE_DEVICE_RGB:
		return "DeviceRGB";
	case COLORSPACE_DEVICE_CMYK:
		return "DeviceCMYK";
	case COLORSPACE_CIEBASE_CALGRAY:
		return "CalGray";
	case COLORSPACE_CIEBASE_CALRGB:
		return "CalRGB";
	case COLORSPACE_CIEBASE_CALCMYK:
		return "CalCMYK";
	case COLORSPACE_CIEBASE_CALLAB:
		return "Lab";
	case COLORSPACE_CIEBASE_ICCBASED:
		return "ICCBased";
	case COLORSPACE_SPECIAL_PATTERN:
		return "Pattern";
	case COLORSPACE_SPECIAL_INDEXED:
		return "Indexed";
	case COLORSPACE_SPECIAL_SEPARATION:
		return "Separation";
	case COLORSPACE_SPECIAL_DEVICEN:
		return "DeviceN";
	default:
		break;
	}
	return "";
}

HE_BYTE CHE_PDF_ColorSpace::GetComponentCount() const
{
	return mComponentCount;
}

HE_ARGB CHE_PDF_ColorSpace::GetARGBValue( CHE_PDF_Color & color ) const
{
	if ( color.GetComponentCount() == 0 )
	{
		return 0xFF00000;
	}

	switch ( mType )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			HE_ARGB valRet = 0xFF000000;
			HE_BYTE tmpByte = 0x00;
			if ( color.GetComponentCount() >= 1 )
			{
				tmpByte = color.GetComponent( 0 ) * 255;
				valRet = 0xFF000000 + (tmpByte << 16) + (tmpByte << 8) + tmpByte;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
		{
			HE_UINT32 valRet = 0xFF000000;
			HE_BYTE tmpByte1 = 0x00;
			HE_BYTE tmpByte2 = 0x00;
			HE_BYTE tmpByte3 = 0x00;
			if ( color.GetComponentCount() >= 3 )
			{
				tmpByte1 = color.GetComponent( 0 ) * 255;
				tmpByte2 = color.GetComponent( 1 ) * 255;
				tmpByte3 = color.GetComponent( 2 ) * 255;
				valRet = 0xFF000000 + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
		{
			if ( color.GetComponentCount() >= 4 )
			{
				HE_FLOAT c = color.GetComponent( 0 ), m = color.GetComponent( 1 ), y = color.GetComponent( 2 ), k = color.GetComponent( 3 );
				HE_FLOAT c1 = 1 - c, m1 = 1 - m, y1 = 1 - y, k1 = 1 - k;
				HE_FLOAT r, g, b, x;
				HE_FLOAT rgb[3];
				HE_BYTE br = 0, bg = 0, bb = 0;
				HE_ARGB valRet = 0;

				/* this is a matrix multiplication, unrolled for performance */
				x = c1 * m1 * y1 * k1;	/* 0 0 0 0 */
				r = g = b = x;
				x = c1 * m1 * y1 * k;	/* 0 0 0 1 */
				r += 0.1373 * x;
				g += 0.1216 * x;
				b += 0.1255 * x;
				x = c1 * m1 * y * k1;	/* 0 0 1 0 */
				r += x;
				g += 0.9490 * x;
				x = c1 * m1 * y * k;	/* 0 0 1 1 */
				r += 0.1098 * x;
				g += 0.1020 * x;
				x = c1 * m * y1 * k1;	/* 0 1 0 0 */
				r += 0.9255 * x;
				b += 0.5490 * x;
				x = c1 * m * y1 * k;	/* 0 1 0 1 */
				r += 0.1412 * x;
				x = c1 * m * y * k1;	/* 0 1 1 0 */
				r += 0.9294 * x;
				g += 0.1098 * x;
				b += 0.1412 * x;
				x = c1 * m * y * k;	/* 0 1 1 1 */
				r += 0.1333 * x;
				x = c * m1 * y1 * k1;	/* 1 0 0 0 */
				g += 0.6784 * x;
				b += 0.9373 * x;
				x = c * m1 * y1 * k;	/* 1 0 0 1 */
				g += 0.0588 * x;
				b += 0.1412 * x;
				x = c * m1 * y * k1;	/* 1 0 1 0 */
				g += 0.6510 * x;
				b += 0.3137 * x;
				x = c * m1 * y * k;	/* 1 0 1 1 */
				g += 0.0745 * x;
				x = c * m * y1 * k1;	/* 1 1 0 0 */
				r += 0.1804 * x;
				g += 0.1922 * x;
				b += 0.5725 * x;
				x = c * m * y1 * k;	/* 1 1 0 1 */
				b += 0.0078 * x;
				x = c * m * y * k1;	/* 1 1 1 0 */
				r += 0.2118 * x;
				g += 0.2119 * x;
				b += 0.2235 * x;

				rgb[0] = fz_clamp(r, 0, 1);
				rgb[1] = fz_clamp(g, 0, 1);
				rgb[2] = fz_clamp(b, 0, 1);

				br = rgb[0] * 255;
				bg = rgb[1] * 255;
				bb = rgb[2] * 255;

				valRet = 0xFF000000 + ( br << 16 ) + ( bg << 8 ) + ( bb );
				return valRet;
			}
		}
    case COLORSPACE_CIEBASE_CALLAB:
        {
            return lab_to_rgb( color );
		}
    case COLORSPACE_CIEBASE_ICCBASED:
        {
			switch ( mComponentCount )
            {
            case 1:
                {
                    HE_UINT32 valRet = 0xFF000000;
                    HE_BYTE tmpByte = 0x00;
                    if ( color.GetComponentCount() >= 1 )
                    {
                        tmpByte = color.GetComponent( 0 ) * 255;
                        valRet = 0xFF000000 + (tmpByte << 16) + (tmpByte << 8) + tmpByte;
                    }
                    return valRet;
                }
            case 3:
                {
                    HE_UINT32 valRet = 0xFF000000;
                    HE_BYTE tmpByte1 = 0x00;
                    HE_BYTE tmpByte2 = 0x00;
                    HE_BYTE tmpByte3 = 0x00;
                    if ( color.GetComponentCount() >= 3 )
                    {
                        tmpByte1 = color.GetComponent( 0 ) * 255;
                        tmpByte2 = color.GetComponent( 1 ) * 255;
                        tmpByte3 = color.GetComponent( 2 ) * 255;
                        valRet = 0xFF000000 + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
                    }
                    return valRet;
                }
            case 4:
                {
                    if ( color.GetComponentCount() >= 4 )
                    {
						HE_FLOAT c = color.GetComponent(0), m = color.GetComponent(1), y = color.GetComponent(2), k = color.GetComponent(3);
						HE_FLOAT c1 = 1 - c, m1 = 1 - m, y1 = 1 - y, k1 = 1 - k;
						HE_FLOAT r, g, b, x;
						HE_FLOAT rgb[3];
						HE_BYTE br = 0, bg = 0, bb = 0;
						HE_ARGB valRet = 0;

						/* this is a matrix multiplication, unrolled for performance */
						x = c1 * m1 * y1 * k1;	/* 0 0 0 0 */
						r = g = b = x;
						x = c1 * m1 * y1 * k;	/* 0 0 0 1 */
						r += 0.1373 * x;
						g += 0.1216 * x;
						b += 0.1255 * x;
						x = c1 * m1 * y * k1;	/* 0 0 1 0 */
						r += x;
						g += 0.9490 * x;
						x = c1 * m1 * y * k;	/* 0 0 1 1 */
						r += 0.1098 * x;
						g += 0.1020 * x;
						x = c1 * m * y1 * k1;	/* 0 1 0 0 */
						r += 0.9255 * x;
						b += 0.5490 * x;
						x = c1 * m * y1 * k;	/* 0 1 0 1 */
						r += 0.1412 * x;
						x = c1 * m * y * k1;	/* 0 1 1 0 */
						r += 0.9294 * x;
						g += 0.1098 * x;
						b += 0.1412 * x;
						x = c1 * m * y * k;	/* 0 1 1 1 */
						r += 0.1333 * x;
						x = c * m1 * y1 * k1;	/* 1 0 0 0 */
						g += 0.6784 * x;
						b += 0.9373 * x;
						x = c * m1 * y1 * k;	/* 1 0 0 1 */
						g += 0.0588 * x;
						b += 0.1412 * x;
						x = c * m1 * y * k1;	/* 1 0 1 0 */
						g += 0.6510 * x;
						b += 0.3137 * x;
						x = c * m1 * y * k;	/* 1 0 1 1 */
						g += 0.0745 * x;
						x = c * m * y1 * k1;	/* 1 1 0 0 */
						r += 0.1804 * x;
						g += 0.1922 * x;
						b += 0.5725 * x;
						x = c * m * y1 * k;	/* 1 1 0 1 */
						b += 0.0078 * x;
						x = c * m * y * k1;	/* 1 1 1 0 */
						r += 0.2118 * x;
						g += 0.2119 * x;
						b += 0.2235 * x;

						rgb[0] = fz_clamp(r, 0, 1);
						rgb[1] = fz_clamp(g, 0, 1);
						rgb[2] = fz_clamp(b, 0, 1);

						br = rgb[0] * 255;
						bg = rgb[1] * 255;
						bb = rgb[2] * 255;

						valRet = 0xFF000000 + ( br << 16 ) + ( bg << 8 ) + ( bb );
						return valRet;
                    }
                }
                break;
            default:
                break;
            }
            break;
        }
    case COLORSPACE_SPECIAL_INDEXED:
        {
			if ( mpBaseColorspace  )
			{
				HE_BYTE index = (HE_BYTE)(color.GetComponent(0));
				HE_ULONG componentCount = mpBaseColorspace->GetComponentCount();
				if ( mpIndexTable && ( index * componentCount < mIndexTableSize ) )
				{
					HE_BYTE component = 0;
					CHE_PDF_Color newColor;
					HE_LPBYTE p = mpIndexTable + ( index * componentCount );
					for ( HE_ULONG i = 0; i < componentCount; ++i )
					{
						component = *(p+i);
						newColor.Push( component / 255.0f );
					}
					return mpBaseColorspace->GetARGBValue( newColor );
				}
			}
            break;
        }
	case COLORSPACE_SPECIAL_SEPARATION:
		{
			if ( mpBaseColorspace && mpFunction )
			{
 				CHE_PDF_Color newColor;
				std::vector<HE_FLOAT> input;
				std::vector<HE_FLOAT> output;
				for ( HE_ULONG i = 0; i < color.GetComponentCount(); ++i )
				{
					input.push_back( color.GetComponent( i ) );
				}
				if ( mpFunction->Calculate( input, output ) )
				{
					for ( HE_ULONG j = 0; j < output.size(); ++j )
					{
						newColor.Push( output[j] );
					}
					return mpBaseColorspace->GetARGBValue( newColor );
				}
			}
			break;
		}
	case COLORSPACE_SPECIAL_DEVICEN:
	case COLORSPACE_SPECIAL_PATTERN:
	default:
        break;
	}
    return 0xFF000000;
}