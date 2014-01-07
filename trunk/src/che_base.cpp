#include "../include/che_base.h"

#include <cstdlib>
#include <cstdio>
#include <memory>

#ifdef WIN32
#include <windows.h>
#include <intrin.h>
#endif

#ifdef _MAC_OS_X_
#include <libkern/OSAtomic.h>
#endif

CHE_DefCrtAllocator gDefCrtAllocator;

CHE_Object::CHE_Object( CHE_Allocator * pAllocator )
{
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	m_pAllocator = pAllocator;
}


inline void* CHE_DefCrtAllocator::Alloc( size_t cb )
{
	return malloc( cb );
}

inline void CHE_DefCrtAllocator::Free( void* data )
{
	free( data );
}

inline size_t CHE_DefCrtAllocator::GetSize( void * data )
{
#ifdef WIN32
	return _msize( data );
#endif
#ifdef _LINUX_
	return malloc_usable_size( data );
#endif
#ifdef _MAC_OS_X_
    return malloc_size( data );
#endif
}

CHE_Allocator * GetDefaultAllocator()
{
	return & gDefCrtAllocator;
}

#ifdef WIN32

CHE_HeapAllocator::CHE_HeapAllocator( size_t initSize )
{
	m_Heap = HeapCreate( 0, initSize, 0 );
}

CHE_HeapAllocator::~CHE_HeapAllocator()
{
	if ( m_Heap )
	{
		HeapDestroy( m_Heap );
	}
}

inline void* CHE_HeapAllocator::Alloc( size_t cb )
{
	return HeapAlloc( m_Heap, 0, cb );
}

inline void CHE_HeapAllocator::Free( void* data )
{
	HeapFree( m_Heap, 0, data );
}

inline size_t CHE_HeapAllocator::GetSize( void * data )
{
	return HeapSize( m_Heap, 0, data );
}

#endif

class IHE_CrtFileWrite: public IHE_Write
{
public:
	IHE_CrtFileWrite( HE_LPCSTR filename,  HE_BYTE mode, CHE_Allocator * pAllocator = NULL );

	virtual ~IHE_CrtFileWrite();

	virtual HE_ULONG	GetSize();

	virtual HE_ULONG	GetCurOffset();

	virtual HE_ULONG	Flush();

	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_ULONG offset, HE_ULONG size);

	virtual void		Release();

private:
	FILE *				m_pFile;
};

IHE_CrtFileWrite::IHE_CrtFileWrite( HE_LPCSTR filename, HE_BYTE mode, CHE_Allocator * pAllocator ) : IHE_Write( pAllocator )
{
	if ( filename == NULL )
	{
		m_pFile = NULL;
	}else{
		if ( mode == FILEWRITE_MODE_NEW )
		{
			m_pFile = fopen( filename, "wb+" );
		}else{
			m_pFile = fopen( filename, "rb+" );
		}
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

HE_ULONG IHE_CrtFileWrite::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

HE_ULONG IHE_CrtFileWrite::GetCurOffset()
{
	return GetSize();
}

HE_ULONG IHE_CrtFileWrite::Flush()
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

HE_BOOL IHE_CrtFileWrite::WriteBlock( const HE_LPVOID pData, HE_ULONG offset, HE_ULONG size )
{
	if ( pData == NULL || size == 0 )
	{
		return FALSE;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		HE_ULONG dwRet = fwrite( pData, 1, size, m_pFile );
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

IHE_Write* HE_CreateFileWrite( HE_LPCSTR filename, HE_BYTE mode, CHE_Allocator * pAllocator )
{
	if ( filename != NULL )
	{
		IHE_Write * pTmp = NULL;
		if ( pAllocator == NULL )
		{
			pTmp = new IHE_CrtFileWrite( filename, mode, NULL );
		}else{
			pTmp = pAllocator->New<IHE_CrtFileWrite>( filename, mode, pAllocator );
		}
		return pTmp;
	}else{
		return NULL;
	}
}

HE_VOID HE_DestoryIHEWrite( IHE_Write * pIHEWrite )
{
	if ( pIHEWrite != NULL )
	{
		pIHEWrite->GetAllocator()->Delete<IHE_Write>( pIHEWrite );
	}
}

class IHE_MemBufRead : public IHE_Read
{
public:
	IHE_MemBufRead( HE_LPCBYTE pBuf, HE_ULONG lSize , CHE_Allocator * pAllocator = NULL ) : IHE_Read( pAllocator )
 	{ m_lSize = lSize; m_pBuf = pBuf; }
	
	virtual ~IHE_MemBufRead() {};
	
	virtual HE_ULONG	GetSize() { return m_lSize; }
	
	virtual HE_ULONG	ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size );
	
	virtual HE_BYTE		ReadByte( HE_ULONG offset );
	
	virtual void		Release() { m_pBuf = NULL; m_lSize = 0; }
	
private:
	HE_LPCBYTE	m_pBuf;
	HE_ULONG	m_lSize;
};

HE_ULONG IHE_MemBufRead::ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size )
{
	if ( buffer == NULL || m_pBuf == NULL || size == 0 )
	{
		return 0;
	}
	if ( offset >= m_lSize )
	{
		return 0;
	}else{
		if ( offset + size > m_lSize )
		{
			size = m_lSize - offset;
		}
		memcpy( buffer, m_pBuf + offset, size );
		return size;
	}
}

HE_BYTE IHE_MemBufRead::ReadByte( HE_ULONG offset )
{
	if ( offset >= m_lSize )
	{
		return 0;
	}else{
		return m_pBuf[offset];
	}
}

IHE_Read*	HE_CreateMemBufRead( HE_LPCBYTE pBuf, HE_ULONG lSize, CHE_Allocator * pAllocator )
{
	if ( pBuf == NULL || lSize == 0 )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		return new IHE_MemBufRead( pBuf, lSize, NULL );
	}else{
		return pAllocator->New<IHE_MemBufRead>( pBuf, lSize, pAllocator );
	}
}

class IHE_CrtFileReadDefault: public IHE_Read
{
public:
	IHE_CrtFileReadDefault( HE_LPCSTR filename, CHE_Allocator * pAllocator = NULL );

	virtual ~IHE_CrtFileReadDefault();

	virtual HE_ULONG	GetSize();

	virtual HE_ULONG	ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size );

	virtual HE_BYTE		ReadByte( HE_ULONG offset );

	virtual void		Release();

private:
	FILE *				m_pFile;
};

