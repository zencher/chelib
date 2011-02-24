#include <stdlib.h>
#include "../include/che_base.h"
#include <stdio.h>
#include <memory.h>



// HE_VOID* CHE_Object::operator new( size_t size )
// {
// 	return malloc( size );
// }
// 
// HE_VOID CHE_Object::operator delete( HE_VOID* p )
// {
// 	free( p );
// }
// 
// HE_VOID* CHE_Object::operator new( size_t size, HE_LPCSTR lpszFileName, HE_INT32 nLine )
// {
// 	return malloc( size );
// }
// 
// 
// HE_VOID CHE_Object::operator delete( HE_VOID* p, HE_LPCSTR lpszFileName, HE_INT32 nLine )
// {
// 	free( p );
// }

class IHE_SysFileWrite: public IHE_FileWrite
{
public:
	IHE_SysFileWrite( FILE * pfile );
	IHE_SysFileWrite( HE_LPCSTR filename );
	~IHE_SysFileWrite();

	virtual HE_DWORD	GetSize();
	virtual HE_DWORD	Flush();

	virtual	HE_BOOL		WriteBlock( const void* pData, HE_DWORD offset, HE_DWORD size);

	virtual void		Release();

private:
	FILE * m_pFile;
};

IHE_SysFileWrite::IHE_SysFileWrite( FILE * pfile )
{
	m_pFile = pfile;
}

IHE_SysFileWrite::IHE_SysFileWrite( HE_LPCSTR filename )
{
	if ( filename == NULL )
	{
		m_pFile = NULL;
	}else{
		m_pFile = fopen( filename, "wb+" );
	}
}

IHE_SysFileWrite::~IHE_SysFileWrite()
{
	if ( m_pFile != NULL )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

HE_DWORD IHE_SysFileWrite::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

HE_DWORD IHE_SysFileWrite::Flush()
{
	if ( m_pFile )
	{
		return fflush( m_pFile );
	}else{
		return 0;
	}
}

void IHE_SysFileWrite::Release()
{
	if ( m_pFile )
	{
		fflush( m_pFile );
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

HE_BOOL IHE_SysFileWrite::WriteBlock( const void* pData, HE_DWORD offset, HE_DWORD size)
{
	if ( pData == NULL || size == 0 )
	{
		return FALSE;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		HE_DWORD dwRet = fwrite( pData, 1, size, m_pFile );
		if ( dwRet > 0 )
		{
			return TRUE;
		}else{
			return FALSE;
		}
	}else{
		return FALSE;
	}
}

IHE_FileWrite* HE_CreateFileWrite( HE_LPCSTR filename )
{
	FILE * pFile = fopen( filename, "wb+" );
	if ( pFile != NULL )
	{
		IHE_FileWrite * pTmp = new IHE_SysFileWrite( pFile );
		return pTmp;
	}else{
		return NULL;
	}
}

// IHE_FileWrite* HE_CreateFileWrite( HE_LPCWSTR filename )
// {
// 
// }

class IHE_SysFileRead: public IHE_FileRead
{
public:
	IHE_SysFileRead( FILE * pfile );
	IHE_SysFileRead( HE_LPCSTR filename );
	~IHE_SysFileRead();

	virtual HE_DWORD	GetSize();
	
	virtual HE_BOOL		ReadBlock( void* buffer, HE_DWORD offset, HE_DWORD size );

	virtual HE_BYTE		ReadByte( HE_DWORD offset );

	virtual void		Release();

private:
	FILE * m_pFile;
};

IHE_SysFileRead::IHE_SysFileRead( FILE * pfile )
{
	m_pFile = pfile;
}

IHE_SysFileRead::IHE_SysFileRead( HE_LPCSTR filename )
{
	if ( filename == NULL )
	{
		m_pFile = NULL;
	}else{
		m_pFile = fopen( filename, "rb" );
	}
}

IHE_SysFileRead::~IHE_SysFileRead()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

HE_DWORD IHE_SysFileRead::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

HE_BOOL IHE_SysFileRead::ReadBlock(void* buffer, HE_DWORD offset, HE_DWORD size)
{
	if ( buffer == NULL )
	{
		return FALSE;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		HE_DWORD dwRet = fread( buffer, 1, size, m_pFile  );
		if ( dwRet > 0 )
		{
			return TRUE;
		}else{
			return FALSE;
		}
	}else{
		return FALSE;
	}
}

HE_BYTE IHE_SysFileRead::ReadByte( HE_DWORD offset )
{
	if ( m_pFile )
	{
		if ( fseek( m_pFile, offset, SEEK_SET ) == -1 )
		{
			return 0;
		}else{
			HE_BYTE byte;
			HE_DWORD dwRet = fread( &byte, 1, 1, m_pFile );
			if ( dwRet == 1 )
			{
				return byte;
			}else{
				return 0;
			}
		}
	}else{
		return 0;
	}
}

void IHE_SysFileRead::Release()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

IHE_FileRead* HE_CreateFileRead( HE_LPCSTR filename )
{
	FILE * pFile = fopen( filename, "rb+" );
	if ( pFile != NULL )
	{
		IHE_FileRead * pTmp = new IHE_SysFileRead( pFile );
		return pTmp;
	}else{
		return NULL;
	}
}

// IHE_FileRead* HE_CreateFileRead(HE_LPCWSTR filename);
// {
// 
// }


CHE_PtrArray::CHE_PtrArray()
{
	m_pData = NULL;
	m_lSize = 0;
	m_lCount = 0;
}

CHE_PtrArray::~CHE_PtrArray()
{
	if ( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
		m_lSize = 0;
		m_lCount = 0;
	}
}
	
HE_BOOL CHE_PtrArray::Append( HE_LPVOID ptr )
{
	if ( m_lSize == 0 || m_lCount == 0 || m_pData == NULL )
	{
		m_pData = new HE_LPVOID[128];
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
		HE_LPVOID * pTmp = new HE_LPVOID[m_lSize];
		memcpy( pTmp, m_pData, m_lSize );
		m_lSize *= 2;
		delete [] m_pData;
		m_pData = new HE_LPVOID[m_lSize];
		memcpy( m_pData, pTmp, m_lSize/2 );
		delete [] pTmp;
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
		delete [] m_pData;
		m_pData = NULL;
		m_lSize = 0;
		m_lCount = 0;
	}	
}





	


