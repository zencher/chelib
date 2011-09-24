#include "../include/che_datastructure.h"
#include <memory>
#include <cstdlib>
#include <ctime>

CHE_DynBuffer::CHE_DynBuffer( HE_DWORD size /*= 1024*/, HE_DWORD increament /*= 1024*/, CHE_Allocator * pAllocator /*= NULL*/ )
: CHE_Object( pAllocator )
{
	m_lSize = size;
	m_lIncreament = increament;

	m_lpData = GetAllocator()->NewArray<HE_BYTE>( m_lSize );
	m_lByteCount = 0;
}

CHE_DynBuffer::CHE_DynBuffer( const CHE_DynBuffer & buf )
: CHE_Object( buf.GetAllocator() )
{
	m_lSize = buf.m_lSize;
	m_lIncreament = buf.m_lIncreament;
	m_lByteCount = buf.m_lByteCount;
	m_lpData = GetAllocator()->NewArray<HE_BYTE>( m_lSize );
	if ( m_lByteCount > 0 )
	{
		memcpy( m_lpData, buf.m_lpData, m_lByteCount );
	}
}

CHE_DynBuffer::~CHE_DynBuffer()
{
	if ( m_lpData )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
		m_lpData = NULL;
	}
}

const CHE_DynBuffer & CHE_DynBuffer::operator = ( const CHE_DynBuffer & buf )
{
	if ( this != &buf )
	{
		if ( m_lpData )
		{
			GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
		}
		m_lSize = buf.m_lSize;
		m_lIncreament = buf.m_lIncreament;
		m_lByteCount = buf.m_lByteCount;
		m_lpData = GetAllocator()->NewArray<HE_BYTE>( m_lSize );
		if ( m_lByteCount > 0 )
		{
			memcpy( m_lpData, buf.m_lpData, m_lByteCount );
		}
	}
	return *this;
}

HE_DWORD CHE_DynBuffer::Write( HE_LPCBYTE pBuffer, HE_DWORD size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lByteCount + size > m_lSize )
	{
		HE_DWORD lNeed = m_lByteCount + size - m_lSize;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_DWORD)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPBYTE tmp = GetAllocator()->NewArray<HE_BYTE>( m_lSize + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lByteCount );
		memcpy( tmp+m_lByteCount, pBuffer, size );
		GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
		m_lpData = tmp;
		m_lByteCount+= size;
		m_lSize += lNeed*m_lIncreament;
		return size;
	}else{
		memcpy( m_lpData+m_lByteCount, pBuffer, size );
		m_lByteCount += size;
		return size;
	}
}

HE_DWORD CHE_DynBuffer::Write( const CHE_DynBuffer & dynBuffer )
{
	if ( dynBuffer.m_lpData == NULL || dynBuffer.m_lByteCount == 0 )
	{
		return 0;
	}

	if ( m_lByteCount + dynBuffer.m_lByteCount > m_lSize )
	{
		HE_DWORD lNeed = m_lByteCount + dynBuffer.m_lByteCount - m_lSize;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_DWORD)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPBYTE tmp = GetAllocator()->NewArray<HE_BYTE>( m_lSize + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lByteCount );
		memcpy( tmp+m_lByteCount, dynBuffer.m_lpData, dynBuffer.m_lByteCount );
		GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
		m_lpData = tmp;
		m_lByteCount+= dynBuffer.m_lByteCount;
		m_lSize += lNeed*m_lIncreament;
		return dynBuffer.m_lByteCount;
	}else{
		memcpy( m_lpData+m_lByteCount, dynBuffer.m_lpData, dynBuffer.m_lByteCount );
		m_lByteCount += dynBuffer.m_lByteCount;
		return dynBuffer.m_lByteCount;
	}
}

HE_DWORD CHE_DynBuffer::Read( HE_LPBYTE pBuffer, HE_DWORD size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lByteCount < size )
	{
		size = m_lByteCount;
	}

	memcpy( pBuffer, m_lpData, size );

	return size;
}

HE_BOOL CHE_DynBuffer::ReadByte( HE_DWORD offset, HE_LPBYTE pByte )
{
	if ( offset < m_lByteCount  )
	{
		*pByte = *(m_lpData+offset);
		return TRUE;
	}
	return FALSE;
}

