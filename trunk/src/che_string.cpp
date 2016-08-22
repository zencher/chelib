#include "../include/che_string.h"

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>

CHE_ByteString::CHE_ByteString( char ch, CHE_Allocator * pAllocator )
: CHE_Object( pAllocator )
{
	if ( ch == '\0' )
	{
		m_lpData = nullptr;
	}else{
		m_lpData = GetAllocator()->New<HE_ByteStringData>();
		m_lpData->m_dwLength = 1;
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( 2 );
		m_lpData->m_lpString[1] = '\0';
	}
}

CHE_ByteString::CHE_ByteString( char const * lpStr, size_t nStrSize /* = 0 */, CHE_Allocator * pAllocator /*= nullptr*/ )
: CHE_Object( pAllocator )
{
	if ( lpStr == nullptr )
	{
		m_lpData = nullptr;
		return;
	}

	size_t nStrlen = 0;
	if ( nStrSize == 0 )
	{
		nStrlen = strlen( lpStr );
	}else{
		nStrlen = nStrSize;
	}

	if ( nStrlen > 0 )
	{
		m_lpData = GetAllocator()->New<HE_ByteStringData>();
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( nStrlen+1 );
		memcpy( m_lpData->m_lpString, lpStr, nStrlen );
		m_lpData->m_lpString[nStrlen] = '\0';
	}else{
		m_lpData = nullptr;
	}
}

CHE_ByteString::CHE_ByteString( const CHE_ByteString & str )
: CHE_Object( str.GetAllocator() )
{
	if ( str.m_lpData == nullptr )
	{
		m_lpData = nullptr;
	}else{
		m_lpData = str.m_lpData;
		str.m_lpData->m_dwRef.AddRef();
	}
}

void CHE_ByteString::Clear()
{
	if ( m_lpData )
	{
		m_lpData->m_dwRef.DecRef();

		if ( m_lpData->m_dwRef == 0 && m_lpData->m_lpString )
		{
			GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
			m_lpData->m_lpString = nullptr;
			GetAllocator()->Delete<HE_ByteStringData>( m_lpData );
		}
		m_lpData = nullptr;
	}
}

CHE_ByteString & CHE_ByteString::operator=( char ch )
{
	Clear();

	if ( ch == '\0' )
	{
		return *this;
	}
	
	m_lpData = GetAllocator()->New<HE_ByteStringData>();
	m_lpData->m_dwLength = 1;
	m_lpData->m_dwRef.AddRef();
	m_lpData->m_lpString = GetAllocator()->NewArray<char>( 2 );
	m_lpData->m_lpString[0] = ch;
	m_lpData->m_lpString[1] = '\0';

	return *this;
}

CHE_ByteString & CHE_ByteString::operator=( char const * lpStr )
{
	Clear();

	if ( lpStr == nullptr )
	{
		return *this;
	}

	size_t nStrlen = strlen( lpStr );
	if ( nStrlen == 0 )
	{
		return *this;
	}

	m_lpData = GetAllocator()->New<HE_ByteStringData>();
	m_lpData->m_dwRef.AddRef();
	m_lpData->m_dwLength = nStrlen;
	m_lpData->m_lpString = GetAllocator()->NewArray<char>( nStrlen+1 );
	strcpy( m_lpData->m_lpString, lpStr );

	return *this;
}

CHE_ByteString & CHE_ByteString::operator=( const CHE_ByteString & str )
{
	if ( this == &str )
	{
		return *this;
	}

	if ( m_lpData == str.m_lpData )
	{
		return *this;
	}

	Clear();

	if ( str.m_lpData )
	{
		m_lpData = str.m_lpData;
		str.m_lpData->m_dwRef.AddRef();
	}

	return *this;
}

bool CHE_ByteString::operator==( char ch )const
{
	if ( m_lpData == nullptr || strlen( m_lpData->m_lpString ) == 0 )
	{
		if ( ch == '\0' )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( m_lpData->m_dwLength != 1 )
		{
			return false;
		}else{
			if ( m_lpData->m_lpString[0] == ch )
			{
				return true;
			}else{
				return false;
			}
		}
	}
}

bool CHE_ByteString::operator==( char const * lpStr )const
{
	if ( m_lpData == nullptr || strlen( m_lpData->m_lpString ) == 0 )
	{
		if ( lpStr == nullptr || strlen( lpStr ) == 0 )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( lpStr == nullptr )
		{
			if ( m_lpData->m_lpString == nullptr )
			{
				return true;
			}else{
				return false;
			}
		}else{
			if ( strcmp( m_lpData->m_lpString, lpStr ) == 0 )
			{
				return true;
			}else{
				return false;
			}	
		}
	}
}

