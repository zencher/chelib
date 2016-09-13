#include "../include/che_base_string.h"

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>

//@zctodo:optimize:simpleness:for,class,CByteString:"make HE_ByteString a class, maintain the refcount and data by itself";
//@zctodo:optimize:performance:for,class,CByteString:"reduce memory copy times";
//@zctodo:optimize:simpleness:for,class,CWideString:"make HE_ByteString a class, maintain the refcount and data by itself"
//@zctodo:optimize:performance:for,class,CWideString:"reduce memory copy times";

namespace chelib
{

struct ByteStringData
{
    ByteStringData()
    {
        mpStr = nullptr;
        mLength = 0;
    }
    char *      mpStr;
    size_t      mLength;
    CRefCount   mRefCount;
};

CByteString::CByteString( char ch, CAllocator * pAllocator )
  : CObject( pAllocator )
{
	if ( ch == '\0' )
	{
		mpData = nullptr;
	}else{
		mpData = GetAllocator()->New<ByteStringData>();
		mpData->mLength = 1;
		mpData->mRefCount.AddRef();
		mpData->mpStr[0] = ch;
		mpData->mpStr = GetAllocator()->NewArray<char>( 2 );
		mpData->mpStr[1] = '\0';
	}
}

CByteString::CByteString( char const * pstr, size_t length /* = 0 */, CAllocator * pAllocator /*= nullptr*/ )
  : CObject( pAllocator )
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
		mpData = GetAllocator()->New<ByteStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = length;
		mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
		memcpy( mpData->mpStr, pstr, length );
		mpData->mpStr[length] = '\0';
	}else{
		mpData = nullptr;
	}
}

CByteString::CByteString( const CByteString & str )
  : CObject( str.GetAllocator() )
{
	if ( str.mpData == nullptr )
	{
		mpData = nullptr;
	}else{
		mpData = str.mpData;
		str.mpData->mRefCount.AddRef();
	}
}

void CByteString::Clear()
{
	if ( mpData )
	{
		mpData->mRefCount.DecRef();

		if ( mpData->mRefCount == 0 && mpData->mpStr )
		{
			GetAllocator()->DeleteArray<char>( mpData->mpStr );
			mpData->mpStr = nullptr;
			GetAllocator()->Delete<ByteStringData>( mpData );
		}
		mpData = nullptr;
	}
}

CByteString & CByteString::operator=( char ch )
{
	Clear();

	if ( ch == '\0' )
	{
		return *this;
	}
	
	mpData = GetAllocator()->New<ByteStringData>();
	mpData->mLength = 1;
	mpData->mRefCount.AddRef();
	mpData->mpStr = GetAllocator()->NewArray<char>( 2 );
	mpData->mpStr[0] = ch;
	mpData->mpStr[1] = '\0';

	return *this;
}

CByteString & CByteString::operator=( char const * pstr )
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

	mpData = GetAllocator()->New<ByteStringData>();
	mpData->mRefCount.AddRef();
	mpData->mLength = length;
	mpData->mpStr = GetAllocator()->NewArray<char>( length+1 );
	strcpy( mpData->mpStr, pstr );

	return *this;
}

CByteString & CByteString::operator=( const CByteString & str )
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

bool CByteString::operator==( char ch ) const
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

bool CByteString::operator==( char const * pstr ) const
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

bool CByteString::operator==( const CByteString & str ) const
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

bool CByteString::SetData( BYTE * pData, size_t length )
{
	if ( pData == nullptr || length == 0 )
	{
		Clear();
		return TRUE;
	}
	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<ByteStringData>();
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
			mpData = GetAllocator()->New<ByteStringData>();
			mpData->mRefCount.AddRef();
			mpData->mLength = length;
			mpData->mpStr= GetAllocator()->NewArray<char>( length+1 );
			memcpy( mpData->mpStr, pData, length );
			mpData->mpStr[length] = '\0';
		}
	}
	return TRUE;
}
    
char const * CByteString::GetData() const
{
    return mpData ? mpData->mpStr : nullptr;
}

size_t CByteString::GetLength() const
{
	if ( mpData == nullptr )
	{
		return 0;
	}else{
		return mpData->mLength;
	}
}

char CByteString::operator[]( size_t index ) const
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

int32 CByteString::GetInteger() const
{
	return atoi( GetData() );
}

FLOAT CByteString::GetFloat() const
{
	return atof( GetData() );
}

CByteString CByteString::operator+( char ch )
{
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CByteString( ch, GetAllocator() );
	}

	size_t length = mpData->mLength + 2;
	char * pstr = GetAllocator()->NewArray<char>( length );

	strcpy( pstr, mpData->mpStr );
	pstr[length-2] = ch;
	pstr[length-1] = '\0';

	CByteString str( pstr, length-1, GetAllocator() );
	GetAllocator()->DeleteArray<char>( pstr );
	return str;
}


