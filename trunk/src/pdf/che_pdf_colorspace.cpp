#include "../../include/pdf/che_pdf_colorspace.h"
#include "../../include/pdf/che_pdf_contentobjs.h"
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

HE_ARGB CHE_PDF_ColorSpace::lab_to_rgb( const CHE_PDF_Color & color ) const
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

		if ( mObjPtr && mObjPtr->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ArrayPtr arrayPtr = mObjPtr->GetArrayPtr();
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

CHE_PDF_ColorSpacePtr gDeviceGrayCSPtr;
CHE_PDF_ColorSpacePtr gDeviceRGBCSPtr;
CHE_PDF_ColorSpacePtr gDeviceCMYKCSPtr;
CHE_PDF_ColorSpacePtr gPatternCSPtr;

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceGray()
{
	if ( !gDeviceGrayCSPtr )
	{
		gDeviceGrayCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>( COLORSPACE_DEVICE_GRAY ) );
	}
	return gDeviceGrayCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceRGB()
{
	if ( !gDeviceRGBCSPtr )
	{
		gDeviceRGBCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>( COLORSPACE_DEVICE_RGB ) );
	}
	return gDeviceRGBCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceCMYK()
{
	if ( !gDeviceCMYKCSPtr )
	{
		gDeviceCMYKCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>( COLORSPACE_DEVICE_CMYK ) );
	}
	return gDeviceCMYKCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreatePattern()
{
	if ( !gPatternCSPtr )
	{
		gPatternCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>( COLORSPACE_SPECIAL_PATTERN ) );
	}
	return gPatternCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create( const CHE_ByteString & name, CHE_Allocator * pAllocator/*= NULL*/ )
{
	CHE_PDF_ColorSpacePtr ptr;
	if ( name == "DeviceGray" || name == "G" )
	{
		return CreateDeviceGray();
	}else if ( name == "DeviceRGB" || name == "RGB" )
	{
		return CreateDeviceRGB();
	}else if ( name == "DeviceCMYK" || name == "CMYK"  )
	{
		return CreateDeviceCMYK();
	}else if ( name == "Pattern" )
	{
		return CreatePattern();
	}
	return ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator/*= NULL*/ )
{
	CHE_PDF_ColorSpacePtr ptr;
	if ( !objPtr )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_PDF_ColorSpace *  pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( objPtr, pAllocator );
	if ( pColorSpace->IsError() )
	{
		pAllocator->Delete( pColorSpace );
		pColorSpace = NULL;
	}else{
		ptr.Reset( pColorSpace );
	}
	return ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Convert( const CHE_PDF_ComponentPtr & componetPtr )
{
	CHE_PDF_ColorSpacePtr ptr;
	if ( componetPtr && componetPtr->GetType() == COMPONENT_TYPE_ColorSpace )
	{
		ptr.Reset( componetPtr.GetPointer() );
	}
	return ptr;
}

// CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create( const CHE_PDF_NamePtr & namePtr, CHE_Allocator * pAllocator/*= NULL*/ )
// {
// 	if ( namePtr )
// 	{
// 		return CHE_PDF_ColorSpace::Create( namePtr->GetString(), pAllocator );
// 	}
// 	CHE_PDF_ColorSpacePtr ptr;
// 	return ptr;
// }


// CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create( const CHE_PDF_ArrayPtr & arrayPtr, CHE_Allocator * pAllocator/*= NULL*/ )
// {
// 	CHE_PDF_ColorSpacePtr ptr;
// 	if ( arrayPtr )
// 	{
// 		return ptr;
// 	}
// 	if ( arrayPtr->GetCount() == 0 )
// 	{
// 		return ptr;
// 	}
// 	if ( pAllocator == NULL )
// 	{
// 		pAllocator = GetDefaultAllocator();
// 	}
// 	CHE_PDF_ObjectPtr objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NAME );
// 	if ( !objPtr || objPtr->GetType() != OBJ_TYPE_NAME )
// 	{
// 		return ptr;
// 	}
// 	CHE_ByteString name = objPtr->GetNamePtr()->GetString();
// 	CHE_PDF_ColorSpace * pColorSpace = NULL;
// 	if ( name == "CalGray" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "CalRGB" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "CalCMYK" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "ICCBased" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "Lab" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "Indexed" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "Separation" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}else if ( name == "DeviceN" )
// 	{
// 		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( arrayPtr, pAllocator );
// 	}
// 	ptr.Reset( pColorSpace );
// 	return ptr;
// }

// CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
// {
// 	CHE_PDF_ColorSpacePtr ptr;
// 	if ( !refPtr )
// 	{
// 		return ptr;
// 	}
// 	CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj();
// 	if ( !objPtr )
// 	{
// 		return ptr;
// 	}
// 	CHE_PDF_ColorSpace * pColorSpace = NULL;
// 	switch ( objPtr->GetType() )
// 	{
// 	case OBJ_TYPE_NAME:
// 		{
// 			pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( objPtr, pAllocator );
// 			break;
// 		}
// 	case OBJ_TYPE_ARRAY:
// 		{
// 			pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( objPtr, pAllocator );
// 			break;
// 		}
// 	default:break;
// 	}
// 	ptr.Reset( pColorSpace );
// 	return ptr;
// }

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace( PDF_COLORSPACE_TYPE type )
	: CHE_PDF_Component(COMPONENT_TYPE_ColorSpace, CHE_PDF_ObjectPtr(), NULL), mColorSpaceType(type),
	mComponentCount(0), mIndexCount(0), mpIndexTable(NULL), mIndexTableSize(0)
{
	mWhitePoint[0] = 1.0f;
	mWhitePoint[1] = 1.0f;
	mWhitePoint[2] = 1.0f;
	mBlackPoint[0] = 0.0f;
	mBlackPoint[1] = 0.0f;
	mBlackPoint[2] = 0.0f;
	mGamma[0] = 1.0f;
	mGamma[1] = 1.0f;
	mGamma[2] = 1.0f;
	mMatrix[0] = 1.0f;
	mMatrix[1] = 0.0f;
	mMatrix[2] = 0.0f;
	mMatrix[3] = 0.0f;
	mMatrix[4] = 1.0f;
	mMatrix[5] = 0.0f;
	mMatrix[6] = 0.0f;
	mMatrix[7] = 0.0f;
	mMatrix[8] = 1.0f;
	mRange[0] = -100;
	mRange[1] = 100;
	mRange[2] = -100;
	mRange[3] = 100;

	switch ( GetColorSpaceType() )
	{
	case COLORSPACE_DEVICE_GRAY:
		{
			mColorSpaceType = COLORSPACE_DEVICE_GRAY;
			mComponentCount = 1;
			break;
		}
	case COLORSPACE_DEVICE_RGB:
		{
			mColorSpaceType = COLORSPACE_DEVICE_RGB;
			mComponentCount = 3;
			break;
		}
	case COLORSPACE_DEVICE_CMYK:
		{
			mColorSpaceType = COLORSPACE_DEVICE_CMYK;
			mComponentCount = 4;
			break;
		}
	case COLORSPACE_SPECIAL_PATTERN:
		{
			mColorSpaceType = COLORSPACE_SPECIAL_PATTERN;
			mComponentCount = 0;
			break;
		}
	default:
		{
			SetError(COMPONENT_ERROR_CONSTRUCTION);
			break;
		}
	}
}

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_Component(COMPONENT_TYPE_ColorSpace, rootObjPtr, pAllocator), mComponentCount(0), mIndexCount(0), 
	mpIndexTable(NULL), mIndexTableSize(0)
{
	CHE_PDF_ObjectPtr objPtr = rootObjPtr;
	if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		objPtr = rootObjPtr->GetRefPtr()->GetRefObj();	
	}

	mWhitePoint[0] = 1.0f;
	mWhitePoint[1] = 1.0f;
	mWhitePoint[2] = 1.0f;
	mBlackPoint[0] = 0.0f;
	mBlackPoint[1] = 0.0f;
	mBlackPoint[2] = 0.0f;
	mGamma[0] = 1.0f;
	mGamma[1] = 1.0f;
	mGamma[2] = 1.0f;
	mMatrix[0] = 1.0f;
	mMatrix[1] = 0.0f;
	mMatrix[2] = 0.0f;
	mMatrix[3] = 0.0f;
	mMatrix[4] = 1.0f;
	mMatrix[5] = 0.0f;
	mMatrix[6] = 0.0f;
	mMatrix[7] = 0.0f;
	mMatrix[8] = 1.0f;
	mRange[0] = -100;
	mRange[1] = 100;
	mRange[2] = -100;
	mRange[3] = 100;

	switch( objPtr->GetType() )
	{
	case OBJ_TYPE_NAME:
		{
			CHE_ByteString name = objPtr->GetNamePtr()->GetString();
			if ( name == "DeviceGray" || name == "G" )
			{
				mColorSpaceType = COLORSPACE_DEVICE_GRAY;
				mComponentCount = 1;
			}else if ( name == "DeviceRGB" || name == "RGB" )
			{
				mColorSpaceType = COLORSPACE_DEVICE_RGB;
				mComponentCount = 3;
			}else if ( name == "DeviceCMYK" || name == "CMYK"  )
			{
				mColorSpaceType = COLORSPACE_DEVICE_CMYK;
				mComponentCount = 4;
			}else if ( name == "Pattern" )
			{
				mColorSpaceType = COLORSPACE_SPECIAL_PATTERN;
				mComponentCount = 0;
			}else{
				SetError(COMPONENT_ERROR_CONSTRUCTION);
			}
			return;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
			if ( arrayPtr->GetCount() < 2 )
			{
				SetError( COMPONENT_ERROR_CONSTRUCTION );
				return;
			}
			objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NAME );
			if ( objPtr )
			{
				CHE_ByteString name = objPtr->GetNamePtr()->GetString();
				if ( name == "CalGray" )
				{
					mColorSpaceType = COLORSPACE_CIEBASE_CALGRAY;
					mComponentCount = 1;
					//todo
				}else if ( name == "CalRGB" )
				{
					mColorSpaceType = COLORSPACE_CIEBASE_CALRGB;
					mComponentCount = 3;
					//todo
				}else if ( name == "CalCMYK" )
				{
					mColorSpaceType = COLORSPACE_CIEBASE_CALCMYK;
					mComponentCount = 4;
					//not to do
				}else if ( name == "ICCBased" )
				{
					//todo
					objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_STREAM );
					if ( objPtr )
					{
						CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
						CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
						if ( dictPtr )
						{
							objPtr = dictPtr->GetElement( "N", OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								mColorSpaceType = COLORSPACE_CIEBASE_ICCBASED;
								mComponentCount = objPtr->GetNumberPtr()->GetInteger();;
							}
						}
					}else{
						SetError( COMPONENT_ERROR_CONSTRUCTION );
					}
				}else if ( name == "Lab" )
				{
					//todo
					mColorSpaceType = COLORSPACE_CIEBASE_CALLAB;
					mComponentCount = 3;
				}else if ( name == "Indexed" )
				{
					mColorSpaceType = COLORSPACE_SPECIAL_INDEXED;
					if ( arrayPtr->GetCount() < 4 )
					{
						SetError( COMPONENT_ERROR_CONSTRUCTION );
					}else{
						objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NAME );
						if ( objPtr )
						{
							if ( objPtr->GetNamePtr()->GetString() != "Indexed" )
							{
								return;
							}
						}
						objPtr = arrayPtr->GetElement( 1 );
						if ( objPtr )
						{
							mBaseColorspace = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
						}
						if ( !mBaseColorspace )
						{
							SetError( COMPONENT_ERROR_CONSTRUCTION );
							return;
						}
						objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
						if ( objPtr )
						{
							mIndexCount = objPtr->GetNumberPtr()->GetInteger();
						}else{
							SetError( COMPONENT_ERROR_CONSTRUCTION );
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
							if ( objPtr )
							{
								CHE_ByteString str = objPtr->GetStringPtr()->GetString();
								mpIndexTable = GetAllocator()->NewArray<HE_BYTE>( str.GetLength() );
								memcpy( mpIndexTable, str.GetData(), str.GetLength() );
								mIndexTableSize = str.GetLength();
							}else{
								SetError( COMPONENT_ERROR_CONSTRUCTION );
								return;
							}
						}
					}
				}else if ( name == "Separation" )
				{
					mColorSpaceType = COLORSPACE_SPECIAL_SEPARATION;
					mComponentCount = 1;
					if (  arrayPtr->GetCount() < 4 )
					{
						SetError( COMPONENT_ERROR_CONSTRUCTION );
						return;
					}else{
// 						objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NAME );
// 						if ( objPtr )
// 						{
// 							mName = objPtr->GetNamePtr()->GetString();
// 						}else{
// 							return;
// 						}
						objPtr = arrayPtr->GetElement( 2 );
						if ( objPtr )
						{
							mBaseColorspace = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
						}else{
							SetError( COMPONENT_ERROR_CONSTRUCTION );
							return;
						}
						if ( !mBaseColorspace )
						{
							SetError( COMPONENT_ERROR_CONSTRUCTION );
							return;
						}
						objPtr = arrayPtr->GetElement( 3 );
						if ( objPtr )
						{
							mFunction = CHE_PDF_Function::Create( objPtr, GetAllocator() );
						}else{
							SetError( COMPONENT_ERROR_CONSTRUCTION );
							return;
						}
						if ( !mFunction )
						{
							SetError( COMPONENT_ERROR_CONSTRUCTION );
							return;
						}
					}
				}else if ( name == "DeviceN" )
				{
					SetError( COMPONENT_ERROR_CONSTRUCTION );
				}
			}else{
				SetError( COMPONENT_ERROR_CONSTRUCTION );
			}
			break;
		}
	default:
		{
			SetError( COMPONENT_ERROR_CONSTRUCTION );
		}
	}
	return;
}