CHE_DynWideByteBuffer::CHE_DynWideByteBuffer( HE_DWORD size /*= 1024*/, HE_DWORD increament /*= 1024*/, CHE_Allocator * pAllocator /*= NULL*/ )
: CHE_Object ( pAllocator )
{
	m_lSize = size;
	m_lIncreament = increament;

	m_lpData = GetAllocator()->NewArray<HE_WCHAR>( m_lSize );
	m_lByteCount = 0;
}

CHE_DynWideByteBuffer::CHE_DynWideByteBuffer( const CHE_DynWideByteBuffer & buf )
: CHE_Object ( buf.GetAllocator() )
{
	m_lSize = buf.m_lSize;
	m_lIncreament = buf.m_lIncreament;
	m_lByteCount = buf.m_lByteCount;
	m_lpData = GetAllocator()->NewArray<HE_WCHAR>( m_lSize );
	if ( m_lByteCount > 0 )
	{
		memcpy( m_lpData, buf.m_lpData, m_lByteCount * sizeof(HE_WCHAR) );
	}
}

CHE_DynWideByteBuffer::~CHE_DynWideByteBuffer()
{
	if ( m_lpData )
	{
		GetAllocator()->DeleteArray<HE_WCHAR>( m_lpData );
		m_lpData = NULL;
	}
}

const CHE_DynWideByteBuffer & CHE_DynWideByteBuffer::operator = ( const CHE_DynWideByteBuffer & buf )
{
	if ( this != &buf )
	{
		if ( m_lpData )
		{
			GetAllocator()->DeleteArray<HE_WCHAR>( m_lpData );
		}
		m_lSize = buf.m_lSize;
		m_lIncreament = buf.m_lIncreament;
		m_lByteCount = buf.m_lByteCount;
		m_lpData = GetAllocator()->NewArray<HE_WCHAR>( m_lSize );
		if ( m_lByteCount > 0 )
		{
			memcpy( m_lpData, buf.m_lpData, m_lByteCount * sizeof(HE_WCHAR) );
		}
	}
	return *this;
}

HE_DWORD CHE_DynWideByteBuffer::Write( HE_LPCWSTR pBuffer, HE_DWORD size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lByteCount + size > m_lSize )
	{
		HE_DWORD lNeed = m_lByteCount + size - m_lSize;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_DWORD)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPWSTR tmp = GetAllocator()->NewArray<HE_WCHAR>( m_lSize + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lByteCount * sizeof(HE_WCHAR) );
		memcpy( tmp+m_lByteCount, pBuffer, size * sizeof(HE_WCHAR) );
		GetAllocator()->DeleteArray<HE_WCHAR>( m_lpData );
		m_lpData = tmp;
		m_lByteCount+= size;
		m_lSize += lNeed*m_lIncreament;
		return size;
	}else{
		memcpy( m_lpData+m_lByteCount, pBuffer, size * sizeof(HE_WCHAR) );
		m_lByteCount += size;
		return size;
	}
}

HE_DWORD CHE_DynWideByteBuffer::Write( const CHE_DynWideByteBuffer & dynBuffer )
{
	if ( dynBuffer.m_lpData == NULL || dynBuffer.m_lByteCount == 0 )
	{
		return 0;
	}

	if ( m_lByteCount + dynBuffer.m_lByteCount > m_lSize )
	{
		HE_DWORD lNeed = m_lByteCount + dynBuffer.m_lByteCount - m_lSize;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_DWORD)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPWSTR tmp = GetAllocator()->NewArray<HE_WCHAR>( m_lSize + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lByteCount * sizeof(HE_WCHAR) );
		memcpy( tmp+m_lByteCount, dynBuffer.m_lpData, dynBuffer.m_lByteCount * sizeof(HE_WCHAR) );
		GetAllocator()->DeleteArray<HE_WCHAR>( m_lpData );
		m_lpData = tmp;
		m_lByteCount+= dynBuffer.m_lByteCount;
		m_lSize += lNeed*m_lIncreament;
		return dynBuffer.m_lByteCount;
	}else{
		memcpy( m_lpData+m_lByteCount, dynBuffer.m_lpData, dynBuffer.m_lByteCount * sizeof(HE_WCHAR) );
		m_lByteCount += dynBuffer.m_lByteCount;
		return dynBuffer.m_lByteCount;
	}
}

HE_DWORD CHE_DynWideByteBuffer::Read( HE_LPWSTR pBuffer, HE_DWORD size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lByteCount < size )
	{
		size = m_lByteCount;
	}

	memcpy( pBuffer, m_lpData, size * sizeof(HE_WCHAR) );

	return size;
}

