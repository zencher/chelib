#include <cassert>
#include <math.h>
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_objectstring.h"

//obj relative
static const char * gpStrSingleSpace	= " ";			//static const size_t glStrSingleSpace		= 1;
static const char * gpStrNullObj		= "null";		static const size_t glStrNullObj			= 5;
static const char * gpStrBoolObjFalse	= "false";		static const size_t glStrBoolObjFalse		= 5;
static const char * gpStrBoolObjTrue	= "true";		static const size_t glStrBoolObjTrue		= 4;
static const char * gpStrNameObjPre		= "/";			//static const size_t glStrNameObjPre         = 1;
static const char * gpStrStrObjLeft		= "(";			static const size_t glStrStrObj             = 1;
static const char * gpStrStrObjRight	= ")";
static const char * gpStrArrayObjLeft	= "[";			//static const size_t glStrArrayObj			= 1;
static const char * gpStrArrayObjRight	= "]";
static const char * gpStrDictObjLeft	= "<<";			//static const size_t glStrDictObj			= 2;
static const char * gpStrDictObjRight	= ">>";

bool CHE_PDF_ObjectString::FloatToBuf( FLOAT val, CHE_DynBuffer & buf )
{
    static char tempStr[1024];
    sprintf( tempStr, "%g", val );
    if ( strchr( tempStr, 'e' ) )
    {
        sprintf( tempStr, (fabsf(val))>1? "%1.1f":"%1.8f", val );
    }
    buf.Write( (PCBYTE)( &tempStr[0] ), strlen( tempStr ) );
    return TRUE;
}

bool CHE_PDF_ObjectString::IntegerToBuf( uint32 val, CHE_DynBuffer & buf )
{
    static char tempStr[1024];
    sprintf( tempStr, "%d", val );
    buf.Write( (PCBYTE)( &tempStr[0] ), strlen( tempStr ) );
    return TRUE;
}

bool CHE_PDF_ObjectString::SpaceToBuf( CHE_DynBuffer & buf )
{
    static char tempStr[16];
    sprintf( tempStr, " " );
    buf.Write( (PCBYTE)( &tempStr[0] ), 1 );
    return TRUE;
}

bool CHE_PDF_ObjectString::StringToBuf( char const * pChar, CHE_DynBuffer & buf )
{
    if ( pChar == nullptr )
    {
        return false;
    }
    buf.Write( (PCBYTE)pChar, strlen( pChar ) );
    return TRUE;
}

bool CHE_PDF_ObjectString::StringToBuf( const CHE_ByteString & str, CHE_DynBuffer & buf )
{
    buf.Write( (PCBYTE)( str.GetData() ), str.GetLength() );
    return TRUE;
}