CHE_PDF_ColorSpace::~CHE_PDF_ColorSpace()
{
	if ( mpIndexTable )
	{
		GetAllocator()->Delete( mpIndexTable );
		mpIndexTable = NULL;
		mIndexTableSize = 0;
	}
}

// HE_BOOL CHE_PDF_ColorSpace::IsDeviceColorSpace() const
// {
// 	switch ( GetType() )
// 	{
// 	case COLORSPACE_DEVICE_GRAY:
// 	case COLORSPACE_DEVICE_RGB:
// 	case COLORSPACE_DEVICE_CMYK:
// 		return TRUE;
// 	default:
// 		break;
// 	}
// 
// 	return FALSE;;
// }

// CHE_ByteString CHE_PDF_ColorSpace::GetName() const
// {
// 	switch ( mType )
// 	{
// 	case COLORSPACE_DEVICE_GRAY:
// 		return "DeviceGray";
// 	case COLORSPACE_DEVICE_RGB:
// 		return "DeviceRGB";
// 	case COLORSPACE_DEVICE_CMYK:
// 		return "DeviceCMYK";
// 	case COLORSPACE_CIEBASE_CALGRAY:
// 		return "CalGray";
// 	case COLORSPACE_CIEBASE_CALRGB:
// 		return "CalRGB";
// 	case COLORSPACE_CIEBASE_CALCMYK:
// 		return "CalCMYK";
// 	case COLORSPACE_CIEBASE_CALLAB:
// 		return "Lab";
// 	case COLORSPACE_CIEBASE_ICCBASED:
// 		return "ICCBased";
// 	case COLORSPACE_SPECIAL_PATTERN:
// 		return "Pattern";
// 	case COLORSPACE_SPECIAL_INDEXED:
// 		return "Indexed";
// 	case COLORSPACE_SPECIAL_SEPARATION:
// 		return "Separation";
// 	case COLORSPACE_SPECIAL_DEVICEN:
// 		return "DeviceN";
// 	default:
// 		break;
// 	}
// 	return "";
// }