IHE_CrtFileReadDefault::IHE_CrtFileReadDefault( HE_LPCSTR filename, CHE_Allocator * pAllocator ) : IHE_Read( pAllocator )
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

HE_ULONG IHE_CrtFileReadDefault::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

HE_ULONG IHE_CrtFileReadDefault::ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size )
{
	if ( buffer == NULL )
	{
		return 0;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		HE_ULONG dwRet = fread( buffer, 1, size, m_pFile );
		return dwRet;
	}else{
		return 0;
	}
}

HE_BYTE IHE_CrtFileReadDefault::ReadByte( HE_ULONG offset )
{
	if ( m_pFile )
	{
		if ( fseek( m_pFile, offset, SEEK_SET ) == -1 )
		{
			return 0;
		}else{
			HE_BYTE byte;
			HE_ULONG dwRet = fread( &byte, 1, 1, m_pFile );
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
	IHE_CrtFileReadMemcopy( HE_LPCSTR filename, CHE_Allocator * pAllocator = NULL );
	virtual ~IHE_CrtFileReadMemcopy();
	
	virtual HE_ULONG	GetSize() { return m_lSize; }
	
	virtual HE_ULONG	ReadBlock( void* buffer, HE_ULONG offset, HE_ULONG size );
	
	virtual HE_BYTE		ReadByte( HE_ULONG offset );
	
	virtual void		Release() {}
	
private:
	HE_LPBYTE			m_pByte;
	HE_ULONG			m_lSize;
};

IHE_CrtFileReadMemcopy::IHE_CrtFileReadMemcopy( HE_LPCSTR filename, CHE_Allocator * pAllocator ) : IHE_Read( pAllocator )
{
	FILE * pFile = fopen( filename, "rb" );
	if ( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		m_lSize = ftell( pFile );
		fseek( pFile, 0, SEEK_SET );
		m_pByte = GetAllocator()->NewArray<HE_BYTE>( m_lSize );
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
		GetAllocator()->DeleteArray<HE_BYTE>(m_pByte);
	}
}

HE_ULONG IHE_CrtFileReadMemcopy::ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size )
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

HE_BYTE IHE_CrtFileReadMemcopy::ReadByte( HE_ULONG offset )
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
	IHE_CrtFileReadBuffer( HE_LPCSTR filename, HE_ULONG dwBufSize, CHE_Allocator * pAllocator = NULL );
	
	virtual ~IHE_CrtFileReadBuffer();
	
	virtual HE_ULONG	GetSize();
	
	virtual HE_ULONG	ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size );
	
	virtual HE_BYTE		ReadByte( HE_ULONG offset );
	
	virtual void		Release();
	
