#include "../include/che_datastructure.h"

#include <memory>
#include <cstdlib>
#include <ctime>

CHE_DynBuffer::CHE_DynBuffer( HE_ULONG capacity /*= 1024*/, HE_ULONG increament /*= 1024*/, CHE_Allocator * pAllocator /*= NULL*/ )
: CHE_Object( pAllocator )
{
	m_lCapacity = capacity;
	m_lIncreament = increament;

	m_lpData = GetAllocator()->NewArray<HE_BYTE>( m_lCapacity );
	memset( m_lpData, 0, m_lCapacity );
	m_lSize = 0;
}

CHE_DynBuffer::CHE_DynBuffer( const CHE_DynBuffer & buf )
: CHE_Object( buf.GetAllocator() )
{
	m_lCapacity = buf.m_lCapacity;
	m_lIncreament = buf.m_lIncreament;
	m_lSize = buf.m_lSize;
	m_lpData = GetAllocator()->NewArray<HE_BYTE>( m_lCapacity );
	memset( m_lpData, 0, m_lCapacity );
	if ( m_lSize > 0 )
	{
		memcpy( m_lpData, buf.m_lpData, m_lSize );
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
		m_lCapacity = buf.m_lCapacity;
		m_lIncreament = buf.m_lIncreament;
		m_lSize = buf.m_lSize;
		m_lpData = GetAllocator()->NewArray<HE_BYTE>( m_lCapacity );
		memset( m_lpData, 0, m_lCapacity );
		if ( m_lSize > 0 )
		{
			memcpy( m_lpData, buf.m_lpData, m_lSize );
		}
	}
	return *this;
}

HE_ULONG CHE_DynBuffer::Write( HE_LPCBYTE pBuffer, HE_ULONG offset, HE_ULONG size )
{
	if ( pBuffer == NULL || size == 0 || offset > m_lSize )
	{
		return 0;
	}

	if ( size + offset > m_lCapacity )
	{
		HE_ULONG lNeed = size + offset - m_lCapacity;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_ULONG)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPBYTE tmp = GetAllocator()->NewArray<HE_BYTE>( m_lCapacity + lNeed * m_lIncreament );
		memset( tmp, 0, m_lCapacity + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lSize );
		memcpy( tmp + offset, pBuffer, size );
		GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
		m_lpData = tmp;
		m_lSize = offset + size;
		m_lCapacity += lNeed * m_lIncreament;
		return size;
	}else{
		memcpy( m_lpData + offset, pBuffer, size );
		m_lSize = offset + size;
		return size;
	}
}

HE_VOID CHE_DynBuffer::Alloc( HE_ULONG size )
{
	if ( size <= m_lCapacity )
	{
		m_lSize = size;
		return;
	}
	if ( m_lpData )
	{
		GetAllocator()->DeleteArray( m_lpData );
	}
	m_lpData = GetAllocator()->NewArray<HE_BYTE>( size );
	m_lSize = size;
	m_lCapacity = size;
	m_lIncreament = size;
}

HE_ULONG CHE_DynBuffer::Write( HE_LPCBYTE pBuffer, HE_ULONG size )
{
	return Write( pBuffer, m_lSize, size );
// 	if ( pBuffer == NULL || size == 0 )
// 	{
// 		return 0;
// 	}
// 
// 	if ( m_lSize + size > m_lCapacity )
// 	{
// 		HE_ULONG lNeed = m_lSize + size - m_lCapacity;
// 		if ( lNeed <= m_lIncreament )
// 		{
// 			lNeed = 1;
// 		}else{
// 			lNeed = (HE_ULONG)( lNeed / m_lIncreament ) + 1;
// 		}
// 		HE_LPBYTE tmp = GetAllocator()->NewArray<HE_BYTE>( m_lCapacity + lNeed * m_lIncreament );
// 		memset( tmp, 0, m_lCapacity + lNeed * m_lIncreament );
// 		memcpy( tmp, m_lpData, m_lSize );
// 		memcpy( tmp+m_lSize, pBuffer, size );
// 		GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
// 		m_lpData = tmp;
// 		m_lSize += size;
// 		m_lCapacity += lNeed*m_lIncreament;
// 		return size;
// 	}else{
// 		memcpy( m_lpData+m_lSize, pBuffer, size );
// 		m_lSize += size;
// 		return size;
// 	}
}

HE_ULONG CHE_DynBuffer::Write( const CHE_DynBuffer & dynBuffer )
{
	if ( dynBuffer.m_lpData == NULL || dynBuffer.m_lSize == 0 )
	{
		return 0;
	}

	if ( m_lSize + dynBuffer.m_lSize > m_lCapacity )
	{
		HE_ULONG lNeed = m_lSize + dynBuffer.m_lSize - m_lCapacity;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_ULONG)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPBYTE tmp = GetAllocator()->NewArray<HE_BYTE>( m_lCapacity + lNeed * m_lIncreament );
		memset( tmp, 0, m_lCapacity + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lSize );
		memcpy( tmp+m_lSize, dynBuffer.m_lpData, dynBuffer.m_lSize );
		GetAllocator()->DeleteArray<HE_BYTE>( m_lpData );
		m_lpData = tmp;
		m_lSize += dynBuffer.m_lSize;
		m_lCapacity += lNeed*m_lIncreament;
		return dynBuffer.m_lSize;
	}else{
		memcpy( m_lpData+m_lSize, dynBuffer.m_lpData, dynBuffer.m_lSize );
		m_lSize += dynBuffer.m_lSize;
		return dynBuffer.m_lSize;
	}
}