bool CHE_ByteString::operator==( const CHE_ByteString & str )const
{
	if ( this == &str )
	{
		return true;
	}else{
		if ( m_lpData == str.m_lpData )
		{
			return true;
		}else{
			if ( m_lpData == nullptr )
			{
				return false;
			}else{
				if ( strcmp( m_lpData->m_lpString, str.m_lpData->m_lpString ) == 0 )
				{
					return true;
				}else{
					return false;
				}
			}
		}
	}
}

bool CHE_ByteString::SetData( BYTE * pData, size_t size )
{
	if ( pData == nullptr || size == 0 )
	{
		Clear();
		return TRUE;
	}
	if ( m_lpData == nullptr )
	{
		m_lpData = GetAllocator()->New<HE_ByteStringData>();
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_dwLength = size;
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( size+1 );
		memcpy( m_lpData->m_lpString, pData, size );
		m_lpData->m_lpString[size] = '\0';
	}else{
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
			}
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString= GetAllocator()->NewArray<char>( size+1 );
			memcpy( m_lpData->m_lpString, pData, size );
			m_lpData->m_lpString[size] = '\0';
		}else {
			m_lpData = GetAllocator()->New<HE_ByteStringData>();
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString= GetAllocator()->NewArray<char>( size+1 );
			memcpy( m_lpData->m_lpString, pData, size );
			m_lpData->m_lpString[size] = '\0';
		}
	}
	return TRUE;
}

size_t CHE_ByteString::GetLength() const
{
	if ( m_lpData == nullptr )
	{
		return 0;
	}else{
		return m_lpData->m_dwLength;
	}
}

char CHE_ByteString::operator[]( size_t index )const
{
	if ( m_lpData != nullptr )
	{
		if ( index >= m_lpData->m_dwLength )
		{
			return 0;
		}else if ( m_lpData->m_lpString )
		{
			return m_lpData->m_lpString[index];
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
	if ( m_lpData == nullptr || m_lpData->m_lpString == nullptr )
	{
		return CHE_ByteString( ch, GetAllocator() );
	}

	size_t dwStrlen = m_lpData->m_dwLength + 2;
	char * pTempStr = GetAllocator()->NewArray<char>( dwStrlen );

	strcpy( pTempStr, m_lpData->m_lpString );
	pTempStr[dwStrlen-2] = ch;
	pTempStr[dwStrlen-1] = '\0';

	CHE_ByteString tmpString( pTempStr, dwStrlen-1, GetAllocator() );
	GetAllocator()->DeleteArray<char>( pTempStr );
	return tmpString;
}

CHE_ByteString CHE_ByteString::operator+( char const * lpStr )
{
	if ( lpStr == nullptr )
	{
		return CHE_ByteString( *this );
	}

	if ( m_lpData == nullptr || m_lpData->m_lpString == nullptr )
	{
		return CHE_ByteString( lpStr, strlen(lpStr), GetAllocator() );
	}

	size_t dwStrlen = m_lpData->m_dwLength + strlen(lpStr) + 1;
	char * pTempStr = GetAllocator()->NewArray<char>( dwStrlen );

	strcpy( pTempStr, m_lpData->m_lpString );
	strcat( pTempStr, lpStr );

	CHE_ByteString tmpString( pTempStr, strlen(pTempStr), GetAllocator() );
	GetAllocator()->DeleteArray<char>( pTempStr );
	return tmpString;
}

CHE_ByteString CHE_ByteString::operator+( const CHE_ByteString& str )
{
	if ( str.m_lpData == nullptr )
	{
		return CHE_ByteString( *this );
	}
	
	if ( m_lpData == nullptr || m_lpData->m_lpString == nullptr )
	{
		return CHE_ByteString( str );
	}
	
	size_t dwStrlen = m_lpData->m_dwLength + str.m_lpData->m_dwLength + 1;
	char * pTempStr = GetAllocator()->NewArray<char>( dwStrlen );
	strcpy( pTempStr, m_lpData->m_lpString );
	strcat( pTempStr, str.m_lpData->m_lpString );
	
	CHE_ByteString tmpString(pTempStr);
	GetAllocator()->DeleteArray<char>( pTempStr );
	return tmpString;
}

CHE_ByteString& CHE_ByteString::operator+=( char ch )
{
	if ( ch == 0 )
	{
		return *this;
	}

	if ( m_lpData == nullptr )
	{
		m_lpData = GetAllocator()->New<HE_ByteStringData>();
		//m_lpData->m_dwRef = 1;
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_dwLength = 1;
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( 2 );
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString[1] = '\0';
	}else{
		if ( m_lpData->m_lpString == nullptr )
		{
			m_lpData->m_dwLength = 1;
			//m_lpData->m_dwRef = 1;
			//m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = GetAllocator()->NewArray<char>( 2 );
			m_lpData->m_lpString[0] = ch;
			m_lpData->m_lpString[1] = '\0';
			return *this;
		}
		char * pTempStr = GetAllocator()->NewArray<char>( strlen(m_lpData->m_lpString)+2 );
		strcpy( pTempStr, m_lpData->m_lpString );
		//m_lpData->m_dwRef--;
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
				m_lpData->m_lpString = nullptr;
			}
			m_lpData->m_dwLength = 0;
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
		}else{
			m_lpData = GetAllocator()->New<HE_ByteStringData>();
			m_lpData->m_dwLength = 0;
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = nullptr;
		}
		
		size_t dwBufferSize = strlen( pTempStr )+2;
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( dwBufferSize );
		strcpy( m_lpData->m_lpString, pTempStr );
		m_lpData->m_lpString[dwBufferSize-2] = ch;
		m_lpData->m_lpString[dwBufferSize-1] = '\0';
		m_lpData->m_dwLength = dwBufferSize-1;
		
		GetAllocator()->DeleteArray<char>( pTempStr );
	}
	return *this;
}

