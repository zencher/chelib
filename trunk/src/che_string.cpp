#include "../include/che_string.h"

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>

//@zctodo:optimize:simpleness:for,class,CHE_ByteString:"make HE_ByteString a class, maintain the refcount and data by itself";
//@zctodo:optimize:performance:for,class,CHE_ByteString:"reduce memory copy times";
//@zctodo:optimize:simpleness:for,class,CHE_WideString:"make HE_ByteString a class, maintain the refcount and data by itself"
//@zctodo:optimize:performance:for,class,CHE_WideString:"reduce memory copy times";

CHE_ByteString::CHE_ByteString( char ch, CHE_Allocator * pAllocator )
: CHE_Object( pAllocator )
{
	if ( ch == '\0' )
	{
		mpData = nullptr;
	}else{
		mpData = GetAllocator()->New<HE_ByteStringData>();
		mpData->mLength = 1;
		mpData->mRefCount.AddRef();
		mpData->mpStr[0] = ch;
		mpData->mpStr = GetAllocator()->NewArray<char>( 2 );
		mpData->mpStr[1] = '\0';
	}
}

CHE_ByteString::CHE_ByteString( char const * pstr, size_t length /* = 0 */, CHE_Allocator * pAllocator /*= nullptr*/ )
: CHE_Object( pAllocator )
{
	if ( pstr == nullptr )
	{
		mpData = nullptr;
		return;
	}
	if ( length == 0 )
	{
		length = strlen( pstr );
	}
	if ( length > 0 )
	{
		mpData = GetAllocator()->New<HE_ByteStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = length;
		mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
		memcpy( mpData->mpStr, pstr, length );
		mpData->mpStr[length] = '\0';
	}else{
		mpData = nullptr;
	}
}

CHE_ByteString::CHE_ByteString( const CHE_ByteString & str )
: CHE_Object( str.GetAllocator() )
{
	if ( str.mpData == nullptr )
	{
		mpData = nullptr;
	}else{
		mpData = str.mpData;
		str.mpData->mRefCount.AddRef();
	}
}

void CHE_ByteString::Clear()
{
	if ( mpData )
	{
		mpData->mRefCount.DecRef();

		if ( mpData->mRefCount == 0 && mpData->mpStr )
		{
			GetAllocator()->DeleteArray<char>( mpData->mpStr );
			mpData->mpStr = nullptr;
			GetAllocator()->Delete<HE_ByteStringData>( mpData );
		}
		mpData = nullptr;
	}
}

CHE_ByteString & CHE_ByteString::operator=( char ch )
{
	Clear();

	if ( ch == '\0' )
	{
		return *this;
	}
	
	mpData = GetAllocator()->New<HE_ByteStringData>();
	mpData->mLength = 1;
	mpData->mRefCount.AddRef();
	mpData->mpStr = GetAllocator()->NewArray<char>( 2 );
	mpData->mpStr[0] = ch;
	mpData->mpStr[1] = '\0';

	return *this;
}

CHE_ByteString & CHE_ByteString::operator=( char const * pstr )
{
	Clear();

	if ( pstr == nullptr )
	{
		return *this;
	}
    
	size_t length = strlen( pstr );
	if ( length == 0 )
	{
		return *this;
	}

	mpData = GetAllocator()->New<HE_ByteStringData>();
	mpData->mRefCount.AddRef();
	mpData->mLength = length;
	mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
	strcpy( mpData->mpStr, pstr );

	return *this;
}

CHE_ByteString & CHE_ByteString::operator=( const CHE_ByteString & str )
{
	if ( this == &str )
	{
		return *this;
	}

	if ( mpData == str.mpData )
	{
		return *this;
	}

	Clear();

	if ( str.mpData )
	{
		mpData = str.mpData;
		str.mpData->mRefCount.AddRef();
	}

	return *this;
}

bool CHE_ByteString::operator==( char ch ) const
{
	if ( mpData == nullptr || strlen( mpData->mpStr ) == 0 )
	{
		if ( ch == '\0' )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( mpData->mLength != 1 )
		{
			return false;
		}else{
			if ( mpData->mpStr[0] == ch )
			{
				return true;
			}else{
				return false;
			}
		}
	}
}

bool CHE_ByteString::operator==( char const * pstr ) const
{
	if ( mpData == nullptr || strlen( mpData->mpStr ) == 0 )
	{
		if ( pstr == nullptr || strlen( pstr ) == 0 )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( pstr == nullptr )
		{
			if ( mpData->mpStr == nullptr )
			{
				return true;
			}else{
				return false;
			}
		}else{
			if ( strcmp( mpData->mpStr, pstr ) == 0 )
			{
				return true;
			}else{
				return false;
			}	
		}
	}
}

