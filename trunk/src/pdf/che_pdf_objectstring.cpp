#include "../../include/che_pdf_filter.h"
#include "../../include/che_pdf_objectstring.h"



namespace chelib {
    
#include <cassert>
#include <math.h>

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

bool CPDF_ObjectString::FloatToBuf( FLOAT val, CDynBuffer & buf )
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

bool CPDF_ObjectString::IntegerToBuf( uint32 val, CDynBuffer & buf )
{
    static char tempStr[1024];
    sprintf( tempStr, "%d", val );
    buf.Write( (PCBYTE)( &tempStr[0] ), strlen( tempStr ) );
    return TRUE;
}

bool CPDF_ObjectString::SpaceToBuf( CDynBuffer & buf )
{
    static char tempStr[16];
    sprintf( tempStr, " " );
    buf.Write( (PCBYTE)( &tempStr[0] ), 1 );
    return TRUE;
}

bool CPDF_ObjectString::StringToBuf( char const * pChar, CDynBuffer & buf )
{
    if ( pChar == nullptr )
    {
        return false;
    }
    buf.Write( (PCBYTE)pChar, strlen( pChar ) );
    return TRUE;
}

bool CPDF_ObjectString::StringToBuf( const CByteString & str, CDynBuffer & buf )
{
    buf.Write( (PCBYTE)( str.GetData() ), str.GetLength() );
    return TRUE;
}

bool CPDF_ObjectString::ObjectToBuf( const CPDF_ObjectPtr & pObj, CDynBuffer & buf )
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
            CPDF_BooleanPtr ptr = pObj->GetBooleanPtr();
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
            CPDF_StringPtr ptr = pObj->GetStringPtr();
            CByteString str = ptr->GetString();
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
            CPDF_NamePtr ptr = pObj->GetNamePtr();
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
            CPDF_NumberPtr ptr = pObj->GetNumberPtr();
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
            CPDF_ReferencePtr ptr = pObj->GetRefPtr();
            sprintf( tempStr, "%ld 0 R", ptr->GetRefNum() );
            buf.Write( (PCBYTE)tempStr, strlen(tempStr) );
            break;
        }
        case OBJ_TYPE_ARRAY:
        {
            CPDF_ArrayPtr ptr = pObj->GetArrayPtr();
            CPDF_ObjectPtr pElement;
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
            CPDF_DictionaryPtr ptr = pObj->GetDictPtr();
            CPDF_ObjectPtr pElement;
            buf.Write( (PCBYTE)gpStrDictObjLeft, 2 );
            
            CByteString keyStr;
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

}//namespace