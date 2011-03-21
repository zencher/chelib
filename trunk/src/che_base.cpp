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

class IHE_CrtFileWrite: public IHE_Write
{
public:
	IHE_CrtFileWrite( HE_LPCSTR filename );

	~IHE_CrtFileWrite();

	virtual HE_DWORD	GetSize();

	virtual HE_DWORD	Flush();

	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_DWORD offset, HE_DWORD size);

	virtual void		Release();

private:
	FILE *				m_pFile;
};

IHE_CrtFileWrite::IHE_CrtFileWrite( HE_LPCSTR filename )
{
	if ( filename == NULL )
	{
		m_pFile = NULL;
	}else{
		m_pFile = fopen( filename, "wb" );
	}
}

IHE_CrtFileWrite::~IHE_CrtFileWrite()
{
	if ( m_pFile != NULL )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

HE_DWORD IHE_CrtFileWrite::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

HE_DWORD IHE_CrtFileWrite::Flush()
{
	if ( m_pFile )
	{
		return fflush( m_pFile );
	}else{
		return 0;
	}
}

void IHE_CrtFileWrite::Release()
{
	if ( m_pFile )
	{
		fflush( m_pFile );
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

HE_BOOL IHE_CrtFileWrite::WriteBlock( const HE_LPVOID pData, HE_DWORD offset, HE_DWORD size )
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

IHE_Write* HE_CreateFileWrite( HE_LPCSTR filename )
{
	if ( filename != NULL )
	{
		IHE_Write * pTmp = new IHE_CrtFileWrite( filename );
		return pTmp;
	}else{
		return NULL;
	}
}

class IHE_CrtFileReadDefault: public IHE_Read
{
public:
	IHE_CrtFileReadDefault( HE_LPCSTR filename );

	~IHE_CrtFileReadDefault();

	virtual HE_DWORD	GetSize();

	virtual HE_DWORD	ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size );

	virtual HE_BYTE		ReadByte( HE_DWORD offset );

	virtual void		Release();

private:
	FILE *				m_pFile;
};

IHE_CrtFileReadDefault::IHE_CrtFileReadDefault( HE_LPCSTR filename )
{
	if ( filename == NULL )
	{
		m_pFile = NULL;
	}else{
		m_pFile = fopen( filename, "rb" );
	}
}

IHE_CrtFileReadDefault::~IHE_CrtFileReadDefault()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

HE_DWORD IHE_CrtFileReadDefault::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

HE_DWORD IHE_CrtFileReadDefault::ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size )
{
	if ( buffer == NULL )
	{
		return 0;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		HE_DWORD dwRet = fread( buffer, 1, size, m_pFile );
		return dwRet;
	}else{
		return 0;
	}
}

HE_BYTE IHE_CrtFileReadDefault::ReadByte( HE_DWORD offset )
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

void IHE_CrtFileReadDefault::Release()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

class IHE_CrtFileReadMemcopy: public IHE_Read
{
public:
	IHE_CrtFileReadMemcopy( HE_LPCSTR filename );
	~IHE_CrtFileReadMemcopy();
	
	virtual HE_DWORD	GetSize() { return m_lSize; }
	
	virtual HE_DWORD	ReadBlock( void* buffer, HE_DWORD offset, HE_DWORD size );
	
	virtual HE_BYTE		ReadByte( HE_DWORD offset );
	
	virtual void		Release() {}
	
private:
	HE_LPBYTE			m_pByte;
	HE_DWORD			m_lSize;
};

IHE_CrtFileReadMemcopy::IHE_CrtFileReadMemcopy( HE_LPCSTR filename )
{
	FILE * pFile = fopen( filename, "rb" );
	if ( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		m_lSize = ftell( pFile );
		fseek( pFile, 0, SEEK_SET );
		m_pByte = new HE_BYTE[m_lSize];
		fread( m_pByte, 1, m_lSize, pFile );
		fclose( pFile );
	}else{
		m_pByte = NULL;
		m_lSize = 0;
	}
}

IHE_CrtFileReadMemcopy::~IHE_CrtFileReadMemcopy()
{
	if ( m_pByte )
	{
		delete [] m_pByte;
	}
}

HE_DWORD IHE_CrtFileReadMemcopy::ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size )
{
	if ( offset + size < m_lSize )
	{
		memcpy( buffer, m_pByte+offset, size );
		return size;
	}else{
		size = m_lSize - offset;
		memcpy( buffer, m_pByte+offset, size );
		return size;
	}
	return 0;
}