bool CHE_ByteString::operator==( const CHE_ByteString & str ) const
{
	if ( this == &str )
	{
		return true;
	}else{
		if ( mpData == str.mpData )
		{
			return true;
		}else{
			if ( mpData == nullptr )
			{
				return false;
			}else{
				if ( strcmp( mpData->mpStr, str.mpData->mpStr ) == 0 )
				{
					return true;
				}else{
					return false;
				}
			}
		}
	}
}

bool CHE_ByteString::SetData( BYTE * pData, size_t length )
{
	if ( pData == nullptr || length == 0 )
	{
		Clear();
		return TRUE;
	}
	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<HE_ByteStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = length;
		mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
		memcpy( mpData->mpStr, pData, length );
		mpData->mpStr[length] = '\0';
	}else{
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<char>( mpData->mpStr );
			}
			mpData->mRefCount.AddRef();
			mpData->mLength = length;
			mpData->mpStr= GetAllocator()->NewArray<char>( length+1 );
			memcpy( mpData->mpStr, pData, length );
			mpData->mpStr[length] = '\0';
		}else {
			mpData = GetAllocator()->New<HE_ByteStringData>();
			mpData->mRefCount.AddRef();
			mpData->mLength = length;
			mpData->mpStr= GetAllocator()->NewArray<char>( length+1 );
			memcpy( mpData->mpStr, pData, length );
			mpData->mpStr[length] = '\0';
		}
	}
	return TRUE;
}

size_t CHE_ByteString::GetLength() const
{
	if ( mpData == nullptr )
	{
		return 0;
	}else{
		return mpData->mLength;
	}
}

char CHE_ByteString::operator[]( size_t index ) const
{
	if ( mpData != nullptr )
	{
		if ( index >= mpData->mLength )
		{
			return 0;
		}else if ( mpData->mpStr )
		{
			return mpData->mpStr[index];
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int32 CHE_ByteString::GetInteger() const
{
	return atoi( GetData() );
}

FLOAT CHE_ByteString::GetFloat() const
{
	return atof( GetData() );
}

CHE_ByteString CHE_ByteString::operator+( char ch )
{
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CHE_ByteString( ch, GetAllocator() );
	}

	size_t length = mpData->mLength + 2;
	char * pstr = GetAllocator()->NewArray<char>( length );

	strcpy( pstr, mpData->mpStr );
	pstr[length-2] = ch;
	pstr[length-1] = '\0';

	CHE_ByteString str( pstr, length-1, GetAllocator() );
	GetAllocator()->DeleteArray<char>( pstr );
	return str;
}


CHE_ByteString CHE_ByteString::operator+( char const * pstr )
{
	if ( pstr == nullptr )
	{
		return CHE_ByteString( *this );
	}

	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CHE_ByteString( pstr, strlen(pstr), GetAllocator() );
	}

	size_t length = mpData->mLength + strlen(pstr) + 1;
	char * pTmpStr = GetAllocator()->NewArray<char>( length );

	strcpy( pTmpStr, mpData->mpStr );
	strcat( pTmpStr, pstr );

	CHE_ByteString str( pTmpStr, strlen(pTmpStr), GetAllocator() );
	GetAllocator()->DeleteArray<char>( pTmpStr );
	return str;
}

CHE_ByteString CHE_ByteString::operator+( const CHE_ByteString & str )
{
	if ( str.mpData == nullptr )
	{
		return CHE_ByteString( *this );
	}
	
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CHE_ByteString( str );
	}
	
	size_t length = mpData->mLength + str.mpData->mLength + 1;
	char * pTempStr = GetAllocator()->NewArray<char>( length );
	strcpy( pTempStr, mpData->mpStr );
	strcat( pTempStr, str.mpData->mpStr );
	
	CHE_ByteString strRet(pTempStr);
	GetAllocator()->DeleteArray<char>( pTempStr );
	return strRet;
}