HE_BYTE CHE_PDF_ColorSpace::GetComponentCount() const
{
	return mComponentCount;
}

HE_ARGB CHE_PDF_ColorSpace::GetARGBValue( const CHE_PDF_Color & color ) const
{
	if ( color.GetComponentCount() == 0 )
	{
		return 0xFF00000;
	}

	switch ( mColorSpaceType )
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
			if ( mBaseColorspace  )
			{
				HE_BYTE index = (HE_BYTE)(color.GetComponent(0));
				HE_ULONG componentCount = mBaseColorspace->GetComponentCount();
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
					return mBaseColorspace->GetARGBValue( newColor );
				}
			}
			break;
		}
	case COLORSPACE_SPECIAL_SEPARATION:
		{
			if ( mBaseColorspace && mFunction )
			{
				CHE_PDF_Color newColor;
				std::vector<HE_FLOAT> input;
				std::vector<HE_FLOAT> output;
				for ( HE_ULONG i = 0; i < color.GetComponentCount(); ++i )
				{
					input.push_back( color.GetComponent( i ) );
				}
				if ( mFunction->Calculate( input, output ) )
				{
					for ( HE_ULONG j = 0; j < output.size(); ++j )
					{
						newColor.Push( output[j] );
					}
					return mBaseColorspace->GetARGBValue( newColor );
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