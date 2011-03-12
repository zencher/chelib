#include "../include/che_string.h"
#include <memory.h>
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
	m_lpData = new HE_ByteStringData;
	
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

CHE_ByteString& CHE_ByteString::operator=( HE_CHAR ch )
{
	if ( ch = 0 )
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
		return *this;
	}

	HE_DWORD nStrlen = strlen( lpStr );
	if ( nStrlen == 0 )
	{
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

bool CHE_ByteString::operator==( HE_CHAR ch )const
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

bool CHE_ByteString::operator==( HE_LPCSTR lpStr )const
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

HE_DWORD CHE_ByteString::GetLength()
{
	if ( m_lpData == NULL )
	{
		return 0;
	}else{
		return m_lpData->m_dwLength;
	}
}

HE_CHAR CHE_ByteString::operator[]( HE_INT32 index )const
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
		m_lpData = new HE_ByteStringData;
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
			m_lpData = new HE_ByteStringData;
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
		if ( ch = 0 )
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
	bool bRet = ( str.operator==(ch) );
	return bRet;
// 	if ( str.m_lpData == NULL )
// 	{
// 		if ( ch == '\0' )
// 		{
// 			return true;
// 		}else{
// 			return false;
// 		}
// 	}else{
// 		if ( str.m_lpData->m_dwLength != 1 )
// 		{
// 			return false
// 		}else{
// 			if ( ch == str.m_lpData->m_lpString[0] )
// 			{
// 				return true;
// 			}else{
// 				return false;
// 			}
// 		}
// 	}
}

bool operator==( HE_LPSTR lpStr, const CHE_ByteString& str )
{
	return ( str == lpStr );
// 	if ( str.m_lpData == NULL )
// 	{
// 		if ( lpStr == NULL )
// 		{
// 			return true;
// 		}else{
// 			false
// 		}
// 	}else{
// 		if ( str.m_lpData->m_lpString == NULL )
// 		{
// 			if ( lpStr == NULL )
// 			{
// 				return true;
// 			}else{
// 				return false;
// 			}
// 		}else{
// 			if ( lpStr == NULL )
// 			{
// 				return false;
// 			}else{
// 				if ( 0 == strcmp( lpStr, str.m_lpData->m_lpString ) )
// 				{
// 					return true;
// 				}else{
// 					return false;
// 				}	 
// 			}
// 		}
// 	}
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

CHE_ByteStringToPtrMap::CHE_ByteStringToPtrMap()
{
	m_pData = NULL;
	m_pString = NULL;	
	m_lSize = 0;
	m_lCount = 0;
}

CHE_ByteStringToPtrMap::~CHE_ByteStringToPtrMap()
{
	if ( m_pData )
	{
		delete [] m_pData;
	}
	if ( m_pString && m_lCount > 0 )
	{
		for ( HE_DWORD i = 0; i < m_lCount; i++ )
		{
			delete m_pString[i];
		}
		delete [] m_pString;
	}
}

HE_BOOL	CHE_ByteStringToPtrMap::Append( CHE_ByteString & str, HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL || m_pString == NULL )
	{
		m_pData = new HE_LPVOID[128];
		m_pString = new CHE_ByteString *[128];
		m_lSize = 128;
		m_lCount = 1;
		m_pData[0] = ptr;
		m_pString[0] = new CHE_ByteString( str );
		return TRUE;
	}
	if ( m_lCount + 1 <= m_lSize )
	{
		m_pData[m_lCount] = ptr;
		m_pString[m_lCount] = new CHE_ByteString( str );
		m_lCount++;
		return TRUE;
	}else{
		HE_LPVOID * pTmp = new HE_LPVOID[m_lSize];
		CHE_ByteString ** pStrTmp = new CHE_ByteString*[m_lSize];
		memcpy( pTmp, m_pData, m_lSize );
		memcpy( pStrTmp, m_pString, m_lSize );
		m_lSize *= 2;
		delete [] m_pData;
		delete [] m_pString;
		m_pData = new HE_LPVOID[m_lSize];
		m_pString = new CHE_ByteString*[m_lSize];
		memcpy( m_pData, pTmp, m_lSize/2 );
		memcpy( m_pString, pStrTmp, m_lSize/2 );
		delete [] pTmp;
		delete [] pStrTmp;
		
		m_pString[m_lCount] = new CHE_ByteString( str );
		m_pData[m_lCount] = ptr;
		m_lCount++;
		return TRUE;
	}
}

HE_LPVOID CHE_ByteStringToPtrMap::GetItem( CHE_ByteString & str ) const
{
	for ( HE_DWORD i = 0; i < m_lCount; i++ )
	{
		if ( *m_pString[i] == str )
		{
			return m_pData[i];
		}
	}
	return NULL;
}