CHE_ByteString & CHE_ByteString::operator+=( char ch )
{
	if ( ch == 0 )
	{
		return *this;
	}

	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<HE_ByteStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = 1;
		mpData->mpStr = GetAllocator()->NewArray<char>( 2 );
		mpData->mpStr[0] = ch;
		mpData->mpStr[1] = '\0';
	}else{
		if ( mpData->mpStr == nullptr )
		{
			mpData->mLength = 1;
			mpData->mRefCount.AddRef();
			mpData->mpStr = GetAllocator()->NewArray<char>( 2 );
			mpData->mpStr[0] = ch;
			mpData->mpStr[1] = '\0';
			return *this;
		}
		char * pTmpStr = GetAllocator()->NewArray<char>( strlen(mpData->mpStr)+2 );
		strcpy( pTmpStr, mpData->mpStr );
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<char>( mpData->mpStr );
				mpData->mpStr = nullptr;
			}
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
		}else{
			mpData = GetAllocator()->New<HE_ByteStringData>();
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
			mpData->mpStr = nullptr;
		}
		
		size_t length = strlen( pTmpStr )+2;
		mpData->mpStr = GetAllocator()->NewArray<char>( length );
		strcpy( mpData->mpStr, pTmpStr );
		mpData->mpStr[length-2] = ch;
		mpData->mpStr[length-1] = '\0';
		mpData->mLength = length-1;
		
		GetAllocator()->DeleteArray<char>( pTmpStr );
	}
	return *this;
}

CHE_ByteString & CHE_ByteString::operator+=( char const * pstr )
{
	if ( pstr == nullptr )
	{
		return *this;
	}

	if ( strlen( pstr ) == 0 )
	{
		return *this;
	}

	if ( mpData == nullptr )
	{
		size_t length = strlen(pstr);
		mpData = GetAllocator()->New<HE_ByteStringData>();
		mpData->mRefCount.AddRef();
		mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
		strcpy( mpData->mpStr, pstr );
		mpData->mLength = length;
	}else{
		if ( mpData->mpStr == nullptr )
		{
			size_t length = strlen(pstr);
			mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
			strcpy( mpData->mpStr, pstr );
			mpData->mLength = length;
			return *this;
		}

		char * pTmpStr = GetAllocator()->NewArray<char>( strlen(mpData->mpStr)+1 );
		strcpy( pTmpStr, mpData->mpStr );

		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<char>( mpData->mpStr );
				mpData->mpStr = nullptr;
			}
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
		}else{
			mpData = GetAllocator()->New<HE_ByteStringData>();
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
			mpData->mpStr = nullptr;
		}

		size_t length = strlen(pTmpStr) + strlen(pstr);
		mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
		strcpy( mpData->mpStr, pTmpStr );
		strcat( mpData->mpStr, pstr );
		mpData->mLength = length;
	}
	
	return *this;
}

CHE_ByteString & CHE_ByteString::operator+=( const CHE_ByteString & str )
{
	if ( str.mpData == nullptr || str.mpData->mpStr == nullptr )
	{
		return *this;
	}

	if ( this == &str )
	{
		char * pTmpStr = GetAllocator()->NewArray<char>( strlen(mpData->mpStr)+1 );
		strcpy( pTmpStr, mpData->mpStr );

		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<char>( mpData->mpStr );
				mpData->mpStr = nullptr;
			}
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
		}else{
			mpData = GetAllocator()->New<HE_ByteStringData>();
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
			mpData->mpStr = nullptr;
		}

		size_t length = strlen(pTmpStr) * 2 + 1;
		mpData->mpStr = GetAllocator()->NewArray<char>( length );
		strcpy( mpData->mpStr, pTmpStr );
		strcat( mpData->mpStr, pTmpStr );

		mpData->mLength = length-1;

		GetAllocator()->DeleteArray<char>( pTmpStr );
		pTmpStr = nullptr;
		
		return *this;
	}else{
		if ( mpData == str.mpData )
		{
			char * pTmpStr = GetAllocator()->NewArray<char>( strlen(mpData->mpStr)+1 );
			strcpy( pTmpStr, mpData->mpStr );

			mpData->mRefCount.DecRef();
			mpData->mRefCount.DecRef();
			if ( mpData->mRefCount == 0 )
			{
				if ( mpData->mpStr )
				{
					GetAllocator()->DeleteArray<char>( mpData->mpStr );
					mpData->mpStr = nullptr;
				}
				mpData->mLength = 0;
				mpData->mRefCount.AddRef();
				mpData->mRefCount.AddRef();
			}else{
				mpData = GetAllocator()->New<HE_ByteStringData>();
				mpData->mRefCount.AddRef();
				mpData->mRefCount.AddRef();
				mpData->mpStr = nullptr;
				mpData->mLength = 0;
			}

			size_t length = strlen(pTmpStr) * 2 + 1;
			mpData->mpStr = GetAllocator()->NewArray<char>( length );
			strcpy( mpData->mpStr, pTmpStr );
			strcat( mpData->mpStr, pTmpStr );
			
			mpData->mLength = length-1;
			
			GetAllocator()->DeleteArray<char>( pTmpStr );
			pTmpStr = nullptr;
			
			return *this;
		}else{
			if ( mpData->mpStr == nullptr )
			{
				size_t length = strlen(str.mpData->mpStr)+1;
				mpData->mpStr = GetAllocator()->NewArray<char>( length );
				strcpy( mpData->mpStr, str.mpData->mpStr );
				return *this;
			}

			char * pTmpStr = GetAllocator()->NewArray<char>( strlen(mpData->mpStr)+1 );
			strcpy( pTmpStr, mpData->mpStr );

			if ( mpData->mRefCount == 1 )
			{
				if ( mpData->mpStr )
				{
					GetAllocator()->DeleteArray<char>( mpData->mpStr );
					mpData->mpStr = nullptr;
					mpData->mLength = 0;
				}
			}else{
				mpData->mRefCount.DecRef();
				mpData = GetAllocator()->New<HE_ByteStringData>();
				mpData->mLength = 0;
				mpData->mRefCount.AddRef();
				mpData->mpStr = nullptr;
			}

			size_t length = strlen( str.mpData->mpStr );
			length += strlen( pTmpStr )+1;
			mpData->mpStr = GetAllocator()->NewArray<char>( length );
			strcpy( mpData->mpStr, pTmpStr );
			strcat( mpData->mpStr, str.mpData->mpStr );

			GetAllocator()->DeleteArray<char>( pTmpStr );
			pTmpStr = nullptr;
			
			mpData->mLength = length-1;
			
			return *this;
		}
	}
}