CHE_ByteString& CHE_ByteString::operator+=( char const * lpStr )
{
	if ( lpStr == nullptr )
	{
		return *this;
	}

	if ( strlen( lpStr ) == 0 )
	{
		return *this;
	}

	if ( m_lpData == nullptr )
	{
		size_t iStrlen = strlen(lpStr);
		m_lpData = GetAllocator()->New<HE_ByteStringData>();
		//m_lpData->m_dwRef = 1;
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( iStrlen+1 );
		strcpy( m_lpData->m_lpString, lpStr );
		m_lpData->m_dwLength = iStrlen;
	}else{
		if ( m_lpData->m_lpString == nullptr )
		{
			size_t dwStrlen = strlen(lpStr);
			m_lpData->m_lpString = GetAllocator()->NewArray<char>( dwStrlen+1 );
			strcpy( m_lpData->m_lpString, lpStr );
			m_lpData->m_dwLength = dwStrlen;
			//m_lpData->m_dwRef = 1;
			return *this;
		}

		char * pTempStr = GetAllocator()->NewArray<char>( strlen(m_lpData->m_lpString)+1 );
		strcpy( pTempStr, m_lpData->m_lpString );

		//m_lpData->m_dwRef--;
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
				m_lpData->m_lpString = nullptr;
			}
			m_lpData->m_dwLength = 0;
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
		}else{
			m_lpData = GetAllocator()->New<HE_ByteStringData>();
			m_lpData->m_dwLength = 0;
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = nullptr;
		}

		size_t dwStrlen = strlen(pTempStr) + strlen(lpStr);
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( dwStrlen+1 );
		strcpy( m_lpData->m_lpString, pTempStr );
		strcat( m_lpData->m_lpString, lpStr );
		m_lpData->m_dwLength = dwStrlen;
	}
	
	return *this;
}

