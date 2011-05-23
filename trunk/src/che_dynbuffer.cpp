#include "../include/che_dynbuffer.h"
#include <memory.h>

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
