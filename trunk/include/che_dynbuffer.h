#ifndef _CHE_DYNBUFFER_H_
#define _CHE_DYNBUFFER_H_

#include "che_base.h"

class CHE_DynBuffer : public CHE_Object
{
public:
	CHE_DynBuffer( HE_DWORD size = 1024, HE_DWORD increament = 1024, CHE_Allocator * pAllocator = NULL );
	CHE_DynBuffer( const CHE_DynBuffer & buf );
	~CHE_DynBuffer();

	const CHE_DynBuffer & operator = ( const CHE_DynBuffer & buf );

	HE_LPBYTE GetData() { return m_lpData; }

	HE_DWORD GetSize() { return m_lSize; }

	HE_DWORD GetByteCount() { return m_lByteCount; }

	HE_DWORD Write( HE_LPCBYTE pBuffer, HE_DWORD size );

	HE_DWORD Read( HE_LPBYTE pBuffer, HE_DWORD size );

	HE_BOOL ReadByte( HE_DWORD offset, HE_LPBYTE pByte );

	HE_DWORD Write( const CHE_DynBuffer & dynBuffer );

	HE_VOID	Clear() { m_lByteCount = 0; }

private:

	HE_DWORD	m_lSize;
	HE_DWORD	m_lIncreament;

	HE_LPBYTE	m_lpData;
	HE_DWORD	m_lByteCount;
};

class CHE_DynWideByteBuffer : public CHE_Object
{
public:
	CHE_DynWideByteBuffer( HE_DWORD size = 1024, HE_DWORD increament = 1024, CHE_Allocator * pAllocator = NULL );
	CHE_DynWideByteBuffer( const CHE_DynWideByteBuffer & buf );
	~CHE_DynWideByteBuffer();
	
	const CHE_DynWideByteBuffer & operator = ( const CHE_DynWideByteBuffer & buf );
	
	HE_LPWSTR GetData() { return m_lpData; }
	
	HE_DWORD GetSize() { return m_lSize; }
	
	HE_DWORD GetByteCount() { return m_lByteCount; }
	
	HE_DWORD Write( HE_LPCWSTR pBuffer, HE_DWORD size );
	
	HE_DWORD Read( HE_LPWSTR pBuffer, HE_DWORD size );
	
	HE_BOOL ReadByte( HE_DWORD offset, HE_LPWSTR pWcharRet );
	
	HE_DWORD Write( const CHE_DynWideByteBuffer & dynBuffer );
	
	HE_VOID	Clear() { m_lByteCount = 0; }
	
private:
	
	HE_DWORD	m_lSize;
	HE_DWORD	m_lIncreament;
	
	HE_LPWSTR	m_lpData;
	HE_DWORD	m_lByteCount;
};


#endif