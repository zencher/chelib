#include <cassert>
#include <math.h>
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_objectstring.h"

//obj relative
static const HE_CHAR * gpStrSingleSpace		= " ";			static const HE_DWORD glStrSingleSpace		= 1;
static const HE_CHAR * gpStrNullObj			= "null";		static const HE_DWORD glStrNullObj			= 5;
static const HE_CHAR * gpStrBoolObjFalse	= "false";		static const HE_DWORD glStrBoolObjFalse		= 5;
static const HE_CHAR * gpStrBoolObjTrue		= "true";		static const HE_DWORD glStrBoolObjTrue		= 4;
static const HE_CHAR * gpStrNameObjPre		= "/";			static const HE_DWORD glStrNameObjPre		= 1;
static const HE_CHAR * gpStrStrObjLeft		= "(";			static const HE_DWORD glStrStrObj			= 1;
static const HE_CHAR * gpStrStrObjRight		= ")";
static const HE_CHAR * gpStrArrayObjLeft	= "[";			static const HE_DWORD glStrArrayObj			= 1;
static const HE_CHAR * gpStrArrayObjRight	= "]";
static const HE_CHAR * gpStrDictObjLeft		= "<<";			static const HE_DWORD glStrDictObj			= 2;
static const HE_CHAR * gpStrDictObjRight	= ">>";

HE_BOOL CHE_PDF_ObjectString::FloatToBuf( HE_FLOAT val, CHE_DynBuffer & buf )
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

HE_BOOL CHE_PDF_ObjectString::IntegerToBuf( HE_INT32 val, CHE_DynBuffer & buf )
{
	static char tempStr[1024];
	sprintf( tempStr, "%d", val );

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), strlen( tempStr ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ObjectString::DWORDToBuf( HE_DWORD val, CHE_DynBuffer & buf )
{
	static char tempStr[1024];
	sprintf( tempStr, "%ld", val );

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), strlen( tempStr ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ObjectString::SpaceToBuf( CHE_DynBuffer & buf )
{
	static char tempStr[16];

	sprintf( tempStr, " " );

	buf.Write( (HE_LPCBYTE)( &tempStr[0] ), 1 );

	return TRUE;
}

HE_BOOL CHE_PDF_ObjectString::StringToBuf( HE_LPCSTR pChar, CHE_DynBuffer & buf )
{
	if ( pChar == NULL )
	{
		return FALSE;
	}

	buf.Write( (HE_LPCBYTE)pChar, strlen( pChar ) );

	return TRUE;
}

HE_BOOL CHE_PDF_ObjectString::StringToBuf( const CHE_ByteString & str, CHE_DynBuffer & buf )
{
	buf.Write( (HE_LPCBYTE)( str.GetData() ), str.GetLength() );

	return TRUE;
}

HE_BOOL CHE_PDF_ObjectString::PdfObjPtrToBuf( const CHE_PDF_ObjectPtr & pObj, CHE_DynBuffer & buf )
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
			buf.Write( (HE_LPCBYTE)gpStrNullObj, glStrNullObj );
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			CHE_PDF_BooleanPtr ptr = pObj->GetBooleanPtr();
			if( ptr->GetValue() != FALSE )
			{
				buf.Write( (HE_LPCBYTE)gpStrBoolObjTrue, glStrBoolObjTrue );
			}else{
				buf.Write( (HE_LPCBYTE)gpStrBoolObjFalse, glStrBoolObjFalse );
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
				if ( pData[i] > 127 || pData[i] < 32 || pData[i] == '(' || pData[i] == ')' ) //´ýÍêÉÆ
				{
					bHex  = TRUE;
					break;
				}
			}
			if ( bHex == FALSE )
			{
				buf.Write( (HE_LPCBYTE)gpStrStrObjLeft, glStrStrObj );
				buf.Write( pData, length );
				buf.Write( (HE_LPCBYTE)gpStrStrObjRight, glStrStrObj );
			}else{
				buf.Write( (HE_LPCBYTE)"<", 1 );
				HE_CHAR tmpByte[32];
				HE_DWORD tmpVal = 0;
				for ( HE_DWORD i = 0; i < length; i++ )
				{
					tmpVal = pData[i];
					sprintf( tmpByte, "%08lX", tmpVal );
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
			buf.Write( (HE_LPCBYTE)gpStrNameObjPre, 1 );
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
			sprintf( tempStr, "%ld 0 R", ptr->GetRefNum() );
			buf.Write( (HE_LPCBYTE)tempStr, strlen(tempStr) );
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_ArrayPtr ptr = pObj->GetArrayPtr();
			CHE_PDF_ObjectPtr pElement;
			buf.Write( (HE_LPCBYTE)gpStrArrayObjLeft, 1 );
			for ( HE_DWORD i = 0; i < ptr->GetCount(); i++ )
			{
				if ( i != 0 )
				{
					buf.Write( (HE_LPCBYTE)gpStrSingleSpace, 1 );
				}
				pElement = ptr->GetElement( i );
				PdfObjPtrToBuf( pElement, buf );
			}
			buf.Write( (HE_LPCBYTE)gpStrArrayObjRight, 1 );
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			CHE_PDF_DictionaryPtr ptr = pObj->GetDictPtr();
			CHE_PDF_ObjectPtr pElement;
			buf.Write( (HE_LPCBYTE)gpStrDictObjLeft, 2 );

			CHE_ByteString keyStr;
			for ( HE_DWORD i = 0; i < ptr->GetCount(); i++ )
			{
				if ( ptr->GetKeyByIndex( i, keyStr ) == TRUE )
				{
					HE_LPBYTE pData = (HE_LPBYTE)( keyStr.GetData() );
					HE_DWORD length = keyStr.GetLength();
					buf.Write( (HE_LPCBYTE)gpStrNameObjPre, 1 );
					buf.Write( pData, length );
					pElement = ptr->GetElementByIndex( i );
					if ( pElement->GetType() == OBJ_TYPE_NULL || pElement->GetType() == OBJ_TYPE_NUMBER || pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() == OBJ_TYPE_BOOLEAN )
					{
						buf.Write( (HE_LPCBYTE)gpStrSingleSpace, 1 );
					}
					if ( pElement )
					{
						PdfObjPtrToBuf( pElement, buf );
					}
				}
			}
			buf.Write( (HE_LPCBYTE)gpStrDictObjRight, 2 );
			break;
		}
	case OBJ_TYPE_STREAM:
	default:
		return FALSE;
	}

	return TRUE;
}