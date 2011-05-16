#include "../include/che_string.h"
#include <memory>
#include <cwctype>

CHE_ByteString::CHE_ByteString()
{
	m_lpData = NULL;
}

CHE_ByteString::~CHE_ByteString()
{
	Clear();
}

CHE_ByteString::CHE_ByteString( HE_CHAR ch )
{
	if ( ch == '\0' )
	{
		m_lpData = NULL;
	}else{
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwLength = 1;
		m_lpData->m_dwRef = 1;
		m_lpData->m_lpString = new HE_CHAR[2];
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString[1] = '\0';
	}
}

CHE_ByteString::CHE_ByteString( HE_LPCSTR lpStr, HE_LONG nStrSize /* = -1 */)
{
	if ( lpStr == NULL )
	{
		m_lpData = NULL;
		return;
	}

	HE_DWORD nStrlen = 0;
	if ( nStrSize <= 0 )
	{
		nStrlen = strlen( lpStr );
	}else{
		nStrlen = nStrSize;
	}

	if ( nStrlen > 0 )
	{
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
		strcpy( m_lpData->m_lpString, lpStr );
		m_lpData->m_lpString[nStrlen] = '\0';
	}else{
		m_lpData = NULL;
	}
}

CHE_ByteString::CHE_ByteString( const CHE_ByteString& str )
{
	if ( str.m_lpData == NULL )
	{
		m_lpData = NULL;
	}else{
		m_lpData = str.m_lpData;
		str.m_lpData->m_dwRef++;
	}
}

HE_VOID CHE_ByteString::Clear()
{
	if ( m_lpData )
	{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			delete m_lpData;
		}
	}
}

CHE_ByteString& CHE_ByteString::operator=( HE_CHAR ch )
{
	if ( ch == '\0' )
	{
		Clear();
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = 1;
		m_lpData->m_lpString = new HE_CHAR[2];
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString[1] = '\0';
	}else{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = 1;
			m_lpData->m_lpString = new HE_CHAR[2];
			m_lpData->m_lpString[0] = ch;
			m_lpData->m_lpString[1] = '\0';
		}else{
			m_lpData = new HE_ByteStringData;
			m_lpData->m_dwLength = 1;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = new HE_CHAR[2];
			m_lpData->m_lpString[0] = ch;
			m_lpData->m_lpString[1] = '\0';
		}
	}
	return *this;
}

CHE_ByteString& CHE_ByteString::operator=( HE_LPCSTR lpStr )
{
	if ( lpStr == NULL )
	{
		Clear();
		return *this;
	}

	HE_DWORD nStrlen = strlen( lpStr );
	if ( nStrlen == 0 )
	{
		Clear();
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
		strcpy( m_lpData->m_lpString, lpStr );
	}else{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = nStrlen;
			m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
			strcpy( m_lpData->m_lpString, lpStr );
		}else {
			m_lpData = new HE_ByteStringData;
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = nStrlen;
			m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
			strcpy( m_lpData->m_lpString, lpStr );
		}
	}
	return *this;
}

CHE_ByteString& CHE_ByteString::operator=( const CHE_ByteString& str )
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
	if ( str.m_lpData != NULL )
	{
		m_lpData = str.m_lpData;
		str.m_lpData->m_dwRef++;
	}
	return *this;
}