CHE_ByteString& CHE_ByteString::operator+=( const CHE_ByteString& str )
{
	if ( str.m_lpData == nullptr || str.m_lpData->m_lpString == nullptr )
	{
		return *this;
	}

	if ( this == &str )
	{
		char * pTempStr = GetAllocator()->NewArray<char>( strlen(m_lpData->m_lpString)+1 );
		strcpy( pTempStr, m_lpData->m_lpString );

		//m_lpData->m_dwRef--;
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
				m_lpData->m_lpString = nullptr;
			}
			m_lpData->m_dwLength = 0;
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
		}else{
			m_lpData = GetAllocator()->New<HE_ByteStringData>();
			m_lpData->m_dwLength = 0;
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = nullptr;
		}

		size_t iBufferSize = strlen(pTempStr) * 2 + 1;
		m_lpData->m_lpString = GetAllocator()->NewArray<char>( iBufferSize );
		strcpy( m_lpData->m_lpString, pTempStr );
		strcat( m_lpData->m_lpString, pTempStr );

		m_lpData->m_dwLength = iBufferSize-1;

		GetAllocator()->DeleteArray<char>( pTempStr );
		pTempStr = nullptr;
		
		return *this;
	}else{
		if ( m_lpData == str.m_lpData )
		{
			char * pTempStr = GetAllocator()->NewArray<char>( strlen(m_lpData->m_lpString)+1 );
			strcpy( pTempStr, m_lpData->m_lpString );

			//m_lpData->m_dwRef-=2;
			m_lpData->m_dwRef.DecRef();
			m_lpData->m_dwRef.DecRef();
			if ( m_lpData->m_dwRef == 0 )
			{
				if ( m_lpData->m_lpString )
				{
					GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
					m_lpData->m_lpString = nullptr;
				}
				m_lpData->m_dwLength = 0;
				//m_lpData->m_dwRef = 2;
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_dwRef.AddRef();
			}else{
				m_lpData = GetAllocator()->New<HE_ByteStringData>();
				//m_lpData->m_dwRef = 2;
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_lpString = nullptr;
				m_lpData->m_dwLength = 0;
			}

			size_t iBufferSize = strlen(pTempStr) * 2 + 1;
			m_lpData->m_lpString = GetAllocator()->NewArray<char>( iBufferSize );
			strcpy( m_lpData->m_lpString, pTempStr );
			strcat( m_lpData->m_lpString, pTempStr );
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			GetAllocator()->DeleteArray<char>( pTempStr );
			pTempStr = nullptr;
			
			return *this;
		}else{
			if ( m_lpData->m_lpString == nullptr )
			{
				size_t iBufferSize = strlen(str.m_lpData->m_lpString)+1;
				m_lpData->m_lpString = GetAllocator()->NewArray<char>( iBufferSize );
				strcpy( m_lpData->m_lpString, str.m_lpData->m_lpString );
				return *this;
			}

			char * pTempStr = GetAllocator()->NewArray<char>( strlen(m_lpData->m_lpString)+1 );
			strcpy( pTempStr, m_lpData->m_lpString );

			if ( m_lpData->m_dwRef == 1 )
			{
				if ( m_lpData->m_lpString )
				{
					GetAllocator()->DeleteArray<char>( m_lpData->m_lpString );
					m_lpData->m_lpString = nullptr;
					m_lpData->m_dwLength = 0;
					//m_lpData->m_dwRef = 1;
				}
			}else{
				//m_lpData->m_dwRef--;
				m_lpData->m_dwRef.DecRef();
				m_lpData = GetAllocator()->New<HE_ByteStringData>();
				m_lpData->m_dwLength = 0;
				//m_lpData->m_dwRef = 1;
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_lpString = nullptr;
			}

			size_t iBufferSize = strlen( str.m_lpData->m_lpString );
			iBufferSize += strlen( pTempStr )+1;
			m_lpData->m_lpString = GetAllocator()->NewArray<char>( iBufferSize );
			strcpy( m_lpData->m_lpString, pTempStr );
			strcat( m_lpData->m_lpString, str.m_lpData->m_lpString );

			GetAllocator()->DeleteArray<char>( pTempStr );
			pTempStr = nullptr;
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			return *this;
		}
	}
}

