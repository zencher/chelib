#include "../include/che_datastructure.h"
#include <memory.h>

CHE_PtrStack::~CHE_PtrStack()
{
	CHE_PtrStackNode * pTmp = m_pTop;
	while ( pTmp )
	{
		m_pTop = m_pTop->pNext;
		delete pTmp;
		pTmp = m_pTop;
	}
}

bool CHE_PtrStack::IsEmpty()
{
	if ( m_pTop == NULL )
	{
		return true;
	}
	return false;
}

bool CHE_PtrStack::Pop( HE_VOID ** pptr )
{
	if ( m_pTop == NULL )
	{
		return false;
	}
	if ( m_pTop->pObj == NULL )
	{
		return false;
	}else{
		*pptr = m_pTop->pObj;
		
		CHE_PtrStackNode * pTmp = m_pTop;
		m_pTop = m_pTop->pNext;
		delete pTmp;
		pTmp = NULL;
		return true;
	}
}

bool CHE_PtrStack::Push( HE_VOID * ptr )
{
	if ( m_pTop == NULL )
	{
		m_pTop = new CHE_PtrStackNode;
		m_pTop->pNext = NULL;
		m_pTop->pObj = ptr;
	}else{
		CHE_PtrStackNode * pTmp = new CHE_PtrStackNode;
		pTmp->pNext = m_pTop;
		pTmp->pObj = ptr;
		m_pTop = pTmp;
	}
	return true;
}

void CHE_PtrStack::Clear()
{
	if ( m_pTop )
	{
		CHE_PtrStackNode * pTmp = m_pTop;
		while( pTmp )
		{
			m_pTop = m_pTop->pNext;
			delete pTmp;
			pTmp = m_pTop;
		}
	}
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