bool CHE_ByteString::operator==( HE_CHAR ch )const
{
	if ( m_lpData == NULL || strlen( m_lpData->m_lpString ) == 0 )
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

bool CHE_ByteString::operator==( HE_LPCSTR lpStr )const
{
	if ( m_lpData == NULL || strlen( m_lpData->m_lpString ) == 0 )
	{
		if ( lpStr == NULL || strlen( lpStr ) == 0 )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( lpStr == NULL )
		{
			if ( m_lpData->m_lpString == NULL )
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

bool CHE_ByteString::operator==( const CHE_ByteString& str )const
{
	if ( this == &str )
	{
		return true;
	}else{
		if ( m_lpData == str.m_lpData )
		{
			return true;
		}else{
			if ( m_lpData == NULL )
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

HE_BOOL	CHE_ByteString::SetData( HE_BYTE * pData, HE_DWORD size )
{
	if ( pData == NULL || size == 0 )
	{
		return FALSE;
	}
	if ( m_lpData == NULL )
	{
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = size;
		m_lpData->m_lpString = new HE_CHAR[size+1];
		memcpy( m_lpData->m_lpString, pData, size );
		m_lpData->m_lpString[size] = '\0';
	}else{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString = new HE_CHAR[size+1];
			memcpy( m_lpData->m_lpString, pData, size );
			m_lpData->m_lpString[size] = '\0';
		}else {
			m_lpData = new HE_ByteStringData;
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString = new HE_CHAR[size+1];
			memcpy( m_lpData->m_lpString, pData, size );
			m_lpData->m_lpString[size] = '\0';
		}
	}
	return TRUE;
}

HE_DWORD CHE_ByteString::GetLength()
{
	if ( m_lpData == NULL )
	{
		return 0;
	}else{
		return m_lpData->m_dwLength;
	}
}

HE_CHAR CHE_ByteString::operator[]( HE_DWORD index )const
{
	if ( m_lpData != NULL )
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

CHE_ByteString CHE_ByteString::operator+( HE_CHAR ch )
{
	if ( m_lpData == NULL || m_lpData->m_lpString == NULL )
	{
		return CHE_ByteString( ch );
	}

	HE_DWORD dwStrlen = m_lpData->m_dwLength + 2;
	HE_CHAR * pTempStr = new HE_CHAR[dwStrlen];

	strcpy( pTempStr, m_lpData->m_lpString );
	pTempStr[dwStrlen-2] = ch;
	pTempStr[dwStrlen-1] = '\0';

	CHE_ByteString tmpString(pTempStr);
	delete [] pTempStr;
	return tmpString;
}

CHE_ByteString CHE_ByteString::operator+( const HE_LPSTR lpStr )
{
	if ( lpStr == NULL )
	{
		return CHE_ByteString(*this);
	}

	if ( m_lpData == NULL || m_lpData->m_lpString == NULL )
	{
		return CHE_ByteString( lpStr );
	}
	
	HE_DWORD dwStrlen = m_lpData->m_dwLength + strlen(lpStr) + 1;
	HE_CHAR * pTempStr = new HE_CHAR[dwStrlen];
	
	strcpy( pTempStr, m_lpData->m_lpString );
	strcat( pTempStr, lpStr );

	CHE_ByteString tmpString(pTempStr);
	delete [] pTempStr;
	return tmpString;
}

CHE_ByteString CHE_ByteString::operator+( const CHE_ByteString& str )
{
	if ( str.m_lpData == NULL )
	{
		return CHE_ByteString(*this);
	}
	
	if ( m_lpData == NULL || m_lpData->m_lpString == NULL )
	{
		return CHE_ByteString( str );
	}
	
	HE_DWORD dwStrlen = m_lpData->m_dwLength + str.m_lpData->m_dwLength + 1;
	HE_CHAR * pTempStr = new HE_CHAR[dwStrlen];
	
	strcpy( pTempStr, m_lpData->m_lpString );
	strcat( pTempStr, str.m_lpData->m_lpString );
	
	CHE_ByteString tmpString(pTempStr);
	delete [] pTempStr;
	return tmpString;
}

CHE_ByteString& CHE_ByteString::operator+=( HE_CHAR ch )
{
	if ( ch == 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = 1;
		m_lpData->m_lpString = new HE_CHAR[2];
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString[1] = '\0';
	}else{
		if ( m_lpData->m_lpString == NULL )
		{
			m_lpData->m_dwLength = 1;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = new HE_CHAR[2];
			m_lpData->m_lpString[0] = ch;
			m_lpData->m_lpString[1] = '\0';
			
			return *this;
		}
		
		HE_LPSTR pTempStr = new HE_CHAR[strlen(m_lpData->m_lpString)+2];
		strcpy( pTempStr, m_lpData->m_lpString );
		
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
				m_lpData->m_lpString = NULL;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
		}else{
			m_lpData = new HE_ByteStringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}
		
		HE_DWORD dwBufferSize = strlen( pTempStr )+2;
		m_lpData->m_lpString = new HE_CHAR[dwBufferSize];
		strcpy( m_lpData->m_lpString, pTempStr );
		m_lpData->m_lpString[dwBufferSize-2] = ch;
		m_lpData->m_lpString[dwBufferSize-1] = '\0';
		m_lpData->m_dwLength = dwBufferSize-1;
		
		delete [] pTempStr;
	}
	return *this;
}

CHE_ByteString& CHE_ByteString::operator+=( const HE_LPSTR lpStr )
{
	if ( lpStr == NULL )
	{
		return *this;
	}

	if ( strlen( lpStr ) == 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		HE_INT32 iStrlen = strlen(lpStr);
		m_lpData = new HE_ByteStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_lpString = new HE_CHAR[iStrlen+1];
		strcpy( m_lpData->m_lpString, lpStr );
		m_lpData->m_dwLength = iStrlen;
	}else{
		if ( m_lpData->m_lpString == NULL )
		{
			HE_DWORD dwStrlen = strlen(lpStr);
			m_lpData->m_lpString = new HE_CHAR[dwStrlen+1];
			strcpy( m_lpData->m_lpString, lpStr );
			m_lpData->m_dwLength = dwStrlen;
			m_lpData->m_dwRef = 1;
			
			return *this;
		}
		
		HE_LPSTR pTempStr = new HE_CHAR[strlen(m_lpData->m_lpString)+1];
		strcpy( pTempStr, m_lpData->m_lpString );
		
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
				m_lpData->m_lpString = NULL;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
		}else{
			m_lpData = new HE_ByteStringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}
		
		HE_DWORD dwStrlen = strlen(pTempStr) + strlen(lpStr);
		m_lpData->m_lpString = new HE_CHAR[dwStrlen+1];
		strcpy( m_lpData->m_lpString, pTempStr );
		strcat( m_lpData->m_lpString, lpStr );
		m_lpData->m_dwLength = dwStrlen;
	}
	
	return *this;
}

CHE_ByteString& CHE_ByteString::operator+=( const CHE_ByteString& str )
{
	if ( str.m_lpData == NULL || str.m_lpData->m_lpString == NULL )
	{
		return *this;
	}

	if ( this == &str )
	{
		HE_LPSTR pTempStr = new HE_CHAR[strlen(m_lpData->m_lpString)+1];
		strcpy( pTempStr, m_lpData->m_lpString );

		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
				m_lpData->m_lpString = NULL;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
		}else{
			m_lpData = new HE_ByteStringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}

		HE_INT32 iBufferSize = strlen(pTempStr) * 2 + 1;
		m_lpData->m_lpString = new HE_CHAR[iBufferSize];
		strcpy( m_lpData->m_lpString, pTempStr );
		strcat( m_lpData->m_lpString, pTempStr );

		m_lpData->m_dwLength = iBufferSize-1;

		delete [] pTempStr;
		pTempStr = NULL;
		
		return *this;
	}else{
		if ( m_lpData == str.m_lpData )
		{
			HE_LPSTR pTempStr = new HE_CHAR[strlen(m_lpData->m_lpString)+1];
			strcpy( pTempStr, m_lpData->m_lpString );

			m_lpData->m_dwRef-=2;
			if ( m_lpData->m_dwRef == 0 )
			{
				if ( m_lpData->m_lpString )
				{
					delete [] m_lpData->m_lpString;
					m_lpData->m_lpString = NULL;
				}
				m_lpData->m_dwLength = 0;
				m_lpData->m_dwRef = 2;
			}else{
				m_lpData = new HE_ByteStringData;
				m_lpData->m_dwRef = 2;
				m_lpData->m_lpString = NULL;
				m_lpData->m_dwLength = 0;
			}

			HE_INT32 iBufferSize = strlen(pTempStr) * 2 + 1;
			m_lpData->m_lpString = new HE_CHAR[iBufferSize];
			strcpy( m_lpData->m_lpString, pTempStr );
			strcat( m_lpData->m_lpString, pTempStr );
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			delete [] pTempStr;
			pTempStr = NULL;
			
			return *this;
		}else{
			if ( m_lpData->m_lpString == NULL )
			{
				HE_INT32 iBufferSize = strlen(str.m_lpData->m_lpString)+1;
				m_lpData->m_lpString = new HE_CHAR[iBufferSize];
				strcpy( m_lpData->m_lpString, str.m_lpData->m_lpString );
				return *this;
			}

			HE_LPSTR pTempStr = new HE_CHAR[strlen(m_lpData->m_lpString)+1];
			strcpy( pTempStr, m_lpData->m_lpString );

			if ( m_lpData->m_dwRef == 1 )
			{
				if ( m_lpData->m_lpString )
				{
					delete [] m_lpData->m_lpString;
					m_lpData->m_lpString = NULL;
					m_lpData->m_dwLength = 0;
					m_lpData->m_dwRef = 1;
				}
			}else{
				m_lpData->m_dwRef--;
				m_lpData = new HE_ByteStringData;
				m_lpData->m_dwLength = 0;
				m_lpData->m_dwRef = 1;
				m_lpData->m_lpString = NULL;
			}

			HE_INT32 iBufferSize = strlen( str.m_lpData->m_lpString );
			iBufferSize += strlen( pTempStr )+1;
			m_lpData->m_lpString = new HE_CHAR[iBufferSize];
			strcpy( m_lpData->m_lpString, pTempStr );
			strcat( m_lpData->m_lpString, str.m_lpData->m_lpString );

			delete [] pTempStr;
			pTempStr = NULL;
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			return *this;
		}
	}
}

bool CHE_ByteString::operator!=( HE_CHAR ch )const
{
	if ( m_lpData == NULL )
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

bool CHE_ByteString::operator!=( const HE_LPSTR lpStr )const
{
	if ( m_lpData == NULL )
	{
		if ( lpStr == NULL || strlen( lpStr ) == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( lpStr == NULL )
		{
			if ( m_lpData->m_lpString == NULL )
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
			if ( m_lpData == NULL )
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

bool operator==( HE_CHAR ch, CHE_ByteString& str )
{
	return ( str == ch );
}

bool operator==( HE_LPSTR lpStr, const CHE_ByteString& str )
{
	return ( str == lpStr );
}

CHE_ByteString operator+( HE_CHAR ch, const CHE_ByteString& str )
{
	CHE_ByteString tempStr( ch );
	tempStr+=str;
	return tempStr;
}

CHE_ByteString operator+( const HE_LPSTR lpStr, const CHE_ByteString& str )
{
	CHE_ByteString tempStr( lpStr );
	tempStr+= str;
	return tempStr;
}

bool operator!=( HE_CHAR ch, const CHE_ByteString& str )
{
	return ( str != ch );
}

bool operator!=( const HE_LPSTR lpStr, const CHE_ByteString& str )
{
	return ( str != lpStr );
}



CHE_WideString::CHE_WideString()
{
	m_lpData = NULL;
}

CHE_WideString::~CHE_WideString()
{
	Clear();
}

CHE_WideString::CHE_WideString( HE_WCHAR wch )
{
	if ( wch == '\0' )
	{
		m_lpData = NULL;
	}else{
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwLength = 1;
		m_lpData->m_dwRef = 1;
		m_lpData->m_lpString = new HE_WCHAR[2];
		m_lpData->m_lpString[0] = wch;
		m_lpData->m_lpString[1] = '\0';
	}
}

CHE_WideString::CHE_WideString( HE_LPCWSTR lpwstr, HE_LONG nStrSize /* = -1 */)
{
	if ( lpwstr == NULL )
	{
		m_lpData = NULL;
		return;
	}

	HE_DWORD nStrlen = 0;
	if ( nStrSize <= 0 )
	{
		nStrlen = wcslen( lpwstr );
	}else{
		nStrlen = nStrSize;
	}

	if ( nStrlen > 0 )
	{
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = new HE_WCHAR[nStrlen+1];
		wcscpy( m_lpData->m_lpString, lpwstr );
		m_lpData->m_lpString[nStrlen] = '\0';
	}else{
		m_lpData = NULL;
	}
}

CHE_WideString::CHE_WideString( const CHE_WideString& wstr )
{
	if ( wstr.m_lpData == NULL )
	{
		m_lpData = NULL;
	}else{
		m_lpData = wstr.m_lpData;
		wstr.m_lpData->m_dwRef++;
	}
}

HE_VOID CHE_WideString::Clear()
{
	if ( m_lpData )
	{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			delete m_lpData;
		}
	}
}

CHE_WideString& CHE_WideString::operator=( HE_WCHAR wch )
{
	if ( wch == '\0' )
	{
		Clear();
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = 1;
		m_lpData->m_lpString = new HE_WCHAR[2];
		m_lpData->m_lpString[0] = wch;
		m_lpData->m_lpString[1] = '\0';
	}else{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = 1;
			m_lpData->m_lpString = new HE_WCHAR[2];
			m_lpData->m_lpString[0] = wch;
			m_lpData->m_lpString[1] = '\0';
		}else{
			m_lpData = new HE_WideStringData;
			m_lpData->m_dwLength = 1;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = new HE_WCHAR[2];
			m_lpData->m_lpString[0] = wch;
			m_lpData->m_lpString[1] = '\0';
		}
	}
	return *this;
}

CHE_WideString& CHE_WideString::operator=( HE_LPCWSTR lpWstr )
{
	if ( lpWstr == NULL )
	{
		Clear();
		return *this;
	}

	HE_DWORD nStrlen = wcslen( lpWstr );
	if ( nStrlen == 0 )
	{
		Clear();
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = new HE_WCHAR[nStrlen+1];
		wcscpy( m_lpData->m_lpString, lpWstr );
	}else{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = nStrlen;
			m_lpData->m_lpString = new HE_WCHAR[nStrlen+1];
			wcscpy( m_lpData->m_lpString, lpWstr );
		}else {
			m_lpData = new HE_WideStringData;
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = nStrlen;
			m_lpData->m_lpString = new HE_WCHAR[nStrlen+1];
			wcscpy( m_lpData->m_lpString, lpWstr );
		}
	}
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
	if ( wstr.m_lpData != NULL )
	{
		m_lpData = wstr.m_lpData;
		wstr.m_lpData->m_dwRef++;
	}
	return *this;
}

bool CHE_WideString::operator==( HE_WCHAR wch )const
{
	if ( m_lpData == NULL || wcslen( m_lpData->m_lpString ) == 0 )
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

bool CHE_WideString::operator==( HE_LPCWSTR lpWstr )const
{
	if ( m_lpData == NULL || wcslen( m_lpData->m_lpString ) == 0 )
	{
		if ( lpWstr == NULL || wcslen( lpWstr ) == 0 )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( lpWstr == NULL )
		{
			if ( m_lpData->m_lpString == NULL )
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
			if ( m_lpData == NULL )
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

HE_BOOL	CHE_WideString::SetData( HE_WCHAR * pData, HE_DWORD size )
{
	if ( pData == NULL || size == 0 )
	{
		return FALSE;
	}
	if ( m_lpData == NULL )
	{
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = size;
		m_lpData->m_lpString = new HE_WCHAR[size+1];
		memcpy( m_lpData->m_lpString, pData, size * sizeof( HE_WCHAR ) );
		m_lpData->m_lpString[size] = '\0';
	}else{
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete[] m_lpData->m_lpString;
			}
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString = new HE_WCHAR[size+1];
			memcpy( m_lpData->m_lpString, pData, size * sizeof( HE_WCHAR ) );
			m_lpData->m_lpString[size] = '\0';
		}else {
			m_lpData = new HE_WideStringData;
			m_lpData->m_dwRef = 1;
			m_lpData->m_dwLength = size;
			m_lpData->m_lpString = new HE_WCHAR[size+1];
			memcpy( m_lpData->m_lpString, pData, size * sizeof( HE_WCHAR ) );
			m_lpData->m_lpString[size] = '\0';
		}
	}
	return TRUE;
}

HE_DWORD CHE_WideString::GetLength()
{
	if ( m_lpData == NULL )
	{
		return 0;
	}else{
		return m_lpData->m_dwLength;
	}
}

HE_WCHAR CHE_WideString::operator[]( HE_DWORD index )const
{
	if ( m_lpData != NULL )
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

CHE_WideString CHE_WideString::operator+( HE_WCHAR wch )
{
	if ( m_lpData == NULL || m_lpData->m_lpString == NULL )
	{
		return CHE_WideString( wch );
	}

	HE_DWORD dwStrlen = m_lpData->m_dwLength + 2;
	HE_WCHAR * pTempStr = new HE_WCHAR[dwStrlen];

	wcscpy( pTempStr, m_lpData->m_lpString );
	pTempStr[dwStrlen-2] = wch;
	pTempStr[dwStrlen-1] = '\0';

	CHE_WideString tmpString(pTempStr);
	delete [] pTempStr;
	return tmpString;
}

CHE_WideString CHE_WideString::operator+( const HE_LPWSTR lpWstr )
{
	if ( lpWstr == NULL )
	{
		return CHE_WideString(*this);
	}

	if ( m_lpData == NULL || m_lpData->m_lpString == NULL )
	{
		return CHE_WideString( lpWstr );
	}
	
	HE_DWORD dwStrlen = m_lpData->m_dwLength + wcslen(lpWstr) + 1;
	HE_WCHAR * pTempStr = new HE_WCHAR[dwStrlen];
	
	wcscpy( pTempStr, m_lpData->m_lpString );
	wcscat( pTempStr, lpWstr );

	CHE_WideString tmpString(pTempStr);
	delete [] pTempStr;
	return tmpString;
}

CHE_WideString CHE_WideString::operator+( const CHE_WideString& wstr )
{
	if ( wstr.m_lpData == NULL )
	{
		return CHE_WideString(*this);
	}
	
	if ( m_lpData == NULL || m_lpData->m_lpString == NULL )
	{
		return CHE_WideString( wstr );
	}
	
	HE_DWORD dwStrlen = m_lpData->m_dwLength + wstr.m_lpData->m_dwLength + 1;
	HE_WCHAR * pTempStr = new HE_WCHAR[dwStrlen];
	
	wcscpy( pTempStr, m_lpData->m_lpString );
	wcscat( pTempStr, wstr.m_lpData->m_lpString );
	
	CHE_WideString tmpString(pTempStr);
	delete [] pTempStr;
	return tmpString;
}

CHE_WideString& CHE_WideString::operator+=( HE_WCHAR wch )
{
	if ( wch == 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_dwLength = 1;
		m_lpData->m_lpString = new HE_WCHAR[2];
		m_lpData->m_lpString[0] = wch;
		m_lpData->m_lpString[1] = '\0';
	}else{
		if ( m_lpData->m_lpString == NULL )
		{
			m_lpData->m_dwLength = 1;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = new HE_WCHAR[2];
			m_lpData->m_lpString[0] = wch;
			m_lpData->m_lpString[1] = '\0';
			
			return *this;
		}
		
		HE_LPWSTR pTempStr = new HE_WCHAR[wcslen(m_lpData->m_lpString)+2];
		wcscpy( pTempStr, m_lpData->m_lpString );
		
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
				m_lpData->m_lpString = NULL;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
		}else{
			m_lpData = new HE_WideStringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}
		
		HE_DWORD dwBufferSize = wcslen( pTempStr )+2;
		m_lpData->m_lpString = new HE_WCHAR[dwBufferSize];
		wcscpy( m_lpData->m_lpString, pTempStr );
		m_lpData->m_lpString[dwBufferSize-2] = wch;
		m_lpData->m_lpString[dwBufferSize-1] = '\0';
		m_lpData->m_dwLength = dwBufferSize-1;
		
		delete [] pTempStr;
	}
	return *this;
}

CHE_WideString& CHE_WideString::operator+=( const HE_LPWSTR lpWstr )
{
	if ( lpWstr == NULL )
	{
		return *this;
	}

	if ( wcslen( lpWstr ) == 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		HE_INT32 iStrlen = wcslen(lpWstr);
		m_lpData = new HE_WideStringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_lpString = new HE_WCHAR[iStrlen+1];
		wcscpy( m_lpData->m_lpString, lpWstr );
		m_lpData->m_dwLength = iStrlen;
	}else{
		if ( m_lpData->m_lpString == NULL )
		{
			HE_DWORD dwStrlen = wcslen(lpWstr);
			m_lpData->m_lpString = new HE_WCHAR[dwStrlen+1];
			wcscpy( m_lpData->m_lpString, lpWstr );
			m_lpData->m_dwLength = dwStrlen;
			m_lpData->m_dwRef = 1;
			
			return *this;
		}
		
		HE_LPWSTR pTempStr = new HE_WCHAR[wcslen(m_lpData->m_lpString)+1];
		wcscpy( pTempStr, m_lpData->m_lpString );
		
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
				m_lpData->m_lpString = NULL;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
		}else{
			m_lpData = new HE_WideStringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}
		
		HE_DWORD dwStrlen = wcslen(pTempStr) + wcslen(lpWstr);
		m_lpData->m_lpString = new HE_WCHAR[dwStrlen+1];
		wcscpy( m_lpData->m_lpString, pTempStr );
		wcscat( m_lpData->m_lpString, lpWstr );
		m_lpData->m_dwLength = dwStrlen;
	}
	
	return *this;
}

CHE_WideString& CHE_WideString::operator+=( const CHE_WideString& wstr )
{
	if ( wstr.m_lpData == NULL || wstr.m_lpData->m_lpString == NULL )
	{
		return *this;
	}

	if ( this == &wstr )
	{
		HE_LPWSTR pTempStr = new HE_WCHAR[wcslen(m_lpData->m_lpString)+1];
		wcscpy( pTempStr, m_lpData->m_lpString );

		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
				m_lpData->m_lpString = NULL;
			}
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
		}else{
			m_lpData = new HE_WideStringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}

		HE_INT32 iBufferSize = wcslen(pTempStr) * 2 + 1;
		m_lpData->m_lpString = new HE_WCHAR[iBufferSize];
		wcscpy( m_lpData->m_lpString, pTempStr );
		wcscat( m_lpData->m_lpString, pTempStr );

		m_lpData->m_dwLength = iBufferSize-1;

		delete [] pTempStr;
		pTempStr = NULL;
		
		return *this;
	}else{
		if ( m_lpData == wstr.m_lpData )
		{
			HE_LPWSTR pTempStr = new HE_WCHAR[wcslen(m_lpData->m_lpString)+1];
			wcscpy( pTempStr, m_lpData->m_lpString );

			m_lpData->m_dwRef-=2;
			if ( m_lpData->m_dwRef == 0 )
			{
				if ( m_lpData->m_lpString )
				{
					delete [] m_lpData->m_lpString;
					m_lpData->m_lpString = NULL;
				}
				m_lpData->m_dwLength = 0;
				m_lpData->m_dwRef = 2;
			}else{
				m_lpData = new HE_WideStringData;
				m_lpData->m_dwRef = 2;
				m_lpData->m_lpString = NULL;
				m_lpData->m_dwLength = 0;
			}

			HE_INT32 iBufferSize = wcslen(pTempStr) * 2 + 1;
			m_lpData->m_lpString = new HE_WCHAR[iBufferSize];
			wcscpy( m_lpData->m_lpString, pTempStr );
			wcscat( m_lpData->m_lpString, pTempStr );
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			delete [] pTempStr;
			pTempStr = NULL;
			
			return *this;
		}else{
			if ( m_lpData->m_lpString == NULL )
			{
				HE_INT32 iBufferSize = wcslen(wstr.m_lpData->m_lpString)+1;
				m_lpData->m_lpString = new HE_WCHAR[iBufferSize];
				wcscpy( m_lpData->m_lpString, wstr.m_lpData->m_lpString );
				return *this;
			}

			HE_LPWSTR pTempStr = new HE_WCHAR[wcslen(m_lpData->m_lpString)+1];
			wcscpy( pTempStr, m_lpData->m_lpString );

			if ( m_lpData->m_dwRef == 1 )
			{
				if ( m_lpData->m_lpString )
				{
					delete [] m_lpData->m_lpString;
					m_lpData->m_lpString = NULL;
					m_lpData->m_dwLength = 0;
					m_lpData->m_dwRef = 1;
				}
			}else{
				m_lpData->m_dwRef--;
				m_lpData = new HE_WideStringData;
				m_lpData->m_dwLength = 0;
				m_lpData->m_dwRef = 1;
				m_lpData->m_lpString = NULL;
			}

			HE_INT32 iBufferSize = wcslen( wstr.m_lpData->m_lpString );
			iBufferSize += wcslen( pTempStr )+1;
			m_lpData->m_lpString = new HE_WCHAR[iBufferSize];
			wcscpy( m_lpData->m_lpString, pTempStr );
			wcscat( m_lpData->m_lpString, wstr.m_lpData->m_lpString );

			delete [] pTempStr;
			pTempStr = NULL;
			
			m_lpData->m_dwLength = iBufferSize-1;
			
			return *this;
		}
	}
}

bool CHE_WideString::operator!=( HE_WCHAR wch )const
{
	if ( m_lpData == NULL )
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

bool CHE_WideString::operator!=( const HE_LPWSTR lpWstr )const
{
	if ( m_lpData == NULL )
	{
		if ( lpWstr == NULL || wcslen( lpWstr ) == 0 )
		{
			return false;
		}else{
			return true;
		}
	}else{
		if ( lpWstr == NULL )
		{
			if ( m_lpData->m_lpString == NULL )
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

bool CHE_WideString::operator!=( const CHE_WideString& wstr )const
{
	if ( this == &wstr )
	{
		return false;
	}else{
		if ( m_lpData == wstr.m_lpData )
		{
			return false;
		}else{
			if ( m_lpData == NULL )
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

bool operator==( HE_WCHAR wch, CHE_WideString& wstr )
{
	return ( wstr == wch );
}

bool operator==( HE_LPWSTR lpWstr, const CHE_WideString& wstr )
{
	return ( wstr == lpWstr );
}

CHE_WideString operator+( HE_WCHAR wch, const CHE_WideString& wstr )
{
	CHE_WideString tempStr( wch );
	tempStr+=wstr;
	return tempStr;
}

CHE_WideString operator+( const HE_LPWSTR lpWstr, const CHE_WideString& wstr )
{
	CHE_WideString tempStr( lpWstr );
	tempStr+= wstr;
	return tempStr;
}

bool operator!=( HE_WCHAR wch, const CHE_WideString& wstr )
{
	return ( wstr != wch );
}

bool operator!=( const HE_LPWSTR lpWstr, const CHE_WideString& wstr )
{
	return ( wstr != lpWstr );
}