HE_ULONG CHE_DynBuffer::Read( HE_LPBYTE pBuffer, HE_ULONG size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lSize < size )
	{
		size = m_lSize;
	}

	memcpy( pBuffer, m_lpData, size );

	return size;
}

HE_BOOL CHE_DynBuffer::ReadByte( HE_ULONG offset, HE_LPBYTE pByte )
{
	if ( offset < m_lSize  )
	{
		*pByte = *(m_lpData+offset);
		return TRUE;
	}
	return FALSE;
}

class IHE_DynBufferWrite : public IHE_Write
{
public:
	IHE_DynBufferWrite( CHE_DynBuffer * pBuffer, CHE_Allocator * pAllocator = NULL )
		: IHE_Write( pAllocator ), mpBuffer( pBuffer ) {}

	~IHE_DynBufferWrite() {}

	HE_ULONG	GetSize()
	{
		if ( mpBuffer )
		{
			return mpBuffer->GetSize();
		}
		return 0;
	}

	HE_ULONG	GetCurOffset()
	{
		if ( GetSize() == 0 )
		{
			return 0;
		}
		return GetSize()-1;
	}

	HE_ULONG	Flush() { return 0; } 

	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_ULONG offset, HE_ULONG size )
	{
		if ( mpBuffer == NULL || pData == NULL || size == 0 || offset > GetSize() )
		{
			return FALSE;
		}
		mpBuffer->Write( (HE_LPCBYTE)pData, offset, size );
		return TRUE;
	}

	void		Release() {}

private:
	CHE_DynBuffer * mpBuffer;
};

IHE_Write * HE_CreateDynBufferWrite( CHE_DynBuffer * pBuffer, CHE_Allocator * pAllocator )
{
	if ( pBuffer == NULL )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	return pAllocator->New<IHE_DynBufferWrite>( pBuffer, pAllocator );
}

CHE_DynWideByteBuffer::CHE_DynWideByteBuffer( HE_ULONG capacity /*= 1024*/, HE_ULONG increament /*= 1024*/, CHE_Allocator * pAllocator /*= NULL*/ )
: CHE_Object ( pAllocator )
{
	m_lCapacity = capacity;
	m_lIncreament = increament;

	m_lpData = GetAllocator()->NewArray<HE_WCHAR>( m_lCapacity );
	memset( m_lpData, 0, m_lCapacity * sizeof(HE_WCHAR) );
	m_lSize = 0;
}

CHE_DynWideByteBuffer::CHE_DynWideByteBuffer( const CHE_DynWideByteBuffer & buf )
: CHE_Object ( buf.GetAllocator() )
{
	m_lCapacity = buf.m_lCapacity;
	m_lIncreament = buf.m_lIncreament;
	m_lSize = buf.m_lSize;
	m_lpData = GetAllocator()->NewArray<HE_WCHAR>( m_lCapacity );
	if ( m_lSize > 0 )
	{
		memcpy( m_lpData, buf.m_lpData, m_lSize * sizeof(HE_WCHAR) );
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
		m_lCapacity = buf.m_lCapacity;
		m_lIncreament = buf.m_lIncreament;
		m_lSize = buf.m_lSize;
		m_lpData = GetAllocator()->NewArray<HE_WCHAR>( m_lCapacity );
		if ( m_lSize > 0 )
		{
			memcpy( m_lpData, buf.m_lpData, m_lSize * sizeof(HE_WCHAR) );
		}
	}
	return *this;
}

HE_ULONG CHE_DynWideByteBuffer::Write( HE_LPCWSTR pBuffer, HE_ULONG size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lSize + size > m_lCapacity )
	{
		HE_ULONG lNeed = m_lSize + size - m_lCapacity;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_ULONG)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPWSTR tmp = GetAllocator()->NewArray<HE_WCHAR>( m_lCapacity + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lSize * sizeof(HE_WCHAR) );
		memcpy( tmp+m_lSize, pBuffer, size * sizeof(HE_WCHAR) );
		GetAllocator()->DeleteArray<HE_WCHAR>( m_lpData );
		m_lpData = tmp;
		m_lSize+= size;
		m_lCapacity += lNeed*m_lIncreament;
		return size;
	}else{
		memcpy( m_lpData+m_lSize, pBuffer, size * sizeof(HE_WCHAR) );
		m_lSize += size;
		return size;
	}
}