bool CHE_ByteString::operator!=( char ch )const
{
	if ( mpData == nullptr )
	{
		if ( ch == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( mpData->mLength != 1 )
		{
			return true;
		}else{
			if ( mpData->mpStr[0] == ch )
			{
				return false;
			}else{
				return true;
			}
		}
	}
}

bool CHE_ByteString::operator!=( char const * pstr )const
{
	if ( mpData == nullptr )
	{
		if ( pstr == nullptr || strlen( pstr ) == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( pstr == nullptr )
		{
			if ( mpData->mpStr == nullptr )
			{
				return false;
			}else{
				return true;
			}
		}else{
			if ( strcmp( mpData->mpStr, pstr ) == 0 )
			{
				return false;
			}else{
				return true;
			}	
		}
	}
}

bool CHE_ByteString::operator!=( const CHE_ByteString & str )const
{
	if ( this == &str )
	{
		return false;
	}else{
		if ( mpData == str.mpData )
		{
			return false;
		}else{
			if ( mpData == nullptr )
			{
				return true;
			}else{
				if ( strcmp( mpData->mpStr, str.mpData->mpStr ) == 0 )
				{
					return false;
				}else{
					return true;
				}
			}
		}
	}	
}

bool operator==( char ch, CHE_ByteString & str )
{
	return ( str == ch );
}

bool operator==( char const * pstr, const CHE_ByteString & str )
{
	return ( str == pstr );
}

CHE_ByteString operator+( char ch, const CHE_ByteString & str )
{
	CHE_ByteString tempStr( ch, str.GetAllocator() );
	tempStr+=str;
	return tempStr;
}

CHE_ByteString operator+( char const * pstr, const CHE_ByteString & str )
{
	CHE_ByteString tempStr( pstr, strlen(pstr), str.GetAllocator() );
	tempStr+= str;
	return tempStr;
}

bool operator!=( char ch, const CHE_ByteString & str )
{
	return ( str != ch );
}

bool operator!=( char const * pstr, const CHE_ByteString & str )
{
	return ( str != pstr );
}



CHE_WideString::CHE_WideString( wchar_t wch, CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	if ( wch == '\0' )
	{
		mpData = nullptr;
	}else{
		mpData = GetAllocator()->New<HE_WideStringData>();
		mpData->mLength = 1;
		mpData->mRefCount.AddRef();
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >(2);
		mpData->mpStr[0] = wch;
		mpData->mpStr[1] = '\0';
	}
}

CHE_WideString::CHE_WideString( wchar_t const * pstr, size_t length /* = 0 */,  CHE_Allocator * pAllocator) : CHE_Object( pAllocator )
{
	if ( pstr == nullptr )
	{
		mpData = nullptr;
		return;
	}
	if ( length == 0 )
	{
		length = wcslen( pstr );
	}
	if ( length > 0 )
	{
		mpData = GetAllocator()->New<HE_WideStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = length;
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
		wcscpy( mpData->mpStr, pstr );
		mpData->mpStr[length] = '\0';
	}else{
		mpData = nullptr;
	}
}

CHE_WideString::CHE_WideString( const CHE_WideString & str ) : CHE_Object( str.GetAllocator() )
{
	if ( str.mpData == nullptr )
	{
		mpData = nullptr;
	}else{
		mpData = str.mpData;
		str.mpData->mRefCount.AddRef();
	}
}

void CHE_WideString::Clear()
{
	if ( mpData )
	{
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 && mpData->mpStr )
		{
			GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
			mpData->mpStr = nullptr;
			GetAllocator()->Delete<HE_WideStringData>( mpData );
		}

		mpData = nullptr;
	}
}

CHE_WideString & CHE_WideString::operator=( wchar_t  wch )
{
	Clear();

	if ( wch == (wchar_t )0x0000 )
	{
		return *this;
	}

	mpData = GetAllocator()->New<HE_WideStringData>();
	mpData->mLength = 1;
	mpData->mRefCount.AddRef();
	mpData->mpStr = GetAllocator()->NewArray<wchar_t >( 2 );
	mpData->mpStr[0] = wch;
	mpData->mpStr[1] = (wchar_t)0x0000;

	return *this;
}

CHE_WideString& CHE_WideString::operator=( wchar_t const * pstr )
{
	Clear();

	if ( pstr  == nullptr )
	{
		return *this;
	}

	size_t length = wcslen( pstr );
	if ( length == 0 )
	{
		return *this;
	}

	mpData = GetAllocator()->New<HE_WideStringData>();
	mpData->mRefCount.AddRef();
	mpData->mLength = length;
	mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
	wcscpy( mpData->mpStr, pstr );
	return *this;
}

CHE_WideString & CHE_WideString::operator=( const CHE_WideString & str )
{
	if ( this == &str )
	{
		return *this;
	}
	
	if ( mpData == str.mpData )
	{
		return *this;
	}

	Clear();

	if ( str.mpData )
	{
		mpData = str.mpData;
		str.mpData->mRefCount.AddRef();
	}

	return *this;
}

bool CHE_WideString::operator==( wchar_t wch )const
{
	if ( mpData == nullptr || wcslen( mpData->mpStr ) == 0 )
	{
		if ( wch == '\0' )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( mpData->mLength != 1 )
		{
			return false;
		}else{
			if ( mpData->mpStr[0] == wch )
			{
				return true;
			}else{
				return false;
			}
		}
	}
}

bool CHE_WideString::operator==( wchar_t const * pstr )const
{
	if ( mpData == nullptr || wcslen( mpData->mpStr ) == 0 )
	{
		if ( pstr == nullptr || wcslen( pstr ) == 0 )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( pstr == nullptr )
		{
			if ( mpData->mpStr == nullptr )
			{
				return true;
			}else{
				return false;
			}
		}else{
			if ( wcscmp( mpData->mpStr, pstr ) == 0 )
			{
				return true;
			}else{
				return false;
			}	
		}
	}
}

bool CHE_WideString::operator==( const CHE_WideString & str )const
{
	if ( this == &str )
	{
		return true;
	}else{
		if ( mpData == str.mpData )
		{
			return true;
		}else{
			if ( mpData == nullptr )
			{
				return false;
			}else{
				if ( wcscmp( mpData->mpStr, str.mpData->mpStr ) == 0 )
				{
					return true;
				}else{
					return false;
				}
			}
		}
	}
}

bool CHE_WideString::SetData( wchar_t * pData, size_t length )
{
	if ( pData == nullptr || length == 0 )
	{
		Clear();
		return TRUE;
	}
	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<HE_WideStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = length;
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
		memcpy( mpData->mpStr, pData, length * sizeof(wchar_t) );
		mpData->mpStr[length] = '\0';
	}else{
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
			}
			mpData->mRefCount.AddRef();
			mpData->mLength = length;
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
			memcpy( mpData->mpStr, pData, length * sizeof(wchar_t) );
			mpData->mpStr[length] = '\0';
		}else {
			mpData = GetAllocator()->New<HE_WideStringData>();
			mpData->mRefCount.AddRef();
			mpData->mLength = length;
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
			memcpy( mpData->mpStr, pData, length * sizeof(wchar_t) );
			mpData->mpStr[length] = '\0';
		}
	}
	return TRUE;
}

