#include "../include/che_string.h"
#include <string.h>

CHE_ByteString::CHE_ByteString()
{
	m_lpData = NULL;
}

CHE_ByteString::~CHE_ByteString()
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

CHE_ByteString::CHE_ByteString( HE_CHAR ch )
{
	m_lpData = new HE_StringData;
	
	m_lpData->m_dwLength = 1;
	m_lpData->m_dwRef = 1;
	m_lpData->m_lpString = new HE_CHAR[2];
	m_lpData->m_lpString[0] = ch;
	m_lpData->m_lpString[1] = '\0';
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
		m_lpData = new HE_StringData;
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

CHE_ByteString& CHE_ByteString::operator=( HE_CHAR ch )
{
	if ( ch = 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_StringData;
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
			m_lpData = new HE_StringData;
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
		return *this;
	}

	HE_DWORD nStrlen = strlen( lpStr );
	if ( nStrlen == 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_StringData;
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
			m_lpData = new HE_StringData;
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

	if ( m_lpData != NULL )
	{
		if ( str.m_lpData == NULL )
		{
			return *this;
		}
		m_lpData->m_dwRef--;
		if ( m_lpData->m_dwRef == 0 )
		{
			if ( m_lpData->m_lpString )
			{
				delete [] m_lpData->m_lpString;
			}
			delete m_lpData;
		}
	}

	m_lpData = str.m_lpData;
	str.m_lpData->m_dwRef++;

	return *this;
}

bool CHE_ByteString::operator==( HE_CHAR ch )
{
	if ( m_lpData == NULL )
	{
		if ( ch = 0 )
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

bool CHE_ByteString::operator==( HE_LPCSTR lpStr )
{
	if ( m_lpData == NULL )
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

bool CHE_ByteString::operator==( const CHE_ByteString& str )
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

HE_CHAR CHE_ByteString::operator[]( HE_INT32 index )
{
	if ( m_lpData != NULL )
	{
		if ( index >= m_lpData->m_dwLength )
		{
			return 0;
		}else if ( index < 0 )
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

	HE_INT32 iStrlen = m_lpData->m_dwLength + 2;
	HE_CHAR * pTempStr = new HE_CHAR[iStrlen];

	strcpy( pTempStr, m_lpData->m_lpString );
	pTempStr[iStrlen-2] = ch;
	pTempStr[iStrlen-1] = '\0';
	return CHE_ByteString(pTempStr);
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
	
	HE_INT32 iStrlen = m_lpData->m_dwLength + strlen(lpStr) + 1;
	HE_CHAR * pTempStr = new HE_CHAR[iStrlen];
	
	strcpy( pTempStr, m_lpData->m_lpString );
	strcat( pTempStr, lpStr );

	return CHE_ByteString(pTempStr);
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
	
	HE_INT32 iStrlen = m_lpData->m_dwLength + str.m_lpData->m_dwLength + 1;
	HE_CHAR * pTempStr = new HE_CHAR[iStrlen];
	
	strcpy( pTempStr, m_lpData->m_lpString );
	strcat( pTempStr, str.m_lpData->m_lpString );
	
	return CHE_ByteString(pTempStr);
}

CHE_ByteString& CHE_ByteString::operator+=( HE_CHAR ch )
{
	if ( ch == 0 )
	{
		return *this;
	}

	if ( m_lpData == NULL )
	{
		m_lpData = new HE_StringData;
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
			m_lpData = new HE_StringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}
		
		HE_INT32 iBufferSize = strlen( pTempStr )+2;
		m_lpData->m_lpString = new HE_CHAR[iBufferSize];
		strcpy( m_lpData->m_lpString, pTempStr );
		m_lpData->m_lpString[iBufferSize-2] = ch;
		m_lpData->m_lpString[iBufferSize-1] = '\0';
		m_lpData->m_dwLength = iBufferSize-1;
		
		delete [] pTempStr;
		pTempStr = NULL;
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
		m_lpData = new HE_StringData;
		m_lpData->m_dwRef = 1;
		m_lpData->m_lpString = new HE_CHAR[iStrlen+1];
		strcpy( m_lpData->m_lpString, lpStr );
		m_lpData->m_dwLength = iStrlen;

	}else{
		if ( m_lpData->m_lpString == NULL )
		{
			HE_INT32 iStrlen = strlen(lpStr);
			m_lpData->m_lpString = new HE_CHAR[iStrlen+1];
			strcpy( m_lpData->m_lpString, lpStr );
			m_lpData->m_dwLength = iStrlen;
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
			m_lpData = new HE_StringData;
			m_lpData->m_dwLength = 0;
			m_lpData->m_dwRef = 1;
			m_lpData->m_lpString = NULL;
		}
		
		HE_INT32 iStrlen = strlen(pTempStr) + strlen(lpStr);
		m_lpData->m_lpString = new HE_CHAR[iStrlen+1];
		strcpy( m_lpData->m_lpString, pTempStr );
		strcat( m_lpData->m_lpString, lpStr );
		m_lpData->m_dwLength = iStrlen;
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
			m_lpData = new HE_StringData;
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
				m_lpData = new HE_StringData;
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
				m_lpData = new HE_StringData;
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

