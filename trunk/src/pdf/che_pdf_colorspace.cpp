#include "../../include/pdf/che_pdf_colorspace.h"

HE_BOOL CHE_PDF_ColorSpace::IsDeviceColorSpace()
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

HE_UINT32 CHE_PDF_ColorSpace::GetArgb( CHE_PDF_Color & color )
{
	switch ( mType )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			HE_UINT32 valRet = 0xFF000000;
			HE_UINT32 tmpByte = 0x00;
			if ( color.mConponents.size() >= 1 )
			{
				tmpByte = (HE_UINT32)( color.mConponents[0] * 255 );
				valRet = valRet | tmpByte | tmpByte << 8 | tmpByte << 16;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
		{
			HE_UINT32 valRet = 0xFF000000;
			HE_UINT32 tmpByte1 = 0x00;
			HE_UINT32 tmpByte2 = 0x00;
			HE_UINT32 tmpByte3 = 0x00;
			if ( color.mConponents.size() >= 3 )
			{
				tmpByte1 = (HE_UINT32)( color.mConponents[0] * 255 );
				tmpByte2 = (HE_UINT32)( color.mConponents[1] * 255 );
				tmpByte3 = (HE_UINT32)( color.mConponents[2] * 255 );
				valRet = valRet | tmpByte1 << 16 | tmpByte2 << 8 | tmpByte3;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
		{
			if ( color.mConponents.size() >= 4 )
			{
				HE_FLOAT bgr[3];
				bgr[0] = ( 1 < color.mConponents[2] + color.mConponents[3] ) ? color.mConponents[2] + color.mConponents[3] : 1;
				bgr[1] = ( 1 < color.mConponents[1] + color.mConponents[3] ) ? color.mConponents[1] + color.mConponents[3] : 1;
				bgr[2] = ( 1 < color.mConponents[0] + color.mConponents[3] ) ? color.mConponents[0] + color.mConponents[3] : 1;
				
				HE_UINT32 valRet = 0xFF000000;
				HE_UINT32 tmpByte1 = 0x00;
				HE_UINT32 tmpByte2 = 0x00;
				HE_UINT32 tmpByte3 = 0x00;
				if ( color.mConponents.size() >= 3 )
				{
					tmpByte1 = (HE_UINT32)( bgr[0] * 255 );
					tmpByte2 = (HE_UINT32)( bgr[1] * 255 );
					tmpByte3 = (HE_UINT32)( bgr[2] * 255 );
					valRet = valRet | tmpByte1 << 16 | tmpByte2 << 8 | tmpByte3;
				}
				return valRet;
			}
		}
	default:
		return 0xFF000000;
	}
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
	if ( pArray->GetCount() > 0 )
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