CByteString CByteString::operator+( char const * pstr )
{
	if ( pstr == nullptr )
	{
		return CByteString( *this );
	}

	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CByteString( pstr, strlen(pstr), GetAllocator() );
	}

	size_t length = mpData->mLength + strlen(pstr) + 1;
	char * pTmpStr = GetAllocator()->NewArray<char>( length );

	strcpy( pTmpStr, mpData->mpStr );
	strcat( pTmpStr, pstr );

	CByteString str( pTmpStr, strlen(pTmpStr), GetAllocator() );
	GetAllocator()->DeleteArray<char>( pTmpStr );
	return str;
}

CByteString CByteString::operator+( const CByteString & str )
{
	if ( str.mpData == nullptr )
	{
		return CByteString( *this );
	}
	
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CByteString( str );
	}
	
	size_t length = mpData->mLength + str.mpData->mLength + 1;
	char * pTempStr = GetAllocator()->NewArray<char>( length );
	strcpy( pTempStr, mpData->mpStr );
	strcat( pTempStr, str.mpData->mpStr );
	
	CByteString strRet(pTempStr);
	GetAllocator()->DeleteArray<char>( pTempStr );
	return strRet;
}

CByteString & CByteString::operator+=( char ch )
{
	if ( ch == 0 )
	{
		return *this;
	}

	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<ByteStringData>();
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
			mpData = GetAllocator()->New<ByteStringData>();
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

CByteString & CByteString::operator+=( char const * pstr )
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
		mpData = GetAllocator()->New<ByteStringData>();
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
			mpData = GetAllocator()->New<ByteStringData>();
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

CByteString & CByteString::operator+=( const CByteString & str )
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
			mpData = GetAllocator()->New<ByteStringData>();
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
				mpData = GetAllocator()->New<ByteStringData>();
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
				mpData = GetAllocator()->New<ByteStringData>();
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

bool CByteString::operator!=( char ch )const
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

bool CByteString::operator!=( char const * pstr )const
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

bool CByteString::operator!=( const CByteString & str )const
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

bool operator==( char ch, CByteString & str )
{
	return ( str == ch );
}

bool operator==( char const * pstr, const CByteString & str )
{
	return ( str == pstr );
}

CByteString operator+( char ch, const CByteString & str )
{
	CByteString tempStr( ch, str.GetAllocator() );
	tempStr+=str;
	return tempStr;
}

CByteString operator+( char const * pstr, const CByteString & str )
{
	CByteString tempStr( pstr, strlen(pstr), str.GetAllocator() );
	tempStr+= str;
	return tempStr;
}

bool operator!=( char ch, const CByteString & str )
{
	return ( str != ch );
}

bool operator!=( char const * pstr, const CByteString & str )
{
	return ( str != pstr );
}



struct WideStringData
{
    WideStringData()
    {
        mpStr = nullptr;
        mLength = 0;
    }
    wchar_t *   mpStr;
    size_t      mLength;
    CRefCount   mRefCount;
};
    
CWideString::CWideString( wchar_t wch, CAllocator * pAllocator ) : CObject( pAllocator )
{
	if ( wch == '\0' )
	{
		mpData = nullptr;
	}else{
		mpData = GetAllocator()->New<WideStringData>();
		mpData->mLength = 1;
		mpData->mRefCount.AddRef();
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >(2);
		mpData->mpStr[0] = wch;
		mpData->mpStr[1] = '\0';
	}
}

CWideString::CWideString( wchar_t const * pstr, size_t length /* = 0 */,  CAllocator * pAllocator) : CObject( pAllocator )
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
		mpData = GetAllocator()->New<WideStringData>();
		mpData->mRefCount.AddRef();
		mpData->mLength = length;
		mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
		wcscpy( mpData->mpStr, pstr );
		mpData->mpStr[length] = '\0';
	}else{
		mpData = nullptr;
	}
}

CWideString::CWideString( const CWideString & str ) : CObject( str.GetAllocator() )
{
	if ( str.mpData == nullptr )
	{
		mpData = nullptr;
	}else{
		mpData = str.mpData;
		str.mpData->mRefCount.AddRef();
	}
}

void CWideString::Clear()
{
	if ( mpData )
	{
		mpData->mRefCount.DecRef();
		if ( mpData->mRefCount == 0 && mpData->mpStr )
		{
			GetAllocator()->DeleteArray<wchar_t >( mpData->mpStr );
			mpData->mpStr = nullptr;
			GetAllocator()->Delete<WideStringData>( mpData );
		}

		mpData = nullptr;
	}
}

CWideString & CWideString::operator=( wchar_t  wch )
{
	Clear();

	if ( wch == (wchar_t )0x0000 )
	{
		return *this;
	}

	mpData = GetAllocator()->New<WideStringData>();
	mpData->mLength = 1;
	mpData->mRefCount.AddRef();
	mpData->mpStr = GetAllocator()->NewArray<wchar_t >( 2 );
	mpData->mpStr[0] = wch;
	mpData->mpStr[1] = (wchar_t)0x0000;

	return *this;
}

