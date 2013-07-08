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
	if ( color.mConponents.size() >= 3 )
	{
		HE_FLOAT lab[3];
		HE_FLOAT range[6] = { -100, 100, -100, 100, -100, 100 };
		for ( HE_BYTE i = 0 ; i < 3; ++i )
		{
			lab[i] = color.mConponents[i];
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

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mType(type), mResName(pAllocator), mpBaseColorspace(NULL), mIndexCount(0), 
	mpIndexTable(NULL), mIndexTableSize(0)
{
}

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type, const CHE_ByteString & resName, 
	const CHE_PDF_ObjectPtr & pObj, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mType(type), mResName(resName), mpObj(pObj), mpBaseColorspace(NULL), 
	mIndexCount(0), mpIndexTable(NULL), mIndexTableSize(0)
{
	if ( type == COLORSPACE_SPECIAL_INDEXED )
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
				mpBaseColorspace = GetColorSpace( objPtr->GetArrayPtr(), GetAllocator() );
			}else{ 
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NAME );
				if ( objPtr )
				{
					mpBaseColorspace = GetColorSpace( objPtr->GetNamePtr()->GetString(), GetAllocator() );
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

HE_BYTE CHE_PDF_ColorSpace::GetComponentCount() const
{
	switch ( mType )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			return 1;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
		{
			return 3;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
		{
			return 4;
		}
	case COLORSPACE_CIEBASE_CALLAB:
		{
			return 3;
		}
	case COLORSPACE_CIEBASE_ICCBASED:
		{
			if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
				if ( arrayPtr->GetCount() < 2 )
				{
					break;
				}
				CHE_PDF_ObjectPtr objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_STREAM );
				if ( !objPtr )
				{
					break;
				}
				CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
				CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
				if ( !dictPtr )
				{
					break;
				}
				objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
				if ( !objPtr ) {
					break;
				}
				return objPtr->GetNumberPtr()->GetInteger();
			}
			break;
		}
	case COLORSPACE_SPECIAL_INDEXED:
		{
			if ( mpBaseColorspace )
			{
				return mpBaseColorspace->GetComponentCount();
			}
			break;
		}
	case COLORSPACE_SPECIAL_PATTERN:
	case COLORSPACE_SPECIAL_SEPARATION:
	case COLORSPACE_SPECIAL_DEVICEN:
	default:
		break;
	}
	return 0;
}

HE_ARGB CHE_PDF_ColorSpace::GetARGBValue( CHE_PDF_Color & color ) const
{
	if ( color.mConponents.size() == 0 )
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
			if ( color.mConponents.size() >= 1 )
			{
				tmpByte = color.mConponents[0] * 255;
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
			if ( color.mConponents.size() >= 3 )
			{
				tmpByte1 = color.mConponents[0] * 255;
				tmpByte2 = color.mConponents[1] * 255;
				tmpByte3 = color.mConponents[2] * 255;
				valRet = 0xFF000000 + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
		{
			if ( color.mConponents.size() >= 4 )
			{
				HE_FLOAT c = color.mConponents[0], m = color.mConponents[1], y = color.mConponents[2], k = color.mConponents[3];
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
// 				HE_FLOAT bgr[3];
// 				bgr[0] = ( 1 < color.mConponents[2] + color.mConponents[3] ) ? color.mConponents[2] + color.mConponents[3] : 1;
// 				bgr[1] = ( 1 < color.mConponents[1] + color.mConponents[3] ) ? color.mConponents[1] + color.mConponents[3] : 1;
// 				bgr[2] = ( 1 < color.mConponents[0] + color.mConponents[3] ) ? color.mConponents[0] + color.mConponents[3] : 1;
// 				
// 				HE_UINT32 valRet = 0xFF000000;
// 				HE_UINT32 tmpByte1 = bgr[0] * 255;
// 				HE_UINT32 tmpByte2 = bgr[1] * 255;
// 				HE_UINT32 tmpByte3 = bgr[2] * 255;
//              valRet = valRet | tmpByte1 << 16 | tmpByte2 << 8 | tmpByte3;
//				return valRet;
			}
		}
    case COLORSPACE_CIEBASE_CALLAB:
        {
            return lab_to_rgb( color );
		}
    case COLORSPACE_CIEBASE_ICCBASED:
        {
            if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
            {
                CHE_PDF_ArrayPtr arrayPtr = mpObj->GetArrayPtr();
                if ( arrayPtr->GetCount() < 2 )
                {
                    break;
                }
                CHE_PDF_ObjectPtr objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_STREAM );
                if ( !objPtr )
                {
                    break;
                }
                CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
                CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
                if ( !dictPtr )
                {
                    break;
                }
                objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
                if ( !objPtr ) {
                    break;
                }
                switch ( objPtr->GetNumberPtr()->GetInteger() )
                {
                case 1:
                    {
                        HE_UINT32 valRet = 0xFF000000;
                        HE_BYTE tmpByte = 0x00;
                        if ( color.mConponents.size() >= 1 )
                        {
                            tmpByte = color.mConponents[0] * 255;
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
                        if ( color.mConponents.size() >= 3 )
                        {
                            tmpByte1 = color.mConponents[0] * 255;
                            tmpByte2 = color.mConponents[1] * 255;
                            tmpByte3 = color.mConponents[2] * 255;
                            valRet = 0xFF000000 + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
                        }
                        return valRet;
                    }
                case 4:
                    {
                        if ( color.mConponents.size() >= 4 )
                        {
							HE_FLOAT c = color.mConponents[0], m = color.mConponents[1], y = color.mConponents[2], k = color.mConponents[3];
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
//                             HE_FLOAT bgr[3];
//                             bgr[0] = ( 1 < color.mConponents[2] + color.mConponents[3] ) ? color.mConponents[2] + color.mConponents[3] : 1;
//                             bgr[1] = ( 1 < color.mConponents[1] + color.mConponents[3] ) ? color.mConponents[1] + color.mConponents[3] : 1;
//                             bgr[2] = ( 1 < color.mConponents[0] + color.mConponents[3] ) ? color.mConponents[0] + color.mConponents[3] : 1;
//                             
//                             HE_UINT32 valRet = 0xFF000000;
//                             HE_UINT32 tmpByte1 = bgr[0] * 255;
//                             HE_UINT32 tmpByte2 = bgr[1] * 255;
//                             HE_UINT32 tmpByte3 = bgr[2] * 255;
//                             valRet = valRet | tmpByte1 << 16 | tmpByte2 << 8 | tmpByte3;
//                             return valRet;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        }
    case COLORSPACE_SPECIAL_INDEXED:
        {
			if ( mpBaseColorspace  )
			{
				HE_BYTE index = (HE_BYTE)(color.mConponents[0]);
				if ( mpIndexTable && ( index * mpBaseColorspace->GetComponentCount() ) < mIndexTableSize )
				{
					HE_BYTE component = 0;
					CHE_PDF_Color newColor;
					HE_LPBYTE p = mpIndexTable + ( index * mpBaseColorspace->GetComponentCount() );
					for ( HE_ULONG i = 0; i < mpBaseColorspace->GetComponentCount(); ++i )
					{
						component = *(p+i);
						newColor.mConponents.push_back( component / 255.0f );
					}
					return mpBaseColorspace->GetARGBValue( newColor );
				}
			}
            break;
        }
    case COLORSPACE_SPECIAL_PATTERN:
	case COLORSPACE_SPECIAL_SEPARATION:
	case COLORSPACE_SPECIAL_DEVICEN:
	default:
        break;
	}
    return 0xFF000000;
}

CHE_PDF_ColorSpace * GetColorSpace( const CHE_ByteString & name, CHE_Allocator * pAllocator )
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

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_ObjectPtr & ObjPtr, CHE_Allocator * pAllocator )
{
	if ( ! ObjPtr )
	{
		return NULL;
	}
	switch ( ObjPtr->GetType() )
	{
	case OBJ_TYPE_NAME:
		return GetColorSpace( ObjPtr->GetNamePtr(), pAllocator );
	case OBJ_TYPE_ARRAY:
		return GetColorSpace( ObjPtr->GetArrayPtr(), pAllocator );
	case OBJ_TYPE_REFERENCE:
		return GetColorSpace( ObjPtr->GetRefPtr(), pAllocator );
	default:;
	}

	return NULL;
}

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_NamePtr & pName, CHE_Allocator * pAllocator )
{
	if ( !pName )
	{
		return NULL;
	}

	return GetColorSpace( pName->GetStringPtr(), pAllocator );
}

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_ArrayPtr & pArray, CHE_Allocator * pAllocator )
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
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALGRAY, "", pArray );
	}else if ( name == "CalRGB" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALRGB, "", pArray );
	}else if ( name == "CalCMYK" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALCMYK, "", pArray );
	}else if ( name == "ICCBased" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_ICCBASED, "", pArray );
	}else if ( name == "Lab" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_CIEBASE_CALLAB, "", pArray );
	}else if ( name == "Indexed" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_INDEXED, "", pArray );
	}else if ( name == "Separation" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_SEPARATION, "", pArray );
	}else if ( name == "DeviceN" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_DEVICEN, "", pArray );
	}
	return pColorSpace;
}

CHE_PDF_ColorSpace * GetColorSpace( const CHE_PDF_ReferencePtr & pRef, CHE_Allocator * pAllocator )
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
			return GetColorSpace( pObj->GetNamePtr() );
		}
	case OBJ_TYPE_ARRAY:
		{
			return GetColorSpace( pObj->GetArrayPtr() );
		}
	default:break;
	}
	return NULL;
}

CHE_ByteString GetColorSpaceName( CHE_PDF_ColorSpace * pColorSpace )
{
	if ( pColorSpace == NULL )
	{
		return "";
	}

	switch ( pColorSpace->GetType() )
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