size_t CHE_WideString::GetLength() const
{
	if ( mpData == nullptr )
	{
		return 0;
	}else{
		return mpData->mLength;
	}
}

wchar_t  CHE_WideString::operator[]( size_t index )const
{
	if ( mpData != nullptr )
	{
		if ( index >= mpData->mLength )
		{
			return 0;
		}else if ( mpData->mpStr )
		{
			return mpData->mpStr[index];
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

int32 CHE_WideString::GetInteger() const
{
	if ( GetLength() == 0 )
	{
		return 0;
	}else{
		bool bBegin = TRUE;
		bool bNegative = false;
		int32 iValue = 0;
		bool bSign = false;
		wchar_t  tmpChar = 0;
		for ( size_t i = 0; i < GetLength(); i++ )
		{
			if ( bBegin && i > 0 )
			{
				bBegin = false;
			}
			tmpChar = GetData()[i];
			switch( tmpChar )
			{
			case '+':
				if ( bSign == false && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = false;
				}else{
					return 0;
				}
				break;
			case '-':
				if ( bSign == false && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = TRUE;
				}else{
					return 0;
				}
				break;
			default:
				if ( L'0' > tmpChar || tmpChar > L'9' )
				{
					return 0;
				}else{
					iValue = iValue * 10 + ( tmpChar - L'0' ); 
				}
				break;
			}
		}
		if( bNegative == TRUE )
		{
			return 0 - iValue;
		}else{
			return iValue;
		}
	}
}

FLOAT CHE_WideString::GetFloat() const
{
	if ( GetLength() == 0 )
	{
		return 0;
	}else{
		bool bNegative = false;
		bool bBegin = TRUE;
		size_t lPointBit = 1;
		FLOAT fValue = 0;
		bool bPoint = false;
		bool bSign = false;
		wchar_t  tmpChar = 0;
		for ( size_t i = 0; i < GetLength(); i++ )
		{
			if ( bBegin && i > 0 )
			{
				bBegin = false;
			}
			tmpChar = GetData()[i];
			switch( tmpChar )
			{
			case L'+':
				if ( bSign == false && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = false;
				}else{
					return 0;
				}
				break;
			case L'-':
				if ( bSign == false && bBegin == TRUE )
				{
					bSign = TRUE;
					bNegative = TRUE;
				}else{
					return 0;
				}
				break;
			case L'.':
				if ( bPoint == false )
				{
					bPoint = TRUE;
				}else{
					return 0;
				}
				break;
			default:
				if ( L'0' > tmpChar || tmpChar > L'9' )
				{
					return 0;
				}else{
					if ( bPoint == false )
					{
						fValue = fValue * 10 + ( tmpChar - L'0' );
					}else{
						fValue = fValue + ( tmpChar - L'0' ) / ( lPointBit * 10 );
						lPointBit++;
					}
				}
				break;
			}
		}
		if ( bNegative == TRUE )
		{
			return 0 - fValue;
		}else{
			return fValue;
		}
	}
}

CHE_WideString CHE_WideString::operator+( wchar_t wch )
{
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CHE_WideString( wch, GetAllocator() );
	}

	size_t length = mpData->mLength + 2;
	wchar_t  * pTempStr = GetAllocator()->NewArray<wchar_t >(length);

	wcscpy( pTempStr, mpData->mpStr );
	pTempStr[length-2] = wch;
	pTempStr[length-1] = '\0';

	CHE_WideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	return tmpString;
}

CHE_WideString CHE_WideString::operator+( wchar_t const * pstr )
{
	if ( pstr == nullptr )
	{
		return CHE_WideString(*this);
	}

	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CHE_WideString( pstr );
	}
	
	size_t length = mpData->mLength + wcslen(pstr);
	wchar_t  * pTempStr = GetAllocator()->NewArray<wchar_t >( length+1 );
	wcscpy( pTempStr, mpData->mpStr );
	wcscat( pTempStr, pstr );
    mpData->mLength = length;

	CHE_WideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	return tmpString;
}

CHE_WideString CHE_WideString::operator+( const CHE_WideString & str )
{
	if ( str.mpData == nullptr )
	{
		return CHE_WideString(*this);
	}
	
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CHE_WideString( str );
	}
	
	size_t length = mpData->mLength + str.mpData->mLength;
	wchar_t  * pTempStr = GetAllocator()->NewArray<wchar_t >( length+1 );
	
	wcscpy( pTempStr, mpData->mpStr );
	wcscat( pTempStr, str.mpData->mpStr );
    mpData->mLength = length;
	
	CHE_WideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	return tmpString;
}

