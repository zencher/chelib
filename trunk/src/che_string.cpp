#include "../include/che_string.h"
#include <string.h>

CHE_ByteString::CHE_ByteString()
{
	m_lpData = new HE_StringData;

	m_lpData->m_dwLength = 0;
	m_lpData->m_dwRef = 1;
	m_lpData->m_lpString = NULL;
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
				m_lpData->m_lpString[m_lpData->m_dwLength-1] = '\0';
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

CHE_ByteString::CHE_ByteString( HE_LPCSTR str, HE_LONG nStrSize /* = -1 */)
{
	m_lpData = new HE_StringData;
	m_lpData->m_dwRef = 1;

	if ( str == NULL )
	{
		m_lpData->m_dwLength = 0;
		m_lpData->m_lpString = NULL;
	}

	HE_DWORD nStrlen = 0;
	if ( nStrSize <= 0 )
	{
		nStrlen = strlen( str );
	}else{
		nStrlen = nStrSize;
	}

	if ( nStrlen > 0 )
	{
		m_lpData->m_dwLength = nStrlen;
		m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
		strcpy( m_lpData->m_lpString, str );
		m_lpData->m_lpString[nStrlen] = '\0';
	}else{
		m_lpData->m_dwLength = 0;
		m_lpData->m_lpString = NULL;
	}
}

CHE_ByteString::CHE_ByteString( const CHE_ByteString& str )
{
	m_lpData = str.m_lpData;
	str.m_lpData->m_dwRef++;
}

CHE_ByteString& CHE_ByteString::operator=( HE_CHAR ch )
{
	if ( m_lpData->m_dwRef == 1 )
	{
		if ( m_lpData->m_lpString )
		{
			delete[] m_lpData->m_lpString;
		}
		m_lpData->m_lpString = new HE_CHAR[2];
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString[1] = '\0';
	}else if ( m_lpData->m_dwRef > 1 )
	{
		m_lpData->m_dwRef--;
		m_lpData = new HE_StringData;
		m_lpData->m_dwLength = 1;
		m_lpData->m_dwRef = 1;
		m_lpData->m_lpString = new HE_CHAR[2];
		m_lpData->m_lpString[0] = ch;
		m_lpData->m_lpString[1] = '\0';
	}
	return *this;
}

CHE_ByteString& CHE_ByteString::operator=( HE_LPCSTR str )
{
	HE_DWORD nStrlen = 0;
	if ( m_lpData->m_dwRef == 1 )
	{
		if ( m_lpData->m_lpString )
		{
			delete[] m_lpData->m_lpString;
		}
		if ( str == NULL )
		{
			m_lpData->m_dwLength = 0;
			m_lpData->m_lpString = NULL;
		}else{
			nStrlen = strlen( str );
			m_lpData->m_dwLength = nStrlen;
			m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
			strcpy( m_lpData->m_lpString, str );
			m_lpData->m_lpString[nStrlen] = '\0';
		}
	}else if ( m_lpData->m_dwRef > 1 )
	{
		m_lpData->m_dwRef--;
		m_lpData = new HE_StringData;
		if ( str == NULL )
		{
			m_lpData->m_dwLength = 0;
			m_lpData->m_lpString = NULL;
		}else{
			m_lpData->m_dwRef = 1;
			nStrlen = strlen( str );
			m_lpData->m_dwLength = nStrlen;
			m_lpData->m_lpString = new HE_CHAR[nStrlen+1];
			strcpy( m_lpData->m_lpString, str );
			m_lpData->m_lpString[nStrlen] = '\0';
		}
	}
	return *this;
}

CHE_ByteString& CHE_ByteString::operator=( const CHE_ByteString& str )
{
	m_lpData = str.m_lpData;
	str.m_lpData->m_dwRef++;
	return *this;
}

bool CHE_ByteString::operator==( HE_CHAR ch )
{
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

bool CHE_ByteString::operator==( HE_LPCSTR str )
{
	if ( str == NULL )
	{
		if ( m_lpData->m_lpString == NULL )
		{
			return true;
		}else{
			return false;
		}
	}else{
		if ( strcmp( m_lpData->m_lpString, str ) == 0 )
		{
			return true;
		}else{
			return false;
		}	
	}
}

bool CHE_ByteString::operator==( const CHE_ByteString& str )
{
	if ( this == &str )
	{
		return true;
	}else{
		if ( m_lpData->m_lpString == str.m_lpData->m_lpString )
		{
			return true;
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

HE_CHAR CHE_ByteString::operator[]( HE_INT32 index )
{
	if ( index >= m_lpData->m_dwLength )
	{
		return -1;
	}else if ( m_lpData->m_lpString )
	{
		return m_lpData->m_lpString[index];
	}else{
		return -1;
	}
}