HE_VOID	CHE_ByteStringToPtrMap::Clear()
{
	if ( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	if ( m_pString && m_lCount > 0 )
	{
		for ( HE_DWORD i = 0; i < m_lCount; i++ )
		{
			delete m_pString[i];
		}
		delete [] m_pString;		
	}
	m_lSize = 0;
	m_lCount = 0;
}

CHE_NumToPtrMap::CHE_NumToPtrMap()
{
	m_lCount = 0;
	m_lSize = 0;
	m_pData = NULL;
	m_pNum = NULL;
}

CHE_NumToPtrMap::~CHE_NumToPtrMap()
{
	if ( m_pData )
	{
		delete [] m_pData;
	}
	if ( m_pNum && m_lCount > 0 )
	{
		delete [] m_pNum;
	}
}

HE_BOOL CHE_NumToPtrMap::Append( HE_DWORD num, HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL || m_pNum == NULL )
	{
		m_pData = new HE_LPVOID[128];
		m_pNum = new HE_DWORD[128];
		m_lSize = 128;
		m_lCount = 1;
		m_pData[0] = ptr;
		m_pNum[0] = num;
		return TRUE;
	}
	if ( m_lCount + 1 <= m_lSize )
	{
		m_pData[m_lCount] = ptr;
		m_pNum[m_lCount] = num;
		m_lCount++;
		return TRUE;
	}else{
		HE_LPVOID * pTmp = new HE_LPVOID[m_lSize];
		HE_DWORD * pNumTmp = new HE_DWORD[m_lSize];
		memcpy( pTmp, m_pData, m_lSize );
		memcpy( pNumTmp, m_pNum, m_lSize );
		m_lSize *= 2;
		delete [] m_pData;
		delete [] m_pNum;
		m_pData = new HE_LPVOID[m_lSize];
		m_pNum = new HE_DWORD[m_lSize];
		memcpy( m_pData, pTmp, m_lSize/2 );
		memcpy( m_pNum, pNumTmp, m_lSize/2 );
		delete [] pTmp;
		delete [] pNumTmp;
		
		m_pNum[m_lCount] = num;
		m_pData[m_lCount] = ptr;
		m_lCount++;
		return TRUE;
	}
}

HE_LPVOID CHE_NumToPtrMap::GetItem( HE_DWORD num ) const
{
	for ( HE_DWORD i = 0; i < m_lCount; i++ )
	{
		if ( m_pNum[i] == num )
		{
			return m_pData[i];
		}
	}
	return NULL;
}

HE_LPVOID CHE_NumToPtrMap::GetItemByIndex( HE_DWORD index )
{
	if ( index >= m_lCount )
	{
		return NULL;
	}
	return m_pData[index]; 
}

HE_VOID CHE_NumToPtrMap::Clear()
{
	m_lSize = 0;
	m_lCount = 0;
	if ( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	if ( m_pNum )
	{
		delete [] m_pNum;
		m_pNum = NULL;
	}
}

// CHE_ByteStringToPtrMap::CHE_ByteStringToPtrMap()
// {
// 	m_pData = NULL;
// 	m_pString = NULL;	
// 	m_lSize = 0;
// 	m_lCount = 0;
// }
// 
// CHE_ByteStringToPtrMap::~CHE_ByteStringToPtrMap()
// {
// 	if ( m_pData )
// 	{
// 		delete [] m_pData;
// 		m_pData = NULL;
// 		m_lSize = 0;
// 	}
// 	if ( m_pString && m_lCount > 0 )
// 	{
// 		for ( HE_DWORD i = 0; i < m_lCount; i++ )
// 		{
// 			delete m_pString[i];
// 		}
// 		delete [] m_pString;
// 		m_lCount = 0;	
// 	}
// }
// 
// HE_BOOL	CHE_ByteStringToPtrMap::Append( CHE_ByteString & str, HE_LPVOID ptr )
// {
// 	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL || m_pString == NULL )
// 	{
// 		m_pData = new HE_LPVOID[128];
// 		m_pString = new CHE_ByteString *[128];
// 		m_lSize = 128;
// 		m_lCount = 1;
// 		m_pData[0] = ptr;
// 		m_pString[0] = new CHE_ByteString( str );
// 		return TRUE;
// 	}
// 	if ( m_lCount + 1 <= m_lSize )
// 	{
// 		m_pData[m_lCount] = ptr;
// 		m_pString[m_lCount] = new CHE_ByteString( str );
// 		m_lCount++;
// 		return TRUE;
// 	}else{
// 		HE_LPVOID * pTmp = new HE_LPVOID[m_lSize];
// 		CHE_ByteString * pStrTmp = new CHE_ByteString*[m_lSize];
// 		memcpy( pTmp, m_pData, m_lSize );
// 		memcpy( pStrTmp, m_pString, m_lSize );
// 		m_lSize *= 2;
// 		delete [] m_pData;
// 		delete [] m_pString;
// 		m_pData = new HE_LPVOID[m_lSize];
// 		m_pString = new CHE_ByteString*[m_lSize];
// 		memcpy( m_pData, pTmp, m_lSize/2 );
// 		memcpy( pStrTmp, m_pString, m_lSize/2 );
// 		delete [] pTmp;
// 		delete [] pStrTmp;
// 		
// 		m_pString[m_lCount] = new CHE_ByteString( str );
// 		m_pData[m_lCount] = ptr;
// 		m_lCount++;
// 		return TRUE;
// 	}
// }
// 
// HE_LPVOID CHE_ByteStringToPtrMap::GetItem( HE_DWORD index )
// {
// 	return NULL;
// }
// 
// HE_VOID	CHE_ByteStringToPtrMap::Clear()
// {
// 	if ( m_pData )
// 	{
// 		delete [] m_pData;
// 		m_pData = NULL;
// 		m_lSize = 0;
// 	}
// 	if ( m_pString && m_lCount > 0 )
// 	{
// 		for ( HE_DWORD i = 0; i < m_lCount; i++ )
// 		{
// 			delete m_pString[i];
// 		}
// 		delete [] m_pString;
// 		m_lCount = 0;	
// 	}
// }