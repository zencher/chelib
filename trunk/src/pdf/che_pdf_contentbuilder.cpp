#include <cassert>
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_contentbuilder.h"

//obj relative
const HE_CHAR * gp1StrSingl1eSpace = " ";				const HE_DWORD gl1StrSingl1eSpace = 1;
const HE_CHAR * gp1StrNullObj = "null";					const HE_DWORD gl1StrNullObj = 5;
const HE_CHAR * gp1StrBoolObjFalse = "false";			const HE_DWORD gl1StrBoolObjFalse = 5;
const HE_CHAR * gp1StrBoolObjTrue = "true";				const HE_DWORD gl1StrBoolObjTrue = 4;
const HE_CHAR * gp1StrNameObjPre = "/";					const HE_DWORD gl1StrNameObjPre = 1;
const HE_CHAR * gp1StrStrObjLeft = "(";					const HE_DWORD gl1StrStrObj = 1;
const HE_CHAR * gp1StrStrObjRight = ")";
const HE_CHAR * gp1StrArrayObjLeft = "[";				const HE_DWORD gl1StrArrayObj = 1;
const HE_CHAR * gp1StrArrayObjRight = "]";
const HE_CHAR * gp1StrDictObjLeft = "<<";				const HE_DWORD gl1StrDictObj = 2;
const HE_CHAR * gp1StrDictObjRight = ">>";
const HE_CHAR * gp1StrStreamObjBegin = "stream\n";		const HE_DWORD gl1StrStreamObjBegin = 7;
const HE_CHAR * gp1StrStreamObjEnd = "\nendstream";		const HE_DWORD gl1StrStreamObjEnd = 10;

