#include "../../include/pdf/che_pdf_colorspace.h"

HE_BOOL CHE_PDF_ColorSpace::IsDeviceColorSpace()
{
	switch ( GetType() )
	{
	case COLORSAPCE_DEVICE_GRAY:
	case COLORSAPCE_DEVICE_RGB:
	case COLORSAPCE_DEVICE_CMYK:
		return TRUE;
	default:
		break;
	}

	return FALSE;;
}

HE_DWORD CHE_PDF_ColorSpace::GetArgb( CHE_PDF_Color & color )
{
	switch ( mType )
	{
	case COLORSAPCE_DEVICE_GRAY:
	case COLORSAPCE_CIEBASE_CALGRAY:
		{
			HE_DWORD valRet = 0xFF000000;
			HE_DWORD tmpByte = 0x00;
			if ( color.mConponents.size() >= 1 )
			{
				tmpByte = color.mConponents[0] * 255;
				valRet = valRet | tmpByte | tmpByte << 8 | tmpByte << 16;
			}
			return valRet;
		}
	case COLORSAPCE_DEVICE_RGB:
	case COLORSAPCE_CIEBASE_CALRGB:
		{
			HE_DWORD valRet = 0xFF000000;
			HE_DWORD tmpByte1 = 0x00;
			HE_DWORD tmpByte2 = 0x00;
			HE_DWORD tmpByte3 = 0x00;
			if ( color.mConponents.size() >= 3 )
			{
				tmpByte1 = color.mConponents[0] * 255;
				tmpByte2 = color.mConponents[1] * 255;
				tmpByte3 = color.mConponents[2] * 255;
				valRet = valRet | tmpByte1 | tmpByte2 << 8 | tmpByte3 << 16;
			}
			return valRet;
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
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_GRAY, pAllocator );
	}else if ( name == "DeviceRGB" || name == "RGB" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_RGB, pAllocator );
	}else if ( name == "DeviceCMYK" || name == "CMYK"  )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_DEVICE_CMYK, pAllocator );
	}else if ( name == "Pattern" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_PATTERN, pAllocator );
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
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALGRAY, "", pArray );
	}else if ( name == "CalRGB" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALRGB, "", pArray );
	}else if ( name == "CalCMYK" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALCMYK, "", pArray );
	}else if ( name == "ICCBased" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_ICCBASED, "", pArray );
	}else if ( name == "Lab" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_CIEBASE_CALLAB, "", pArray );
	}else if ( name == "Indexed" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_INDEXED, "", pArray );
	}else if ( name == "Separation" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_SEPARATION, "", pArray );
	}else if ( name == "DeviceN" )
	{
		pColorSpace = pAllocator->New<CHE_PDF_ColorSpace>( COLORSAPCE_SPECIAL_DEVICEN, "", pArray );
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
	case COLORSAPCE_DEVICE_GRAY:
		return "DeviceGray";
	case COLORSAPCE_DEVICE_RGB:
		return "DeviceRGB";
	case COLORSAPCE_DEVICE_CMYK:
		return "DeviceCMYK";
	case COLORSAPCE_CIEBASE_CALGRAY:
		return "CalGray";
	case COLORSAPCE_CIEBASE_CALRGB:
		return "CalRGB";
	case COLORSAPCE_CIEBASE_CALCMYK:
		return "CalCMYK";
	case COLORSAPCE_CIEBASE_CALLAB:
		return "Lab";
	case COLORSAPCE_CIEBASE_ICCBASED:
		return "ICCBased";
	case COLORSAPCE_SPECIAL_PATTERN:
		return "Pattern";
	case COLORSAPCE_SPECIAL_INDEXED:
		return "Indexed";
	case COLORSAPCE_SPECIAL_SEPARATION:
		return "Separation";
	case COLORSAPCE_SPECIAL_DEVICEN:
		return "DeviceN";
	default:
		break;
	}

	return "";
}