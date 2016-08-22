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
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	m_pAllocator = pAllocator;
}


inline void * CHE_DefCrtAllocator::Alloc( size_t cb )
{
	return malloc( cb );
}

inline void CHE_DefCrtAllocator::Free( void * data )
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

inline void * CHE_HeapAllocator::Alloc( size_t cb )
{
	return HeapAlloc( m_Heap, 0, cb );
}

inline void CHE_HeapAllocator::Free( void * data )
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
	IHE_CrtFileWrite( char const * filename,  BYTE mode, CHE_Allocator * pAllocator = nullptr );

	virtual ~IHE_CrtFileWrite();

	virtual size_t GetSize();

	virtual size_t GetCurOffset();

	virtual size_t Flush();

	virtual	bool WriteBlock( const void * pData, size_t offset, size_t size);

	virtual void Release();

private:
	FILE * m_pFile;
};

IHE_CrtFileWrite::IHE_CrtFileWrite( char const * filename, BYTE mode, CHE_Allocator * pAllocator ) : IHE_Write( pAllocator )
{
	if ( filename == nullptr )
	{
		m_pFile = nullptr;
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
	if ( m_pFile != nullptr )
	{
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

size_t IHE_CrtFileWrite::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

size_t IHE_CrtFileWrite::GetCurOffset()
{
	return GetSize();
}

size_t IHE_CrtFileWrite::Flush()
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
		m_pFile = nullptr;
	}
}

bool IHE_CrtFileWrite::WriteBlock( const void * pData, size_t offset, size_t size )
{
	if ( pData == nullptr || size == 0 )
	{
		return false;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		size_t dwRet = fwrite( pData, 1, size, m_pFile );
		if ( dwRet > 0 )
		{
			return TRUE;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

IHE_Write* HE_CreateFileWrite( char const * filename, BYTE mode, CHE_Allocator * pAllocator )
{
	if ( filename != nullptr )
	{
		IHE_Write * pTmp = nullptr;
		if ( pAllocator == nullptr )
		{
			pTmp = new IHE_CrtFileWrite( filename, mode, nullptr );
		}else{
			pTmp = pAllocator->New<IHE_CrtFileWrite>( filename, mode, pAllocator );
		}
		return pTmp;
	}else{
		return nullptr;
	}
}

void HE_DestoryIHEWrite( IHE_Write * pIHEWrite )
{
	if ( pIHEWrite != nullptr )
	{
		pIHEWrite->GetAllocator()->Delete<IHE_Write>( pIHEWrite );
	}
}

class IHE_MemBufRead : public IHE_Read
{
public:
	IHE_MemBufRead( PCBYTE pBuf, size_t lSize , CHE_Allocator * pAllocator = nullptr ) : IHE_Read( pAllocator )
 	{ m_lSize = lSize; m_pBuf = pBuf; }
	
	virtual ~IHE_MemBufRead() {};
	
	virtual size_t GetSize() { return m_lSize; }
	
	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size );
	
	virtual BYTE ReadByte( size_t offset );
	
	virtual void Release() { m_pBuf = nullptr; m_lSize = 0; }
	
private:
	PCBYTE m_pBuf;
    size_t m_lSize;
};

size_t IHE_MemBufRead::ReadBlock( void * buffer, size_t offset, size_t size )
{
	if ( buffer == nullptr || m_pBuf == nullptr || size == 0 )
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

BYTE IHE_MemBufRead::ReadByte( size_t offset )
{
	if ( offset >= m_lSize )
	{
		return 0;
	}else{
		return m_pBuf[offset];
	}
}

IHE_Read * HE_CreateMemBufRead( PCBYTE pBuf, size_t lSize, CHE_Allocator * pAllocator )
{
	if ( pBuf == nullptr || lSize == 0 )
	{
		return nullptr;
	}
	if ( pAllocator == nullptr )
	{
		return new IHE_MemBufRead( pBuf, lSize, nullptr );
	}else{
		return pAllocator->New<IHE_MemBufRead>( pBuf, lSize, pAllocator );
	}
}

class IHE_CrtFileReadDefault: public IHE_Read
{
public:
	IHE_CrtFileReadDefault( char const * filename, CHE_Allocator * pAllocator = nullptr );

	virtual ~IHE_CrtFileReadDefault();

	virtual size_t GetSize();

	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size );

	virtual BYTE ReadByte( size_t offset );

	virtual void Release();

private:
	FILE * m_pFile;
};

IHE_CrtFileReadDefault::IHE_CrtFileReadDefault( char const * filename, CHE_Allocator * pAllocator ) : IHE_Read( pAllocator )
{
	if ( filename == nullptr )
	{
		m_pFile = nullptr;
	}else{
		m_pFile = fopen( filename, "rb" );
	}
}

IHE_CrtFileReadDefault::~IHE_CrtFileReadDefault()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

size_t IHE_CrtFileReadDefault::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

size_t IHE_CrtFileReadDefault::ReadBlock( void * buffer, size_t offset, size_t size )
{
	if ( buffer == nullptr )
	{
		return 0;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		size_t dwRet = fread( buffer, 1, size, m_pFile );
		return dwRet;
	}else{
		return 0;
	}
}

BYTE IHE_CrtFileReadDefault::ReadByte( size_t offset )
{
	if ( m_pFile )
	{
		if ( fseek( m_pFile, offset, SEEK_SET ) == -1 )
		{
			return 0;
		}else{
			BYTE byte;
			size_t dwRet = fread( &byte, 1, 1, m_pFile );
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
		m_pFile = nullptr;
	}
}

class IHE_CrtFileReadMemcopy: public IHE_Read
{
public:
	IHE_CrtFileReadMemcopy( char const * filename, CHE_Allocator * pAllocator = nullptr );
	virtual ~IHE_CrtFileReadMemcopy();
	
	virtual size_t GetSize() { return m_lSize; }
	
	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size );
	
	virtual BYTE ReadByte( size_t offset );
	
	virtual void Release() {}
	
private:
	PBYTE m_pByte;
    size_t m_lSize;
};

IHE_CrtFileReadMemcopy::IHE_CrtFileReadMemcopy( char const * filename, CHE_Allocator * pAllocator ) : IHE_Read( pAllocator )
{
	FILE * pFile = fopen( filename, "rb" );
	if ( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		m_lSize = ftell( pFile );
		fseek( pFile, 0, SEEK_SET );
		m_pByte = GetAllocator()->NewArray<BYTE>( m_lSize );
		fread( m_pByte, 1, m_lSize, pFile );
		fclose( pFile );
	}else{
		m_pByte = nullptr;
		m_lSize = 0;
	}
}

IHE_CrtFileReadMemcopy::~IHE_CrtFileReadMemcopy()
{
	if ( m_pByte )
	{
		GetAllocator()->DeleteArray<BYTE>(m_pByte);
	}
}

size_t IHE_CrtFileReadMemcopy::ReadBlock( void * buffer, size_t offset, size_t size )
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

BYTE IHE_CrtFileReadMemcopy::ReadByte( size_t offset )
{
	if ( offset < m_lSize )
	{
		return m_pByte[offset];
	}else{
		return 0;
	}
}

IHE_Read * HE_CreateFileRead( char const * filename, BYTE mode, size_t param, CHE_Allocator * pAllocator )
{
	CHE_DefCrtAllocator defcrtAllocator;
	CHE_Allocator * pTmpAllocator = nullptr;
	if ( pAllocator == nullptr )
	{
		pTmpAllocator = &defcrtAllocator;
	}else{
		pTmpAllocator = pAllocator;
	}
	if ( filename != nullptr )
	{
		switch ( mode )
		{
		case FILEREAD_MODE_DEFAULT:
			return pTmpAllocator->New<IHE_CrtFileReadDefault>( filename, pAllocator );
		case FILEREAD_MODE_MEMCOPY:
			return pTmpAllocator->New<IHE_CrtFileReadMemcopy>( filename, pAllocator );
		case FILEREAD_MODE_BUFFER:
			return nullptr;
		case FILEREAD_MODE_BLOCKLINK:
			return nullptr;
		default:
			return nullptr;
		}
	}else{
		return nullptr;
	}
}

void HE_DestoryIHERead( IHE_Read * pIHERead )
{
	if ( pIHERead != nullptr )
	{
		pIHERead->GetAllocator()->Delete<IHE_Read>( pIHERead );
	}
}

class IHE_File : public CHE_Object 
{
	IHE_File( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ) {}

	virtual ~IHE_File() {};

	virtual size_t GetSize() = 0;

	virtual size_t GetCurOffset() = 0;

	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size ) = 0;

	virtual BYTE ReadByte( size_t offset ) = 0;

	virtual	bool WriteBlock( const void * pData, size_t offset, size_t size ) = 0;

	virtual	bool WriteBlock( const void * pData, size_t size )
	{
		return WriteBlock( pData, GetSize(), size );
	}

	virtual bool WriteByte( BYTE byte )
	{
		return WriteBlock( &byte, 1 );
	}

	virtual size_t Flush() = 0;

	virtual void Release() = 0;
};

CHE_DynBuffer::CHE_DynBuffer( size_t capacity /*= 1024*/, size_t increament /*= 1024*/, CHE_Allocator * pAllocator /*= nullptr*/ )
	: CHE_Object( pAllocator )
{
	m_lCapacity = capacity;
	m_lIncreament = increament;

	m_lpData = GetAllocator()->NewArray<BYTE>( m_lCapacity );
	memset( m_lpData, 0, m_lCapacity );
	m_lSize = 0;
}

CHE_DynBuffer::CHE_DynBuffer( const CHE_DynBuffer & buf )
	: CHE_Object( buf.GetAllocator() )
{
	m_lCapacity = buf.m_lCapacity;
	m_lIncreament = buf.m_lIncreament;
	m_lSize = buf.m_lSize;
	m_lpData = GetAllocator()->NewArray<BYTE>( m_lCapacity );
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
		GetAllocator()->DeleteArray<BYTE>( m_lpData );
		m_lpData = nullptr;
	}
}

const CHE_DynBuffer & CHE_DynBuffer::operator = ( const CHE_DynBuffer & buf )
{
	if ( this != &buf )
	{
		if ( m_lpData )
		{
			GetAllocator()->DeleteArray<BYTE>( m_lpData );
		}
		m_lCapacity = buf.m_lCapacity;
		m_lIncreament = buf.m_lIncreament;
		m_lSize = buf.m_lSize;
		m_lpData = GetAllocator()->NewArray<BYTE>( m_lCapacity );
		memset( m_lpData, 0, m_lCapacity );
		if ( m_lSize > 0 )
		{
			memcpy( m_lpData, buf.m_lpData, m_lSize );
		}
	}
	return *this;
}

size_t CHE_DynBuffer::Write( PCBYTE pBuffer, size_t offset, size_t size )
{
	if ( pBuffer == nullptr || size == 0 || offset > m_lSize )
	{
		return 0;
	}

	if ( size + offset > m_lCapacity )
	{
		size_t lNeed = size + offset - m_lCapacity;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (size_t)( lNeed / m_lIncreament ) + 1;
		}
		PBYTE tmp = GetAllocator()->NewArray<BYTE>( m_lCapacity + lNeed * m_lIncreament );
		memset( tmp, 0, m_lCapacity + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lSize );
		memcpy( tmp + offset, pBuffer, size );
		GetAllocator()->DeleteArray<BYTE>( m_lpData );
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

void CHE_DynBuffer::Alloc( size_t size )
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
	m_lpData = GetAllocator()->NewArray<BYTE>( size );
	m_lSize = size;
	m_lCapacity = size;
	m_lIncreament = size;
}

size_t CHE_DynBuffer::Write( PCBYTE pBuffer, size_t size )
{
	return Write( pBuffer, m_lSize, size );
}

size_t CHE_DynBuffer::Write( const CHE_DynBuffer & dynBuffer )
{
	if ( dynBuffer.m_lpData == nullptr || dynBuffer.m_lSize == 0 )
	{
		return 0;
	}

	if ( m_lSize + dynBuffer.m_lSize > m_lCapacity )
	{
		size_t lNeed = m_lSize + dynBuffer.m_lSize - m_lCapacity;
		if ( lNeed <= m_lIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (size_t)( lNeed / m_lIncreament ) + 1;
		}
		PBYTE tmp = GetAllocator()->NewArray<BYTE>( m_lCapacity + lNeed * m_lIncreament );
		memset( tmp, 0, m_lCapacity + lNeed * m_lIncreament );
		memcpy( tmp, m_lpData, m_lSize );
		memcpy( tmp+m_lSize, dynBuffer.m_lpData, dynBuffer.m_lSize );
		GetAllocator()->DeleteArray<BYTE>( m_lpData );
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

size_t CHE_DynBuffer::Read( PBYTE pBuffer, size_t size )
{
	if ( pBuffer == nullptr || size == 0 )
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

bool CHE_DynBuffer::ReadByte( size_t offset, PBYTE pByte )
{
	if ( offset < m_lSize  )
	{
		*pByte = *(m_lpData+offset);
		return TRUE;
	}
	return false;
}

class IHE_DynBufferWrite : public IHE_Write
{
public:
	IHE_DynBufferWrite( CHE_DynBuffer * pBuffer, CHE_Allocator * pAllocator = nullptr )
		: IHE_Write( pAllocator ), mpBuffer( pBuffer ) {}

	~IHE_DynBufferWrite() {}

	size_t GetSize()
	{
		if ( mpBuffer )
		{
			return mpBuffer->GetSize();
		}
		return 0;
	}

	size_t GetCurOffset()
	{
		if ( GetSize() == 0 )
		{
			return 0;
		}
		return GetSize()-1;
	}

	size_t Flush() { return 0; }

	bool WriteBlock( const void * pData, size_t offset, size_t size )
	{
		if ( mpBuffer == nullptr || pData == nullptr || size == 0 || offset > GetSize() )
		{
			return false;
		}
		mpBuffer->Write( (PCBYTE)pData, offset, size );
		return TRUE;
	}

	void Release() {}

private:
	CHE_DynBuffer * mpBuffer;
};

IHE_Write * HE_CreateDynBufferWrite( CHE_DynBuffer * pBuffer, CHE_Allocator * pAllocator )
{
	if ( pBuffer == nullptr )
	{
		return nullptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	return pAllocator->New<IHE_DynBufferWrite>( pBuffer, pAllocator );
}

void CHE_RefCount::AddRef()
{
#ifdef WIN32
	_InterlockedIncrement( &mRefCount );
#else
    OSAtomicIncrement32( &mRefCount );
#endif
}

void CHE_RefCount::DecRef()
{
#ifdef WIN32
	_InterlockedDecrement( &mRefCount );
#else
    OSAtomicDecrement32( &mRefCount );
#endif
}


CHE_Lock::CHE_Lock()
{
#ifdef WIN32
	mMutex = CreateMutex( nullptr, false, nullptr );
#endif

#ifdef _MAC_OS_X_
    pthread_mutex_init(&mMutex, nullptr);
#endif
}

CHE_Lock::~CHE_Lock()
{
#ifdef WIN32
	if ( mMutex )
	{
		CloseHandle( mMutex );
		mMutex = nullptr;
	}
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_destroy(&mMutex);
#endif
}

void CHE_Lock::Lock()
{
#ifdef WIN32
	WaitForSingleObject( mMutex, INFINITE );
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_lock(&mMutex);
#endif
}

void CHE_Lock::UnLock()
{
#ifdef WIN32
	ReleaseMutex( mMutex );
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_unlock(&mMutex);
#endif
}