HE_ULONG CHE_DynWideByteBuffer::Write( const CHE_DynWideByteBuffer & dynBuffer )
{
	if ( dynBuffer.m_lpData == NULL || dynBuffer.m_lSize == 0 )
	{
		return 0;
	}

	if ( m_lSize + dynBuffer.m_lSize > m_lCapacity )
	{
		HE_ULONG lNeed = m_lSize + dynBuffer.m_lSize - m_lCapacity;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (HE_ULONG)( lNeed / m_lIncreament ) + 1;
		}
		HE_LPWSTR tmp = GetAllocator()->NewArray<HE_WCHAR>( m_lCapacity + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lSize * sizeof(HE_WCHAR) );
		memcpy( tmp+m_lSize, dynBuffer.m_lpData, dynBuffer.m_lSize * sizeof(HE_WCHAR) );
		GetAllocator()->DeleteArray<HE_WCHAR>( m_lpData );
		m_lpData = tmp;
		m_lSize+= dynBuffer.m_lSize;
		m_lCapacity += lNeed*m_lIncreament;
		return dynBuffer.m_lSize;
	}else{
		memcpy( m_lpData+m_lSize, dynBuffer.m_lpData, dynBuffer.m_lSize * sizeof(HE_WCHAR) );
		m_lSize += dynBuffer.m_lSize;
		return dynBuffer.m_lSize;
	}
}

HE_ULONG CHE_DynWideByteBuffer::Read( HE_LPWSTR pBuffer, HE_ULONG size )
{
	if ( pBuffer == NULL || size == 0 )
	{
		return 0;
	}

	if ( m_lSize < size )
	{
		size = m_lSize;
	}

	memcpy( pBuffer, m_lpData, size * sizeof(HE_WCHAR) );

	return size;
}

HE_BOOL CHE_DynWideByteBuffer::ReadByte( HE_ULONG offset, HE_LPWSTR pWchar )
{
	if ( offset < m_lSize  )
	{
		*pWchar = *(m_lpData+offset);
		return TRUE;
	}
	return FALSE;
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

HE_LPVOID CHE_PtrArray::GetItem( HE_ULONG index ) const
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
	if ( m_lCount < m_lSize )
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
		for ( HE_ULONG i = 0; i < m_lCount; i++ )
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
	for ( HE_ULONG i = 0; i < m_lCount; i++ )
	{
		if ( *m_pString[i] == str )
		{
			return m_pData[i];
		}
	}
	return NULL;
}

HE_LPVOID CHE_ByteStringToPtrMap::GetItemByIndex( HE_ULONG index ) const
{
	if ( index >= m_lCount )
	{
		return NULL;
	}
	return m_pData[index]; 
}

HE_BOOL CHE_ByteStringToPtrMap::GetKeyByIndex( HE_ULONG index, CHE_ByteString & strRet ) const
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
	if ( m_pString )
	{
		for ( HE_ULONG i = 0; i < m_lCount; i++ )
		{
			GetAllocator()->Delete<CHE_ByteString>( m_pString[i] );
		}
		GetAllocator()->DeleteArray<CHE_ByteString*>( m_pString );
		m_pString = NULL;
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

HE_BOOL CHE_NumToPtrMap::Append( HE_ULONG num, HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL || m_pNum == NULL )
	{
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( 128 );
		m_pNum = GetAllocator()->NewArray<HE_ULONG>( 128 );
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
		HE_ULONG * pNumTmp = GetAllocator()->NewArray<HE_ULONG>( m_lSize );
		memcpy( pTmp, m_pData, m_lSize * sizeof(HE_LPVOID) );
		memcpy( pNumTmp, m_pNum, m_lSize * sizeof(HE_LPVOID) );
		m_lSize *= 2;
		GetAllocator()->DeleteArray<HE_LPVOID>( m_pData );
		GetAllocator()->DeleteArray<HE_ULONG>( m_pNum );
		m_pData = GetAllocator()->NewArray<HE_LPVOID>( m_lSize );
		m_pNum = GetAllocator()->NewArray<HE_ULONG>( m_lSize );
		memcpy( m_pData, pTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		memcpy( m_pNum, pNumTmp, m_lSize/2 * sizeof(HE_LPVOID) );
		GetAllocator()->DeleteArray<HE_LPVOID>( pTmp );
		GetAllocator()->DeleteArray<HE_ULONG>( pNumTmp );		
		m_pNum[m_lCount] = num;
		m_pData[m_lCount] = ptr;
		m_lCount++;
		return TRUE;
	}
}

HE_LPVOID CHE_NumToPtrMap::GetItem( HE_ULONG num ) const
{
	for ( HE_ULONG i = 0; i < m_lCount; i++ )
	{
		if ( m_pNum[i] == num )
		{
			return m_pData[i];
		}
	}
	return NULL;
}

HE_LPVOID CHE_NumToPtrMap::GetItemByIndex( HE_ULONG index )
{
	if ( index >= m_lCount )
	{
		return NULL;
	}
	return m_pData[index]; 
}

HE_ULONG CHE_NumToPtrMap::GetNumByIndex( HE_ULONG index )
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
	if ( m_pNum )
	{
		GetAllocator()->DeleteArray<HE_ULONG>( m_pNum );
		m_pNum = NULL;
	}
}