CHE_WideString & CHE_WideString::operator+=( wchar_t wch )
{
	if ( wch == 0 )
	{
		return *this;
	}

	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<HE_WideStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = 1;
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >(2);
		mpData->mpStr[0] = wch;
		mpData->mpStr[1] = '\0';
	}else{
		if ( mpData->mpStr == nullptr )
		{
			mpData->mLength = 1;
			mpData->mRefCount.AddRef();
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >(2);
			mpData->mpStr[0] = wch;
			mpData->mpStr[1] = '\0';
			return *this;
		}
		
		wchar_t *  pTempStr = GetAllocator()->NewArray<wchar_t >( wcslen(mpData->mpStr)+2 );
		wcscpy( pTempStr, mpData->mpStr );
		
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
				mpData->mpStr = nullptr;
			}
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
		}else{
			mpData = GetAllocator()->New<HE_WideStringData>();
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
			mpData->mpStr = nullptr;
		}
		
		size_t dwBufferSize = wcslen( pTempStr )+2;
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( dwBufferSize );
		wcscpy( mpData->mpStr, pTempStr );
		mpData->mpStr[dwBufferSize-2] = wch;
		mpData->mpStr[dwBufferSize-1] = '\0';
		mpData->mLength = dwBufferSize-1;
		
		GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	}
	return *this;
}