private:
	FILE *				m_pFile;
	HE_LPBYTE			m_pBytes;
	HE_ULONG			m_dwBufSize;
	HE_ULONG			m_dwBufPos;
	HE_ULONG			m_dwFileSize;
};

IHE_CrtFileReadBuffer::IHE_CrtFileReadBuffer( HE_LPCSTR filename, HE_ULONG dwBufSize, CHE_Allocator * pAllocator ) : IHE_Read( pAllocator )
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
		GetAllocator()->DeleteArray<HE_BYTE>( m_pBytes );
	}
	if ( m_pFile )
	{
		fclose( m_pFile );
	}
}

HE_ULONG IHE_CrtFileReadBuffer::GetSize()
{
	return m_dwFileSize;
}

HE_ULONG IHE_CrtFileReadBuffer::ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size )
{
	if( buffer == NULL || size == 0 || offset >= m_dwFileSize )
	{
		return 0;
	}
    while ( true )
	{
		if ( m_pBytes == NULL )
		{
			m_pBytes = GetAllocator()->NewArray<HE_BYTE>( m_dwBufSize );
			fseek( m_pFile, offset, SEEK_SET );
			fread( m_pBytes, 1, m_dwBufSize, m_pFile );
		}

		if ( offset < m_dwBufPos || offset > m_dwBufPos + m_dwBufSize  )
		{
			//∏¸–¬ª∫¥Ê
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

HE_BYTE	IHE_CrtFileReadBuffer::ReadByte( HE_ULONG offset )
{
	return 0;
}

void IHE_CrtFileReadBuffer::Release()
{

}

IHE_Read* HE_CreateFileRead( HE_LPCSTR filename, HE_BYTE mode, HE_ULONG param, CHE_Allocator * pAllocator )
{
	CHE_DefCrtAllocator defcrtAllocator;
	CHE_Allocator * pTmpAllocator = NULL;
	if ( pAllocator == NULL )
	{
		pTmpAllocator = &defcrtAllocator;
	}else{
		pTmpAllocator = pAllocator;
	}
	if ( filename != NULL )
	{
		switch ( mode )
		{
		case FILEREAD_MODE_DEFAULT:
			return pTmpAllocator->New<IHE_CrtFileReadDefault>( filename, pAllocator );
		case FILEREAD_MODE_MEMCOPY:
			return pTmpAllocator->New<IHE_CrtFileReadMemcopy>( filename, pAllocator );
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

HE_VOID	HE_DestoryIHERead( IHE_Read * pIHERead )
{
	if ( pIHERead != NULL )
	{
		pIHERead->GetAllocator()->Delete<IHE_Read>( pIHERead );
	}
}

class IHE_File : public CHE_Object 
{
	IHE_File( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ) {}

	virtual ~IHE_File() {};

	virtual HE_ULONG	GetSize() = 0;

	virtual HE_ULONG	GetCurOffset() = 0;

	virtual HE_ULONG	ReadBlock( HE_LPVOID buffer, HE_ULONG offset, HE_ULONG size ) = 0;

	virtual HE_BYTE		ReadByte( HE_ULONG offset ) = 0;

	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_ULONG offset, HE_ULONG size ) = 0;

	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_ULONG size )
	{
		return WriteBlock( pData, GetSize(), size );
	}

	virtual HE_BOOL		WriteByte( HE_BYTE byte )
	{
		return WriteBlock( &byte, 1 );
	}

	virtual HE_ULONG	Flush() = 0;

	virtual HE_VOID		Release() = 0;
};

/*inline*/ HE_VOID CHE_RefCount::AddRef()
{
#ifdef WIN32
	_InterlockedIncrement( &mRefCount );
#else
	//++mRefCount;
    OSAtomicIncrement32( &mRefCount );
#endif
}

/*inline*/ HE_VOID CHE_RefCount::DecRef()
{
#ifdef WIN32
	_InterlockedDecrement( &mRefCount );
#else
	//--mRefCount;
    OSAtomicDecrement32( &mRefCount );
#endif
}


CHE_Lock::CHE_Lock()
{
#ifdef WIN32
	mMutex = CreateMutex( NULL, FALSE, NULL );
#endif
}

CHE_Lock::~CHE_Lock()
{
#ifdef WIN32
	if ( mMutex )
	{
		CloseHandle( mMutex );
		mMutex = NULL;
	}
#endif
}

HE_VOID CHE_Lock::Lock() const
{
#ifdef WIN32
	WaitForSingleObject( mMutex, INFINITE );
#endif
}

HE_VOID CHE_Lock::UnLock() const
{
#ifdef WIN32
	ReleaseMutex( mMutex );
#endif
}