bool CHE_ByteString::operator!=( char ch )const
{
	if ( m_lpData == nullptr )
	{
		if ( ch == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( m_lpData->m_dwLength != 1 )
		{
			return true;
		}else{
			if ( m_lpData->m_lpString[0] == ch )
			{
				return false;
			}else{
				return true;
			}
		}
	}
}

bool CHE_ByteString::operator!=( char const * lpStr )const
{
	if ( m_lpData == nullptr )
	{
		if ( lpStr == nullptr || strlen( lpStr ) == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( lpStr == nullptr )
		{
			if ( m_lpData->m_lpString == nullptr )
			{
				return false;
			}else{
				return true;
			}
		}else{
			if ( strcmp( m_lpData->m_lpString, lpStr ) == 0 )
			{
				return false;
			}else{
				return true;
			}	
		}
	}
}

bool CHE_ByteString::operator!=( const CHE_ByteString& str )const
{
	if ( this == &str )
	{
		return false;
	}else{
		if ( m_lpData == str.m_lpData )
		{
			return false;
		}else{
			if ( m_lpData == nullptr )
			{
				return true;
			}else{
				if ( strcmp( m_lpData->m_lpString, str.m_lpData->m_lpString ) == 0 )
				{
					return false;
				}else{
					return true;
				}
			}
		}
	}	
}

bool operator==( char ch, CHE_ByteString& str )
{
	return ( str == ch );
}

bool operator==( char const * lpStr, const CHE_ByteString& str )
{
	return ( str == lpStr );
}

CHE_ByteString operator+( char ch, const CHE_ByteString& str )
{
	CHE_ByteString tempStr( ch, str.GetAllocator() );
	tempStr+=str;
	return tempStr;
}

CHE_ByteString operator+( char const * lpStr, const CHE_ByteString& str )
{
	CHE_ByteString tempStr( lpStr, strlen(lpStr), str.GetAllocator() );
	tempStr+= str;
	return tempStr;
}

bool operator!=( char ch, const CHE_ByteString& str )
{
	return ( str != ch );
}

bool operator!=( char const * lpStr, const CHE_ByteString& str )
{
	return ( str != lpStr );
}


CHE_WideString::CHE_WideString( WCHAR wch, CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	if ( wch == '\0' )
	{
		m_lpData = nullptr;
	}else{
		m_lpData = GetAllocator()->New<HE_WideStringData>();
		m_lpData->m_dwLength = 1;
		//m_lpData->m_dwRef = 1;
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>(2);
		m_lpData->m_lpString[0] = wch;
		m_lpData->m_lpString[1] = '\0';
	}
}

CHE_WideString::CHE_WideString( PCWSTR lpwstr, size_t nStrSize /* = -1 */,  CHE_Allocator * pAllocator) : CHE_Object( pAllocator )
{
	if ( lpwstr == nullptr )
	{
		m_lpData = nullptr;
		return;
	}

	size_t nStrlen = 0;
	if ( nStrSize <= 0 )
	{
		nStrlen = wcslen( lpwstr );
	}else{
		nStrlen = nStrSize;
	}

	if ( nStrlen > 0 )
	{
		m_lpData = GetAllocator()->New<HE_WideStringData>();
		//m_lpData->m_dwRef = 1;
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( nStrlen+1 );
		wcscpy( m_lpData->m_lpString, lpwstr );
		m_lpData->m_lpString[nStrlen] = '\0';
	}else{
		m_lpData = nullptr;
	}
}

CHE_WideString::CHE_WideString( const CHE_WideString& wstr ) : CHE_Object( wstr.GetAllocator() )
{
	if ( wstr.m_lpData == nullptr )
	{
		m_lpData = nullptr;
	}else{
		m_lpData = wstr.m_lpData;
		//wstr.m_lpData->m_dwRef++;
		wstr.m_lpData->m_dwRef.AddRef();
	}
}

void CHE_WideString::Clear()
{
	if ( m_lpData )
	{
		//--(m_lpData->m_dwRef);
		m_lpData->m_dwRef.DecRef();

		if ( m_lpData->m_dwRef == 0 && m_lpData->m_lpString )
		{
			GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
			m_lpData->m_lpString = nullptr;
			GetAllocator()->Delete<HE_WideStringData>( m_lpData );
		}

		m_lpData = nullptr;
	}
}

CHE_WideString& CHE_WideString::operator=( WCHAR wch )
{
	Clear();

	if ( wch == (WCHAR)( 0x0000 ) )
	{
		return *this;
	}

	m_lpData = GetAllocator()->New<HE_WideStringData>();
	m_lpData->m_dwLength = 1;
	//m_lpData->m_dwRef = 1;
	m_lpData->m_dwRef.AddRef();
	m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( 2 );
	m_lpData->m_lpString[0] = wch;
	m_lpData->m_lpString[1] = (WCHAR)( 0x0000 );

	return *this;
}

CHE_WideString& CHE_WideString::operator=( PCWSTR lpWstr )
{
	Clear();

	if ( lpWstr == nullptr )
	{
		return *this;
	}

	size_t nStrlen = wcslen( lpWstr );
	if ( nStrlen == 0 )
	{
		return *this;
	}

	m_lpData = GetAllocator()->New<HE_WideStringData>();
	//m_lpData->m_dwRef = 1;
	m_lpData->m_dwRef.AddRef();
	m_lpData->m_dwLength = nStrlen;
	m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( nStrlen+1 );
	wcscpy( m_lpData->m_lpString, lpWstr );

	return *this;
}

CHE_WideString& CHE_WideString::operator=( const CHE_WideString& wstr )
{
	if ( this == &wstr )
	{
		return *this;
	}
	
	if ( m_lpData == wstr.m_lpData )
	{
		return *this;
	}

	Clear();

	if ( wstr.m_lpData )
	{
		m_lpData = wstr.m_lpData;
		//wstr.m_lpData->m_dwRef++;
		wstr.m_lpData->m_dwRef.AddRef();
	}

	return *this;
}

bool CHE_WideString::operator==( WCHAR wch )const
{
	if ( m_lpData == nullptr || wcslen( m_lpData->m_lpString ) == 0 )
	{
		if ( wch == '\0' )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( m_lpData->m_dwLength != 1 )
		{
			return false;
		}else{
			if ( m_lpData->m_lpString[0] == wch )
			{
				return true;
			}else{
				return false;
			}
		}
	}
}

bool CHE_WideString::operator==( PCWSTR lpWstr )const
{
	if ( m_lpData == nullptr || wcslen( m_lpData->m_lpString ) == 0 )
	{
		if ( lpWstr == nullptr || wcslen( lpWstr ) == 0 )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( lpWstr == nullptr )
		{
			if ( m_lpData->m_lpString == nullptr )
			{
				return true;
			}else{
				return false;
			}
		}else{
			if ( wcscmp( m_lpData->m_lpString, lpWstr ) == 0 )
			{
				return true;
			}else{
				return false;
			}	
		}
	}
}

bool CHE_WideString::operator==( const CHE_WideString& wstr )const
{
	if ( this == &wstr )
	{
		return true;
	}else{
		if ( m_lpData == wstr.m_lpData )
		{
			return true;
		}else{
			if ( m_lpData == nullptr )
			{
				return false;
			}else{
				if ( wcscmp( m_lpData->m_lpString, wstr.m_lpData->m_lpString ) == 0 )
				{
					return true;
				}else{
					return false;
				}
			}
		}
	}
}

bool	CHE_WideString::SetData( WCHAR * pData, size_t size )
{
	if ( pData == nullptr || size == 0 )
	{
		Clear();
		return TRUE;
	}
	if ( m_lpData == nullptr )
	{
		m_lpData = GetAllocator()->New<HE_WideStringData>();
		//m_lpData->m_dwRef = 1;
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_dwLength = size;
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>(size+1);
		memcpy( m_lpData->m_lpString, pData, size * sizeof( WCHAR ) );
		m_lpData->m_lpString[size] = '\0';
	}else{
		//m_lpData->m_dwRef--;
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
			}
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>(size+1);
			memcpy( m_lpData->m_lpString, pData, size * sizeof( WCHAR ) );
			m_lpData->m_lpString[size] = '\0';
		}else {
			m_lpData = GetAllocator()->New<HE_WideStringData>();
			//m_lpData->m_dwRef = 1;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( size+1 );
			memcpy( m_lpData->m_lpString, pData, size * sizeof( WCHAR ) );
			m_lpData->m_lpString[size] = '\0';
		}
	}
	return TRUE;
}