CHE_WideString & CHE_WideString::operator+=( wchar_t const * pstr )
{
	if ( pstr == nullptr )
	{
		return *this;
	}

	if ( wcslen( pstr ) == 0 )
	{
		return *this;
	}

	if ( mpData == nullptr )
	{
		size_t length = wcslen(pstr);
		mpData = GetAllocator()->New<HE_WideStringData>();
		mpData->mRefCount.AddRef();
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
		wcscpy( mpData->mpStr, pstr );
		mpData->mLength = length;
	}else{
		if ( mpData->mpStr == nullptr )
		{
			size_t length = wcslen(pstr);
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
			wcscpy( mpData->mpStr, pstr );
			mpData->mLength = length;
			mpData->mRefCount.AddRef();
			return *this;
		}
		
		wchar_t * pTempStr = GetAllocator()->NewArray<wchar_t >( wcslen(mpData->mpStr)+1 );
		wcscpy( pTempStr, mpData->mpStr );
		
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
				mpData->mpStr = nullptr;
			}
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
		}else{
			mpData = GetAllocator()->New<HE_WideStringData>();
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
			mpData->mpStr = nullptr;
		}
		
		size_t length = wcslen(pTempStr) + wcslen(pstr);
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
		wcscpy( mpData->mpStr, pTempStr );
		wcscat( mpData->mpStr, pstr );
		mpData->mLength = length;
	}
	
	return *this;
}