HE_BOOL CHE_PDF_ContentString::FloatToBuf( HE_FLOAT val, CHE_DynBuffer & buf )
{
	static char tempStr[1024];
	sprintf( tempStr, "%g", val );
	if ( strchr( tempStr, 'e' ) )
	{
		sprintf( tempStr, (fabsf(val))>1? "%1.1f":"%1.8f", val );
	}

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), strlen( tempStr ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::IntegerToBuf( HE_INT32 val, CHE_DynBuffer & buf )
{
	static char tempStr[1024];
	sprintf( tempStr, "%d", val );

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), strlen( tempStr ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::DWORDToBuf( HE_DWORD val, CHE_DynBuffer & buf )
{
	static char tempStr[1024];
	sprintf( tempStr, "%d", val );

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), strlen( tempStr ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::PdfMatrixToBuf( CHE_PDF_Matrix & matrix, CHE_DynBuffer & buf )
{
	FloatToBuf( matrix.a, buf );
	SpaceToBuf( buf );

	FloatToBuf( matrix.b, buf );
	SpaceToBuf( buf );

	FloatToBuf( matrix.c, buf );
	SpaceToBuf( buf );

	FloatToBuf( matrix.d, buf );
	SpaceToBuf( buf );

	FloatToBuf( matrix.e, buf );
	SpaceToBuf( buf );

	FloatToBuf( matrix.f, buf );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::SpaceToBuf( CHE_DynBuffer & buf )
{
	static char tempStr[16];

	sprintf( tempStr, " " );

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), 1 );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::StringToBuf( HE_LPCSTR pChar, CHE_DynBuffer & buf )
{
	if ( pChar == NULL )
	{
		return FALSE;
	}

	buf.Write( (HE_LPCBYTE)pChar, strlen( pChar ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf )
{
	switch ( colorSpace.GetType() )
	{
	case COLORSAPCE_DEVICE_GRAY:
		StringToBuf( "/DeviceGray", buf );
		break;
	case COLORSAPCE_DEVICE_RGB:
		StringToBuf( "/DeviceRGB", buf );
		break;
	case COLORSAPCE_DEVICE_CMYK:
		StringToBuf( "/DeviceCMYK", buf );
		break;
	case COLORSAPCE_CIEBASE_CALGRAY:
	case COLORSAPCE_CIEBASE_CALRGB:
	case COLORSAPCE_CIEBASE_CALCMYK:
	case COLORSAPCE_CIEBASE_CALLAB:
	case COLORSAPCE_CIEBASE_ICCBASED:
	case COLORSAPCE_SPECIAL_INDEXED:
	case COLORSAPCE_SPECIAL_SEPARATION:
	case COLORSAPCE_SPECIAL_DEVICEN:
		{
			CHE_ByteString resName = colorSpace.GetResName();
			if ( resName.GetLength() != 0 )
			{
				StringToBuf( "/", buf );
				StringToBuf( resName, buf );
			}
			break;
		}
	case COLORSAPCE_SPECIAL_PATTERN:
		StringToBuf( "/Pattern", buf );
		break;
	default:
		break;
	}
	
	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf )
{
	for ( size_t i = 0; i < color.mConponents.size(); ++i )
	{
		FloatToBuf( color.mConponents[i], buf );
		StringToBuf( " ", buf );
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::StringToBuf( const CHE_ByteString & str, CHE_DynBuffer & buf )
{
	buf.Write( (HE_LPCBYTE)( str.GetData() ), str.GetLength() );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::PdfObjPtrToBuf( const CHE_PDF_ObjectPtr & pObj, CHE_DynBuffer & buf )
{
	if ( ! pObj )
	{
		return FALSE;
	}

	static HE_CHAR tempStr[32768];

	switch( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			buf.Write( (HE_LPCBYTE)gp1StrNullObj, gl1StrNullObj );
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			CHE_PDF_BooleanPtr ptr = pObj->GetBooleanPtr();
			if( ptr->GetValue() != FALSE )
			{
				buf.Write( (HE_LPCBYTE)gp1StrBoolObjTrue, gl1StrBoolObjTrue );
			}else{
				buf.Write( (HE_LPCBYTE)gp1StrBoolObjFalse, gl1StrBoolObjFalse );
			}
			break;
		}
	case OBJ_TYPE_STRING:
		{
			CHE_PDF_StringPtr ptr = pObj->GetStringPtr();
			CHE_ByteString str = ptr->GetString();
			HE_LPBYTE pData = (HE_LPBYTE)( str.GetData() );
			HE_DWORD length = str.GetLength();
			HE_BOOL bHex = FALSE;
			for ( HE_DWORD i = 0; i < length; i++ )
			{
				if ( pData[i] > 127 || pData[i] < 32 ) //������
				{
					bHex  = TRUE;
					break;
				}
			}
			if ( bHex == FALSE )
			{
				buf.Write( (HE_LPCBYTE)gp1StrStrObjLeft, gl1StrStrObj );
				buf.Write( pData, length );
				buf.Write( (HE_LPCBYTE)gp1StrStrObjRight, gl1StrStrObj );
			}else{
				buf.Write( (HE_LPCBYTE)"<", 1 );
				HE_CHAR tmpByte[32];
				HE_DWORD tmpVal = 0;
				for ( HE_DWORD i = 0; i < length; i++ )
				{
					tmpVal = pData[i];
					sprintf( tmpByte, "%08X", tmpVal );
					buf.Write( (HE_LPCBYTE)(tmpByte+6), 2 );
				}
				buf.Write( (HE_LPCBYTE)">", 1 );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			CHE_PDF_NamePtr ptr = pObj->GetNamePtr();
			HE_LPBYTE pData = (HE_LPBYTE)( ptr->GetString().GetData() );
			HE_DWORD length = ptr->GetString().GetLength();
			buf.Write( (HE_LPCBYTE)gp1StrNameObjPre, 1 );
			char tmpStr[16];
			for ( HE_DWORD i = 0; i < length; ++i )
			{
				if ( 32 < pData[i] && pData[i] < 127 )
				{
					buf.Write( (HE_LPCBYTE)(pData+i), 1 );
				}else{
					sprintf( tmpStr, "#%02X", pData[i] ); 
					buf.Write( (HE_LPCBYTE)tmpStr, 3 );
				}
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			CHE_PDF_NumberPtr ptr = pObj->GetNumberPtr();
			if ( ptr->IsInteger() == TRUE )
			{
				sprintf( tempStr, "%d", ptr->GetInteger() );
				buf.Write( (HE_LPCBYTE)tempStr, strlen(tempStr) );
			}else{
				float value = ptr->GetFloat();
				sprintf( tempStr, "%g", value );
				if ( strchr( tempStr, 'e' ) )
				{
					sprintf( tempStr, (fabsf(value))>1? "%1.1f":"%1.8f", value );
				}
				buf.Write( (HE_LPCBYTE)tempStr, strlen(tempStr) );
			}
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			CHE_PDF_ReferencePtr ptr = pObj->GetRefPtr();
			sprintf( tempStr, "%d 0 R", ptr->GetRefNum() );
			buf.Write( (HE_LPCBYTE)tempStr, strlen(tempStr) );
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_ArrayPtr ptr = pObj->GetArrayPtr();
			CHE_PDF_ObjectPtr pElement;
			buf.Write( (HE_LPCBYTE)gp1StrArrayObjLeft, 1 );
			for ( HE_DWORD i = 0; i < ptr->GetCount(); i++ )
			{
				if ( i != 0 )
				{
					buf.Write( (HE_LPCBYTE)gp1StrSingl1eSpace, 1 );
				}
				pElement = ptr->GetElement( i );
				PdfObjPtrToBuf( pElement, buf );
			}
			buf.Write( (HE_LPCBYTE)gp1StrArrayObjRight, 1 );
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			CHE_PDF_DictionaryPtr ptr = pObj->GetDictPtr();
			CHE_PDF_ObjectPtr pElement;
			buf.Write( (HE_LPCBYTE)gp1StrDictObjLeft, 2 );

			CHE_ByteString keyStr;
			for ( HE_DWORD i = 0; i < ptr->GetCount(); i++ )
			{
				if ( ptr->GetKeyByIndex( i, keyStr ) == TRUE )
				{
					HE_LPBYTE pData = (HE_LPBYTE)( keyStr.GetData() );
					HE_DWORD length = keyStr.GetLength();
					buf.Write( (HE_LPCBYTE)gp1StrNameObjPre, 1 );
					buf.Write( pData, length );
					pElement = ptr->GetElementByIndex( i );
					if ( pElement->GetType() == OBJ_TYPE_NULL || pElement->GetType() == OBJ_TYPE_NUMBER || pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() == OBJ_TYPE_BOOLEAN )
					{
						buf.Write( (HE_LPCBYTE)gp1StrSingl1eSpace, 1 );
					}
					if ( pElement )
					{
						PdfObjPtrToBuf( pElement, buf );
					}
				}
			}
			buf.Write( (HE_LPCBYTE)gp1StrDictObjRight, 2 );
			break;
		}
	case OBJ_TYPE_STREAM:
	default:
		return FALSE;
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::PathDataToBuf( CHE_PDF_Path * pPath, CHE_DynBuffer & buf )
{
	assert( pPath );

	PDF_PATHITEM_TYPE type;

	for ( size_t i = 0; i < pPath->mItems.size(); ++i )
	{
		type = pPath->mItems[i].type;

		switch ( type )
		{
		case PathItem_MoveTo:
			{
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				StringToBuf( "m\n", buf );
				break;
			}
		case PathItem_LineTo:
			{
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				StringToBuf( "l\n", buf );
				break;
			}
		case PathItem_CurveTo:
			{
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				StringToBuf( "c\n", buf );
				break;
			}
		case PathItem_Rectangle:
			{
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				FloatToBuf( pPath->mItems[++i].value, buf );
				SpaceToBuf( buf );
				StringToBuf( "re\n", buf );
				break;
			}
		case PathItem_Close:
			{
				StringToBuf( "h\n", buf );
				break;
			}
		default:;
		}
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::PathToBuf( CHE_PDF_Path * pPath, CHE_DynBuffer & buf )
{
	if ( ! pPath )
	{
		return FALSE;
	}

	PathDataToBuf( pPath, buf );

	PDF_FILL_MODE fillMode = pPath->GetFillMode();

	switch ( pPath->GetPaintType() )
	{
	case Paint_Fill:
		{
			if ( fillMode == Mode_Nonzero )
			{
				StringToBuf( "f\n", buf ); 
			}else{
				StringToBuf( "f*\n", buf );
			}
			break;
		}
	case Paint_Stroke:
		{
			StringToBuf( "S\n", buf );
			break;
		}
	case Paint_FillStroke:
		{
			if ( fillMode == Mode_Nonzero )
			{
				StringToBuf( "B\n", buf ); 
			}else{
				StringToBuf( "B*\n", buf );
			}
			break;
		}
	case Paint_None:
		{
			StringToBuf( "n\n", buf );
			break;
		}
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::TextToBuf( CHE_PDF_Text * pText, CHE_DynBuffer & buf )
{
	if ( ! pText )
	{
		return FALSE;
	}

	CHE_PDF_ObjectPtr objPtr = pText->GetText();

	CHE_PDF_GState * pGState = pText->GetGState();
	
	TextStateToBuf( pGState, buf );

	if ( IsPdfStringPtr( objPtr ) )
	{
		PdfObjPtrToBuf( objPtr, buf );
		StringToBuf( " Tj\n", buf );
	}
	else if ( IsPdfArrayPtr( objPtr ) )
	{
		PdfObjPtrToBuf( objPtr, buf );
		StringToBuf( " TJ\n", buf );
	}else{
		return FALSE;
	}

	return TRUE;
}

HE_VOID CHE_PDF_ContentString::TextBlockBeginToBuf( CHE_DynBuffer & buf )
{
	StringToBuf( "BT\n", buf );
}

HE_VOID CHE_PDF_ContentString::TextBlockEndToBuf( CHE_DynBuffer & buf )
{
	StringToBuf( "ET\n", buf );
}

HE_BOOL CHE_PDF_ContentString::RefImageToBuf( CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf )
{
	if ( ! pImage )
	{
		return FALSE;
	}

	CHE_ByteString str = pImage->GetName();

	if ( str.GetLength() == 0 )
	{
		return FALSE;
	}

	StringToBuf( "/", buf );

	StringToBuf( str, buf );

	StringToBuf( " Do\n", buf );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::InlineImageToBuf( CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf )
{
	if ( ! pImage )
	{
		return FALSE;
	}

	StringToBuf( "BI\n", buf );

	StringToBuf( "/W ", buf );
	DWORDToBuf( pImage->GetWidth(), buf );
	StringToBuf( "\n", buf );

	StringToBuf( "/H ", buf );
	DWORDToBuf( pImage->GetHight(), buf );
	StringToBuf( "\n", buf );

	StringToBuf( "/BPC ", buf );
	DWORDToBuf( pImage->GetBitps(), buf );
	StringToBuf( "\n", buf );

	if ( pImage->IsMask() )
	{
		StringToBuf( "/IM true\n", buf );
	}

	if ( pImage->GetColorspace() )
	{
		StringToBuf( "/CS /", buf );
		if ( pImage->GetColorspace()->GetResName().GetLength() > 0 )
		{
			StringToBuf( pImage->GetColorspace()->GetResName(), buf );
		}else{
			StringToBuf( GetColorSpaceName( pImage->GetColorspace() ), buf );
		}
		StringToBuf( "\n", buf );
	}

	StringToBuf( "ID\n", buf );

	//ѹ������
	CHE_PDF_FlateFilter flateFilter;
	CHE_PDF_ASCII85Filter ascii85Filter;

	CHE_DynBuffer tmpBuf;
	CHE_DynBuffer bynBuf;

	flateFilter.Encode( pImage->GetData(), pImage->GetDataSize(), tmpBuf );

	ascii85Filter.Encode( tmpBuf.GetData(), tmpBuf.GetSize(), bynBuf );

	buf.Write( bynBuf );

	StringToBuf( "EI\n", buf );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::ShadingToBuf( CHE_PDF_Shading * pShading, CHE_DynBuffer & buf )
{
	if ( ! pShading )
	{
		return FALSE;
	}

	StringToBuf( pShading->GetName(), buf );

	StringToBuf( " sh\n", buf );

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::MarkToBuf( CHE_PDF_Mark * pMark, CHE_DynBuffer & buf )
{
	if ( ! pMark )
	{
		return FALSE;
	}

	switch ( pMark->GetMarkType() )
	{
	case Mark_MP:
		{
			StringToBuf( "/", buf );
			StringToBuf( pMark->GetTag(), buf );
			StringToBuf( " MP\n", buf );
			break;
		}
	case Mark_DP:
		{
			StringToBuf( "/", buf );
			StringToBuf( pMark->GetTag(), buf );
			SpaceToBuf( buf );
			if ( pMark->GetPropertyResName().GetLength() > 0 )
			{
				StringToBuf( pMark->GetPropertyResName(), buf );
			}else{
				PdfObjPtrToBuf( pMark->GetProperty(), buf );
			}
			StringToBuf( " DP\n", buf );
			break;
		}
	case Mark_BMC:
		{
			StringToBuf( "/", buf );
			StringToBuf( pMark->GetTag(), buf );
			StringToBuf( " BMC\n", buf );
			break;
		}
	case Mark_BDC:
		{
			StringToBuf( "/", buf );
			StringToBuf( pMark->GetTag(), buf );
			SpaceToBuf( buf );
			if ( pMark->GetPropertyResName().GetLength() > 0 )
			{
				StringToBuf( pMark->GetPropertyResName(), buf );
			}else{
				PdfObjPtrToBuf( pMark->GetProperty(), buf );
			}
			StringToBuf( " BDC\n", buf );
			break;
		}
	case Mark_EMC:
		{
			StringToBuf( "EMC\n", buf );
			break;
		}
	default:
		return FALSE;
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::FormToBuf( CHE_PDF_Form * pForm, CHE_DynBuffer & buf )
{
	if ( ! pForm )
	{
		return FALSE;
	}

	if ( pForm->GetName().GetLength() > 0 )
	{
		StringToBuf( "/", buf );
		StringToBuf( pForm->GetName(), buf );
		SpaceToBuf( buf );
		StringToBuf( " Do\n", buf );

		return TRUE;
	}

	return FALSE;
}

HE_BOOL CHE_PDF_ContentString::TextStateToBuf( CHE_PDF_GState * pState, CHE_DynBuffer & buf )
{
	if ( ! pState )
	{
		return FALSE;
	}

	HE_FLOAT val = 0;
	CHE_ByteString name;
	CHE_PDF_Matrix textMatrix;
	PDF_GSTATE_TEXTRENDERMODE textRenderMode = TextRenderMode_Fill;

	if ( pState->GetTextCharSpace( val ) /*&& ! IsDefTextCharSpace( val )*/ )
	{
		FloatToBuf( val, buf );
		StringToBuf( " Tc\n", buf );
	}

	if ( pState->GetTextWordSpace( val ) /*&& ! IsDefTextWrodSpace( val )*/ )
	{
		FloatToBuf( val, buf );
		StringToBuf( " Tw\n", buf );
	}

	if ( pState->GetTextFontResName( name ) && pState->GetTextFontSize( val ) )
	{
		StringToBuf( "/", buf );
		StringToBuf( name, buf );
		SpaceToBuf( buf );
		FloatToBuf( val, buf );
		StringToBuf( " Tf\n", buf );
	}

	if ( pState->GetTextMatrix( textMatrix ) /*&& ! IsDefMatrix( textMatrix )*/ )
	{
		PdfMatrixToBuf( textMatrix, buf );
		StringToBuf( " Tm\n", buf );
	}

	if ( pState->GetTextScaling( val ) /*&& ! IsDefTextScaling( val )*/ )
	{
		FloatToBuf( val, buf );
		StringToBuf( " Tz\n", buf );
	}

	if ( pState->GetTextRise( val ) /*&& ! IsDefTextRise( val )*/ )
	{
		FloatToBuf( val, buf );
		StringToBuf( " Ts\n", buf );
	}

	if ( pState->GetTextRenderMode( textRenderMode ) /*&& ! IsDefTextRenderMode( textRenderMode )*/ )
	{
		HE_DWORD tmpVal = (HE_DWORD)( textRenderMode );
		DWORDToBuf( tmpVal, buf );
		StringToBuf( " Tr\n", buf );
	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::TextStateToBuf( CHE_PDF_GState * pCurGSData, CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf )
{
	if ( pCurGSData == NULL )
	{
		return TextStateToBuf( pTargetGSData,  buf );
	}

	if ( pTargetGSData == NULL )
	{
		return FALSE;
	}

// 	HE_FLOAT val = 0, tmpVal = 0;
// 	CHE_ByteString name;
// 	CHE_PDF_Matrix textMatrix;
// 	PDF_GSTATE_TEXTRENDERMODE textRenderMode = TextRenderMode_Fill;
// 
// 	if ( pCurGSData->GetTextCharSpace( val ) )
// 	{
// 		
// 	}
// 	else if ( pCurState->GetTextCharSpace( val ) && ! IsDefTextCharSpace( val ) )
// 	{
// 		FloatToBuf( val, buf );
// 		StringToBuf( " Tc\n", buf );
// 	}
// 
// 	if ( pState->GetTextWordSpace( val ) && ! IsDefTextWrodSpace( val ) )
// 	{
// 		FloatToBuf( val, buf );
// 		StringToBuf( " Tw\n", buf );
// 	}
// 
// 	if ( pState->GetTextFontResName( name ) && pState->GetTextFontSize( val ) )
// 	{
// 		StringToBuf( "/", buf );
// 		StringToBuf( name, buf );
// 		SpaceToBuf( buf );
// 		FloatToBuf( val, buf );
// 		StringToBuf( " Tf\n", buf );
// 	}
// 
// 	if ( pState->GetTextMatrix( textMatrix ) && ! IsDefMatrix( textMatrix ) )
// 	{
// 		PdfMatrixToBuf( textMatrix, buf );
// 		StringToBuf( " Tm\n", buf );
// 	}
// 
// 	if ( pState->GetTextScaling( val ) && ! IsDefTextScaling( val ) )
// 	{
// 		FloatToBuf( val, buf );
// 		StringToBuf( " Tz\n", buf );
// 	}
// 
// 	if ( pState->GetTextRise( val ) && ! IsDefTextRise( val ) )
// 	{
// 		FloatToBuf( val, buf );
// 		StringToBuf( " Ts\n", buf );
// 	}
// 
// 	if ( pState->GetTextRenderMode( textRenderMode ) && ! IsDefTextRenderMode( textRenderMode ))
// 	{
// 		HE_DWORD tmpVal = (HE_DWORD)( textRenderMode );
// 		DWORDToBuf( tmpVal, buf );
// 		StringToBuf( " Tr\n", buf );
// 	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentString::GSDataToBuf( CHE_PDF_GState * pGSData, CHE_DynBuffer & buf )
{
	if ( ! pGSData )
	{
		return FALSE;
	}

	HE_FLOAT floatVal = 0;
	PDF_GSTATE_LINECAP lineCap = LineCap_Butt;
	PDF_GSTATE_LINEJOIN lineJoin = LineJoin_Miter;
	PDF_GSTATE_DASHPATTERN lineDash;
	PDF_GSTATE_RENDERINTENTS ri = RI_AbsoluteColorimetric;

	if ( pGSData->GetLineWidth( floatVal ) && ! IsDefLineWidth( floatVal ) )
	{
		FloatToBuf( floatVal, buf );
		StringToBuf( " w\n", buf );
	}

	if ( pGSData->GetMiterLimit( floatVal ) && ! IsDefMiterLimit( floatVal ) )
	{
		FloatToBuf( floatVal, buf );
		StringToBuf( " M\n", buf );
	}
	
	if ( pGSData->GetLineCap( lineCap ) && ! IsDefLineCap( lineCap ) )
	{
		switch ( lineCap )
		{
		case LineCap_Butt:
			IntegerToBuf( 0, buf );
			StringToBuf( " J\n", buf );
			break;
		case LineCap_Round:
			IntegerToBuf( 1, buf );
			StringToBuf( " J\n", buf );
			break;
		case LineCap_Square:
			IntegerToBuf( 2, buf );
			StringToBuf( " J\n", buf );
			break;
		default:
			break;
		}
	}

	if ( pGSData->GetLineJoin( lineJoin ) && ! IsDefLineJoin( lineJoin ) )
	{
		switch ( lineJoin )
		{
		case LineJoin_Miter:
			IntegerToBuf( 0, buf );
			StringToBuf( " j\n", buf );
			break;
		case LineJoin_Round:
			IntegerToBuf( 1, buf );
			StringToBuf( " j\n", buf );
		case LineJoin_Bevel:
			IntegerToBuf( 2, buf );
			StringToBuf( " j\n", buf );
		default:
			break;
		}
	}

	if ( pGSData->GetLineDash( lineDash ) && ! IsDefLineDash( lineDash ) )
	{
		StringToBuf( "[", buf );
		for ( size_t i = 0; lineDash.dashArray.size(); ++i )
		{
			StringToBuf( " ", buf );
			FloatToBuf( lineDash.dashArray[i], buf );
		}
		StringToBuf( " ] ", buf );
		FloatToBuf( lineDash.dashPhase, buf );
		StringToBuf( " d\n", buf );
	}

	ri = pGSData->GetRenderIntents();
	if ( ! IsDefRenderIntents( ri ) )
	{
		switch ( ri )
		{
		case RI_AbsoluteColorimetric:
			IntegerToBuf( 0, buf );
			StringToBuf( " ri\n", buf );
			break;
		case RI_RelativeColorimetric:
			IntegerToBuf( 1, buf );
			StringToBuf( " ri\n", buf );
			break;
		case RI_Saturation:
			IntegerToBuf( 2, buf );
			StringToBuf( " ri\n", buf );
			break;
		case RI_Perceptual:
			IntegerToBuf( 3, buf );
			StringToBuf( " ri\n", buf );
			break;
		default:
			break;
		}
	}

	floatVal = pGSData->GetFlatness();
	if ( ! IsDefFlatness( floatVal )  )
	{
		FloatToBuf( floatVal, buf );
		StringToBuf( " i\n", buf );
	}
	
	CHE_PDF_ColorSpace colorSpace( COLORSAPCE_DEVICE_GRAY );
	CHE_PDF_Color color;

	if ( pGSData->GetFillColorSpace( colorSpace ) && ! IsDefColorSpace( colorSpace ) )
	{
		if ( ! colorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( colorSpace, buf );
			StringToBuf( " cs\n", buf );
		}

		if ( pGSData->GetFillColor( color ) )
		{
			ColorToBuf( color,  buf );

			switch (  colorSpace.GetType() )
			{
			case COLORSAPCE_DEVICE_GRAY:
				StringToBuf( "g\n", buf );
				break;
			case COLORSAPCE_DEVICE_RGB:
				StringToBuf( "rg\n", buf );
				break;
			case COLORSAPCE_DEVICE_CMYK:
				StringToBuf( "k\n", buf );
				break;
			case COLORSAPCE_CIEBASE_CALGRAY:
			case COLORSAPCE_CIEBASE_CALRGB:
			case COLORSAPCE_CIEBASE_CALCMYK:
			case COLORSAPCE_CIEBASE_CALLAB:
				StringToBuf( "sc\n", buf );
				break;
			case COLORSAPCE_CIEBASE_ICCBASED:
			case COLORSAPCE_SPECIAL_INDEXED:
			case COLORSAPCE_SPECIAL_SEPARATION:
			case COLORSAPCE_SPECIAL_DEVICEN:
				StringToBuf( "scn\n", buf );
				break;
			case COLORSAPCE_SPECIAL_PATTERN:
				StringToBuf( colorSpace.GetResName(), buf );
				break;
			}
		}
	}

	if ( pGSData->GetStrokeColorSpace( colorSpace ) && ! IsDefColorSpace( colorSpace ) )
	{
		if ( ! colorSpace.IsDeviceColorSpace() )
		{
			ColorSpaceToBuf( colorSpace, buf );
			StringToBuf( " cs\n", buf );
		}

		if ( pGSData->GetStrokeColor( color ) )
		{
			ColorToBuf( color,  buf );

			switch (  colorSpace.GetType() )
			{
			case COLORSAPCE_DEVICE_GRAY:
				StringToBuf( "G\n", buf );
				break;
			case COLORSAPCE_DEVICE_RGB:
				StringToBuf( "RG\n", buf );
				break;
			case COLORSAPCE_DEVICE_CMYK:
				StringToBuf( "K\n", buf );
				break;
			case COLORSAPCE_CIEBASE_CALGRAY:
			case COLORSAPCE_CIEBASE_CALRGB:
			case COLORSAPCE_CIEBASE_CALCMYK:
			case COLORSAPCE_CIEBASE_CALLAB:
				StringToBuf( "SC\n", buf );
				break;
			case COLORSAPCE_CIEBASE_ICCBASED:
			case COLORSAPCE_SPECIAL_INDEXED:
			case COLORSAPCE_SPECIAL_SEPARATION:
			case COLORSAPCE_SPECIAL_DEVICEN:
				StringToBuf( "SCN\n", buf );
				break;
			case COLORSAPCE_SPECIAL_PATTERN:
				StringToBuf( colorSpace.GetResName(), buf );
				break;
			}
		}
	}

	CHE_PDF_ExtGState * pExtState = pGSData->GetExtGState();
	if ( pExtState )
	{
		CHE_ByteString resName;

		std::list<CHE_ByteString>::iterator it = pExtState->mExtDictNameList.begin();

		for ( ; it != pExtState->mExtDictNameList.end(); ++it )
		{
			resName = *it;
			StringToBuf( "/", buf );
			StringToBuf( resName, buf );
			StringToBuf( " gs\n", buf );
		}
	}

	CHE_PDF_Matrix curMatrix;

	CHE_PDF_ClipState * pClipState = pGSData->GetClipState();
	
	if ( pClipState )
	{
		CHE_PDF_Matrix clipMatrix;

		std::list<CHE_PDF_ClipStateItem*>::iterator it;

		CHE_PDF_ContentObject * pClipElement = NULL;

		for ( it = pClipState->mClipElementList.begin(); it != pClipState->mClipElementList.end(); ++it )
		{
			clipMatrix = (*it)->GetMatrix();
			if ( clipMatrix != curMatrix )
			{
				CHE_PDF_Matrix revertMatrix;
				clipMatrix.Invert( revertMatrix );
				revertMatrix.Concat( clipMatrix );
				clipMatrix = revertMatrix;

				if ( ! IsDefMatrix( clipMatrix ) )
				{
					PdfMatrixToBuf( clipMatrix, buf );
					StringToBuf( " cm\n", buf );
				}

// 				pClipElement = it->GetElement();
// 
// 				if ( pc )
// 				{
// 				}

				curMatrix = clipMatrix;
			}
		}
	}

	CHE_PDF_Matrix ctm;
	ctm = pGSData->GetMatrix();
	if ( ! IsDefMatrix( ctm ) )
	{
		PdfMatrixToBuf( ctm, buf );
		StringToBuf( " cm\n", buf );
	}

	return TRUE; 
}

HE_BOOL CHE_PDF_ContentString::GSDataToBuf( CHE_PDF_GState * pCurGSData, CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf )
{
	if ( ! pTargetGSData )
	{
		return FALSE;
	}

	if ( ! pCurGSData )
	{
		return GSDataToBuf( pTargetGSData, buf );
	}

// 	HE_FLOAT floatVal = 0, tmpFloatVal = 0;
// 	PDF_GSTATE_LINECAP lineCap = LineCap_Butt, tmpLineCap = LineCap_Butt;
// 	PDF_GSTATE_LINEJOIN lineJoin = LineJoin_Miter, tmpLineJoin = LineJoin_Miter;
// 	PDF_GSTATE_DASHPATTERN lineDash, tmpLineDash;
// 	PDF_GSTATE_RENDERINTENTS ri = RI_AbsoluteColorimetric, tmpRi = RI_AbsoluteColorimetric;
// 
// 	if ( pCurGSData->GetLineWidth( floatVal ) )
// 	{
// 		if ( pTargetGSData->GetLineWidth( tmpFloatVal ) )
// 		{
// 			if ( ! fabsf( floatVal - tmpFloatVal)  <= FLT_EPSILON )
// 			{
// 				FloatToBuf( floatVal, buf );
// 				StringToBuf( " w\n", buf );
// 			}
// 		}else{
// 			pTargetGSData->SetLineWidth( floatVal );
// 		}
// 	}
// 	else if ( pTargetGSData->GetLineWidth( floatVal ) && ! IsDefLineWidth( floatVal ) )
// 	{
// 		FloatToBuf( floatVal, buf );
// 		StringToBuf( " w\n", buf );
// 	}
// 
// 	if ( pCurGSData->GetMiterLimit( floatVal ) )
// 	{
// 		if ( pTargetGSData->GetMiterLimit( tmpFloatVal ) )
// 		{
// 			if ( ! fabsf( floatVal - tmpFloatVal)  <= FLT_EPSILON )
// 			{
// 				FloatToBuf( floatVal, buf );
// 				StringToBuf( " M\n", buf );
// 			}
// 		}else{
// 			pTargetGSData->SetMiterLimit( floatVal );
// 		}
// 	}
// 	else if ( pTargetGSData->GetMiterLimit( floatVal ) && ! IsDefMiterLimit( floatVal ) )
// 	{
// 		FloatToBuf( floatVal, buf );
// 		StringToBuf( " M\n", buf );
// 	}

// 	if ( pCurGSData->GetLineCap( lineCap ) )
// 	{
// 		if ( pTargetGSData->GetLineCap( tmpLineCap ) )
// 		{
// 			if ( ! fabsf( floatVal - tmpFloatVal)  <= FLT_EPSILON )
// 			{
// 				FloatToBuf( floatVal, buf );
// 				StringToBuf( " M\n", buf );
// 			}
// 		}else{
// 			pTargetGSData->SetMiterLimit( floatVal );
// 		}
// 	}
// 	else if ( pTargetGSData->GetMiterLimit( floatVal ) && ! IsDefMiterLimit( floatVal ) )
// 	{
// 		FloatToBuf( floatVal, buf );
// 		StringToBuf( " M\n", buf );
// 	}

// 	if ( pGSData->GetLineCap( lineCap ) && ! IsDefLineCap( lineCap ) )
// 	{
// 		switch ( lineCap )
// 		{
// 		case LineCap_Butt:
// 			IntegerToBuf( 0, buf );
// 			StringToBuf( " J\n", buf );
// 			break;
// 		case LineCap_Round:
// 			IntegerToBuf( 1, buf );
// 			StringToBuf( " J\n", buf );
// 			break;
// 		case LineCap_Square:
// 			IntegerToBuf( 2, buf );
// 			StringToBuf( " J\n", buf );
// 			break;
// 		default:
// 			break;
// 		}
// 	}
// 
// 	if ( pGSData->GetLineJoin( lineJoin ) && ! IsDefLineJoin( lineJoin ) )
// 	{
// 		switch ( lineJoin )
// 		{
// 		case LineJoin_Miter:
// 			IntegerToBuf( 0, buf );
// 			StringToBuf( " j\n", buf );
// 			break;
// 		case LineJoin_Round:
// 			IntegerToBuf( 1, buf );
// 			StringToBuf( " j\n", buf );
// 		case LineJoin_Bevel:
// 			IntegerToBuf( 2, buf );
// 			StringToBuf( " j\n", buf );
// 		default:
// 			break;
// 		}
// 	}
// 
// 	if ( pGSData->GetLineDash( lineDash ) && ! IsDefLineDash( lineDash ) )
// 	{
// 		StringToBuf( "[", buf );
// 		for ( size_t i = 0; lineDash.dashArray.size(); ++i )
// 		{
// 			StringToBuf( " ", buf );
// 			FloatToBuf( lineDash.dashArray[i], buf );
// 		}
// 		StringToBuf( " ] ", buf );
// 		FloatToBuf( lineDash.dashPhase, buf );
// 		StringToBuf( " d\n", buf );
// 	}
// 
// 	ri = pGSData->GetRenderIntents();
// 	if ( ! IsDefRenderIntents( ri ) )
// 	{
// 		switch ( ri )
// 		{
// 		case RI_AbsoluteColorimetric:
// 			IntegerToBuf( 0, buf );
// 			StringToBuf( " ri\n", buf );
// 			break;
// 		case RI_RelativeColorimetric:
// 			IntegerToBuf( 1, buf );
// 			StringToBuf( " ri\n", buf );
// 			break;
// 		case RI_Saturation:
// 			IntegerToBuf( 2, buf );
// 			StringToBuf( " ri\n", buf );
// 			break;
// 		case RI_Perceptual:
// 			IntegerToBuf( 3, buf );
// 			StringToBuf( " ri\n", buf );
// 			break;
// 		default:
// 			break;
// 		}
// 	}
// 
// 	floatVal = pGSData->GetFlatness();
// 	if ( ! IsDefFlatness( floatVal )  )
// 	{
// 		FloatToBuf( floatVal, buf );
// 		StringToBuf( " i\n", buf );
// 	}
// 
// 	CHE_PDF_ColorSpace colorSpace( COLORSAPCE_DEVICE_GRAY );
// 	CHE_PDF_Color color;
// 
// 	if ( pGSData->GetFillColorSpace( colorSpace ) && ! IsDefColorSpace( colorSpace ) )
// 	{
// 		if ( ! colorSpace.IsDeviceColorSpace() )
// 		{
// 			ColorSpaceToBuf( colorSpace, buf );
// 			StringToBuf( " cs\n", buf );
// 		}
// 
// 		if ( pGSData->GetFillColor( color ) )
// 		{
// 			ColorToBuf( color,  buf );
// 
// 			switch (  colorSpace.GetType() )
// 			{
// 			case COLORSAPCE_DEVICE_GRAY:
// 				StringToBuf( "g\n", buf );
// 				break;
// 			case COLORSAPCE_DEVICE_RGB:
// 				StringToBuf( "rg\n", buf );
// 				break;
// 			case COLORSAPCE_DEVICE_CMYK:
// 				StringToBuf( "k\n", buf );
// 				break;
// 			case COLORSAPCE_CIEBASE_CALGRAY:
// 			case COLORSAPCE_CIEBASE_CALRGB:
// 			case COLORSAPCE_CIEBASE_CALCMYK:
// 			case COLORSAPCE_CIEBASE_CALLAB:
// 				StringToBuf( "sc\n", buf );
// 				break;
// 			case COLORSAPCE_CIEBASE_ICCBASED:
// 			case COLORSAPCE_SPECIAL_INDEXED:
// 			case COLORSAPCE_SPECIAL_SEPARATION:
// 			case COLORSAPCE_SPECIAL_DEVICEN:
// 				StringToBuf( "scn\n", buf );
// 				break;
// 			case COLORSAPCE_SPECIAL_PATTERN:
// 				StringToBuf( colorSpace.GetResName(), buf );
// 				break;
// 			}
// 		}
// 	}
// 
// 	if ( pGSData->GetStrokeColorSpace( colorSpace ) && ! IsDefColorSpace( colorSpace ) )
// 	{
// 		if ( ! colorSpace.IsDeviceColorSpace() )
// 		{
// 			ColorSpaceToBuf( colorSpace, buf );
// 			StringToBuf( " cs\n", buf );
// 		}
// 
// 		if ( pGSData->GetStrokeColor( color ) )
// 		{
// 			ColorToBuf( color,  buf );
// 
// 			switch (  colorSpace.GetType() )
// 			{
// 			case COLORSAPCE_DEVICE_GRAY:
// 				StringToBuf( "G\n", buf );
// 				break;
// 			case COLORSAPCE_DEVICE_RGB:
// 				StringToBuf( "RG\n", buf );
// 				break;
// 			case COLORSAPCE_DEVICE_CMYK:
// 				StringToBuf( "K\n", buf );
// 				break;
// 			case COLORSAPCE_CIEBASE_CALGRAY:
// 			case COLORSAPCE_CIEBASE_CALRGB:
// 			case COLORSAPCE_CIEBASE_CALCMYK:
// 			case COLORSAPCE_CIEBASE_CALLAB:
// 				StringToBuf( "SC\n", buf );
// 				break;
// 			case COLORSAPCE_CIEBASE_ICCBASED:
// 			case COLORSAPCE_SPECIAL_INDEXED:
// 			case COLORSAPCE_SPECIAL_SEPARATION:
// 			case COLORSAPCE_SPECIAL_DEVICEN:
// 				StringToBuf( "SCN\n", buf );
// 				break;
// 			case COLORSAPCE_SPECIAL_PATTERN:
// 				StringToBuf( colorSpace.GetResName(), buf );
// 				break;
// 			}
// 		}
// 	}
// 
// 	CHE_PDF_ExtGState * pExtState = pGSData->GetExtGState();
// 	if ( pExtState )
// 	{
// 		CHE_ByteString resName;
// 
// 		std::list<CHE_ByteString>::iterator it = pExtState->mExtDictNameList.begin();
// 
// 		for ( ; it != pExtState->mExtDictNameList.end(); ++it )
// 		{
// 			resName = *it;
// 			StringToBuf( "/", buf );
// 			StringToBuf( resName, buf );
// 			StringToBuf( " gs\n", buf );
// 		}
// 	}
// 
// 	CHE_PDF_Matrix curMatrix;
// 
// 	CHE_PDF_ClipState * pClipState = pGSData->GetClipState();
// 
// 	if ( pClipState )
// 	{
// 		CHE_PDF_Matrix clipMatrix;
// 
// 		std::list<CHE_PDF_ClipStateItem*>::iterator it;
// 
// 		CHE_PDF_ContentObject * pClipElement = NULL;
// 
// 		for ( it = pClipState->mClipElementList.begin(); it != pClipState->mClipElementList.end(); ++it )
// 		{
// 			clipMatrix = (*it)->GetMatrix();
// 			if ( clipMatrix != curMatrix )
// 			{
// 				CHE_PDF_Matrix revertMatrix;
// 				clipMatrix.Invert( revertMatrix );
// 				revertMatrix.Concat( clipMatrix );
// 				clipMatrix = revertMatrix;
// 
// 				if ( ! IsDefMatrix( clipMatrix ) )
// 				{
// 					PdfMatrixToBuf( clipMatrix, buf );
// 					StringToBuf( " cm\n", buf );
// 				}
// 
// 				// 				pClipElement = it->GetElement();
// 				// 
// 				// 				if ( pc )
// 				// 				{
// 				// 				}
// 
// 				curMatrix = clipMatrix;
// 			}
// 		}
// 	}
// 
// 	CHE_PDF_Matrix ctm;
// 	ctm = pGSData->GetMatrix();
// 	if ( ! IsDefMatrix( ctm ) )
// 	{
// 		PdfMatrixToBuf( ctm, buf );
// 		StringToBuf( " cm\n", buf );
// 	}

	return TRUE;
}

HE_BOOL CHE_PDF_ContentBuilder::ContentToBuf( CHE_PDF_ContentObjectList * pList, CHE_DynBuffer & buf )
{
	if ( pList == NULL )
	{
		return FALSE;
	}

	HE_BOOL bTextBlock = FALSE;

	PDF_CONTENTOBJ_TYPE itemType;

	ContentObjectList::iterator it;

	for ( it = pList->Begin(); it != pList->End(); ++it )
	{
		itemType = (*it)->GetType();
		if ( itemType == ContentType_Text )
		{
			if ( ! bTextBlock )
			{
				bTextBlock = TRUE;
				CHE_PDF_ContentString::TextBlockBeginToBuf( buf );
			}
		}else{
			if ( bTextBlock )
			{
				bTextBlock = FALSE;
				CHE_PDF_ContentString::TextBlockEndToBuf( buf );
			}
		}

		CHE_PDF_GState * pCurGState = (*it)->GetGState();

		CHE_PDF_ContentString::GSDataToBuf( pCurGState, buf );

		switch ( itemType )
		{
		case ContentType_Text:
			CHE_PDF_ContentString::TextToBuf( (CHE_PDF_Text*)( *it ), buf );
			break;
		case ContentType_Path:
			CHE_PDF_ContentString::PathToBuf( (CHE_PDF_Path*)( *it ), buf );
			break;
		case ContentType_Mark:
			CHE_PDF_ContentString::MarkToBuf( (CHE_PDF_Mark *)( *it ), buf );
			break;
		case ContentType_RefImage:
			CHE_PDF_ContentString::RefImageToBuf( (CHE_PDF_RefImage*)( *it ), buf );
			break;
		case ContentType_InlineImage:
			CHE_PDF_ContentString::InlineImageToBuf( (CHE_PDF_InlineImage*)( *it ), buf );
			break;
		case ContentType_Form:
			CHE_PDF_ContentString::FormToBuf( (CHE_PDF_Form*)( *it ), buf );
			break;
		case ContentType_Shading:
			CHE_PDF_ContentString::ShadingToBuf( (CHE_PDF_Shading*)( *it ), buf );
		default:
			break;
		}
	}

	return TRUE;
}