HE_BYTE IHE_CrtFileReadMemcopy::ReadByte( HE_DWORD offset )
{
	if ( offset < m_lSize )
	{
		return m_pByte[offset];
	}else{
		return 0;
	}
}

class IHE_CrtFileReadBuffer: public IHE_Read
{
public:
	IHE_CrtFileReadBuffer( HE_LPCSTR filename, HE_DWORD dwBufSize );
	
	~IHE_CrtFileReadBuffer();
	
	virtual HE_DWORD	GetSize();
	
	virtual HE_DWORD	ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size );
	
	virtual HE_BYTE		ReadByte( HE_DWORD offset );
	
	virtual void		Release();
	
private:
	FILE *				m_pFile;
	HE_LPBYTE			m_pBytes;
	HE_DWORD			m_dwBufSize;
	HE_DWORD			m_dwBufPos;
	HE_DWORD			m_dwFileSize;
};

IHE_CrtFileReadBuffer::IHE_CrtFileReadBuffer( HE_LPCSTR filename, HE_DWORD dwBufSize )
{
	m_pFile = NULL;
	m_pBytes = NULL;
	m_dwBufSize = dwBufSize;
	m_dwBufPos = 0;
	m_dwFileSize = 0;

	if ( filename == NULL )
	{
		m_pFile = NULL;
	}else{
		m_pFile = fopen( filename, "rb" );
	}
}

IHE_CrtFileReadBuffer::~IHE_CrtFileReadBuffer()
{
	if ( m_pBytes )
	{
		delete [] m_pBytes;
	}
	if ( m_pFile )
	{
		fclose( m_pFile );
	}
}

HE_DWORD IHE_CrtFileReadBuffer::GetSize()
{
	return m_dwFileSize;
}

HE_DWORD IHE_CrtFileReadBuffer::ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size )
{
	if( buffer == NULL || size == 0 || offset >= m_dwFileSize )
	{
		return 0;
	}
	HE_DWORD oriOffset = offset, oriSize = size;

	while ( true )
	{
		if ( m_pBytes == NULL )
		{
			m_pBytes = new HE_BYTE[m_dwBufSize];
			fseek( m_pFile, offset, SEEK_SET );
			fread( m_pBytes, 1, m_dwBufSize, m_pFile );
			//memcpy(  )
		}

		if ( offset < m_dwBufPos || offset > m_dwBufPos + m_dwBufSize  )
		{
			//¸üÐÂ»º´æ
		}else{
			if ( offset + size <= m_dwBufPos + m_dwBufSize )
			{
				memcpy( buffer, m_pBytes + (offset-m_dwBufPos), size );
				return size;
			}else{

			}
		}
	}
	


}

HE_BYTE	IHE_CrtFileReadBuffer::ReadByte( HE_DWORD offset )
{
	return 0;
}

void IHE_CrtFileReadBuffer::Release()
{

}

IHE_Read* HE_CreateFileRead( HE_LPCSTR filename, HE_BYTE mode /*= 0*/, HE_DWORD param /*= 4096*/ )
{
	if ( filename != NULL )
	{
		switch ( mode )
		{
		case FILEREAD_MODE_DEFAULT:
			return new IHE_CrtFileReadDefault( filename );
		case FILEREAD_MODE_MEMCOPY:
			return new IHE_CrtFileReadMemcopy( filename );
		case FILEREAD_MODE_BUFFER:
			return NULL;
		case FILEREAD_MODE_BLOCKLINK:
			return NULL;
		default:
			return NULL;
		}
	}else{
		return NULL;
	}
}





	