CWideString& CWideString::operator=( wchar_t const * pstr )
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

	mpData = GetAllocator()->New<WideStringData>();
	mpData->mRefCount.AddRef();
	mpData->mLength = length;
	mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
	wcscpy( mpData->mpStr, pstr );
	return *this;
}

CWideString & CWideString::operator=( const CWideString & str )
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

bool CWideString::operator==( wchar_t wch )const
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

bool CWideString::operator==( wchar_t const * pstr )const
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

bool CWideString::operator==( const CWideString & str )const
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

bool CWideString::SetData( wchar_t * pData, size_t length )
{
	if ( pData == nullptr || length == 0 )
	{
		Clear();
		return TRUE;
	}
	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<WideStringData>();
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
			mpData = GetAllocator()->New<WideStringData>();
			mpData->mRefCount.AddRef();
			mpData->mLength = length;
			mpData->mpStr = GetAllocator()->NewArray<wchar_t >( length+1 );
			memcpy( mpData->mpStr, pData, length * sizeof(wchar_t) );
			mpData->mpStr[length] = '\0';
		}
	}
	return TRUE;
}
    
wchar_t const * CWideString::GetData() const
{
    return ( mpData ) ? mpData->mpStr : nullptr;
}

size_t CWideString::GetLength() const
{
	if ( mpData == nullptr )
	{
		return 0;
	}else{
		return mpData->mLength;
	}
}

wchar_t CWideString::operator[]( size_t index )const
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

int32 CWideString::GetInteger() const
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

FLOAT CWideString::GetFloat() const
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

CWideString CWideString::operator+( wchar_t wch )
{
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CWideString( wch, GetAllocator() );
	}

	size_t length = mpData->mLength + 2;
	wchar_t  * pTempStr = GetAllocator()->NewArray<wchar_t >(length);

	wcscpy( pTempStr, mpData->mpStr );
	pTempStr[length-2] = wch;
	pTempStr[length-1] = '\0';

	CWideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	return tmpString;
}

CWideString CWideString::operator+( wchar_t const * pstr )
{
	if ( pstr == nullptr )
	{
		return CWideString(*this);
	}

	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CWideString( pstr );
	}
	
	size_t length = mpData->mLength + wcslen(pstr);
	wchar_t  * pTempStr = GetAllocator()->NewArray<wchar_t >( length+1 );
	wcscpy( pTempStr, mpData->mpStr );
	wcscat( pTempStr, pstr );
    mpData->mLength = length;

	CWideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	return tmpString;
}

CWideString CWideString::operator+( const CWideString & str )
{
	if ( str.mpData == nullptr )
	{
		return CWideString(*this);
	}
	
	if ( mpData == nullptr || mpData->mpStr == nullptr )
	{
		return CWideString( str );
	}
	
	size_t length = mpData->mLength + str.mpData->mLength;
	wchar_t  * pTempStr = GetAllocator()->NewArray<wchar_t >( length+1 );
	
	wcscpy( pTempStr, mpData->mpStr );
	wcscat( pTempStr, str.mpData->mpStr );
    mpData->mLength = length;
	
	CWideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<wchar_t >( pTempStr );
	return tmpString;
}

CWideString & CWideString::operator+=( wchar_t wch )
{
	if ( wch == 0 )
	{
		return *this;
	}

	if ( mpData == nullptr )
	{
		mpData = GetAllocator()->New<WideStringData>();
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
			mpData = GetAllocator()->New<WideStringData>();
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

CWideString & CWideString::operator+=( wchar_t const * pstr )
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
		mpData = GetAllocator()->New<WideStringData>();
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
			mpData = GetAllocator()->New<WideStringData>();
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

CWideString & CWideString::operator+=( const CWideString & str )
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
			mpData = GetAllocator()->New<WideStringData>();
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
				mpData = GetAllocator()->New<WideStringData>();
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
				mpData = GetAllocator()->New<WideStringData>();
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

bool CWideString::operator!=( wchar_t  wch )const
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

bool CWideString::operator!=( wchar_t const * pstr  )const
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

bool CWideString::operator!=( const CWideString & str )const
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

bool operator==( wchar_t  wch, CWideString & str )
{
	return ( str == wch );
}

bool operator==( wchar_t const * pstr, const CWideString & str )
{
	return ( str == pstr );
}

CWideString operator+( wchar_t wch, const CWideString & str )
{
	CWideString tempStr( wch, str.GetAllocator() );
	tempStr+=str;
	return tempStr;
}

CWideString operator+( wchar_t const * pstr, const CWideString & str )
{
	CWideString tempStr( pstr, wcslen(pstr), str.GetAllocator() );
	tempStr+= str;
	return tempStr;
}

bool operator!=( wchar_t  wch, const CWideString & str )
{
	return ( str != wch );
}

bool operator!=( wchar_t const * pstr, const CWideString & str )
{
	return ( str != pstr );
}

uint32 StringToUINT32( const CByteString & str )
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
    
}//end of namespce chelib