CHE_WideString & CHE_WideString::operator+=( const CHE_WideString & str )
{
	if ( str.mpData == nullptr || str.mpData->mpStr == nullptr )
	{
		return *this;
	}

	if ( this == &str )
	{
		wchar_t * pTempStr = GetAllocator()->NewArray<wchar_t >( wcslen(mpData->mpStr)+1 );
		wcscpy( pTempStr, mpData->mpStr );

		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 )
		{
			if ( mpData->mpStr )
			{
				GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
				mpData->mpStr = nullptr;
			}
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
		}else{
			mpData = GetAllocator()->New<HE_WideStringData>();
			mpData->mLength = 0;
			mpData->mRefCount.AddRef();
			mpData->mpStr = nullptr;
		}

		size_t length = wcslen(pTempStr) * 2;
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
		wcscpy( mpData->mpStr, pTempStr );
		wcscat( mpData->mpStr, pTempStr );

		mpData->mLength = length;

		GetAllocator()->DeleteArray<wchar_t >( pTempStr );
		pTempStr = nullptr;
		
		return *this;
	}else{
		if ( mpData == str.mpData )
		{
			wchar_t *  pTempStr = GetAllocator()->NewArray<wchar_t >( wcslen(mpData->mpStr)+1 );
			wcscpy( pTempStr, mpData->mpStr );

			mpData->mRefCount.DecRef();
			mpData->mRefCount.DecRef();
			if ( mpData->mRefCount == 0 )
			{
				if ( mpData->mpStr )
				{
					GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
					mpData->mpStr = nullptr;
				}
				mpData->mLength = 0;
				mpData->mRefCount.AddRef();
				mpData->mRefCount.AddRef();
			}else{
				mpData = GetAllocator()->New<HE_WideStringData>();
				mpData->mRefCount.AddRef();
				mpData->mRefCount.AddRef();
				mpData->mpStr = nullptr;
				mpData->mLength = 0;
			}

			size_t length = wcslen(pTempStr) * 2;
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
			wcscpy( mpData->mpStr, pTempStr );
			wcscat( mpData->mpStr, pTempStr );
			
			mpData->mLength = length;
			
			GetAllocator()->DeleteArray<wchar_t >( pTempStr );
			pTempStr = nullptr;
			
			return *this;
		}else{
			if ( mpData->mpStr == nullptr )
			{
				size_t length = wcslen(str.mpData->mpStr);
				mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
				wcscpy( mpData->mpStr, str.mpData->mpStr );
				return *this;
			}

			wchar_t * pTempStr = GetAllocator()->NewArray<wchar_t >( wcslen(mpData->mpStr)+1 );
			wcscpy( pTempStr, mpData->mpStr );

			if ( mpData->mRefCount == 1 )
			{
				if ( mpData->mpStr )
				{
					GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
					mpData->mpStr = nullptr;
					mpData->mLength = 0;
				}
			}else{
				mpData->mRefCount.DecRef();
				mpData = GetAllocator()->New<HE_WideStringData>();
				mpData->mLength = 0;
				mpData->mRefCount.AddRef();
				mpData->mpStr = nullptr;
			}

			size_t length = wcslen( str.mpData->mpStr );
			length += wcslen( pTempStr );
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
			wcscpy( mpData->mpStr, pTempStr );
			wcscat( mpData->mpStr, str.mpData->mpStr );

			GetAllocator()->DeleteArray<wchar_t >( pTempStr );
			pTempStr = nullptr;
			
			mpData->mLength = length;
			
			return *this;
		}
	}
}

bool CHE_WideString::operator!=( wchar_t  wch )const
{
	if ( mpData == nullptr )
	{
		if ( wch == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( mpData->mLength != 1 )
		{
			return true;
		}else{
			if ( mpData->mpStr[0] == wch )
			{
				return false;
			}else{
				return true;
			}
		}
	}
}

bool CHE_WideString::operator!=( wchar_t const * pstr  )const
{
	if ( mpData == nullptr )
	{
		if ( pstr == nullptr || wcslen( pstr ) == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( pstr == nullptr )
		{
			if ( mpData->mpStr == nullptr )
			{
				return false;
			}else{
				return true;
			}
		}else{
			if ( wcscmp( mpData->mpStr, pstr ) == 0 )
			{
				return false;
			}else{
				return true;
			}	
		}
	}
}

bool CHE_WideString::operator!=( const CHE_WideString & str )const
{
	if ( this == &str )
	{
		return false;
	}else{
		if ( mpData == str.mpData )
		{
			return false;
		}else{
			if ( mpData == nullptr )
			{
				return true;
			}else{
				if ( wcscmp( mpData->mpStr, str.mpData->mpStr ) == 0 )
				{
					return false;
				}else{
					return true;
				}
			}
		}
	}	
}

bool operator==( wchar_t  wch, CHE_WideString & str )
{
	return ( str == wch );
}

bool operator==( wchar_t const * pstr, const CHE_WideString & str )
{
	return ( str == pstr );
}

CHE_WideString operator+( wchar_t wch, const CHE_WideString & str )
{
	CHE_WideString tempStr( wch, str.GetAllocator() );
	tempStr+=str;
	return tempStr;
}

CHE_WideString operator+( wchar_t const * pstr, const CHE_WideString & str )
{
	CHE_WideString tempStr( pstr, wcslen(pstr), str.GetAllocator() );
	tempStr+= str;
	return tempStr;
}

bool operator!=( wchar_t  wch, const CHE_WideString & str )
{
	return ( str != wch );
}

bool operator!=( wchar_t const * pstr, const CHE_WideString & str )
{
	return ( str != pstr );
}

uint32 StringToUINT32( const CHE_ByteString & str )
{
    if ( str.GetLength() == 0 )
    {
        return 0;
    }
    size_t length = 4;
    if ( str.GetLength() < length )
    {
        length = str.GetLength();
    }
    uint32 valRet = 0;
    for ( size_t i = length; i > 0; --i )
    {
        valRet = valRet<<8;
        valRet |= str.GetData()[i-1];
    }
    return valRet;
}