HE_BOOL CHE_DynWideByteBuffer::ReadByte( HE_DWORD offset, HE_LPWSTR pWchar )
{
	if ( offset < m_lByteCount  )
	{
		*pWchar = *(m_lpData+offset);
		return TRUE;
	}
	return FALSE;
}

CHE_PtrStack::~CHE_PtrStack()
{
	Clear();
}

HE_BOOL CHE_PtrStack::IsEmpty()
{
	if ( m_pTop == NULL )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PtrStack::Pop( HE_VOID ** pptr )
{
	if ( pptr == NULL )
	{
		return FALSE;
	}
	if ( m_pTop == NULL )
	{
		return FALSE;
	}
	*pptr = m_pTop->pObj;
	
	CHE_PtrNode * pTmp = m_pTop;
	m_pTop = m_pTop->pNext;
	GetAllocator()->Delete<CHE_PtrNode>( pTmp );
	pTmp = NULL;
	return TRUE;
}

HE_BOOL CHE_PtrStack::Push( HE_VOID * ptr )
{
	if ( m_pTop == NULL )
	{
		m_pTop = GetAllocator()->New<CHE_PtrNode>( GetAllocator() );
		m_pTop->pNext = NULL;
		m_pTop->pObj = ptr;
	}else{
		CHE_PtrNode * pTmp = GetAllocator()->New<CHE_PtrNode>( GetAllocator() );
		pTmp->pNext = m_pTop;
		pTmp->pObj = ptr;
		m_pTop = pTmp;
	}
	return TRUE;
}

HE_VOID CHE_PtrStack::Clear()
{
	if ( m_pTop )
	{
		CHE_PtrNode * pTmp = m_pTop;
		while( pTmp )
		{
			m_pTop = m_pTop->pNext;
			GetAllocator()->Delete<CHE_PtrNode>( pTmp );
			pTmp = m_pTop;
		}
	}
}

CHE_PtrQueue::CHE_PtrQueue( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_pHead = NULL;
	m_pTail = NULL;
}

CHE_PtrQueue::~CHE_PtrQueue()
{
	Clear();
}

HE_VOID CHE_PtrQueue::Clear()
{
	CHE_PtrNode * pNode = m_pHead;
	while ( pNode )
	{
		m_pHead = m_pHead->pNext;
		GetAllocator()->Delete<CHE_PtrNode>( pNode );
		pNode = m_pHead;
	}
	m_pTail = NULL;
}

HE_BOOL CHE_PtrQueue::IsEmpty()
{
	if ( m_pHead == NULL && m_pTail == NULL )
	{
		return TRUE;
	}else{
		return FALSE;
	}
}

HE_BOOL	CHE_PtrQueue::Pop( HE_VOID** pptr )
{
	if ( pptr == NULL )
	{
		return FALSE;
	}
	if ( m_pTail == NULL )
	{
		return FALSE;
	}
	*pptr = m_pHead->pObj;

	if ( m_pHead == m_pTail )
	{
		GetAllocator()->Delete<CHE_PtrNode>( m_pHead );
		m_pHead = NULL;
		m_pTail =  NULL;
	}else{
		CHE_PtrNode * pTmp = m_pHead;
		m_pHead = m_pHead->pNext;
		GetAllocator()->Delete<CHE_PtrNode>( pTmp );
	}
	return TRUE;
}

HE_BOOL	CHE_PtrQueue::Push( HE_VOID* ptr )
{
	if ( m_pHead == NULL )
	{
		m_pHead = GetAllocator()->New<CHE_PtrNode>( GetAllocator() );
		m_pHead->pObj = ptr;
		m_pHead->pNext = NULL;
		m_pTail = m_pHead;
	}else{
		m_pTail->pNext = GetAllocator()->New<CHE_PtrNode>( GetAllocator() );
		m_pTail = m_pTail->pNext;
		m_pTail->pNext = NULL;
		m_pTail->pObj = ptr;
	}
	return TRUE;
}

CHE_PtrArray::CHE_PtrArray( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_pData = NULL;
	m_lSize = 0;
	m_lCount = 0;
}

CHE_PtrArray::~CHE_PtrArray()
{
	Clear();
}

HE_BOOL CHE_PtrArray::Append( HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL )
	{
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( 128 );
		m_lSize = 128;
		m_lCount = 1;
		m_pData[0] = ptr;
		return TRUE;
	}
	if ( m_lCount + 1 <= m_lSize )
	{
		m_pData[m_lCount] = ptr;
		m_lCount++;
		return TRUE;
	}else{
		HE_LPVOID * pTmp = GetAllocator()->NewArray<HE_LPVOID>( m_lSize );
		memcpy( pTmp, m_pData, m_lSize * sizeof(HE_LPVOID) );
		m_lSize *= 2;
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		m_pData = GetAllocator()->NewArray<HE_LPVOID>(m_lSize);
		memcpy( m_pData, pTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		GetAllocator()->DeleteArray<HE_LPVOID>( pTmp );
		m_pData[m_lCount] = ptr;
		m_lCount++;
		return TRUE;
	}
}

HE_LPVOID CHE_PtrArray::GetItem( HE_DWORD index ) const
{
	if ( index >= m_lCount )
	{
		return NULL;
	}else{
		return m_pData[index];
	}
}
	
HE_VOID	CHE_PtrArray::Clear()
{
	if ( m_pData )
	{
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		m_pData = NULL;
		m_lSize = 0;
		m_lCount = 0;
	}	
}

CHE_ByteStringToPtrMap::CHE_ByteStringToPtrMap( CHE_Allocator * pAllocator )
: CHE_Object( pAllocator )
{
	m_pData = NULL;
	m_pString = NULL;	
	m_lSize = 0;
	m_lCount = 0;
}

CHE_ByteStringToPtrMap::~CHE_ByteStringToPtrMap()
{
	Clear();
}

HE_BOOL	CHE_ByteStringToPtrMap::Append( const CHE_ByteString & str, HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL || m_pString == NULL )
	{
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( 128 );
		m_pString = GetAllocator()->NewArray<CHE_ByteString*>( 128 );
		m_lSize = 128;
		m_lCount = 1;
		m_pData[0] = ptr;
		m_pString[0] = GetAllocator()->New<CHE_ByteString>( str );
		return TRUE;
	}
	if ( m_lCount + 1 <= m_lSize )
	{
		m_pData[m_lCount] = ptr;
		m_pString[m_lCount] = GetAllocator()->New<CHE_ByteString>( str );
		m_lCount++;
		return TRUE;
	}else{
		HE_LPVOID * pTmp = GetAllocator()->NewArray<HE_LPVOID>( m_lSize );
		CHE_ByteString ** pStrTmp = GetAllocator()->NewArray<CHE_ByteString*>( m_lSize );
		memcpy( pTmp, m_pData, m_lSize * sizeof(HE_LPVOID) );
		memcpy( pStrTmp, m_pString, m_lSize * sizeof(HE_LPVOID) );
		m_lSize *= 2;
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		GetAllocator()->DeleteArray<CHE_ByteString*>( m_pString );
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( m_lSize );
		m_pString = GetAllocator()->NewArray<CHE_ByteString*>( m_lSize );
		memcpy( m_pData, pTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		memcpy( m_pString, pStrTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		GetAllocator()->DeleteArray<HE_LPVOID>( pTmp );
		GetAllocator()->DeleteArray<CHE_ByteString*>( pStrTmp );
		
		m_pString[m_lCount] = GetAllocator()->New<CHE_ByteString>( str );
		m_pData[m_lCount] = ptr;
		m_lCount++;
		return TRUE;
	}
}

HE_BOOL CHE_ByteStringToPtrMap::UpdateItem( const CHE_ByteString & str, HE_LPVOID * ptr )
{
	if ( str.GetLength() != 0 && *ptr != NULL )
	{
		for ( HE_DWORD i = 0; i < m_lCount; i++ )
		{
			if ( *m_pString[i] == str )
			{
				HE_LPVOID tmpPtr = m_pData[i];
				m_pData[i] = *ptr;
				*ptr = tmpPtr;
				return TRUE;
			}
		}
	}
	return FALSE;
}

HE_LPVOID CHE_ByteStringToPtrMap::GetItem( const CHE_ByteString & str ) const
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

HE_LPVOID CHE_ByteStringToPtrMap::GetItemByIndex( HE_DWORD index ) const
{
	if ( index >= m_lCount )
	{
		return NULL;
	}
	return m_pData[index]; 
}

HE_BOOL CHE_ByteStringToPtrMap::GetKeyByIndex( HE_DWORD index, CHE_ByteString & strRet ) const
{
	if ( index >= m_lCount )
	{
		return FALSE;
	}
	strRet = *(m_pString[index]);
	return TRUE;
}

HE_VOID	CHE_ByteStringToPtrMap::Clear()
{
	if ( m_pData )
	{
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		m_pData = NULL;
	}
	if ( m_pString && m_lCount > 0 )
	{
		for ( HE_DWORD i = 0; i < m_lCount; i++ )
		{
			GetAllocator()->DeleteArray<CHE_ByteString>( m_pString[i] );
		}
		GetAllocator()->DeleteArray<CHE_ByteString*>( m_pString );
	}
	m_lSize = 0;
	m_lCount = 0;
}

CHE_NumToPtrMap::CHE_NumToPtrMap( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_lCount = 0;
	m_lSize = 0;
	m_pData = NULL;
	m_pNum = NULL;
}

CHE_NumToPtrMap::~CHE_NumToPtrMap()
{
	Clear();
}

HE_BOOL CHE_NumToPtrMap::Append( HE_DWORD num, HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL || m_pNum == NULL )
	{
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( 128 );
		m_pNum = GetAllocator()->NewArray<HE_DWORD>( 128 );
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
		HE_LPVOID * pTmp = GetAllocator()->NewArray<HE_LPVOID>( m_lSize );
		HE_DWORD * pNumTmp = GetAllocator()->NewArray<HE_DWORD>( m_lSize );
		memcpy( pTmp, m_pData, m_lSize * sizeof(HE_LPVOID) );
		memcpy( pNumTmp, m_pNum, m_lSize * sizeof(HE_LPVOID) );
		m_lSize *= 2;
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		GetAllocator()->DeleteArray<HE_DWORD>( m_pNum );
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( m_lSize );
		m_pNum = GetAllocator()->NewArray<HE_DWORD>( m_lSize );
		memcpy( m_pData, pTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		memcpy( m_pNum, pNumTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		GetAllocator()->DeleteArray<HE_LPVOID>( pTmp );
		GetAllocator()->DeleteArray<HE_DWORD>( pNumTmp );		
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

HE_DWORD CHE_NumToPtrMap::GetNumByIndex( HE_DWORD index )
{
	if ( index >= m_lCount )
	{
		return NULL;
	}
	return m_pNum[index]; 
}

HE_VOID CHE_NumToPtrMap::Clear()
{
	m_lSize = 0;
	m_lCount = 0;
	if ( m_pData )
	{
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		m_pData = NULL;
	}
	if ( m_pNum && m_lCount > 0 )
	{
		GetAllocator()->DeleteArray<HE_DWORD>( m_pNum );
		m_pNum = NULL;
	}
}


CHE_SkipList::CHE_SkipList( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ), m_lLevel( 0 ), m_lCount( 0 ) {}

CHE_SkipList::~CHE_SkipList() { Clear(); }

HE_BOOL CHE_SkipList::Append( HE_DWORD val )
{
	if ( val == 36395064 || val == 36395013 )
	{
		int x = 0;
	}
	if ( m_Forward.size() == 0 )
	{
		SkipListNode * pNewNode = GetAllocator()->New<SkipListNode>();
		pNewNode->lValue = val;
		m_Forward.push_back( pNewNode );
		m_lCount++;
		return TRUE;
	}else{
		HE_DWORD lLevelRet = 0;
		std::vector<SkipListNode*> nodeVector;
		if ( Find( val, lLevelRet, nodeVector ) == FALSE )
		{
			if ( nodeVector.size() == 1 )
			{
				SkipListNode * pNewNode = GetAllocator()->New<SkipListNode>();
				pNewNode->lValue = val;
				m_Forward.push_back( pNewNode );
				m_lCount++;
				return TRUE;
			}else{
				HE_DWORD targetLevel = RandomLevel();
				if ( targetLevel > m_lLevel )
				{
					m_lLevel = targetLevel;
				}
				SkipListNode * pNewNode = GetAllocator()->New<SkipListNode>();
				pNewNode->lValue = val;
				for ( size_t i = 0, index = nodeVector.size() - 1; i <= targetLevel; i++, index-- )
				{
					if ( nodeVector[index] == NULL )
					{
						if ( m_Forward.size() > i )
						{
							pNewNode->Forward.push_back( m_Forward[i] );
							m_Forward[i] = pNewNode;
						}else{
							m_Forward.push_back( pNewNode );
						}
					}else if ( nodeVector[index]->Forward.size() > i )
					{
						pNewNode->Forward.push_back( nodeVector[index]->Forward[i] );
						nodeVector[index]->Forward[i] = pNewNode;
					}else{
						nodeVector[index]->Forward.push_back( pNewNode );
					}
				}
				m_lCount++;
				return TRUE;
			}
		}else{
			return FALSE;
		}
	}
	return FALSE;
}

HE_BOOL CHE_SkipList::IsExist( HE_DWORD val )
{
	if ( m_Forward.size() == 0 )
	{
		return FALSE;
	}
	HE_DWORD curLevel = m_lLevel;
	SkipListNode* pTmpNode = m_Forward[m_Forward.size()-1];
	SkipListNode* pPreNode = NULL;

	while( pTmpNode )
	{
		if ( val == pTmpNode->lValue )
		{
			return TRUE;
		}else if ( val < pTmpNode->lValue )
		{
			//to lower level
			if ( curLevel == 0 )
			{
				return FALSE;
			}else{
				if ( pPreNode == NULL )
				{
					pTmpNode = m_Forward[--curLevel];
				}else{
					pTmpNode = pPreNode->Forward[--curLevel];
				}
			}
		}else if ( val > pTmpNode->lValue )
		{
			if ( pTmpNode->Forward.size() > curLevel )
			{
				//to next node
				pPreNode = pTmpNode;
				pTmpNode = pTmpNode->Forward[curLevel];
			}else if ( pTmpNode->Forward.size() == 0 )
			{
				return FALSE;
			}else{
				--curLevel;
			}
		}
	}
	return FALSE;
}

HE_VOID	CHE_SkipList::Clear()
{
	if ( m_Forward.size() == 0 )
	{
		return;
	}
	m_lLevel = 0;
	SkipListNode* pTmpNode = m_Forward[0];
	SkipListNode* pTmpNode2 = NULL;
	m_Forward.clear();
	while( true )
	{
		if ( pTmpNode->Forward.size() > 0 )
		{
			pTmpNode2 = pTmpNode->Forward[0];
			pTmpNode->Forward.clear();
			pTmpNode = pTmpNode2;
		}else{
			break;
		}
	}
}

HE_BOOL CHE_SkipList::Find( HE_DWORD val, HE_DWORD & levelRet, std::vector<SkipListNode*> & nodeVector ) const
{
	if ( m_Forward.size() == 0 )
	{
		return FALSE;
	}
	HE_DWORD curLevel = m_lLevel;
	SkipListNode* pTmpNode = m_Forward[m_Forward.size()-1];
	SkipListNode* pPreNode = NULL;

	nodeVector.push_back( NULL );
	
	while( pTmpNode )
	{
		if ( val == pTmpNode->lValue )
		{
			levelRet = curLevel;
			return TRUE;
		}else if ( val < pTmpNode->lValue )
		{
			//to lower level
			if ( curLevel == 0 )
			{
				levelRet = 0;
				nodeVector.push_back( pPreNode );
				return FALSE;
			}else{
				if ( pPreNode == NULL )
				{
					nodeVector.push_back( NULL );
					pTmpNode = m_Forward[--curLevel];
				}else{
					nodeVector.push_back( pPreNode );
					pTmpNode = pPreNode->Forward[--curLevel];
				}
			}
		}else if ( val > pTmpNode->lValue )
		{
			if ( pTmpNode->Forward.size() > curLevel )
			{
				//to next node
				pPreNode = pTmpNode;
				pTmpNode = pTmpNode->Forward[curLevel];
			}else if ( pTmpNode->Forward.size() == 0 )
			{
				levelRet = curLevel;
				for ( HE_DWORD i = curLevel+1; i > 0; i-- )
				{
					nodeVector.push_back( pTmpNode );
				}
				return FALSE;
			}else{
				for ( HE_DWORD i = curLevel+1; i > pTmpNode->Forward.size(); i-- )
				{
					nodeVector.push_back( pTmpNode );
					--curLevel;
				}
			}
		}
	}
	levelRet = 0;
	return FALSE;
}

HE_DWORD CHE_SkipList::RandomLevel()
{
// 	srand( time(0) );
// 	HE_DWORD levelRet = 0;
// 	while ( levelRet <= m_lLevel  )
// 	{
// 		if ( ( rand() * 1.0 / 0x7FFF ) > 0.2 || rand()%2 == 0 )
// 		{
// 			levelRet++;
// 		}else{
// 			break;
// 		}
// 	}
	static HE_DWORD flag = 0;
	HE_DWORD levelRet = 0;
	levelRet = rand()%(m_lLevel+1);
	if ( flag == 500 )
	{
		flag = 0;
		return m_lLevel+1;
	}else{
		flag++;
	}
	return levelRet;
}