bool CHE_PDF_ObjectString::ObjectToBuf( const CHE_PDF_ObjectPtr & pObj, CHE_DynBuffer & buf )
{
    if ( ! pObj )
    {
        return false;
    }
    static char tempStr[32768];
    switch( pObj->GetType() )
    {
        case OBJ_TYPE_NULL:
        {
            buf.Write( (PCBYTE)gpStrNullObj, glStrNullObj );
            break;
        }
        case OBJ_TYPE_BOOLEAN:
        {
            CHE_PDF_BooleanPtr ptr = pObj->GetBooleanPtr();
            if( ptr->GetValue() != false )
            {
                buf.Write( (PCBYTE)gpStrBoolObjTrue, glStrBoolObjTrue );
            }else{
                buf.Write( (PCBYTE)gpStrBoolObjFalse, glStrBoolObjFalse );
            }
            break;
        }
        case OBJ_TYPE_STRING:
        {
            CHE_PDF_StringPtr ptr = pObj->GetStringPtr();
            CHE_ByteString str = ptr->GetString();
            PBYTE pData = (PBYTE)( str.GetData() );
            size_t length = str.GetLength();
            bool bHex = false;
            for ( size_t i = 0; i < length; i++ )
            {
                if ( pData[i] > 127 || pData[i] < 32 || pData[i] == '(' || pData[i] == ')' )
                {
                    bHex  = TRUE;
                    break;
                }
            }
            if ( bHex == false )
            {
                buf.Write( (PCBYTE)gpStrStrObjLeft, glStrStrObj );
                buf.Write( pData, length );
                buf.Write( (PCBYTE)gpStrStrObjRight, glStrStrObj );
            }else{
                buf.Write( (PCBYTE)"<", 1 );
                char tmpByte[32];
                size_t tmpVal = 0;
                for ( size_t i = 0; i < length; i++ )
                {
                    tmpVal = pData[i];
                    sprintf( tmpByte, "%08lX", tmpVal );
                    buf.Write( (PCBYTE)(tmpByte+6), 2 );
                }
                buf.Write( (PCBYTE)">", 1 );
            }
            break;
        }
        case OBJ_TYPE_NAME:
        {
            CHE_PDF_NamePtr ptr = pObj->GetNamePtr();
            PBYTE pData = (PBYTE)( ptr->GetString().GetData() );
            size_t length = ptr->GetString().GetLength();
            buf.Write( (PCBYTE)gpStrNameObjPre, 1 );
            char tmpStr[16];
            for ( size_t i = 0; i < length; ++i )
            {
                if ( 32 < pData[i] && pData[i] < 127 )
                {
                    buf.Write( (PCBYTE)(pData+i), 1 );
                }else{
                    sprintf( tmpStr, "#%02X", pData[i] );
                    buf.Write( (PCBYTE)tmpStr, 3 );
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
                buf.Write( (PCBYTE)tempStr, strlen(tempStr) );
            }else{
                float value = ptr->GetFloat();
                sprintf( tempStr, "%g", value );
                if ( strchr( tempStr, 'e' ) )
                {
                    sprintf( tempStr, (fabsf(value))>1? "%1.1f":"%1.8f", value );
                }
                buf.Write( (PCBYTE)tempStr, strlen(tempStr) );
            }
            break;
        }
        case OBJ_TYPE_REFERENCE:
        {
            CHE_PDF_ReferencePtr ptr = pObj->GetRefPtr();
            sprintf( tempStr, "%ld 0 R", ptr->GetRefNum() );
            buf.Write( (PCBYTE)tempStr, strlen(tempStr) );
            break;
        }
        case OBJ_TYPE_ARRAY:
        {
            CHE_PDF_ArrayPtr ptr = pObj->GetArrayPtr();
            CHE_PDF_ObjectPtr pElement;
            buf.Write( (PCBYTE)gpStrArrayObjLeft, 1 );
            for ( size_t i = 0; i < ptr->GetCount(); i++ )
            {
                if ( i != 0 )
                {
                    buf.Write( (PCBYTE)gpStrSingleSpace, 1 );
                }
                pElement = ptr->GetElement( i );
                ObjectToBuf( pElement, buf );
            }
            buf.Write( (PCBYTE)gpStrArrayObjRight, 1 );
            break;
        }
        case OBJ_TYPE_DICTIONARY:
        {
            CHE_PDF_DictionaryPtr ptr = pObj->GetDictPtr();
            CHE_PDF_ObjectPtr pElement;
            buf.Write( (PCBYTE)gpStrDictObjLeft, 2 );
            
            CHE_ByteString keyStr;
            ptr->MoveToFirst();
            while ( ptr->GetKeyAndElement( keyStr, pElement ) )
            {
                PBYTE pData = (PBYTE)( keyStr.GetData() );
                size_t length = keyStr.GetLength();
                buf.Write( (PCBYTE)gpStrNameObjPre, 1 );
                buf.Write( pData, length );
                if ( pElement->GetType() == OBJ_TYPE_NULL ||
                    pElement->GetType() == OBJ_TYPE_NUMBER ||
                    pElement->GetType() == OBJ_TYPE_REFERENCE ||
                    pElement->GetType() == OBJ_TYPE_BOOLEAN )
                {
                    buf.Write( (PCBYTE)gpStrSingleSpace, 1 );
                }
                if ( pElement )
                {
                    ObjectToBuf( pElement, buf );
                }
            }
            buf.Write( (PCBYTE)gpStrDictObjRight, 2 );
            break;
        }
        case OBJ_TYPE_STREAM:
        default:
            return false;
    }
    return TRUE;
}