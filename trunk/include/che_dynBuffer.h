#ifndef _CHE_DYNBUFFER_H_
#define _CHE_DYNBUFFER_H_

#include "che_base.h"

class CHE_DynBuffer : public CHE_Object
{
public:
	CHE_DynBuffer( HE_DWORD size = 1024, HE_DWORD increament = 1024 );
	CHE_DynBuffer( const CHE_DynBuffer & buf );
	~CHE_DynBuffer();

	const CHE_DynBuffer & operator = ( const CHE_DynBuffer & buf );

	HE_DWORD GetSize() { return m_lSize; }

	HE_DWORD GetByteCount() { return m_lByteCount; }

	HE_DWORD Write( HE_LPCBYTE pBuffer, HE_DWORD size );

	HE_DWORD Read( HE_LPBYTE pBuffer, HE_DWORD size );

	HE_BOOL ReadByte( HE_DWORD offset, HE_LPBYTE pByte );

	HE_DWORD Write( const CHE_DynBuffer & dynBuffer );

	HE_VOID	Clear() { m_lByteCount = 0; }

	//HE_DWORD Read( HE_DWORD offset, HE_LPBYTE  )

	//HE_DWORD SetData( HE_DWORD offset, const HE_LPBYTE pBuffer, HE_DWORD size );

	//HE_DWORD GetData( HE_DWORD offset, HE_LPBYTE pBuffer, HE_DWORD size );

private:

	HE_DWORD	m_lSize;
	HE_DWORD	m_lIncreament;

	HE_LPBYTE	m_lpData;
	HE_DWORD	m_lByteCount;
}; 


#endif