size_t CHE_WideString::GetLength() const
{
	if ( m_lpData == nullptr )
	{
		return 0;
	}else{
		return m_lpData->m_dwLength;
	}
}

WCHAR CHE_WideString::operator[]( size_t index )const
{
	if ( m_lpData != nullptr )
	{
		if ( index >= m_lpData->m_dwLength )
		{
			return 0;
		}else if ( m_lpData->m_lpString )
		{
			return m_lpData->m_lpString[index];
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
		bool	bNegative = false;
		int32 iValue = 0;
		bool bSign = false;
		WCHAR tmpChar = 0;
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
		bool	bNegative = false;
		bool bBegin = TRUE;
		size_t lPointBit = 1;
		FLOAT fValue = 0;
		bool bPoint = false;
		bool bSign = false;
		WCHAR tmpChar = 0;
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

CHE_WideString CHE_WideString::operator+( WCHAR wch )
{
	if ( m_lpData == nullptr || m_lpData->m_lpString == nullptr )
	{
		return CHE_WideString( wch, GetAllocator() );
	}

	size_t dwStrlen = m_lpData->m_dwLength + 2;
	WCHAR * pTempStr = GetAllocator()->NewArray<WCHAR>(dwStrlen);

	wcscpy( pTempStr, m_lpData->m_lpString );
	pTempStr[dwStrlen-2] = wch;
	pTempStr[dwStrlen-1] = '\0';

	CHE_WideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<WCHAR>( pTempStr );
	return tmpString;
}

CHE_WideString CHE_WideString::operator+( PCWSTR lpWstr )
{
	if ( lpWstr == nullptr )
	{
		return CHE_WideString(*this);
	}

	if ( m_lpData == nullptr || m_lpData->m_lpString == nullptr )
	{
		return CHE_WideString( lpWstr );
	}
	
	size_t dwStrlen = m_lpData->m_dwLength + wcslen(lpWstr) + 1;
	WCHAR * pTempStr = GetAllocator()->NewArray<WCHAR>( dwStrlen );
	
	wcscpy( pTempStr, m_lpData->m_lpString );
	wcscat( pTempStr, lpWstr );

	CHE_WideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<WCHAR>( pTempStr );
	return tmpString;
}

CHE_WideString CHE_WideString::operator+( const CHE_WideString& wstr )
{
	if ( wstr.m_lpData == nullptr )
	{
		return CHE_WideString(*this);
	}
	
	if ( m_lpData == nullptr || m_lpData->m_lpString == nullptr )
	{
		return CHE_WideString( wstr );
	}
	
	size_t dwStrlen = m_lpData->m_dwLength + wstr.m_lpData->m_dwLength + 1;
	WCHAR * pTempStr = GetAllocator()->NewArray<WCHAR>( dwStrlen );
	
	wcscpy( pTempStr, m_lpData->m_lpString );
	wcscat( pTempStr, wstr.m_lpData->m_lpString );
	
	CHE_WideString tmpString(pTempStr);
	GetAllocator()->DeleteArray<WCHAR>( pTempStr );
	return tmpString;
}

CHE_WideString& CHE_WideString::operator+=( WCHAR wch )
{
	if ( wch == 0 )
	{
		return *this;
	}

	if ( m_lpData == nullptr )
	{
		m_lpData = GetAllocator()->New<HE_WideStringData>();
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_dwLength = 1;
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>(2);
		m_lpData->m_lpString[0] = wch;
		m_lpData->m_lpString[1] = '\0';
	}else{
		if ( m_lpData->m_lpString == nullptr )
		{
			m_lpData->m_dwLength = 1;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>(2);
			m_lpData->m_lpString[0] = wch;
			m_lpData->m_lpString[1] = '\0';
			
			return *this;
		}
		
		PWSTR pTempStr = GetAllocator()->NewArray<WCHAR>( wcslen(m_lpData->m_lpString)+2 );
		wcscpy( pTempStr, m_lpData->m_lpString );
		
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
				m_lpData->m_lpString = nullptr;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef.AddRef();
		}else{
			m_lpData = GetAllocator()->New<HE_WideStringData>();
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = nullptr;
		}
		
		size_t dwBufferSize = wcslen( pTempStr )+2;
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( dwBufferSize );
		wcscpy( m_lpData->m_lpString, pTempStr );
		m_lpData->m_lpString[dwBufferSize-2] = wch;
		m_lpData->m_lpString[dwBufferSize-1] = '\0';
		m_lpData->m_dwLength = dwBufferSize-1;
		
		GetAllocator()->DeleteArray<WCHAR>( pTempStr );
	}
	return *this;
}

CHE_WideString & CHE_WideString::operator+=( PCWSTR lpWstr )
{
	if ( lpWstr == nullptr )
	{
		return *this;
	}

	if ( wcslen( lpWstr ) == 0 )
	{
		return *this;
	}

	if ( m_lpData == nullptr )
	{
		size_t iStrlen = wcslen(lpWstr);
		m_lpData = GetAllocator()->New<HE_WideStringData>();
		m_lpData->m_dwRef.AddRef();
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( iStrlen+1 );
		wcscpy( m_lpData->m_lpString, lpWstr );
		m_lpData->m_dwLength = iStrlen;
	}else{
		if ( m_lpData->m_lpString == nullptr )
		{
			size_t dwStrlen = wcslen(lpWstr);
			m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( dwStrlen+1 );
			wcscpy( m_lpData->m_lpString, lpWstr );
			m_lpData->m_dwLength = dwStrlen;
			m_lpData->m_dwRef.AddRef();
			return *this;
		}
		
		PWSTR pTempStr = GetAllocator()->NewArray<WCHAR>( wcslen(m_lpData->m_lpString)+1 );
		wcscpy( pTempStr, m_lpData->m_lpString );
		
		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
				m_lpData->m_lpString = nullptr;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef.AddRef();
		}else{
			m_lpData = GetAllocator()->New<HE_WideStringData>();
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = nullptr;
		}
		
		size_t dwStrlen = wcslen(pTempStr) + wcslen(lpWstr);
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( dwStrlen+1 );
		wcscpy( m_lpData->m_lpString, pTempStr );
		wcscat( m_lpData->m_lpString, lpWstr );
		m_lpData->m_dwLength = dwStrlen;
	}
	
	return *this;
}

CHE_WideString & CHE_WideString::operator+=( const CHE_WideString & wstr )
{
	if ( wstr.m_lpData == nullptr || wstr.m_lpData->m_lpString == nullptr )
	{
		return *this;
	}

	if ( this == &wstr )
	{
		PWSTR pTempStr = GetAllocator()->NewArray<WCHAR>( wcslen(m_lpData->m_lpString)+1 );
		wcscpy( pTempStr, m_lpData->m_lpString );

		m_lpData->m_dwRef.DecRef();
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
				m_lpData->m_lpString = nullptr;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef.AddRef();
		}else{
			m_lpData = GetAllocator()->New<HE_WideStringData>();
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef.AddRef();
			m_lpData->m_lpString = nullptr;
		}

		size_t iBufferSize = wcslen(pTempStr) * 2 + 1;
		m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( iBufferSize );
		wcscpy( m_lpData->m_lpString, pTempStr );
		wcscat( m_lpData->m_lpString, pTempStr );

		m_lpData->m_dwLength = iBufferSize-1;

		GetAllocator()->DeleteArray<WCHAR>( pTempStr );
		pTempStr = nullptr;
		
		return *this;
	}else{
		if ( m_lpData == wstr.m_lpData )
		{
			PWSTR pTempStr = GetAllocator()->NewArray<WCHAR>( wcslen(m_lpData->m_lpString)+1 );
			wcscpy( pTempStr, m_lpData->m_lpString );

			m_lpData->m_dwRef.DecRef();
			m_lpData->m_dwRef.DecRef();
			if ( m_lpData->m_dwRef == 0 )
			{
				if ( m_lpData->m_lpString )
				{
					GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
					m_lpData->m_lpString = nullptr;
				}
				m_lpData->m_dwLength = 0;
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_dwRef.AddRef();
			}else{
				m_lpData = GetAllocator()->New<HE_WideStringData>();
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_lpString = nullptr;
				m_lpData->m_dwLength = 0;
			}

			size_t iBufferSize = wcslen(pTempStr) * 2 + 1;
			m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( iBufferSize );
			wcscpy( m_lpData->m_lpString, pTempStr );
			wcscat( m_lpData->m_lpString, pTempStr );
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			GetAllocator()->DeleteArray<WCHAR>( pTempStr );
			pTempStr = nullptr;
			
			return *this;
		}else{
			if ( m_lpData->m_lpString == nullptr )
			{
				size_t iBufferSize = wcslen(wstr.m_lpData->m_lpString)+1;
				m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( iBufferSize );
				wcscpy( m_lpData->m_lpString, wstr.m_lpData->m_lpString );
				return *this;
			}

			PWSTR pTempStr = GetAllocator()->NewArray<WCHAR>( wcslen(m_lpData->m_lpString)+1 );
			wcscpy( pTempStr, m_lpData->m_lpString );

			if ( m_lpData->m_dwRef == 1 )
			{
				if ( m_lpData->m_lpString )
				{
					GetAllocator()->DeleteArray<WCHAR>( m_lpData->m_lpString );
					m_lpData->m_lpString = nullptr;
					m_lpData->m_dwLength = 0;
				}
			}else{
				m_lpData->m_dwRef.DecRef();
				m_lpData = GetAllocator()->New<HE_WideStringData>();
				m_lpData->m_dwLength = 0;
				m_lpData->m_dwRef.AddRef();
				m_lpData->m_lpString = nullptr;
			}

			size_t iBufferSize = wcslen( wstr.m_lpData->m_lpString );
			iBufferSize += wcslen( pTempStr )+1;
			m_lpData->m_lpString = GetAllocator()->NewArray<WCHAR>( iBufferSize );
			wcscpy( m_lpData->m_lpString, pTempStr );
			wcscat( m_lpData->m_lpString, wstr.m_lpData->m_lpString );

			GetAllocator()->DeleteArray<WCHAR>( pTempStr );
			pTempStr = nullptr;
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			return *this;
		}
	}
}

bool CHE_WideString::operator!=( WCHAR wch )const
{
	if ( m_lpData == nullptr )
	{
		if ( wch == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( m_lpData->m_dwLength != 1 )
		{
			return true;
		}else{
			if ( m_lpData->m_lpString[0] == wch )
			{
				return false;
			}else{
				return true;
			}
		}
	}
}

bool CHE_WideString::operator!=( PCWSTR lpWstr )const
{
	if ( m_lpData == nullptr )
	{
		if ( lpWstr == nullptr || wcslen( lpWstr ) == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( lpWstr == nullptr )
		{
			if ( m_lpData->m_lpString == nullptr )
			{
				return false;
			}else{
				return true;
			}
		}else{
			if ( wcscmp( m_lpData->m_lpString, lpWstr ) == 0 )
			{
				return false;
			}else{
				return true;
			}	
		}
	}
}

bool CHE_WideString::operator!=( const CHE_WideString & wstr )const
{
	if ( this == &wstr )
	{
		return false;
	}else{
		if ( m_lpData == wstr.m_lpData )
		{
			return false;
		}else{
			if ( m_lpData == nullptr )
			{
				return true;
			}else{
				if ( wcscmp( m_lpData->m_lpString, wstr.m_lpData->m_lpString ) == 0 )
				{
					return false;
				}else{
					return true;
				}
			}
		}
	}	
}

bool operator==( WCHAR wch, CHE_WideString & wstr )
{
	return ( wstr == wch );
}

bool operator==( PCWSTR lpWstr, const CHE_WideString & wstr )
{
	return ( wstr == lpWstr );
}

CHE_WideString operator+( WCHAR wch, const CHE_WideString & wstr )
{
	CHE_WideString tempStr( wch, wstr.GetAllocator() );
	tempStr+=wstr;
	return tempStr;
}

CHE_WideString operator+( PCWSTR lpWstr, const CHE_WideString & wstr )
{
	CHE_WideString tempStr( lpWstr, wcslen(lpWstr), wstr.GetAllocator() );
	tempStr+= wstr;
	return tempStr;
}

bool operator!=( WCHAR wch, const CHE_WideString& wstr )
{
	return ( wstr != wch );
}

bool operator!=( PCWSTR lpWstr, const CHE_WideString& wstr )
{
	return ( wstr != lpWstr );
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
